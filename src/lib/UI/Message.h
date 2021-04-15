#ifndef SM_MESSAGE_H
#define SM_MESSAGE_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../Common/Result.h"
#include "../Common/Types.h"

#endif

/** @addtogroup selfmakeFunctions
 *  @{
 */

    SM_RESULT sm_operationf(
        SM_BYTE *format_p, ...
    );
    
    SM_RESULT sm_messagef(
        SM_BYTE *format_p, ...
    );

    SM_RESULT sm_noticef(
        SM_BYTE *format_p, ...
    );

    SM_RESULT sm_externalMessage(
        SM_BYTE *prepend_p, SM_BYTE *message_p
    );

    SM_RESULT sm_exitMessage(
        SM_RESULT result
    );

/** @} */

#endif
