#ifndef SM_SOURCE_CONTEXTS_H
#define SM_SOURCE_CONTEXTS_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "Parser.h"

#include "../Common/Types.h"
#include "../Common/Result.h"

#endif

/** @addtogroup selfmakeEnums
 *  @{
 */

    typedef enum SM_SOURCE_CONTEXT {
        SM_SOURCE_CONTEXT_SHARED_LIBRARY,
        SM_SOURCE_CONTEXT_STATIC_LIBRARY,
    } SM_SOURCE_CONTEXT;

/** @} */

/** @addtogroup selfmakeStructs
 *  @{
 */

    typedef struct sm_SourceContext {
        SM_SOURCE_CONTEXT type;
        SM_BYTE *path_p;
        SM_BYTE *name_p;
        SM_BYTE *compileArgs_p;
        SM_BYTE *linkArgs_p;
        SM_BOOL build;
        SM_BOOL install;
        long major, minor, patch;
    } sm_SourceContext;

    typedef struct sm_SourceContextArray {
        int length;
        int maxNameLength;
        sm_SourceContext *SourceContexts_p;
    } sm_SourceContextArray;

/** @} */

/** @addtogroup selfmakeFunctions
 *  @{
 */

    void sm_initSourceContextArray(
        sm_SourceContextArray *Array_p
    );

    void sm_getIndentAfterSourceContext(
        SM_BYTE *name_p, SM_BYTE *buffer_p, int size, sm_SourceContextArray *Array_p
    );

    SM_RESULT sm_addSourceContext(
        sm_SourceContextArray *Array_p, sm_Function *Function_p, SM_SOURCE_CONTEXT type
    );

    SM_RESULT sm_addCompileArguments(
        sm_SourceContextArray *ContextArray_p, sm_Function *Function_p
    );
    
    SM_RESULT sm_addLinkArguments(
        sm_SourceContextArray *Array_p, sm_Function *Function_p
    );

/** @} */

#endif
