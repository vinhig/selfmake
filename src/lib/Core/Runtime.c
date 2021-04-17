// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Runtime.h"
#include "Thread.h"
#include "File.h"
#include "Build.h"

#include "../Parser/Functions.h"
#include "../Parser/Variables.h"

#include "../Common/Functions.h"
#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// VARS ============================================================================================

sm_Runtime SM_DEFAULT_RUNTIME;

// HELPER ==========================================================================================

static sm_LongOption *sm_getLongOption(
    sm_Parser *Parser_p, SM_BYTE *arg_p)
{
SM_BEGIN()

    sm_LongOption *Option_p = NULL;

    for (int d = 0; d < Parser_p->definitions; ++d) {
        if (Parser_p->Definitions_p[d].type == SM_DEFINITION_LONG_OPTION) {
            if (!strcmp(Parser_p->Definitions_p[d].LongOption.name_p, arg_p)) {
                Option_p = &Parser_p->Definitions_p[d].LongOption;
                break;
            }
        }
    }

SM_END(Option_p)
}

static SM_RESULT sm_executeLongOption(
    sm_Runtime *Runtime_p, sm_LongOption *Option_p)
{
SM_BEGIN()

    sm_operationf("--%s %s", Option_p->name_p, Option_p->description_p);

    for (int d = 0; d < Option_p->Block_p->Block.definitions; ++d) {
        if (Option_p->Block_p->Block.Definitions_p[d].type == SM_DEFINITION_FUNCTION) {
            SM_CHECK(sm_executeFunction(Runtime_p, &Option_p->Block_p->Block.Definitions_p[d].Function))
        }
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_BYTE *sm_getSourceContextName(
    sm_Runtime *Runtime_p, sm_SourceContext *SourceContext_p)
{
SM_BEGIN()

    sm_Variable *Prefix_p = sm_getVariable(&Runtime_p->VariableArray, "PREFIX");

    if (!Prefix_p) {
        SM_END(SourceContext_p->name_p)
    }

    int offset = 0;
    for (int i = 0; i < strlen(Prefix_p->values_pp[0]) && i < strlen(SourceContext_p->name_p); ++i) {
        if (*Prefix_p->values_pp[i] != SourceContext_p->name_p[i]) {
            break;
        }
        offset++;
    }

SM_END(SourceContext_p->name_p + offset)
}

// PARSE ===========================================================================================

static SM_RESULT sm_parseArguments(
    sm_Runtime *Runtime_p, int argc, SM_BYTE **argv_pp)
{
SM_BEGIN()

    SM_BOOL negate = SM_FALSE, build = SM_FALSE;

    for (int i = 1; i < argc; ++i) 
    {
        if (argv_pp[i][0] == '-' && argv_pp[i][1] != '-')
        {
            negate = SM_FALSE, build = SM_FALSE;

            if (strstr(argv_pp[i], "n")) {
                negate = SM_TRUE;
                if (negate) {
                    for (int j = 0; j < Runtime_p->SourceContextArray.length; ++j) {
                        sm_SourceContext *Context_p = &Runtime_p->SourceContextArray.SourceContexts_p[j];
                        Context_p->build = SM_TRUE;
                    }
                }
            }
            if (strstr(argv_pp[i], "b")) {
                build = SM_TRUE;
            }
//            if (strstr(argv_pp[i], "i")) {
//                if (libs) {INSTALL_ALL_LIBRARIES = SM_TRUE;}
//                if (bins) {INSTALL_ALL_BINARIES = SM_TRUE;}
//                *build_p = SM_TRUE; 
//            }
            if (!strstr(argv_pp[i], "b")) {
                sm_noticef("Invalid option \"%s\"", argv_pp[i]);
                SM_END(SM_ERROR_INVALID_OPTION)
            }
            continue;
        }

        if (argv_pp[i][0] == '-' && argv_pp[i][1] == '-')
        {
            sm_LongOption *Option_p = NULL;

            for (int j = 0; j < Runtime_p->ParserArray.length; ++j) {
                sm_Parser *Parser_p = &Runtime_p->ParserArray.Parsers_p[j];
                Option_p = sm_getLongOption(Parser_p, argv_pp[i] + 2);
                if (Option_p) {break;}
            }

            if (!Option_p) {SM_DIAGNOSTIC_END(SM_ERROR_INVALID_OPTION)}
            SM_CHECK(sm_executeLongOption(Runtime_p, Option_p))

            continue;
        }

        if (build)
        {
            SM_BOOL valid = SM_FALSE;
            for (int j = 0; j < Runtime_p->SourceContextArray.length; ++j) {
                if (!strcmp(argv_pp[i], sm_getSourceContextName(Runtime_p, &Runtime_p->SourceContextArray.SourceContexts_p[j]))) {
                    valid = SM_TRUE;
                    break;
                }
            }
            if (!valid) {
                sm_noticef("Invalid option \"%s\"", argv_pp[i]);
                SM_END(SM_ERROR_INVALID_OPTION)
            }
        }
        if (build && !negate) {
            for (int j = 0; j < Runtime_p->SourceContextArray.length; ++j) {
                sm_SourceContext *Context_p = &Runtime_p->SourceContextArray.SourceContexts_p[j];
                Context_p->build = !strcmp(argv_pp[i], sm_getSourceContextName(Runtime_p, Context_p)) || Context_p->build;
            }
        }
        if (build && negate) {
            for (int j = 0; j < Runtime_p->SourceContextArray.length; ++j) {
                sm_SourceContext *Context_p = &Runtime_p->SourceContextArray.SourceContexts_p[j];
                Context_p->build = strcmp(argv_pp[i], sm_getSourceContextName(Runtime_p, Context_p)) && Context_p->build;
            }
        }
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// RUN =============================================================================================

SM_RESULT sm_executeRuntime(
    sm_Runtime *Runtime_p, SM_BYTE **args_pp, int args)
{
SM_BEGIN()

    for (int i = 0; i < Runtime_p->FileArray.length; ++i) {
        SM_CHECK(sm_appendParser(
            &Runtime_p->ParserArray, &Runtime_p->FileArray.Files_p[i], Runtime_p->showParseTree
        ))
        sm_operationf("EXECUTE_FUNCTIONS");
        SM_CHECK(sm_executeFunctions(
            Runtime_p, &Runtime_p->ParserArray.Parsers_p[Runtime_p->ParserArray.length - 1]
        ))
    }

    SM_RESULT result = sm_parseArguments(Runtime_p, args, args_pp);

    if (result == SM_SUCCESS)
    {
        for (int i = 0; i < Runtime_p->SourceContextArray.length; ++i) {
            if (Runtime_p->SourceContextArray.SourceContexts_p[i].build) {
                SM_CHECK(sm_build(Runtime_p, &Runtime_p->SourceContextArray.SourceContexts_p[i]))
            }
        }
    }

    SM_CHECK(sm_exitMessage(result))

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_RESULT sm_run(
    sm_Runtime *Runtime_p, int argc, SM_BYTE **argv_pp)
{
SM_BEGIN()
SM_DIAGNOSTIC_END(sm_runThread(Runtime_p, argv_pp, argc))
} 

// CREATE ==========================================================================================

static SM_RESULT sm_initRuntime(
    sm_Runtime *Runtime_p)
{
SM_BEGIN()

    Runtime_p->showParseTree = SM_TRUE;
    Runtime_p->GUI = SM_FALSE;
    Runtime_p->quiet = SM_FALSE;
    Runtime_p->functionCallback_f = NULL;
    Runtime_p->sourceContextCallback_f = NULL;

    sm_initFileArray(&Runtime_p->FileArray);
    sm_initParserArray(&Runtime_p->ParserArray);
    sm_initSourceArray(&Runtime_p->SourceArray);
    sm_initSourceContextArray(&Runtime_p->SourceContextArray);
    sm_initVariableArray(&Runtime_p->VariableArray);

    SM_BYTE *wrk_p = sm_getWorkDirectory();

    sm_updateVariable(&Runtime_p->VariableArray, "WRK_DIR", &wrk_p, 1);
    sm_updateVariable(&Runtime_p->VariableArray, "PROJ_DIR", &wrk_p, 1);
    sm_updateVariable(&Runtime_p->VariableArray, "LIB_DEST", &wrk_p, 1);

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

sm_Runtime *sm_createRuntime(
    SM_BYTE *name_p)
{
SM_BEGIN()

#include SM_CUSTOM_CHECK

    SM_CHECK_NULL(NULL, name_p)

    sm_Runtime *Runtime_p = malloc(sizeof(sm_Runtime));
    SM_CHECK_NULL(NULL, Runtime_p)

    SM_CHECK(NULL, sm_initRuntime(Runtime_p))

    sm_updateVariable(&Runtime_p->VariableArray, "NAME", &name_p, 1);

#include SM_DEFAULT_CHECK

SM_END(Runtime_p)
}

// INIT ============================================================================================

SM_RESULT sm_init()
{
    sm_initThreadPool();
    SM_CHECK(sm_initRuntime(&SM_DEFAULT_RUNTIME))
    SM_BYTE *name_p = "selfmake";
    sm_updateVariable(&SM_DEFAULT_RUNTIME.VariableArray, "NAME", &name_p, 1);
}

