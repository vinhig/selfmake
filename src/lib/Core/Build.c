// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Build.h"
#include "Utils.h"

#include "../UI/Message.h"
#include "../Common/Macros/Macros.h"

#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>

// CREATE BINARY ===================================================================================

static SM_RESULT sm_createBinary(
    sm_Runtime *Runtime_p, sm_SourceContext *SourceContext_p)
{
SM_BEGIN()

    chdir(SourceContext_p->path_p);

    SM_BYTE binPath_p[256] = {'\0'};
    sprintf(binPath_p, "%s/%s", sm_getVariable(&Runtime_p->VariableArray, "BIN_DEST")->values_pp[0], SourceContext_p->name_p);

    int length = 0;
    for (int i = 0; i < Runtime_p->SourceArray.length; ++i) {
        sm_Source *Source_p = &Runtime_p->SourceArray.Sources_p[i];
        if (Source_p->Context_p == SourceContext_p) {
            length += strlen(Source_p->path_p) + 1;
        }
    }
   
    SM_BYTE *sources_p = malloc(length + 1);
    SM_CHECK_NULL(sources_p)
    memset(sources_p, 0, length + 1);

    int offset = 0;
    for (int i = 0; i < Runtime_p->SourceArray.length; ++i) {
        sm_Source *Source_p = &Runtime_p->SourceArray.Sources_p[i];
        if (Source_p->Context_p == SourceContext_p) {
            sprintf(sources_p + strlen(sources_p), "%s ", Source_p->path_p);
        }
    }

    int commandLength = strlen(sources_p) + strlen(SourceContext_p->compileArgs_p) + strlen(SourceContext_p->linkArgs_p) + strlen(binPath_p) + 128;
    SM_BYTE *command_p = malloc(commandLength);
    SM_CHECK_NULL(command_p)
    memset(command_p, 0, commandLength);

    // set -no-pie because of https://stackoverflow.com/questions/41398444/gcc-creates-mime-type-application-x-sharedlib-instead-of-application-x-applicati
    sprintf(command_p, "gcc %s -o%s -no-pie %s %s", SourceContext_p->compileArgs_p, binPath_p, SourceContext_p->linkArgs_p, sources_p);

    sm_messagef(command_p);

    int status = system(command_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SM_DIAGNOSTIC_END(SM_ERROR_GCC_EXECUTION_FAILED)}

    chdir(sm_getVariable(&Runtime_p->VariableArray, "WRK_DIR")->values_pp[0]);

    free(command_p);
    free(sources_p);

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// CREATE LIBRARY ==================================================================================

static SM_RESULT sm_createSharedLibraryUsingGCC(
    const SM_BYTE *objects_p, const SM_BYTE *out_p, SM_BYTE *compile_p, const SM_BYTE *link_p)
{
SM_BEGIN()

    static SM_BYTE command_p[16384] = {'\0'};
    sprintf(command_p, "gcc -shared %s %s %s -o %s", objects_p, compile_p, link_p, out_p);

    sm_messagef(command_p);

    int status = system(command_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SM_DIAGNOSTIC_END(SM_ERROR_GCC_EXECUTION_FAILED)}

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

static SM_RESULT sm_createLibrary(
    sm_Runtime *Runtime_p, sm_SourceContext *Context_p)
{
SM_BEGIN()

    SM_BYTE empty = 0;
    SM_BYTE *libName_p = Context_p->name_p;
    SM_BYTE *extra_p = Context_p->linkArgs_p ? Context_p->linkArgs_p : &empty;

    int major = Context_p->major; 
    int minor = Context_p->minor; 
    int patch = Context_p->patch;

    SM_BYTE tmp_p[255] = {0};
    sprintf(tmp_p, "/tmp/%s", sm_getVariable(&Runtime_p->VariableArray, "NAME")->values_pp[0]);
    chdir(tmp_p);

#ifdef __unix__

    int index = 0;
    SM_BYTE objects_p[16384] = {'\0'};

    DIR *dir;
    struct dirent *ent;
    
    if ((dir = opendir(tmp_p)) != NULL) 
    {
        while ((ent = readdir(dir)) != NULL) {
            for (int i = 0; ent->d_name[i] != '\0' && ent->d_name[0] != '.'; ++i) {
                objects_p[index++] = ent->d_name[i];
            }
            objects_p[index++] = ' ';
        }
        closedir(dir);
    } 
    else {SM_DIAGNOSTIC_END(SM_ERROR_CANT_OPEN_DIR)}

    SM_BYTE libPath_p[256] = {'\0'};
    sprintf(libPath_p, "%s/lib%s.so.%d.%d.%d", sm_getVariable(&Runtime_p->VariableArray, "LIB_DEST")->values_pp[0], libName_p, major, minor, patch);
    SM_BYTE symPath1_p[256] = {'\0'};
    sprintf(symPath1_p, "%s/lib%s.so.%d", sm_getVariable(&Runtime_p->VariableArray, "LIB_DEST")->values_pp[0], libName_p, major);
    SM_BYTE symPath2_p[256] = {'\0'};
    sprintf(symPath2_p, "%s/lib%s.so", sm_getVariable(&Runtime_p->VariableArray, "LIB_DEST")->values_pp[0], libName_p);
  
    SM_CHECK(sm_createSharedLibraryUsingGCC(objects_p, libPath_p, Context_p->compileArgs_p, extra_p))

    memset(libPath_p, 0, 256); 
    sprintf(libPath_p, "lib%s.so.%d.%d.%d", libName_p, major, minor, patch);
    sm_createSymLink(libPath_p, symPath1_p, SM_FALSE);
    sm_createSymLink(libPath_p, symPath2_p, SM_FALSE);

#endif 

    chdir(sm_getVariable(&Runtime_p->VariableArray, "WRK_DIR")->values_pp[0]);

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// COMPILE =========================================================================================

static void sm_getObjectFileName(
    sm_Source *Source_p, SM_BYTE *name_p)
{
SM_BEGIN()

    SM_BYTE path_p[255] = {0};
    strcpy(path_p, Source_p->path_p);

    int index = strlen(path_p) - 1;
    while (path_p[index] != '/' && index) {index--;}

    SM_BYTE fileName_p[255] = {0};
    strcpy(fileName_p, index ? path_p + index + 1 : path_p);

    int tmp = index ? index - 1 : 0;
    while (path_p[tmp] != '/' && tmp) {tmp--;}

    if (index) {
        path_p[index] = '\0';
        sprintf(name_p, "%s%s.o", path_p + tmp, fileName_p);
    }
    else {
        sprintf(name_p, "%s.o", fileName_p);
    }

SM_SILENT_END()
}

static SM_RESULT sm_createPICObjectFileUsingGCC(
    const SM_BYTE *in_p, const SM_BYTE *out_p, SM_BYTE *extra_p)
{
SM_BEGIN()

    SM_BYTE realout_p[1024] = {'\0'};
    realpath(out_p, realout_p);

    SM_BYTE command_p[1024] = {'\0'};

    if (extra_p != NULL) {sprintf(command_p, "gcc -fPIC %s -c %s -o %s", extra_p, in_p, out_p);}
    else {sprintf(command_p, "gcc -fPIC -c %s -o %s", in_p, out_p);}

    sm_messagef(command_p);

    int status = system(command_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SM_DIAGNOSTIC_END(SM_ERROR_GCC_EXECUTION_FAILED)}

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

static SM_RESULT sm_compileFiles(
    sm_Runtime *Runtime_p, sm_SourceContext *Context_p)
{
SM_BEGIN()

    chdir(Context_p->path_p);

    for (int i = 0; i < Runtime_p->SourceArray.length; ++i) 
    {
        sm_Source *Source_p = &Runtime_p->SourceArray.Sources_p[i];

        if (Source_p->Context_p == Context_p) 
        {
            SM_BYTE tmp_p[255] = {0};
            SM_BYTE fileName_p[255] = {0};
            sm_getObjectFileName(Source_p, fileName_p);

            sprintf(tmp_p, "/tmp/%s/%s", sm_getVariable(&Runtime_p->VariableArray, "NAME")->values_pp[0], fileName_p);
            SM_BYTE empty = 0;

            if (Context_p->type == SM_SOURCE_CONTEXT_SHARED_LIBRARY) {
                SM_CHECK(sm_createPICObjectFileUsingGCC(
                        Source_p->path_p, tmp_p, Context_p->compileArgs_p ? Context_p->compileArgs_p : &empty
                ))
            }
        }
    }

    chdir(sm_getVariable(&Runtime_p->VariableArray, "WRK_DIR")->values_pp[0]);

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// INSTALL =========================================================================================

//static SM_RESULT sm_installLibrary(
//    SM_BYTE *libName_p, int major, int minor, int patch)
//{
//SM_BEGIN()
//
//    SM_BYTE wrkDir_p[2048] = {'\0'};
//    SM_CHECK_NULL(SM_ERROR_GET_WORK_DIRECTORY, getcwd(wrkDir_p, 2048))
//
//    SM_BYTE projDir_p[2048] = {'\0'};
//    SM_CHECK(SM_ERROR_GET_PROJECT_DIRECTORY, sm_getProjectDir(projDir_p, 2048))
//
//    chdir(projDir_p);
//
//    SM_BYTE libPath_p[512] = {'\0'};
//    sprintf(libPath_p, "lib/lib%s.so.%d.%d.%d", libName_p, major, minor, patch);
//
//    SM_CHECK(SM_ERROR_COPY_FAILED, sm_copy(libPath_p, "/usr/local/lib", SM_FALSE, SM_TRUE))
//
//    chdir("/usr/local/lib");
//
//    SM_BYTE cpyPath_p[256] = {'\0'};
//    sprintf(cpyPath_p, "lib%s.so.%d.%d.%d", libName_p, major, minor, patch);
//    SM_BYTE symPath1_p[256] = {'\0'};
//    sprintf(symPath1_p, "lib%s.so.%d", libName_p, major);
//    SM_BYTE symPath2_p[256] = {'\0'};
//    sprintf(symPath2_p, "lib%s.so", libName_p);
//
//    sm_createSymLink(cpyPath_p, symPath1_p, SM_TRUE);
//    sm_createSymLink(cpyPath_p, symPath2_p, SM_TRUE);
//
//    chdir(wrkDir_p);
//
//SM_DIAGNOSTIC_END(SM_SUCCESS)
//}

// BUILD ===========================================================================================

static SM_RESULT sm_buildSourceContext(
    sm_Runtime *Runtime_p, sm_SourceContext *Context_p) 
{
SM_BEGIN()

    SM_BYTE empty = 0;
    SM_BYTE tmp_p[255];
    sprintf(tmp_p, "/tmp/%s", sm_getVariable(&Runtime_p->VariableArray, "NAME")->values_pp[0]);

    SM_CHECK(sm_createDir(tmp_p))

    if (Runtime_p->beforeBuildCallback_f) {
        SM_CHECK(Runtime_p->beforeBuildCallback_f(Runtime_p, Context_p))
    }

    if (Context_p->type == SM_SOURCE_CONTEXT_BINARY) {
        SM_CHECK(sm_createBinary(Runtime_p, Context_p))
    }
    else {
        SM_CHECK(sm_compileFiles(Runtime_p, Context_p))
        SM_CHECK(sm_createLibrary(Runtime_p, Context_p))
    }

    if (Runtime_p->afterBuildCallback_f) {
        SM_CHECK(Runtime_p->afterBuildCallback_f(Runtime_p, Context_p))
    }

//    if (install) {
//        SM_CHECK(SM_ERROR_BAD_STATE, sm_installLibrary(
//            Library_p->name_p, 0, 0, 0 
//        ))
//    }

    SM_CHECK(sm_removeDir(tmp_p))

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_RESULT sm_build(
    sm_Runtime *Runtime_p, SM_BYTE *name_p)
{
SM_BEGIN()

    for (int i = 0; i < Runtime_p->SourceContextArray.length; ++i) 
    {
        sm_SourceContext *SourceContext_p = &Runtime_p->SourceContextArray.SourceContexts_p[i];
        if (!strcmp(SourceContext_p->name_p, name_p)) {
            SM_CHECK(sm_buildSourceContext(Runtime_p, SourceContext_p))
        }
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

