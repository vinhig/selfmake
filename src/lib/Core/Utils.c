// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

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

// DIRECTORY =======================================================================================

SM_RESULT sm_getProcessDirectory(
    SM_BYTE *buffer_p, unsigned int size)
{
SM_BEGIN()

    if (readlink("/proc/self/exe", buffer_p, size) == -1 
    &&  readlink("/proc/curproc/file", buffer_p, size) == -1
    &&  readlink("/proc/self/path/a.out", buffer_p, size) == -1) {SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)}

    int i;
    for (i = strlen(buffer_p); i > -1 && buffer_p[i] != '/'; --i) {}
    if (i == -1) {SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)}
    buffer_p[i] = '\0'; // remove exe name

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

const SM_BYTE *sm_getHomeDir()
{
SM_BEGIN()

    const SM_BYTE *homedir_p = NULL;
    if ((homedir_p = getenv("HOME")) == NULL) {
        homedir_p = getpwuid(getuid())->pw_dir;
    }

SM_END(homedir_p)
}

SM_RESULT sm_createDir(
    SM_BYTE *path_p)
{
SM_BEGIN()

    sm_messagef("CREATE DIR %s", path_p);
 
    struct stat st = {0};
    if (stat(path_p, &st) == -1) {
        int err = mkdir(path_p, 0755);  // 0755 aka drwxr-xr-x
        if (err) {
            SM_DIAGNOSTIC_END(SM_ERROR_CANT_CREATE_DIRECTORY)
        }
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_RESULT sm_removeDir(
    SM_BYTE *path_p)
{
SM_BEGIN()

    sm_messagef("REMOVE DIR %s", path_p);
 
    int err = rmdir(path_p);
    if (err) {
        SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// COMMAND =========================================================================================

SM_BOOL sm_canRunCommand(
    const SM_BYTE *cmd) 
{
SM_BEGIN()

    if(strchr(cmd, '/')) {
        // if cmd includes a slash, no path search must be performed,
        // go straight to checking if it's executable
        return access(cmd, X_OK)==0;
    }
    const SM_BYTE *path = getenv("PATH");
    if(!path) {SM_DIAGNOSTIC_END(SM_FALSE)} // something is horribly wrong...
    // we are sure we won't need a buffer any longer
    SM_BYTE *buf = malloc(strlen(path)+strlen(cmd)+3);
    if(!buf) {SM_DIAGNOSTIC_END(SM_FALSE)} // actually useless, see comment
    // loop as long as we have stuff to examine in path
    for(; *path; ++path) {
        // start from the beginning of the buffer
        SM_BYTE *p = buf;
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
            SM_DIAGNOSTIC_END(SM_TRUE)
        }
        // quit at last cycle
        if(!*path) break;
    }
    // not found
    free(buf);

SM_DIAGNOSTIC_END(SM_FALSE)
}

