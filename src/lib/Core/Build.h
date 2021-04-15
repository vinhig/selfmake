#ifndef SM_BUILD_H
#define SM_BUILD_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../Parser/Functions.h"
#include "../Common/Result.h"
#include "../Common/Types.h"

#endif

/** @addtogroup selfmakeFunctions
 *  @{
 */

    SM_RESULT sm_build(
        sm_Runtime *Runtime_p, sm_SourceContext *Context_p, SM_BOOL install
    ); 

/** @} */

#endif
