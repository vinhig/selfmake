#ifndef SM_FILE_H
#define SM_FILE_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../Parser/Variables.h"

#include "../Common/Result.h"
#include "../Common/Types.h"

#include <stddef.h>

#endif

/** @addtogroup selfmakeStructs
 *  @{
 */

    typedef struct sm_File {
        SM_BYTE *path_p;
        SM_BYTE *data_p;
    } sm_File;

    typedef struct sm_FileArray {
        int length;
        sm_File *Files_p;
    } sm_FileArray;

/** @} */

/** @addtogroup selfmakeFunctions
 *  @{
 */

    SM_BOOL sm_canFindSharedLib(
        const SM_BYTE *lib_p
    );

    SM_RESULT sm_copy(
        sm_VariableArray *Array_p, SM_BYTE *in_p, SM_BYTE *out_p, SM_BOOL recursive, SM_BOOL sudo
    );

    SM_RESULT sm_createSymLink(
        SM_BYTE *filePath_p, SM_BYTE *symLinkPath_p, SM_BOOL sudo
    );

    SM_BOOL sm_fileExists(
        SM_BYTE *filename_p
    );

    SM_RESULT sm_writeCharsToFile(
        SM_BYTE *filename_p, SM_BYTE *SM_BYTEs_p
    );

    void sm_initFileArray(
        sm_FileArray *Array_p
    );
    
    SM_RESULT sm_appendFile(
        sm_FileArray *Array_p, SM_BYTE *path_p
    );

/** @} */

#endif
