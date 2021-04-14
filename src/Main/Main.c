// LICENSE NOTICE ==================================================================================

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

// INCLUDE =========================================================================================

#include "Main.h"
#include "Util.h"
#include "Binaries.h"
#include "Libraries.h"
#include "../UI/GUI.h"
#include "../UI/Message.h"
#include "../Parser/Parser.h"
#include "../Parser/Functions.h"
#include "../Parser/Variables.h"

#include "../Common/Macro.h"

#include SELFMAKE_FLOW
#include SELFMAKE_DEFAULT_CHECK

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// DATA ============================================================================================

static NH_BOOL RUN = NH_FALSE;
static NH_BOOL BUILD_A_LIBRARY     = NH_FALSE;
static NH_BOOL BUILD_ALL_LIBRARIES = NH_FALSE;
static NH_BOOL BUILD_A_BINARY     = NH_FALSE;
static NH_BOOL BUILD_ALL_BINARIES = NH_FALSE;
static NH_BOOL BINARY_WEB_BROWSER   = NH_FALSE;
static NH_BOOL BINARY_TERMINAL      = NH_FALSE;
static NH_BOOL BINARY_WINDOW_SYSTEM = NH_FALSE;
static NH_BOOL INSTALL_ALL_LIBRARIES = NH_FALSE;
static NH_BOOL INSTALL_ALL_BINARIES  = NH_FALSE;
static NH_BOOL GENERATE_DOCS  = NH_FALSE;
static NH_BOOL VULKAN_SHADERS = NH_FALSE;
static NH_BOOL OFFLINE        = NH_FALSE;

// PARSE INPUT =====================================================================================

static selfmake_LongOption *selfmake_getLongOption(
    NH_BYTE *arg_p)
{
SELFMAKE_BEGIN()

    selfmake_LongOption *Option_p = NULL;

    for (int d = 0; d < SELFMAKE_PARSER.definitions; ++d) {
        if (SELFMAKE_PARSER.Definitions_p[d].type == SELFMAKE_DEFINITION_LONG_OPTION) {
            if (!strcmp(SELFMAKE_PARSER.Definitions_p[d].LongOption.name_p, arg_p)) {
                Option_p = &SELFMAKE_PARSER.Definitions_p[d].LongOption;
                break;
            }
        }
    }

SELFMAKE_END(Option_p)
}

static SELFMAKE_RESULT selfmake_executeLongOption(
    selfmake_LongOption *Option_p)
{
SELFMAKE_BEGIN()

    selfmake_operationf("--%s %s", Option_p->name_p, Option_p->description_p);

    for (int d = 0; d < Option_p->Block_p->Block.definitions; ++d) {
        if (Option_p->Block_p->Block.Definitions_p[d].type == SELFMAKE_DEFINITION_FUNCTION) {
            SELFMAKE_CHECK(selfmake_executeFunction(&Option_p->Block_p->Block.Definitions_p[d].Function))
        }
    }

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

NH_BYTE *selfmake_getLibraryName(
    int i)
{
SELFMAKE_BEGIN()

    static NH_BYTE libraryName_p[255];

    memset(libraryName_p, 0, 255);
    selfmake_Library *Library_p = &SELFMAKE_LIBRARIES_P[i];

    if (!Library_p->name_p) {SELFMAKE_END(libraryName_p)}

    else if (!strcmp(Library_p->name_p, "netzhaut")) {
        strcpy(libraryName_p, "netzhaut");
    }
    else {
        strcpy(libraryName_p, Library_p->name_p + 2);
        for (int j = 0; j < strlen(libraryName_p); ++j) {
            libraryName_p[j] = tolower(libraryName_p[j]);
        }
    }

SELFMAKE_END(libraryName_p)
}

static SELFMAKE_RESULT selfmake_parseInput(
    int argc, char **argv_pp)
{
SELFMAKE_BEGIN()

    NH_BOOL libs = NH_FALSE, bins = NH_FALSE;
    NH_BOOL negate = NH_FALSE;

    for (int i = 1; i < argc; ++i) 
    {
        if (argv_pp[i][0] == '-' && argv_pp[i][1] != '-')
        {
            negate = NH_FALSE, libs = NH_FALSE, bins = NH_FALSE;

            if (strstr(argv_pp[i], "n")) {
                negate = NH_TRUE;
            }
            if (strstr(argv_pp[i], "l")) {
                RUN = libs = NH_TRUE;
                BUILD_ALL_LIBRARIES = NH_TRUE;
                if (negate) {
                    for (int j = 0; j < SELFMAKE_MAX_LIBRARIES; ++j) {
                        if (SELFMAKE_LIBRARIES_P[j].name_p) {
                            SELFMAKE_LIBRARIES_P[j].build = NH_TRUE;
                        }
                    }
                }
            }
            if (strstr(argv_pp[i], "b")) {
                RUN = bins = NH_TRUE; 
                BUILD_ALL_BINARIES = NH_TRUE;
            }
            if (strstr(argv_pp[i], "i")) {
                if (libs) {INSTALL_ALL_LIBRARIES = NH_TRUE;}
                if (bins) {INSTALL_ALL_BINARIES = NH_TRUE;}
                RUN = NH_TRUE; 
            }
            if (!strstr(argv_pp[i], "l")
            &&  !strstr(argv_pp[i], "b")) {
                selfmake_noticef("Invalid option \"%s\"", argv_pp[i]);
                SELFMAKE_END(SELFMAKE_ERROR_INVALID_OPTION)
            }
            continue;
        }

        if (argv_pp[i][0] == '-' && argv_pp[i][1] == '-')
        {
            selfmake_LongOption *Option_p = selfmake_getLongOption(argv_pp[i] + 2);
            if (!Option_p) {SELFMAKE_END(SELFMAKE_ERROR_INVALID_OPTION)}
            SELFMAKE_CHECK(selfmake_executeLongOption(Option_p))

//            else if (!strcmp(argv_pp[i] + 2, "flow")) {
//                libs = bins = NH_FALSE; 
//                SELFMAKE_FLOW_LOGGING = NH_TRUE;
//            }
//            else if (!strcmp(argv_pp[i] + 2, "offline")) {
//                libs = bins = NH_FALSE; 
//                OFFLINE = NH_TRUE;
//            }
//            else if (!strcmp(argv_pp[i] + 2, "version")) {
//                RUN = NH_FALSE; 
//                selfmake_printVersion(); break;
//            }
//            else if (!strcmp(argv_pp[i] + 2, "help")) {
//                RUN = NH_FALSE; 
//                selfmake_printHelp(); break;
//            }
//            else if (!strcmp(argv_pp[i] + 2, "quiet")) {
//                libs = bins = NH_FALSE; 
//                SELFMAKE_QUIET = NH_TRUE;
//            }
//            else {
//                selfmake_noticef("Invalid option \"%s\"", argv_pp[i]);
//                SELFMAKE_END(SELFMAKE_ERROR_INVALID_OPTION)
//            }
            continue;
        }

        if (libs)
        {
            NH_BOOL valid = NH_FALSE;
            for (int j = 0; j < SELFMAKE_MAX_LIBRARIES; ++j) {
                if (!strcmp(argv_pp[i], selfmake_getLibraryName(j))) {
                    valid = NH_TRUE;
                    break;
                }
            }
            if (!valid) {
                selfmake_noticef("Invalid option \"%s\"", argv_pp[i]);
                SELFMAKE_END(SELFMAKE_ERROR_INVALID_OPTION)
            }
        }

        if (libs && !negate) {
            for (int j = 0; j < SELFMAKE_MAX_LIBRARIES; ++j) {
                SELFMAKE_LIBRARIES_P[j].build = !strcmp(argv_pp[i], selfmake_getLibraryName(j)) || SELFMAKE_LIBRARIES_P[j].build;
            }
        }
        if (libs && negate) {
            for (int j = 0; j < SELFMAKE_MAX_LIBRARIES; ++j) {
                SELFMAKE_LIBRARIES_P[j].build = strcmp(argv_pp[i], selfmake_getLibraryName(j)) && SELFMAKE_LIBRARIES_P[j].build;
            }
        }

        if (bins)
        {
            if (strcmp(argv_pp[i], "webbrowser")
            &&  strcmp(argv_pp[i], "terminal")
            &&  strcmp(argv_pp[i], "windowsystem")) {
                selfmake_noticef("Invalid option \"%s\"", argv_pp[i]);
                SELFMAKE_END(SELFMAKE_ERROR_INVALID_OPTION)
            }
        }
        if (bins)
        {
            BINARY_TERMINAL      = !strcmp(argv_pp[i], "terminal") || BINARY_TERMINAL;
            BINARY_WEB_BROWSER   = !strcmp(argv_pp[i], "webbrowser") || BINARY_WEB_BROWSER;
            BINARY_WINDOW_SYSTEM = !strcmp(argv_pp[i], "windowsystem") || BINARY_WINDOW_SYSTEM;
        }
        if (bins && negate)
        {
            BINARY_WEB_BROWSER   = BINARY_WEB_BROWSER ? !BINARY_WEB_BROWSER : BINARY_WEB_BROWSER;
            BINARY_WINDOW_SYSTEM = BINARY_WINDOW_SYSTEM ? !BINARY_WINDOW_SYSTEM : BINARY_WINDOW_SYSTEM;
            BINARY_TERMINAL      = BINARY_TERMINAL ? !BINARY_TERMINAL : BINARY_TERMINAL;
        }
    }

    for (int j = 0; j < SELFMAKE_MAX_LIBRARIES; ++j) {
        if (SELFMAKE_LIBRARIES_P[j].build && SELFMAKE_LIBRARIES_P[j].name_p) {
            BUILD_A_LIBRARY = NH_TRUE;
            break;
        }
    }

    if (!BUILD_A_LIBRARY && BUILD_ALL_LIBRARIES) {
        for (int j = 0; j < SELFMAKE_MAX_LIBRARIES; ++j) {
            if (SELFMAKE_LIBRARIES_P[j].name_p) {
                SELFMAKE_LIBRARIES_P[j].build = NH_TRUE;
            }
        }
        BUILD_A_LIBRARY = NH_TRUE;
    }

    BUILD_A_BINARY = 
        BINARY_WEB_BROWSER || BINARY_WINDOW_SYSTEM || BINARY_TERMINAL;

    if (!BUILD_A_BINARY && BUILD_ALL_BINARIES) {
        BINARY_WEB_BROWSER   = NH_TRUE;
        BINARY_WINDOW_SYSTEM = NH_TRUE;
        BINARY_TERMINAL      = NH_TRUE;
        BUILD_A_BINARY       = NH_TRUE;
    }

SELFMAKE_END(SELFMAKE_SUCCESS)
}

// RUN =============================================================================================

static SELFMAKE_RESULT selfmake_run()
{
SELFMAKE_BEGIN()

//    if (GENERATE_DOCS) {
//        selfmake_operationf("GENERATE DOCS");
//        SELFMAKE_CHECK(SELFMAKE_ERROR_GENERATE_DOCUMENTS_FAILURE, selfmake_generateDocs())
//    }
//
//    if (!OFFLINE && (BUILD_A_LIBRARY)) {
//        selfmake_operationf("DOWNLOAD EXTERNALS");
//        SELFMAKE_CHECK(SELFMAKE_ERROR_DOWNLOAD_FAILED, selfmake_downloadExternals())
//    }
//
//    if (BUILD_A_LIBRARY) {
//        SELFMAKE_CHECK(SELFMAKE_ERROR_BUILD_LIBRARY_FAILED, selfmake_prepareLibraryBuild())
//        SELFMAKE_CHECK(SELFMAKE_ERROR_BUILD_LIBRARY_FAILED, selfmake_buildnhexternal(INSTALL_ALL_LIBRARIES))
//    }

    for (int i = 0; i < SELFMAKE_MAX_LIBRARIES; ++i) {
        if (SELFMAKE_LIBRARIES_P[i].build && SELFMAKE_LIBRARIES_P[i].name_p) {
            SELFMAKE_CHECK(selfmake_buildLibrary(&SELFMAKE_LIBRARIES_P[i], INSTALL_ALL_LIBRARIES))
        }
    }

//    if (BINARY_TERMINAL) {
//        selfmake_operationf("BUILD TERMINAL");
//        SELFMAKE_CHECK(SELFMAKE_ERROR_BUILD_WEB_BROWSER_FAILED, selfmake_buildnhterminal())
//        if (INSTALL_ALL_BINARIES) {
//            selfmake_operationf("INSTALL_ALL TERMINAL");
//            SELFMAKE_CHECK(SELFMAKE_ERROR_INSTALL_ALL_WEB_BROWSER_FAILED, selfmake_installnhterminal())
//        }
//    }

    if (BUILD_ALL_BINARIES || BINARY_WEB_BROWSER) 
    {
        selfmake_operationf("BUILD WEB BROWSER");
        SELFMAKE_CHECK(selfmake_buildnhwebbrowser())

        if (INSTALL_ALL_BINARIES)
        {
            selfmake_operationf("INSTALL_ALL WEB BROWSER");
            SELFMAKE_CHECK(selfmake_installnhwebbrowser())
        }
    }

//    if (INSTALL_ALL_BINARIES && (BUILD_ALL_BINARIES || BUILD_A_BINARY)) {
//        selfmake_operationf("INSTALL_ALL LOGO");
//        SELFMAKE_CHECK(SELFMAKE_ERROR_INSTALL_ALL_LOGO_FAILED, selfmake_installLogo())
//    }

#include SELFMAKE_DEFAULT_CHECK

SELFMAKE_END(SELFMAKE_SUCCESS)
}

// CHECK LOCATION ==================================================================================

static NH_BOOL selfmake_validLocation()
{
SELFMAKE_BEGIN()

    char check_p[2048] = {'\0'};
    sprintf(check_p, "%s%s", SELFMAKE_PROJECT_DIR_P, "/build");
    if (!selfmake_fileExists(check_p)) {SELFMAKE_END(NH_FALSE)}

    memset(check_p, '\0', sizeof(char) * 2048); 
    sprintf(check_p, "%s%s", SELFMAKE_PROJECT_DIR_P, "/src");
    if (!selfmake_fileExists(check_p)) {SELFMAKE_END(NH_FALSE)}

    memset(check_p, '\0', sizeof(char) * 2048);
    sprintf(check_p, "%s%s", SELFMAKE_PROJECT_DIR_P, "/README.md");
    if (!selfmake_fileExists(check_p)) {SELFMAKE_END(NH_FALSE)}

SELFMAKE_END(NH_TRUE)
}

// MAIN ============================================================================================

SELFMAKE_RESULT selfmake_main(
    int argc, char **argv_pp)
{
SELFMAKE_BEGIN()

    SELFMAKE_RESULT result = SELFMAKE_SUCCESS;

    if (selfmake_validLocation()) 
    {
        selfmake_initFunctions();
        selfmake_initVariables();
        selfmake_updateVariable(SELFMAKE_VARIABLE_PROJECT_DIRECTORY, SELFMAKE_PROJECT_DIR_P);
        selfmake_parseInstallerFile();
        selfmake_showParserOutput();

        SELFMAKE_CHECK(selfmake_executeTopLevelFunctions())

        if (argc == 1) {
            SELFMAKE_GUI = NH_TRUE;
            result = selfmake_runGUI();
        }
        else { 
            result = selfmake_parseInput(argc, argv_pp);
            if (result == SELFMAKE_SUCCESS && RUN) {
                result = selfmake_run();
            }
        }
    }
    else {result = SELFMAKE_ERROR_BAD_STATE;}

    SELFMAKE_CHECK(selfmake_exitMessage(result))

SELFMAKE_DIAGNOSTIC_END(result)
}

int main(
    int argc, char **argv_pp)
{
SELFMAKE_BEGIN()

    memset(SELFMAKE_PROJECT_DIR_P, 0, SELFMAKE_PROJECT_DIR_LENGTH);
    SELFMAKE_CHECK(selfmake_getProjectDir(SELFMAKE_PROJECT_DIR_P, SELFMAKE_PROJECT_DIR_LENGTH))

    SELFMAKE_RESULT result = selfmake_main(argc, argv_pp);

SELFMAKE_DIAGNOSTIC_END(result)
} 

