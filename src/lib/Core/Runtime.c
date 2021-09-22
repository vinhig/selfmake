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
#include "Options.h"

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
        if (Prefix_p->values_pp[0][i] != SourceContext_p->name_p[i]) {
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

    for (int i = 0; i < argc; ++i) {
        int advance = 0;
        SM_CHECK(sm_parseOption(Runtime_p, argc - i, &argv_pp[i], &advance))
        i += advance - 1;
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// RUN =============================================================================================

SM_BYTE **sm_processArguments(
    sm_VariableArray *Variables_p, SM_BYTE **args_pp, int args, int *processedArgCount_p)
{
SM_BEGIN()

#include SM_CUSTOM_CHECK

    int processedArgCount = 0;
    SM_BYTE **processedArgs_pp = malloc(sizeof(SM_BYTE*)); 
    SM_CHECK_NULL(NULL, processedArgs_pp)    

    for (int i = 0; i < args; ++i) 
    {
        sm_Variable *Variable_p = sm_getVariable(Variables_p, args_pp[i]);
        if (Variable_p) {
            for (int j = 0; j < Variable_p->valueCount; ++j) {
                processedArgs_pp[processedArgCount] = malloc(strlen(Variable_p->values_pp[j]) + 1);
                SM_CHECK_NULL(NULL, processedArgs_pp[processedArgCount])
                strcpy(processedArgs_pp[processedArgCount++], Variable_p->values_pp[j]);
                processedArgs_pp = realloc(processedArgs_pp, sizeof(SM_BYTE*) * (processedArgCount + 1));
                SM_CHECK_NULL(NULL, processedArgs_pp)
            } 
        }
        else {
            processedArgs_pp[processedArgCount] = malloc(strlen(args_pp[i]) + 1);
            SM_CHECK_NULL(NULL, processedArgs_pp[processedArgCount])
            strcpy(processedArgs_pp[processedArgCount++], args_pp[i]);
            processedArgs_pp = realloc(processedArgs_pp, sizeof(SM_BYTE*) * (processedArgCount + 1));
            SM_CHECK_NULL(NULL, processedArgs_pp)
        }
    }

#include SM_DEFAULT_CHECK

    *processedArgCount_p = processedArgCount;

SM_END(processedArgs_pp)
}

static SM_RESULT sm_executeGlobalFunctions(
    sm_Runtime *Runtime_p, sm_Parser *Parser_p)
{
SM_BEGIN()

    if (Parser_p->executed) {SM_END(SM_SUCCESS)}

    sm_Block Block;
    Block.definitions = Parser_p->definitions;
    Block.Definitions_p = Parser_p->Definitions_p;

    SM_CHECK(sm_executeBlock(Runtime_p, &Block))

    Parser_p->executed = SM_TRUE;

SM_END(SM_SUCCESS)
}

SM_RESULT sm_executeRuntime(
    sm_Runtime *Runtime_p, SM_BYTE **args_pp, int args)
{
SM_BEGIN()

    for (int i = 0; i < Runtime_p->FileArray.length; ++i) 
    {
        SM_CHECK(sm_appendParser(
            &Runtime_p->ParserArray, &Runtime_p->FileArray.Files_p[i], Runtime_p->showParseTree
        ))

        sm_operationf("Execute Global Functions");
        sm_Parser *Parser_p = &Runtime_p->ParserArray.Parsers_p[Runtime_p->ParserArray.length - 1];

        if (Parser_p->executed) {continue;}
        SM_CHECK(sm_executeGlobalFunctions(Runtime_p, Parser_p))
        Parser_p->executed = SM_TRUE;

        sm_Variable *All_p = sm_getVariable(&Runtime_p->VariableArray, "ALL");
        for (int j = 0; All_p && j < All_p->valueCount; ++j) {
            SM_CHECK(sm_addBuildOption(Runtime_p, All_p->values_pp[j]))
            SM_CHECK(sm_addTestOption(Runtime_p, All_p->values_pp[j]))
        }
    }
    
    int processedArgs = 0;
    SM_BYTE **processedArgs_pp = sm_processArguments(&Runtime_p->VariableArray, args_pp, args, &processedArgs);
    SM_CHECK_NULL(processedArgs_pp)

    SM_RESULT result = sm_parseArguments(Runtime_p, processedArgs, processedArgs_pp);

    for (int i = 0; i < processedArgs; ++i) {
        free(processedArgs_pp[i]);
    }
    free(processedArgs_pp);

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

    Runtime_p->functionCallback_f      = NULL;
    Runtime_p->sourceContextCallback_f = NULL;
    Runtime_p->beforeBuildCallback_f   = NULL;
    Runtime_p->afterBuildCallback_f    = NULL;

    sm_initFileArray(&Runtime_p->FileArray);
    sm_initParserArray(&Runtime_p->ParserArray);
    sm_initTestArray(&Runtime_p->TestArray);
    sm_initSourceArray(&Runtime_p->SourceArray);
    sm_initSourceContextArray(&Runtime_p->SourceContextArray);
    sm_initVariableArray(&Runtime_p->VariableArray);

    SM_BYTE *wrk_p = sm_getWorkDirectory();

    sm_updateVariable(&Runtime_p->VariableArray, "WRK_DIR", &wrk_p, 1);
    sm_updateVariable(&Runtime_p->VariableArray, "PROJ_DIR", &wrk_p, 1);
    sm_updateVariable(&Runtime_p->VariableArray, "LIB_DEST", &wrk_p, 1);
    sm_updateVariable(&Runtime_p->VariableArray, "BIN_DEST", &wrk_p, 1);

    SM_BYTE *true_p = "true";

#ifdef __APPLE__
    sm_updateVariable(&Runtime_p->VariableArray, "MAC", &true_p, 1);
#elif WIN32 
    sm_updateVariable(&Runtime_p->VariableArray, "WINDOWS", &true_p, 1);
#elif __linux__
    sm_updateVariable(&Runtime_p->VariableArray, "LINUX", &true_p, 1);
#endif

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

void sm_destroyRuntime(
    sm_Runtime *Runtime_p)
{
SM_BEGIN()
SM_SILENT_END()
}

// INITIALIZE/TERMINATE ============================================================================

SM_RESULT sm_initialize()
{
    sm_initThreadPool();
    SM_CHECK(sm_initRuntime(&SM_DEFAULT_RUNTIME))
    SM_BYTE *name_p = "selfmake";
    sm_updateVariable(&SM_DEFAULT_RUNTIME.VariableArray, "NAME", &name_p, 1);
}

void sm_terminate()
{
SM_BEGIN()
SM_SILENT_END()
}

