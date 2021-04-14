#ifndef SELFMAKE_MESSAGE_H
#define SELFMAKE_MESSAGE_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "../Common/Result.h"
#include "../Common/API.h"

#endif

/** @addtogroup nhmakeVars
 *  @{
 */

    extern NH_BOOL SELFMAKE_QUIET;

/** @} */

/** @addtogroup nhmakeFunctions
 *  @{
 */

    SELFMAKE_RESULT selfmake_printVersion(
    );

    SELFMAKE_RESULT selfmake_printHelp(
    );

    SELFMAKE_RESULT selfmake_operationf(
        char *format_p, ...
    );
    
    SELFMAKE_RESULT selfmake_messagef(
        char *format_p, ...
    );

    SELFMAKE_RESULT selfmake_noticef(
        char *format_p, ...
    );

    SELFMAKE_RESULT selfmake_externalMessage(
        char *prepend_p, char *message_p
    );

    SELFMAKE_RESULT selfmake_exitMessage(
        SELFMAKE_RESULT result
    );

/** @} */

#endif
