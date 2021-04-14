#ifndef SELFMAKE_BINARIES_H
#define SELFMAKE_BINARIES_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "../Common/Result.h"

#endif

/** @addtogroup InstallerFunctions Functions
 *  \ingroup Installer 
 *  @{
 */

    SELFMAKE_RESULT selfmake_buildnhwebbrowser(
    );

    SELFMAKE_RESULT selfmake_installnhwebbrowser(
    );

    SELFMAKE_RESULT selfmake_buildnhterminal(
    );

    SELFMAKE_RESULT selfmake_installnhterminal(
    );

    SELFMAKE_RESULT selfmake_installLogo(
    );

/** @} */

#endif
