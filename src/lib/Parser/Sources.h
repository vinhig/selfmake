#ifndef SM_SOURCES_H
#define SM_SOURCES_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "SourceContexts.h"

#include "../Common/Types.h"
#include "../Common/Result.h"

#endif

/** @addtogroup selfmakeStructs
 *  @{
 */

    typedef struct sm_Source {
        sm_SourceContext *Context_p;
        SM_BYTE *path_p;
    } sm_Source;

    typedef struct sm_SourceArray {
        int length;
        sm_Source *Sources_p;
    } sm_SourceArray;

/** @} */

/** @addtogroup selfmakeFunctions
 *  @{
 */

    void sm_initSourceArray(
        sm_SourceArray *Array_p
    );

    SM_RESULT sm_addSource(
        sm_SourceContextArray *ContextArray_p, sm_SourceArray *SourceArray_p, sm_Function *Function_p
    );

/** @} */

#endif
