// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Process.h"
#include "Channel.h"

#include "../Common/Types.h"
#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#if defined(__linux__) || defined(__APPLE__)
    #include <sys/wait.h>
    #include <signal.h>
#endif

// FORK ============================================================================================

static sm_Process sm_initProcess()
{
SM_BEGIN()

    sm_Process Process;
    Process.id = 0;

    sm_initChannel(&Process.IPC.In);
    sm_initChannel(&Process.IPC.Out);

SM_END(Process)
}

sm_Process sm_fork()
{
SM_BEGIN()

#include SM_CUSTOM_CHECK

    sm_Process Fork = sm_initProcess();

    sm_openChannel(&Fork.IPC.In);
    sm_openChannel(&Fork.IPC.Out);

    Fork.id = fork();

    if (Fork.id == 0) { // child
        sm_closeChannelWriteAccess(&Fork.IPC.In);
        sm_closeChannelReadAccess(&Fork.IPC.Out);
        SM_END(Fork)
    }

    sm_closeChannelReadAccess(&Fork.IPC.In);
    sm_closeChannelWriteAccess(&Fork.IPC.Out);

#include SM_DEFAULT_CHECK

SM_END(Fork)
}












// TODO ============================================================================

static SM_RESULT sm_unregisterFork(
    sm_Process *Fork_p)
{
SM_BEGIN()

    if (Fork_p->id == 0) {SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)}

    sm_closeChannelWriteAccess(&Fork_p->IPC.In);
    sm_closeChannelReadAccess(&Fork_p->IPC.Out);

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

void sm_checkForks()
{
SM_BEGIN()

//    if (init == SM_FALSE) {SM_SILENT_END()}
//
//    for (int i = 0; i < SM_MAX_FORKS; ++i) {
//        sm_Process *Proc_p = &SM_PROCESS_POOL.Forks_p[i];
//        if (Proc_p->id != 0) {
//            int status;
//#if defined(__linux__) || defined(__APPLE__)
//            int result = waitpid(Proc_p->id, &status, WNOHANG);
//            if (result == -1) {
//                printf("sm_checkForks %s\n", strerror(errno));
//            }
//            if (result == -1 || WIFEXITED(status)) {
//                sm_unregisterFork(&SM_PROCESS_POOL.Forks_p[i]);
//            }
//#endif
//        }
//    }

SM_SILENT_END()
}

void sm_killFork(
    sm_Process *Fork_p)
{
SM_BEGIN()

    kill(Fork_p->id, SIGTERM);
    sm_unregisterFork(Fork_p);

SM_SILENT_END()
}

// WRITE ===========================================================================================

SM_BYTE *_sm_writeToProcess(
    sm_Process *Proc_p, SM_BYTE *write_p, int writeLen, SM_BOOL getResponse)
{
    sm_writeToChannel(&Proc_p->IPC.In, write_p, writeLen);

    while (getResponse) 
    {
        SM_BYTE *response_p = sm_readFromChannel(&Proc_p->IPC.Out, NULL);
        if (response_p != NULL) {
            return response_p;
        }
    }

    return NULL;
}

SM_BYTE *sm_writeToProcess(
    sm_Process *Proc_p, SM_BYTE *write_p, int writeLen, SM_BOOL getResponse)
{
SM_BEGIN()
SM_END(_sm_writeToProcess(Proc_p, write_p, writeLen, getResponse))
}

