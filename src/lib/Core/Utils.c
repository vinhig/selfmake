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
#include SM_CUSTOM_CHECK

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>
#include <pwd.h>

#if defined(__linux__) || defined(__APPLE__)
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

#if defined(__APPLE__)
    #include <mach-o/dyld.h>
#endif

// DIRECTORY =======================================================================================

static SM_BYTE *procDir_p = NULL;
static SM_BYTE *wrkDir_p = NULL;

SM_BYTE *sm_getProcessDirectory()
{
SM_BEGIN()
    if (procDir_p) {SM_END(procDir_p)}

    int size = 1024;
    SM_BYTE buffer_p[1024] = {0};

#if defined(__linux__)
    if (readlink("/proc/self/exe", buffer_p, size) == -1 
    &&  readlink("/proc/curproc/file", buffer_p, size) == -1
    &&  readlink("/proc/self/path/a.out", buffer_p, size) == -1) {SM_END(NULL)}
#elif defined(__APPLE__)
    size = 0;
    _NSGetExecutablePath(NULL, &size);
    _NSGetExecutablePath(buffer_p, &size);
#endif

    int i;
    for (i = strlen(buffer_p); i > -1 && buffer_p[i] != '/'; --i) {}
    if (i == -1) {SM_END(NULL)}
    buffer_p[i] = '\0'; // remove exe name

    procDir_p = malloc(strlen(buffer_p) + 1);
    SM_CHECK_NULL(NULL, procDir_p)
    sprintf(procDir_p, buffer_p);

SM_END(procDir_p)
}

SM_BYTE *sm_getWorkDirectory()
{
SM_BEGIN()

    if (wrkDir_p) {SM_END(wrkDir_p)}

    int size = 1024;
    SM_BYTE buffer_p[1024] = {0};
    getcwd(buffer_p, size);

    wrkDir_p = malloc(strlen(buffer_p) + 1);
    SM_CHECK_NULL(NULL, wrkDir_p)
    sprintf(wrkDir_p, buffer_p);

SM_END(wrkDir_p)
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
 
    SM_BYTE command_p[255];
    sprintf(command_p, "rm -r %s", path_p);

    int status = system(command_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SM_DIAGNOSTIC_END(SM_ERROR_CP_EXECUTION_FAILED)}

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

