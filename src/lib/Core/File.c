// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "File.h"
#include "Utils.h"

#include "../UI/Message.h"
#include "../Common/Macros/Macros.h"

#include SM_FLOW
#include SM_DEFAULT_CHECK

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

// FILE ============================================================================================

SM_BOOL sm_canFindSharedLib(
    const SM_BYTE *lib_p)
{
SM_BEGIN()

    SM_BYTE path_p[1024] = {'\0'};
    sprintf(path_p, "/usr/lib/lib%s.so", lib_p);

    if(access(path_p, F_OK) != -1) {SM_DIAGNOSTIC_END(SM_TRUE)}

SM_DIAGNOSTIC_END(SM_FALSE)
}

SM_RESULT sm_copy(
    sm_VariableArray *Array_p, SM_BYTE *in_p, SM_BYTE *out_p, SM_BOOL recursive, SM_BOOL sudo)
{
SM_BEGIN()

    SM_BYTE realin_p[1024] = {'\0'}, realout_p[1024] = {'\0'};
    realpath(out_p, realout_p);
    realpath(in_p, realin_p);
    sm_messagef(sudo ? "COPY %s TO %s \e[1;31mSUDO PREPENDED\e[0m" : "COPY %s TO %s", realin_p, realout_p);

    if (!sm_canRunCommand("cp")) {SM_DIAGNOSTIC_END(SM_ERROR_CP_NOT_FOUND)}

#ifdef __unix__

    SM_BYTE command_p[32] = {'\0'};
    sprintf(command_p, sudo ? "sudo cp" : "cp");
    SM_BYTE fullCommand_p[1024] = {'\0'};

    if (out_p[0] != '/') {
        sprintf(fullCommand_p, recursive ? "%s -rp %s/%s %s/%s" : "%s -p %s/%s %s/%s", command_p, sm_getVariable(Array_p, "PROJ_DIR")->values_pp[0], in_p, sm_getVariable(Array_p, "PROJ_DIR")->values_pp[0], out_p);
    }
    else {
        sprintf(fullCommand_p, recursive ? "%s -rp %s/%s %s" : "%s -p %s/%s %s", command_p, sm_getVariable(Array_p, "PROJ_DIR")->values_pp[0], in_p, out_p);
    }

    int status = system(fullCommand_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SM_DIAGNOSTIC_END(SM_ERROR_CP_EXECUTION_FAILED)}

#elif defined(WIN_32)

#endif

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_RESULT sm_createSymLink(
    SM_BYTE *filePath_p, SM_BYTE *symLinkPath_p, SM_BOOL sudo)
{
SM_BEGIN()

#ifdef __unix__

    if (!sm_canRunCommand("ln")) {SM_DIAGNOSTIC_END(SM_ERROR_CP_NOT_FOUND)}

    SM_BYTE command_p[64] = {'\0'};
    sprintf(command_p, sudo ? "sudo ln -s" : "ln -s");

    SM_BYTE fullCommand_p[1024] = {'\0'};
    sprintf(fullCommand_p, "%s %s %s", command_p, filePath_p, symLinkPath_p);

    int status = system(fullCommand_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SM_DIAGNOSTIC_END(SM_ERROR_CP_EXECUTION_FAILED)}

#elif defined(WIN_32)

#endif

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_BOOL sm_fileExists(
    SM_BYTE *filename_p)
{
SM_BEGIN()
SM_END(access(filename_p, F_OK) != -1 ? SM_TRUE : SM_FALSE)
}

SM_RESULT sm_writeBytesToFile(
    SM_BYTE *filename_p, SM_BYTE *bytes_p)
{
SM_BEGIN()

    SM_CHECK_NULL(bytes_p)
    SM_CHECK_NULL(filename_p)

    FILE *f = fopen(filename_p, "w");
    SM_CHECK_NULL(f)    
    
    fprintf(f, "%s\n", bytes_p);
    fclose(f);

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// FILE ARRAY ======================================================================================

SM_BYTE *sm_getFileData(
    const SM_BYTE* path_p, long *size_p)
{
SM_BEGIN()

    FILE *fh = fopen(path_p, "rb");
    if (fh == NULL) {SM_END(NULL)}
    
    if (fseek(fh, 0, SEEK_END) != 0) {SM_END(NULL)}
    long size = ftell(fh);
    rewind(fh);

    if(size <= 0) {
        fclose(fh);
        SM_END(NULL)
    }

    SM_BYTE *data_p = malloc(size + 1); 
    if (data_p == NULL) {SM_END(NULL)}
    
    fread(data_p, 1, size, fh);
    fclose(fh);

    data_p[size] = 0;
    if (size_p != NULL) {*size_p = size;}

SM_END(data_p)
}

void sm_initFileArray(
    sm_FileArray *Array_p)
{
SM_BEGIN()

    Array_p->length = 0;
    Array_p->Files_p = NULL;

SM_SILENT_END()
}

SM_RESULT sm_appendFile(
    sm_FileArray *Array_p, SM_BYTE *path_p)
{
SM_BEGIN()

    SM_BYTE *data_p = sm_getFileData(path_p, NULL);
    SM_CHECK_NULL(data_p)

    if (!Array_p->length) {
        Array_p->Files_p = malloc(sizeof(sm_File));
        SM_CHECK_NULL(Array_p->Files_p)       
    }
    else {
        Array_p->Files_p = realloc(Array_p->Files_p, sizeof(sm_File) * (Array_p->length + 1));
        SM_CHECK_NULL(Array_p->Files_p)       
    }

    Array_p->Files_p[Array_p->length].data_p = data_p;
    Array_p->Files_p[Array_p->length].path_p = malloc(strlen(path_p) + 1);
    SM_CHECK_NULL(Array_p->Files_p[Array_p->length].path_p)
    strcpy(Array_p->Files_p[Array_p->length].path_p, path_p);

    Array_p->length++;

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

