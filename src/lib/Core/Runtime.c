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

#include "../Parser/Functions.h"
#include "../Parser/Variables.h"

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

//SM_BYTE *sm_getLibraryName(
//    int i)
//{
//SM_BEGIN()
//
//    static SM_BYTE libraryName_p[255];
//
//    memset(libraryName_p, 0, 255);
//    sm_Library *Library_p = &SM_LIBRARIES_P[i];
//
//    if (!Library_p->name_p) {SM_END(libraryName_p)}
//
//    else if (!strcmp(Library_p->name_p, "netzhaut")) {
//        strcpy(libraryName_p, "netzhaut");
//    }
//    else {
//        strcpy(libraryName_p, Library_p->name_p + 2);
//        for (int j = 0; j < strlen(libraryName_p); ++j) {
//            libraryName_p[j] = tolower(libraryName_p[j]);
//        }
//    }
//
//SM_END(libraryName_p)
//}

// PARSE ===========================================================================================

static SM_RESULT sm_parseArguments(
    int argc, SM_BYTE **argv_pp)
{
SM_BEGIN()

//    SM_BOOL libs = SM_FALSE, bins = SM_FALSE;
//    SM_BOOL negate = SM_FALSE;
//
//    for (int i = 1; i < argc; ++i) 
//    {
//        if (argv_pp[i][0] == '-' && argv_pp[i][1] != '-')
//        {
//            negate = SM_FALSE, libs = SM_FALSE, bins = SM_FALSE;
//
//            if (strstr(argv_pp[i], "n")) {
//                negate = SM_TRUE;
//            }
//            if (strstr(argv_pp[i], "l")) {
//                libs = SM_TRUE;
//                BUILD_ALL_LIBRARIES = SM_TRUE;
//                if (negate) {
//                    for (int j = 0; j < SM_MAX_LIBRARIES; ++j) {
//                        if (SM_LIBRARIES_P[j].name_p) {
//                            SM_LIBRARIES_P[j].build = SM_TRUE;
//                        }
//                    }
//                }
//            }
//            if (strstr(argv_pp[i], "b")) {
//                bins = SM_TRUE; 
//                BUILD_ALL_BINARIES = SM_TRUE;
//            }
////            if (strstr(argv_pp[i], "i")) {
////                if (libs) {INSTALL_ALL_LIBRARIES = SM_TRUE;}
////                if (bins) {INSTALL_ALL_BINARIES = SM_TRUE;}
////                *build_p = SM_TRUE; 
////            }
//            if (!strstr(argv_pp[i], "l")
//            &&  !strstr(argv_pp[i], "b")) {
//                sm_noticef("Invalid option \"%s\"", argv_pp[i]);
//                SM_END(SM_ERROR_INVALID_OPTION)
//            }
//            continue;
//        }
//
//        if (argv_pp[i][0] == '-' && argv_pp[i][1] == '-')
//        {
//            sm_LongOption *Option_p = sm_getLongOption(argv_pp[i] + 2);
//            if (!Option_p) {SM_END(SM_ERROR_INVALID_OPTION)}
//            SM_CHECK(sm_executeLongOption(Option_p))
//            continue;
//        }
//
//        if (libs)
//        {
//            SM_BOOL valid = SM_FALSE;
//            for (int j = 0; j < SM_MAX_LIBRARIES; ++j) {
//                if (!strcmp(argv_pp[i], sm_getLibraryName(j))) {
//                    valid = SM_TRUE;
//                    break;
//                }
//            }
//            if (!valid) {
//                sm_noticef("Invalid option \"%s\"", argv_pp[i]);
//                SM_END(SM_ERROR_INVALID_OPTION)
//            }
//        }
//        if (libs && !negate) {
//            for (int j = 0; j < SM_MAX_LIBRARIES; ++j) {
//                SM_LIBRARIES_P[j].build = !strcmp(argv_pp[i], sm_getLibraryName(j)) || SM_LIBRARIES_P[j].build;
//            }
//        }
//        if (libs && negate) {
//            for (int j = 0; j < SM_MAX_LIBRARIES; ++j) {
//                SM_LIBRARIES_P[j].build = strcmp(argv_pp[i], sm_getLibraryName(j)) && SM_LIBRARIES_P[j].build;
//            }
//        }
//    }
//
//    *build_p = SM_TRUE;
//    for (int j = 0; j < SM_MAX_LIBRARIES; ++j) {
//        if (SM_LIBRARIES_P[j].build && SM_LIBRARIES_P[j].name_p) {
//            BUILD_A_LIBRARY = SM_TRUE;
//            break;
//        }
//    }
//
//    if (!BUILD_A_LIBRARY && BUILD_ALL_LIBRARIES) {
//        for (int j = 0; j < SM_MAX_LIBRARIES; ++j) {
//            if (SM_LIBRARIES_P[j].name_p) {
//                SM_LIBRARIES_P[j].build = SM_TRUE;
//            }
//        }
//        BUILD_A_LIBRARY = SM_TRUE;
//    }

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
        SM_CHECK(sm_executeFunctions(
            Runtime_p, &Runtime_p->ParserArray.Parsers_p[Runtime_p->ParserArray.length - 1]
        ))
    }

    SM_RESULT result = sm_parseArguments(args, args_pp);

    if (result == SM_SUCCESS)
    {
        for (int i = 0; i < Runtime_p->SourceContextArray.length; ++i) {
            if (Runtime_p->SourceContextArray.SourceContexts_p[i].build) {
//                SM_CHECK(sm_buildLibrary(&Runtime_p->Libraries_p[i], Runtime_p->Libraries_p[i].install))
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

sm_Runtime *sm_createRuntime(
    SM_BYTE *name_p, SM_BYTE *projectDirectory_p)
{
SM_BEGIN()

#include SM_CUSTOM_CHECK

    SM_CHECK_NULL(NULL, projectDirectory_p)

    sm_Runtime *Runtime_p = malloc(sizeof(sm_Runtime));
    SM_CHECK_NULL(NULL, Runtime_p)

    Runtime_p->showParseTree = SM_TRUE;
    Runtime_p->GUI = SM_FALSE;
    Runtime_p->quiet = SM_FALSE;
    Runtime_p->buildCallback_p = NULL;

    Runtime_p->name_p = malloc(strlen(name_p) + 1);
    SM_CHECK_NULL(NULL, Runtime_p->name_p)
    sprintf(Runtime_p->name_p, name_p);

    Runtime_p->projectDirectory_p = malloc(strlen(projectDirectory_p) + 1);
    SM_CHECK_NULL(NULL, Runtime_p->projectDirectory_p)
    sprintf(Runtime_p->projectDirectory_p, projectDirectory_p);

    sm_initFileArray(&Runtime_p->FileArray);
    sm_initParserArray(&Runtime_p->ParserArray);
    sm_initSourceArray(&Runtime_p->SourceArray);
    sm_initSourceContextArray(&Runtime_p->SourceContextArray);
    SM_CHECK(NULL, sm_initVariableArray(&Runtime_p->VariableArray))

    sm_updateVariable(&Runtime_p->VariableArray, "PROJECT_DIRECTORY", projectDirectory_p);
    sm_updateVariable(&Runtime_p->VariableArray, "LIB_DEST", projectDirectory_p);

#include SM_DEFAULT_CHECK

SM_END(Runtime_p)
}
