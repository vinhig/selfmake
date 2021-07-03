#ifndef SM_LIBRARY_H
#define SM_LIBRARY_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../Common/Result.h"
#include "../Common/Types.h"

#include <stddef.h>

#endif

/** @addtogroup selfmakeFunctions
 *  @{
 */

    void *sm_openLibrary(
        SM_BYTE *path_p
    );
    
    void sm_closeLibrary(
        void *lib_p
    );
    
    SM_RESULT sm_getExeDir(
        SM_BYTE *buffer_p, size_t size
    );
    
    void *sm_loadSymbol(
        void *lib_p, const SM_BYTE *name_p
    );

/** @} */

#endif
