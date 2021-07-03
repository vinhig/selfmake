// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Library.h"

#include "../Common/Types.h"
#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#ifdef __unix__
    #include <sys/wait.h>
#endif

// TEST ============================================================================================

void *sm_openLibrary(
    SM_BYTE *path_p)
{
#ifdef __unix__

    SM_BYTE *error_p;
    dlerror();

    void *dl_p = dlopen(path_p, RTLD_NOW | RTLD_GLOBAL);

    if ((error_p = dlerror()) != NULL) { 
        fprintf(stderr, "%s\n", error_p);
        return NULL;
    }

    return dl_p;

#endif
}

void sm_closeLibrary(
    void *lib_p)
{
#ifdef __unix__

    dlclose(lib_p);

#endif
}

SM_RESULT sm_getExeDir(
    SM_BYTE *buffer_p, size_t size)
{
#ifdef __unix__

    if (readlink("/proc/self/exe", buffer_p, 1024) == -1 
    &&  readlink("/proc/curproc/file", buffer_p, 1024) == -1
    &&  readlink("/proc/self/path/a.out", buffer_p, 1024) == -1) {return SM_ERROR_BAD_STATE;}

    int i;
    for (i = strlen(buffer_p); i > -1 && buffer_p[i] != '/'; --i) {}
    if (i == -1) {return SM_ERROR_BAD_STATE;}

    buffer_p[i] = '\0'; // remove exe name
    return SM_SUCCESS;

#endif
}

void *sm_loadSymbol(
    void *lib_p, const SM_BYTE *name_p)
{
#ifdef __unix__

    SM_BYTE *error_p;
    dlerror(); // clear any existing error

    void *function_p = dlsym(lib_p, name_p); 
    if ((error_p = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error_p);
        return NULL;
    }

    return function_p;

#endif
}

