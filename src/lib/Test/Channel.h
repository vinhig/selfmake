#ifndef SM_CHANNEL_H
#define SM_CHANNEL_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../Common/Result.h"

#include <stddef.h>

#endif

/** @addtogroup smcoreStructs
 *  @{
 */

    // TODO semaphore etc.
    typedef struct sm_Channel {
#if defined(__linux__) || defined(__APPLE__)
        int rw_p[2];
#endif
    } sm_Channel;

/** @} */

/** @addtogroup smcoreFunctions
 *  @{
 */

    SM_RESULT sm_initChannel(
        sm_Channel *Channel_p
    );
    
    SM_RESULT sm_openChannel(
        sm_Channel *Channel_p
    );
    
    void sm_closeChannelReadAccess(
        sm_Channel *Channel_p
    );
    
    void sm_closeChannelWriteAccess(
        sm_Channel *Channel_p
    );
    
    int sm_writeToChannel(
        sm_Channel *Channel_p, char *bytes_p, int byteCount
    );
    
    char *sm_readFromChannel(
        sm_Channel *Channel_p, size_t *size_p
    );

/** @} */

#endif 
