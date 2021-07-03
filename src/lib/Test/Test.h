#ifndef SM_TEST_H
#define SM_TEST_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "Process.h"

#include "../Common/Result.h"
#include "../Common/Types.h"

#endif

/** @addtogroup selfmakeStructs
 *  @{
 */

    typedef struct sm_ExternalFunction {
        SM_BYTE *libraryName_p;
        SM_BYTE *functionName_p;
    } sm_ExternalFunction;

    typedef struct sm_Test {
        sm_SourceContext *Context_p;
        SM_BYTE *name_p;
        void *dl_p;
        int arguments;
        sm_TestArgument *Arguments_p;
    } sm_Test;

    typedef struct sm_TestArray {
        int length;
        sm_Test *Tests_p;
    } sm_TestArray;

    typedef struct sm_TestEnvironment {
        sm_Process Process;
        sm_ExternalFunction *Initializer_p;
        sm_ExternalFunction *Loader_p;
        sm_TestArray *TestArray_p;
        SM_BYTE *target_p;
    } sm_TestEnvironment;

/** @} */

/** @addtogroup selfmakeFunctions
 *  @{
 */

    void sm_initTestArray(
        sm_TestArray *Array_p
    );
    
    SM_RESULT sm_addTest(
        sm_SourceContextArray *ContextArray_p, sm_TestArray *TestArray_p, sm_Function *Function_p
    );

    SM_RESULT sm_test(
        sm_Runtime *Runtime_p, SM_BYTE *name_p
    );

/** @} */

#endif
