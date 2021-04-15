// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Thread.h"

#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <string.h>
#include <stdio.h>

#ifdef __unix__
    #include <sys/sysinfo.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <sys/time.h>
#elif defined(_WIN32) || defined (WIN32)
    #include <windows.h>
#endif

// DATA ============================================================================================

typedef struct sm_ThreadPool {
    sm_Thread Threads_p[SM_MAX_THREADS];
    int threadCount;    
} sm_ThreadPool;

static sm_ThreadPool SM_THREAD_POOL;

// INIT ============================================================================================

static inline void sm_initThread(
    sm_Thread *Thread_p)
{
SM_BEGIN()

    Thread_p->depth = 0;
    Thread_p->id = 0;
    Thread_p->running = SM_FALSE;
    Thread_p->Runtime_p = NULL;

SM_SILENT_END()
}

void sm_initThreadPool()
{
SM_BEGIN()

    SM_THREAD_POOL.threadCount = 0;

    for (int i = 0; i < SM_MAX_THREADS; ++i) {
        sm_initThread(&SM_THREAD_POOL.Threads_p[i]);
    }

SM_SILENT_END()
}

// RUN =============================================================================================

typedef struct sm_ThreadArguments {
    SM_BYTE **args_pp;
    int args;
    sm_Runtime *Runtime_p;
} sm_ThreadArguments;

static void *sm_executeThread(
    void *args_p)
{
SM_BEGIN()

    sm_ThreadArguments *Arguments_p = args_p;

    SM_RESULT result = sm_executeRuntime(Arguments_p->Runtime_p, Arguments_p->args_pp, Arguments_p->args);

    sm_Thread *Thread_p = _sm_getThread();
    Thread_p->running = SM_FALSE;
    Thread_p->Runtime_p = NULL;

    free(args_p);

SM_END((void*)result)
}

SM_RESULT sm_runThread(
    sm_Runtime *Runtime_p, SM_BYTE **args_pp, int args)
{
SM_BEGIN()

    sm_Thread *Thread_p = NULL;
    for (int i = 0; i < SM_MAX_THREADS; ++i) {
        if (!SM_THREAD_POOL.Threads_p[i].running) {
            Thread_p = &SM_THREAD_POOL.Threads_p[i];
        }
    } 

    SM_CHECK_NULL(Thread_p)

    sm_ThreadArguments *Arguments_p = malloc(sizeof(sm_ThreadArguments));
    SM_CHECK_NULL(Arguments_p)

    Arguments_p->Runtime_p = Runtime_p;
    Arguments_p->args = args;
    Arguments_p->args_pp = args_pp;

    Thread_p->running = SM_TRUE;
    Thread_p->Runtime_p = Runtime_p;

#ifdef __unix__

    pthread_create(&Thread_p->id, NULL, sm_executeThread, (void*)Arguments_p);

#endif

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// COUNT ==========================================================================================

int sm_isRunning()
{
SM_BEGIN()

    int count = 0;
    for (int i = 0; i < SM_MAX_THREADS; ++i) {
        if (SM_THREAD_POOL.Threads_p[i].running) {count++;}
    }

SM_END(count)
}

// GET =============================================================================================

sm_Thread *_sm_getThread()
{
    for (int i = 0; i < SM_MAX_THREADS; ++i) 
    {
#ifdef __unix__
        if (SM_THREAD_POOL.Threads_p[i].id == pthread_self())
#elif defined(_WIN32) || defined (WIN32)
        if (SM_THREAD_POOL.Threads_p[i].id == GetCurrentThreadId())
#endif
        {
            return &SM_THREAD_POOL.Threads_p[i];
        }
    }

    return NULL;
}

// SLEEP ===========================================================================================

SM_RESULT sm_sleepMs(
    int milliseconds)
{
SM_BEGIN()

#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

