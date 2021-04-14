// LICENSE NOTICE ==================================================================================

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

// INCLUDE =========================================================================================

#include "Functions.h"
#include "Variables.h"

#include "../Main/Documents.h"
#include "../Main/Util.h"
#include "../UI/Message.h"
#include "../Common/Macro.h"
#include SELFMAKE_FLOW
#include SELFMAKE_DEFAULT_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/types.h> 
#include <sys/stat.h>

// VARS ============================================================================================

selfmake_Library SELFMAKE_LIBRARIES_P[SELFMAKE_MAX_LIBRARIES];
selfmake_Source SELFMAKE_SOURCES_P[SELFMAKE_MAX_SOURCES];
NH_BYTE *SELFMAKE_NEWS_PP[SELFMAKE_MAX_NEWS];

// HELPER ==========================================================================================

static int MAX_LIB_LENGTH = 0;

static void selfmake_getLibNameOffset(
    NH_BYTE *name_p, NH_BYTE *buffer_p, int size)
{
SELFMAKE_BEGIN()

    memset(buffer_p, 0, size);
    int diff = MAX_LIB_LENGTH - strlen(name_p);
    for (int i = 0; i < diff; ++i) {buffer_p[i] = ' ';}

SELFMAKE_SILENT_END()
}

static NH_BOOL selfmake_caseInsensitiveMatch(
    NH_BYTE *str1_p, NH_BYTE *str2_p)
{
SELFMAKE_BEGIN()

    if (strlen(str1_p) != strlen(str2_p)) {SELFMAKE_END(NH_FALSE)}

    for (int i = 0; i < strlen(str1_p); ++i) {
        int d = tolower((unsigned char)str1_p[i]) - tolower((unsigned char)str2_p[i]);
        if (d != 0) {
            SELFMAKE_END(NH_FALSE)
        }
    }
   
SELFMAKE_END(NH_TRUE)
}

// EXECUTE =========================================================================================

static long selfmake_getVersion(
    NH_BYTE *data_p, NH_BYTE *macro_p)
{
SELFMAKE_BEGIN()

    NH_BYTE *p = strstr(data_p, macro_p); 
    if (!p) {SELFMAKE_END(0)}
    p += strlen(macro_p);
    long val = strtol(p, &p, 10);

SELFMAKE_END(val)
}

static void selfmake_getVersionDate(
    NH_BYTE *data_p, NH_BYTE *macro_p, long date_p[3])
{
SELFMAKE_BEGIN()

    date_p[0] = 0;
    date_p[1] = 0;
    date_p[2] = 0;

    NH_BYTE *p = strstr(data_p, macro_p); 
    if (!p) {SELFMAKE_SILENT_END()}

    while (p[0] != '/' || p[1] != '*' || p[2] != '*') {p--;}
    while (*p && !isdigit(*p++)) {}
    if (!*p) {SELFMAKE_SILENT_END()}

    p--;

    date_p[0] = strtol(p, &p, 10);
    p++;
    date_p[1] = strtol(p, &p, 10);
    p++;
    date_p[2] = strtol(p, &p, 10);
    p++;

SELFMAKE_SILENT_END()
}

static SELFMAKE_RESULT selfmake_getLibraryVersion(
    selfmake_Library *Lib_p)
{
SELFMAKE_BEGIN()

    NH_BYTE name_p[255];
    strcpy(name_p, Lib_p->name_p + 2);
    for (int j = 0; j < strlen(name_p); ++j) {name_p[j] = toupper(name_p[j]);}

    NH_BYTE path_p[512]; 
    NH_BYTE major_p[255];
    NH_BYTE minor_p[255];
    NH_BYTE patch_p[255];

    if (!strcmp(Lib_p->name_p, "netzhaut")) {
        sprintf(major_p, "NETZHAUT_MAJOR_VERSION"); 
        sprintf(minor_p, "NETZHAUT_MINOR_VERSION");
        sprintf(patch_p, "NETZHAUT_PATCH_VERSION");
        sprintf(path_p, "src/lib/netzhaut/netzhaut.h");
    }
    else {
        sprintf(major_p, "NH_%s_MAJOR_VERSION", name_p); 
        sprintf(minor_p, "NH_%s_MINOR_VERSION", name_p);
        sprintf(patch_p, "NH_%s_PATCH_VERSION", name_p);
        sprintf(path_p, "src/lib/%s/Common/About.h", Lib_p->name_p);
    }

    NH_BYTE *data_p = selfmake_getFileData(path_p, NULL);
    if (!data_p) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

    Lib_p->major = selfmake_getVersion(data_p, major_p);
    Lib_p->minor = selfmake_getVersion(data_p, minor_p);
    Lib_p->patch = selfmake_getVersion(data_p, patch_p);

    selfmake_getVersionDate(data_p, major_p, Lib_p->majorDate_p);
    selfmake_getVersionDate(data_p, minor_p, Lib_p->minorDate_p);
    selfmake_getVersionDate(data_p, patch_p, Lib_p->patchDate_p);

    free(data_p);

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

static SELFMAKE_RESULT selfmake_executeLibraryFunction(
    selfmake_Function *Function_p)
{
SELFMAKE_BEGIN()

    if (Function_p->arguments != 1) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

    selfmake_Library *Library_p = NULL;
    for (int i = 0; i < SELFMAKE_MAX_LIBRARIES; ++i) {
        Library_p = &SELFMAKE_LIBRARIES_P[i];
        if (!Library_p->name_p) {break;}
        Library_p = NULL;
    }

    if (!Library_p) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

    Library_p->name_p = malloc(strlen(Function_p->arguments_pp[0]) + 1);
    SELFMAKE_CHECK_NULL(Library_p->name_p)
    sprintf(Library_p->name_p, Function_p->arguments_pp[0]);

    if (strlen(Library_p->name_p) > MAX_LIB_LENGTH) {MAX_LIB_LENGTH = strlen(Library_p->name_p);}

    selfmake_getLibraryVersion(Library_p);
    selfmake_messagef("Add library [%s v%d.%d.%d]", Library_p->name_p, Library_p->major, Library_p->minor, Library_p->patch);

    NH_BYTE version_p[255];
    selfmake_getProjectVersion(version_p);
    SELFMAKE_CHECK(selfmake_updateVariable(SELFMAKE_VARIABLE_PROJECT_VERSION, version_p))

SELFMAKE_END(SELFMAKE_SUCCESS)
}

static SELFMAKE_RESULT selfmake_executeCompileArgumentsFunction(
    selfmake_Function *Function_p)
{
SELFMAKE_BEGIN()

    if (Function_p->arguments != 2) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

    selfmake_Library *Library_p = NULL;
    for (int i = 0; i < SELFMAKE_MAX_LIBRARIES; ++i) {
        Library_p = &SELFMAKE_LIBRARIES_P[i];
        if (Library_p->name_p != NULL && !strcmp(Library_p->name_p, Function_p->arguments_pp[0])) {break;}
        Library_p = NULL;
    }

    if (!Library_p) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_LIBRARY_NOT_FOUND)}

    Library_p->compileArgs_p = malloc(strlen(Function_p->arguments_pp[1]) + 1);
    SELFMAKE_CHECK_NULL(Library_p->compileArgs_p)
    sprintf(Library_p->compileArgs_p, Function_p->arguments_pp[1]);

    NH_BYTE offset_p[32];
    selfmake_getLibNameOffset(Library_p->name_p, offset_p, 32);
    selfmake_messagef("[%s]%s Set compile arguments \"%s\"", Library_p->name_p, offset_p, Library_p->compileArgs_p);

SELFMAKE_END(SELFMAKE_SUCCESS)
}

static SELFMAKE_RESULT selfmake_executeLinkArgumentsFunction(
    selfmake_Function *Function_p)
{
SELFMAKE_BEGIN()

    if (Function_p->arguments != 2) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

    selfmake_Library *Library_p = NULL;
    for (int i = 0; i < SELFMAKE_MAX_LIBRARIES; ++i) {
        Library_p = &SELFMAKE_LIBRARIES_P[i];
        if (Library_p->name_p != NULL && !strcmp(Library_p->name_p, Function_p->arguments_pp[0])) {break;}
        Library_p = NULL;
    }

    if (!Library_p) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

    Library_p->linkArgs_p = malloc(strlen(Function_p->arguments_pp[1]) + 1);
    SELFMAKE_CHECK_NULL(Library_p->linkArgs_p)
    sprintf(Library_p->linkArgs_p, Function_p->arguments_pp[1]);

    NH_BYTE offset_p[32];
    selfmake_getLibNameOffset(Library_p->name_p, offset_p, 32);
    selfmake_messagef("[%s]%s Set link arguments \"%s\"", Library_p->name_p, offset_p, Library_p->linkArgs_p);

SELFMAKE_END(SELFMAKE_SUCCESS)
}

static SELFMAKE_RESULT selfmake_executeSourceFunction(
    selfmake_Function *Function_p, NH_BOOL external)
{
SELFMAKE_BEGIN()

    for (int arg = 0; arg < Function_p->arguments; ++arg)
    {
        selfmake_Source *Source_p = NULL;
        for (int i = 0; i < SELFMAKE_MAX_SOURCES; ++i) 
        {
            Source_p = &SELFMAKE_SOURCES_P[i];
            if (!Source_p->path_p) {break;}
            Source_p = NULL;
        }
    
        if (!Source_p) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

        Source_p->path_p = malloc(strlen(Function_p->arguments_pp[arg]) + 1);
        SELFMAKE_CHECK_NULL(Source_p->path_p)
        sprintf(Source_p->path_p, Function_p->arguments_pp[arg]);

        Source_p->external = external;
   
        if (external) {
            Source_p->Library_p = &SELFMAKE_LIBRARIES_P[0];
        }
        else {
            selfmake_Library *Library_p = NULL;
            for (int i = 0; i < SELFMAKE_MAX_LIBRARIES; ++i) {
                Library_p = &SELFMAKE_LIBRARIES_P[i];
                if (Library_p->name_p && strstr(Source_p->path_p, Library_p->name_p)) {
                    Source_p->Library_p = Library_p;
                    break;
                }
            }
        }

        NH_BYTE offset_p[32];
        selfmake_getLibNameOffset(Source_p->Library_p->name_p, offset_p, 32);
        selfmake_messagef("[%s]%s Add source file \"%s\"", Source_p->Library_p->name_p, offset_p, Source_p->path_p);
    }

SELFMAKE_END(SELFMAKE_SUCCESS)
}

static SELFMAKE_RESULT selfmake_executeCopyFunction(
    selfmake_Function *Function_p)
{
SELFMAKE_BEGIN()

    if (Function_p->arguments <= 0 || Function_p->arguments > 4) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

    chdir(SELFMAKE_PROJECT_DIR_P);

    switch(Function_p->arguments)
    {
        case 2 :
            selfmake_copy(Function_p->arguments_pp[0], Function_p->arguments_pp[1], NH_FALSE, NH_FALSE);
            break;
        case 3 :
            selfmake_copy(Function_p->arguments_pp[0], Function_p->arguments_pp[1], !strcmp(Function_p->arguments_pp[2], "true") ? NH_TRUE : NH_FALSE, NH_FALSE);
            break;
        case 4 :
            selfmake_copy(Function_p->arguments_pp[0], Function_p->arguments_pp[1], !strcmp(Function_p->arguments_pp[2], "true") ? NH_TRUE : NH_FALSE, !strcmp(Function_p->arguments_pp[3], "true") ? NH_TRUE : NH_FALSE);
            break;
    }

SELFMAKE_END(SELFMAKE_SUCCESS)
}

static SELFMAKE_RESULT selfmake_executeChdirFunction(
    selfmake_Function *Function_p)
{
SELFMAKE_BEGIN()

    chdir(SELFMAKE_PROJECT_DIR_P);

    if (Function_p->arguments != 1) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)}

    selfmake_messagef("chdir %s", Function_p->arguments_pp[0]);
    chdir(Function_p->arguments_pp[0]);

SELFMAKE_END(SELFMAKE_SUCCESS)
}

static SELFMAKE_RESULT selfmake_executeMkdirFunction(
    selfmake_Function *Function_p)
{
SELFMAKE_BEGIN()

    if (Function_p->arguments != 1) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)}

    struct stat st = {0};
    if (stat(Function_p->arguments_pp[0], &st) == -1) {
        int err = mkdir(Function_p->arguments_pp[0], 0755);  // 0755 -> drwxr-xr-x
        if (err != 0) {
            SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_CANT_CREATE_DIRECTORY)
        }
    }

    selfmake_messagef("mkdir %s", Function_p->arguments_pp[0]);

SELFMAKE_END(SELFMAKE_SUCCESS)
}

static SELFMAKE_RESULT selfmake_executeSystemFunction(
    selfmake_Function *Function_p)
{
SELFMAKE_BEGIN()

    if (Function_p->arguments != 1) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}
    selfmake_messagef("system %s", Function_p->arguments_pp[0]);
    system(Function_p->arguments_pp[0]);

SELFMAKE_END(SELFMAKE_SUCCESS)
}

static SELFMAKE_RESULT selfmake_executeAddNews(
    selfmake_Function *Function_p)
{
SELFMAKE_BEGIN()

    if (Function_p->arguments != 1) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

    NH_BYTE **news_pp = NULL;
    for (int i = 0; i < SELFMAKE_MAX_NEWS; ++i) {
        if (!SELFMAKE_NEWS_PP[i]) {news_pp = &SELFMAKE_NEWS_PP[i]; break;}
    }

    SELFMAKE_CHECK_NULL(news_pp)

    *news_pp = malloc(strlen(Function_p->arguments_pp[0]) + 1);
    SELFMAKE_CHECK_NULL(*news_pp)

    sprintf(*news_pp, Function_p->arguments_pp[0]);

SELFMAKE_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_executeFunction(
    selfmake_Function *Function_p) 
{
SELFMAKE_BEGIN()

    NH_BYTE **arguments_pp = malloc(sizeof(NH_BYTE*) * Function_p->arguments);
    memcpy(arguments_pp, Function_p->arguments_pp, sizeof(NH_BYTE*) * Function_p->arguments);

    for (int i = 0; i < Function_p->arguments; ++i) {
        Function_p->arguments_pp[i] = selfmake_replaceVariables(Function_p->arguments_pp[i]);
        SELFMAKE_CHECK_NULL(Function_p->arguments_pp[i])
    }

    SELFMAKE_RESULT result;

    if (selfmake_caseInsensitiveMatch(Function_p->name_p, "library")) {
        result = selfmake_executeLibraryFunction(Function_p);
    }
    else if (selfmake_caseInsensitiveMatch(Function_p->name_p, "compileArguments")) {
        result = selfmake_executeCompileArgumentsFunction(Function_p);
    }
    else if (selfmake_caseInsensitiveMatch(Function_p->name_p, "linkArguments")) {
        result = selfmake_executeLinkArgumentsFunction(Function_p);
    }
    else if (selfmake_caseInsensitiveMatch(Function_p->name_p, "source")) {
        result = selfmake_executeSourceFunction(Function_p, NH_FALSE);
    }
    else if (selfmake_caseInsensitiveMatch(Function_p->name_p, "externalSource")) {
        result = selfmake_executeSourceFunction(Function_p, NH_TRUE);
    }
    else if (selfmake_caseInsensitiveMatch(Function_p->name_p, "copy")) {
        result = selfmake_executeCopyFunction(Function_p);
    }
    else if (selfmake_caseInsensitiveMatch(Function_p->name_p, "chdir")) {
        result = selfmake_executeChdirFunction(Function_p);
    }
    else if (selfmake_caseInsensitiveMatch(Function_p->name_p, "mkdir")) {
        result = selfmake_executeMkdirFunction(Function_p);
    }
    else if (selfmake_caseInsensitiveMatch(Function_p->name_p, "system")) {
        result = selfmake_executeSystemFunction(Function_p);
    }
    else if (selfmake_caseInsensitiveMatch(Function_p->name_p, "generateHomepage")) {
        result = selfmake_generateHomepage();
    }
    else if (selfmake_caseInsensitiveMatch(Function_p->name_p, "generateFooter")) {
        result = selfmake_generateFooter();
    }
    else if (selfmake_caseInsensitiveMatch(Function_p->name_p, "addNews")) {
        result = selfmake_executeAddNews(Function_p);
    }

    for (int i = 0; i < Function_p->arguments; ++i) {
        free(Function_p->arguments_pp[i]);
    }

    memcpy(Function_p->arguments_pp, arguments_pp, sizeof(NH_BYTE*) * Function_p->arguments);
    free(arguments_pp);

SELFMAKE_DIAGNOSTIC_END(result)
}

void selfmake_initFunctions()
{
SELFMAKE_BEGIN()

    for (int i = 0; i < SELFMAKE_MAX_LIBRARIES; ++i) {
        SELFMAKE_LIBRARIES_P[i].name_p = NULL;
        SELFMAKE_LIBRARIES_P[i].compileArgs_p = NULL;
        SELFMAKE_LIBRARIES_P[i].linkArgs_p = NULL;
        SELFMAKE_LIBRARIES_P[i].build = NH_FALSE;
    } 

    for (int i = 0; i < SELFMAKE_MAX_SOURCES; ++i) {
        SELFMAKE_SOURCES_P[i].Library_p = NULL;
        SELFMAKE_SOURCES_P[i].path_p = NULL;
    } 

    for (int i = 0; i < SELFMAKE_MAX_NEWS; ++i) {
        SELFMAKE_NEWS_PP[i] = NULL;
    } 

    static NH_BYTE *externalName_p = "nhexternal";
    SELFMAKE_LIBRARIES_P[0].name_p = externalName_p;

SELFMAKE_SILENT_END()
}

// TOP LEVEL =======================================================================================

static NH_BOOL executed = NH_FALSE;

SELFMAKE_RESULT selfmake_executeTopLevelFunctions()
{
SELFMAKE_BEGIN()

    if (!executed) {
	for (int d = 0; d < SELFMAKE_PARSER.definitions; ++d) {
	    if (SELFMAKE_PARSER.Definitions_p[d].type == SELFMAKE_DEFINITION_FUNCTION) {
		SELFMAKE_CHECK(selfmake_executeFunction(&SELFMAKE_PARSER.Definitions_p[d].Function))
	    }
	}
	executed = NH_TRUE;
    }

SELFMAKE_END(SELFMAKE_SUCCESS)
}

