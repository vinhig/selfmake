#ifndef SM_GUI_H
#define SM_GUI_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../Common/Result.h"
#include "../Common/Types.h"

#endif

/** @addtogroup selfmakeVars
 *  @{
 */

    extern SM_BOOL SM_GUI;

/** @} */

/** @addtogroup selfmakeFunctions
 *  @{
 */

    SM_RESULT sm_runGUI(
    );

    SM_RESULT sm_handleGUIExpose(
    );

    void sm_handleGUIExit(
    );

/** @} */

#endif
