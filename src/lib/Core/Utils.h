#ifndef SM_UTILS_H
#define SM_UTILS_H

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

    const SM_BYTE *sm_getHomeDir(
    );

    SM_RESULT sm_createDir(
        SM_BYTE *path_p
    );
    
    SM_RESULT sm_removeDir(
        SM_BYTE *path_p
    );

    SM_BOOL sm_canRunCommand(
        const SM_BYTE *cmd
    );

/** @} */

#endif
