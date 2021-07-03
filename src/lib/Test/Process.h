#ifndef SM_PROCESS_H
#define SM_PROCESS_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "Channel.h"

#include "../Common/Result.h"
#include "../Common/Types.h"

#include <sys/types.h> 

#endif

/** @addtogroup selfmakeStructs
 *  @{
 */

#ifdef __unix__
    typedef pid_t SM_PROCESS;
#endif

    typedef struct sm_ProcessIPC {
        sm_Channel In;
        sm_Channel Out;
    } sm_ProcessIPC;

    typedef struct sm_Process {
        SM_PROCESS id;
        sm_ProcessIPC IPC;
    } sm_Process;

/** @} */

/** @addtogroup selfmakeFunctions
 *  @{
 */

    sm_Process sm_fork(
    );

/** @} */

#endif
