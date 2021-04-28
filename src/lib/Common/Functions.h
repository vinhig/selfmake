#ifndef SELFMAKE_H
#define SELFMAKE_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

/**
 * This file contains API-level functions.
 */

#include "Result.h"
#include "Types.h"

#endif

/** @addtogroup selfmakeAPIFunctions
 *  @{
 */

    SM_RESULT sm_initialize(
    );
    
    void sm_terminate(
    );

    sm_Runtime *sm_createRuntime(
        SM_BYTE *name_p
    );

    void sm_destroyRuntime(
        sm_Runtime *Runtime_p
    );

    SM_RESULT sm_run(
        sm_Runtime *Runtime_p, int argc, SM_BYTE **argv_pp
    );

    SM_RESULT sm_addFile(
        sm_Runtime *Runtime_p, SM_BYTE *path_p
    );

    void sm_setFunctionCallback(
        sm_Runtime *Runtime_p, sm_functionCallback_f functionCallback_f
    );

    void sm_setSourceContextCallback(
        sm_Runtime *Runtime_p, sm_sourceContextCallback_f sourceContextCallback_f
    );

    void sm_setBeforeBuildCallback(
        sm_Runtime *Runtime_p, sm_sourceContextCallback_f sourceContextCallback_f
    );

    void sm_setAfterBuildCallback(
        sm_Runtime *Runtime_p, sm_sourceContextCallback_f sourceContextCallback_f
    );

    sm_SourceContextArray *sm_getSourceContextArray(
        sm_Runtime *Runtime_p
    );

    sm_SourceContext *sm_getSourceContext(
        sm_Runtime *Runtime_p, SM_BYTE *name_p
    );

    void sm_setQuiet(
        sm_Runtime *Runtime_p, SM_BOOL quiet
    );
    
    void sm_setShowParseTree(
        sm_Runtime *Runtime_p, SM_BOOL showParseTree
    );

    SM_RESULT sm_setVariable(
        sm_Runtime *Runtime_p, SM_BYTE *variables_p, SM_BYTE **values_pp, int valueCount
    );

    sm_ValueArray sm_getVariableValues(
        sm_Runtime *Runtime_p, SM_BYTE *variable_p
    );

    int sm_isRunning(
    );
    
    SM_RESULT sm_sleepMs(
        int milliseconds
    );

    SM_BYTE *sm_getProcessDirectory(
    );

    SM_BYTE *sm_getWorkDirectory(
    );

    SM_BYTE *sm_getFileData(
        const SM_BYTE* path_p, long *size_p
    );

    SM_RESULT sm_writeBytesToFile(
        SM_BYTE *filename_p, SM_BYTE *bytes_p
    );

    SM_RESULT sm_messagef(
        SM_BYTE *format_p, ...
    );

/** @} */

#endif
