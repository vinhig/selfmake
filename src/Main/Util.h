#ifndef SELFMAKE_UTIL_H
#define SELFMAKE_UTIL_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "../Common/Result.h"
#include "../Common/API.h"

#include <stddef.h>

#endif

/** @addtogroup nhmakeMacros
 *  @{
 */

    #define SELFMAKE_PROJECT_DIR_LENGTH 1024

/** @} */

/** @addtogroup nhmakeVars
 *  @{
 */

    extern NH_BYTE SELFMAKE_PROJECT_DIR_P[SELFMAKE_PROJECT_DIR_LENGTH];

/** @} */

/** @addtogroup nhmakeFunctions
 *  @{
 */

    SELFMAKE_RESULT selfmake_getExeDir(
        char *buffer_p, size_t size
    );

    SELFMAKE_RESULT selfmake_getProjectDir(
        char *buffer_p, size_t size
    );

    const char *selfmake_getHomeDir(
    );

    SELFMAKE_RESULT selfmake_createDir(
        const char *path_p
    );

    NH_BOOL selfmake_canRunCommand(
        const char *cmd
    );

    NH_BOOL selfmake_canFindSharedLib(
        const char *lib_p
    );

    SELFMAKE_RESULT selfmake_createPICObjectFileUsingGCC(
        const char *in_p, const char *out_p, char *extra_p
    );

    SELFMAKE_RESULT selfmake_createSharedLibraryUsingGCC(
        const char *objects_p, const char *out_p, const char *extra_p
    );

    SELFMAKE_RESULT selfmake_copy(
        char *in_p, char *out_p, NH_BOOL recursive, NH_BOOL sudo
    );

    SELFMAKE_RESULT selfmake_createSymLink(
        char *filePath_p, char *symLinkPath_p, NH_BOOL sudo
    );

    NH_BOOL selfmake_fileExists(
        char *filename_p
    );

    void *selfmake_getFileData(
        const char* path_p, long *size_p
    );

    SELFMAKE_RESULT selfmake_writeCharsToFile(
        char *filename_p, char *chars_p
    );

/** @} */

#endif
