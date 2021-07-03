#ifndef SM_OPTIONS_H
#define SM_OPTIONS_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../Common/Result.h"
#include "../Common/Types.h"

#include "../Parser/Parser.h"
#include "../Parser/Variables.h"
#include "../Parser/Sources.h"
#include "../Parser/SourceContexts.h"

#endif

/** @addtogroup selfmakeFunctions
 *  @{
 */

    SM_RESULT sm_addBuildOption(
        sm_Runtime *Runtime_p, SM_BYTE *name_p
    );

    SM_RESULT sm_addTestOption(
        sm_Runtime *Runtime_p, SM_BYTE *name_p
    );

    SM_RESULT sm_parseOption(
        sm_Runtime *Runtime_p, int argc, SM_BYTE **argv_pp, int *advance_p
    );

/** @} */

#endif
