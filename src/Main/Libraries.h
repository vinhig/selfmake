#ifndef SELFMAKE_LIBRARIES_H
#define SELFMAKE_LIBRARIES_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "../Parser/Functions.h"
#include "../Common/Result.h"
#include "../Common/API.h"

#endif

/** @addtogroup InstallerFunctions Functions
 *  \ingroup Installer 
 *  @{
 */

    SELFMAKE_RESULT selfmake_prepareLibraryBuild(
    );
    
    SELFMAKE_RESULT selfmake_buildLibrary(
        selfmake_Library *Library_p, NH_BOOL installLibraries
    ); 

    SELFMAKE_RESULT selfmake_buildnhexternal(
        NH_BOOL install
    );

/** @} */

#endif
