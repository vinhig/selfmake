#ifndef VERSION_H
#define VERSION_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../../../include/selfmake/selfmake.h"

#endif

/** @addtogroup selfmakeFunctions
 *  @{
 */

    SM_RESULT getVersionData(
        int version_p[3], long versionDates_pp[3][3]
    );

    void getProjectVersion(
        sm_Runtime *Runtime_p, SM_BYTE *version_p
    );

/** @} */

#endif
