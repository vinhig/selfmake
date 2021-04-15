#ifndef SM_THREAD_H
#define SM_THREAD_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../Core/Runtime.h"

#include "../Common/Types.h"
#include "../Common/Result.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
    
#ifdef __unix__
#include <pthread.h>
#elif defined(_WIN32) || defined(WIN32)
#include <windows.h>
#endif

#endif

/** @addtogroup selfmakeMacros
 *  @{
 */

    #define SM_MAX_THREADS 64

/** @} */

/** @addtogroup selfmakeStructs
 *  @{
 */

    typedef struct sm_Thread {
        int depth;
        SM_BOOL running;
        sm_Runtime *Runtime_p;
    #ifdef __unix__
        pthread_t id;              
    #elif defined(_WIN32) || defined (WIN32)
        DWORD id;              
    #endif
    } sm_Thread;

/** @} */

/** @addtogroup selfmakeFunctions
 *  @{
 */

    void sm_initThreadPool(
    );
    
    SM_RESULT sm_runThread(
        sm_Runtime *Runtime_p, SM_BYTE **args_pp, int args
    );

    sm_Thread *_sm_getThread(
    );
    
/** @} */

#endif 
