#ifndef SELFMAKE_DOCUMENTS_H
#define SELFMAKE_DOCUMENTS_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "../Common/API.h"
#include "../Common/Result.h"

#endif

/** @addtogroup nhmakeFunctions
 *  @{
 */

    void selfmake_getProjectVersion(
        NH_BYTE *version_p
    );

    SELFMAKE_RESULT selfmake_generateHomepage(
    );

    SELFMAKE_RESULT selfmake_generateFooter(
    );

/** @} */

#endif
