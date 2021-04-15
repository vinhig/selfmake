#ifndef SELFMAKE_H
#define SELFMAKE_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "Common/Result.h"
#include "Common/Types.h"

typedef struct sm_Runtime sm_Runtime;

#endif

/** @addtogroup selfmakeFunctions
 *  @{
 */

    SM_RESULT sm_init(
    );
    
    sm_Runtime *sm_createRuntime(
        SM_BYTE *name_p, SM_BYTE *projectDirectory_p
    );

    SM_RESULT sm_run(
        sm_Runtime *Runtime_p, int argc, SM_BYTE **argv_pp
    );

    SM_RESULT sm_addFile(
        sm_Runtime *Runtime_p, SM_BYTE *path_p
    );

    void sm_setQuiet(
        sm_Runtime *Runtime_p, SM_BOOL quiet
    );
    
    void sm_setShowParseTree(
        sm_Runtime *Runtime_p, SM_BOOL showParseTree
    );

    int sm_isRunning(
    );
    
    SM_RESULT sm_sleepMs(
        int milliseconds
    );

    SM_RESULT sm_getProcessDirectory(
        SM_BYTE *buffer_p, unsigned int size
    );

/** @} */

#endif
