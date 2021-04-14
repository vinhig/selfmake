// LICENSE NOTICE ==================================================================================

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

// INCLUDE =========================================================================================

#include "Util.h"

#include "../UI/Message.h"
#include "../Common/Macro.h"

#include SELFMAKE_FLOW
#include SELFMAKE_DEFAULT_CHECK

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <pwd.h>

#ifdef __unix__
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

// VARS ============================================================================================

NH_BYTE SELFMAKE_PROJECT_DIR_P[SELFMAKE_PROJECT_DIR_LENGTH];

// DIRECTORY =======================================================================================

SELFMAKE_RESULT selfmake_getExeDir(
    NH_BYTE *buffer_p, size_t size)
{
SELFMAKE_BEGIN()

    if (readlink("/proc/self/exe", buffer_p, 1024) == -1 
    &&  readlink("/proc/curproc/file", buffer_p, 1024) == -1
    &&  readlink("/proc/self/path/a.out", buffer_p, 1024) == -1) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

    int i;
    for (i = strlen(buffer_p); i > -1 && buffer_p[i] != '/'; --i) {}
    if (i == -1) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}
    buffer_p[i] = '\0'; // remove exe name

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_getProjectDir(
    NH_BYTE *buffer_p, size_t size)
{
SELFMAKE_BEGIN()

    if (selfmake_getExeDir(buffer_p, size) != SELFMAKE_SUCCESS) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

    int i;
    for (i = strlen(buffer_p); i > -1 && buffer_p[i] != '/'; --i) {}
    if (i == -1) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}
    buffer_p[i] = '\0';

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

const NH_BYTE *selfmake_getHomeDir()
{
SELFMAKE_BEGIN()

    const NH_BYTE *homedir_p = NULL;
    if ((homedir_p = getenv("HOME")) == NULL) {
        homedir_p = getpwuid(getuid())->pw_dir;
    }

SELFMAKE_END(homedir_p)
}

SELFMAKE_RESULT selfmake_createDir(
    const NH_BYTE *path_p)
{
SELFMAKE_BEGIN()

    NH_BYTE wrkDir_p[2048] = {'\0'};
    getcwd(wrkDir_p, 2048);

    selfmake_messagef("CREATE DIR %s/%s", wrkDir_p, path_p);
 
    struct stat st = {0};
    if (stat(path_p, &st) == -1) {
        int err = mkdir(path_p, 0755);  // 0755 aka drwxr-xr-x
        if (err != 0) {
            SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_CANT_CREATE_DIRECTORY)
        }
    }

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

// COMMAND =========================================================================================

NH_BOOL selfmake_canRunCommand(
    const NH_BYTE *cmd) 
{
SELFMAKE_BEGIN()

    if(strchr(cmd, '/')) {
        // if cmd includes a slash, no path search must be performed,
        // go straight to checking if it's executable
        return access(cmd, X_OK)==0;
    }
    const NH_BYTE *path = getenv("PATH");
    if(!path) {SELFMAKE_DIAGNOSTIC_END(NH_FALSE)} // something is horribly wrong...
    // we are sure we won't need a buffer any longer
    NH_BYTE *buf = malloc(strlen(path)+strlen(cmd)+3);
    if(!buf) {SELFMAKE_DIAGNOSTIC_END(NH_FALSE)} // actually useless, see comment
    // loop as long as we have stuff to examine in path
    for(; *path; ++path) {
        // start from the beginning of the buffer
        NH_BYTE *p = buf;
        // copy in buf the current path element
        for(; *path && *path!=':'; ++path,++p) {
            *p = *path;
        }
        // empty path entries are treated like "."
        if(p==buf) *p++='.';
        // slash and command name
        if(p[-1]!='/') *p++='/';
        strcpy(p, cmd);
        // check if we can execute it
        if(access(buf, X_OK)==0) {
            free(buf);
            SELFMAKE_DIAGNOSTIC_END(NH_TRUE)
        }
        // quit at last cycle
        if(!*path) break;
    }
    // not found
    free(buf);

SELFMAKE_DIAGNOSTIC_END(NH_FALSE)
}

// FILE ============================================================================================

NH_BOOL selfmake_canFindSharedLib(
    const NH_BYTE *lib_p)
{
SELFMAKE_BEGIN()

    NH_BYTE path_p[1024] = {'\0'};
    sprintf(path_p, "/usr/lib/lib%s.so", lib_p);

    if(access(path_p, F_OK) != -1) {SELFMAKE_DIAGNOSTIC_END(NH_TRUE)}

SELFMAKE_DIAGNOSTIC_END(NH_FALSE)
}

SELFMAKE_RESULT selfmake_createPICObjectFileUsingGCC(
    const NH_BYTE *in_p, const NH_BYTE *out_p, NH_BYTE *extra_p)
{
SELFMAKE_BEGIN()

    NH_BYTE realout_p[1024] = {'\0'};
    realpath(out_p, realout_p);
    selfmake_messagef("CREATE OBJECT FILE %s", realout_p);

    NH_BYTE command_p[1024] = {'\0'};

    if (extra_p != NULL) {sprintf(command_p, "gcc -std=gnu99 -fPIC %s -c %s -o %s", extra_p, in_p, out_p);}
    else {sprintf(command_p, "gcc -std=gnu99 -fPIC -c %s -o %s", in_p, out_p);}

    int status = system(command_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_GCC_EXECUTION_FAILED)}

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_createSharedLibraryUsingGCC(
    const NH_BYTE *objects_p, const NH_BYTE *out_p, const NH_BYTE *extra_p)
{
SELFMAKE_BEGIN()

    static NH_BYTE command_p[16384] = {'\0'};
    sprintf(command_p, "gcc -std=gnu99 -shared %s %s -o %s", objects_p, extra_p, out_p);

    int status = system(command_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_GCC_EXECUTION_FAILED)}

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_copy(
    NH_BYTE *in_p, NH_BYTE *out_p, NH_BOOL recursive, NH_BOOL sudo)
{
SELFMAKE_BEGIN()

    NH_BYTE realin_p[1024] = {'\0'}, realout_p[1024] = {'\0'};
    realpath(out_p, realout_p);
    realpath(in_p, realin_p);
    selfmake_messagef(sudo ? "COPY %s TO %s \e[1;31mSUDO PREPENDED\e[0m" : "COPY %s TO %s", realin_p, realout_p);

#include SELFMAKE_CUSTOM_CHECK

    NH_BYTE projDir_p[2048] = {'\0'};
    SELFMAKE_CHECK(SELFMAKE_ERROR_GET_PROJECT_DIRECTORY, selfmake_getProjectDir(projDir_p, 2048))

#include SELFMAKE_DEFAULT_CHECK

    if (!selfmake_canRunCommand("cp")) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_CP_NOT_FOUND)}

#ifdef __unix__

    NH_BYTE command_p[32] = {'\0'};
    sprintf(command_p, sudo ? "sudo cp" : "cp");
    NH_BYTE fullCommand_p[1024] = {'\0'};

    if (out_p[0] != '/') {
        sprintf(fullCommand_p, recursive ? "%s -rp %s/%s %s/%s" : "%s -p %s/%s %s/%s", command_p, projDir_p, in_p, projDir_p, out_p);
    }
    else {
        sprintf(fullCommand_p, recursive ? "%s -rp %s/%s %s" : "%s -p %s/%s %s", command_p, projDir_p, in_p, out_p);
    }

    int status = system(fullCommand_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_CP_EXECUTION_FAILED)}

#elif defined(WIN_32)

#endif

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_createSymLink(
    NH_BYTE *filePath_p, NH_BYTE *symLinkPath_p, NH_BOOL sudo)
{
SELFMAKE_BEGIN()

#ifdef __unix__

    if (!selfmake_canRunCommand("ln")) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_CP_NOT_FOUND)}

    NH_BYTE command_p[64] = {'\0'};
    sprintf(command_p, sudo ? "sudo ln -s" : "ln -s");

    NH_BYTE fullCommand_p[1024] = {'\0'};
    sprintf(fullCommand_p, "%s %s %s", command_p, filePath_p, symLinkPath_p);

    int status = system(fullCommand_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_CP_EXECUTION_FAILED)}

#elif defined(WIN_32)

#endif

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

void *selfmake_getFileData(
    const NH_BYTE* path_p, long *size_p)
{
SELFMAKE_BEGIN()

    FILE *fh = fopen(path_p, "rb");
    if (fh == NULL) {SELFMAKE_END(NULL)}
    
    if (fseek(fh, 0L, SEEK_END) != 0) {SELFMAKE_END(NULL)}

    long size = ftell(fh);

    if (fseek(fh, 0L, SEEK_SET) != 0) {SELFMAKE_END(NULL)}

    if(size <= 0) {
        fclose(fh);
        SELFMAKE_END(NULL)
    }

    NH_BYTE *data_p = (NH_BYTE*)malloc(((size_t)size) + 1); 
    if (data_p == NULL) {SELFMAKE_END(NULL)}
    
    fread(data_p, 1, size, fh);
    fclose(fh);

    data_p[size] = 0;
    if (size_p != NULL) {*size_p = size;}

SELFMAKE_END(data_p)
}

NH_BOOL selfmake_fileExists(
    NH_BYTE *filename_p)
{
SELFMAKE_BEGIN()
SELFMAKE_END(access(filename_p, F_OK) != -1 ? NH_TRUE : NH_FALSE)
}

SELFMAKE_RESULT selfmake_writeCharsToFile(
    NH_BYTE *filename_p, NH_BYTE *NH_BYTEs_p)
{
SELFMAKE_BEGIN()

    SELFMAKE_CHECK_NULL(NH_BYTEs_p)
    SELFMAKE_CHECK_NULL(filename_p)

    FILE *f = fopen(filename_p, "w");
    SELFMAKE_CHECK_NULL(f)    
    
    fprintf(f, "%s\n", NH_BYTEs_p);
    fclose(f);

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

