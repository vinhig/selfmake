#ifndef SELFMAKE_GUI_H
#define SELFMAKE_GUI_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "../Common/Result.h"
#include "../Common/API.h"

#endif

/** @addtogroup nhmakeVars
 *  @{
 */

    extern NH_BOOL SELFMAKE_GUI;

/** @} */

/** @addtogroup nhmakeFunctions
 *  @{
 */

    SELFMAKE_RESULT selfmake_runGUI(
    );

    SELFMAKE_RESULT selfmake_handleGUIExpose(
    );

    void selfmake_handleGUIExit(
    );

/** @} */

#endif
