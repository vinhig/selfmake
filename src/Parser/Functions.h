#ifndef SELFMAKE_FUNCTIONS_H
#define SELFMAKE_FUNCTIONS_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "Parser.h"

#include "../Common/API.h"
#include "../Common/Result.h"

#endif

/** @addtogroup nhmakeMacros
 *  @{
 */

    #define SELFMAKE_MAX_SOURCES   1024
    #define SELFMAKE_MAX_LIBRARIES 255
    #define SELFMAKE_MAX_NEWS      2048

/** @} */

/** @addtogroup nhmakeStructs
 *  @{
 */

    typedef struct selfmake_Library {
        NH_BYTE *name_p;
        NH_BYTE *compileArgs_p;
        NH_BYTE *linkArgs_p;
        NH_BOOL build;
        long major, minor, patch;
        long majorDate_p[3];
        long minorDate_p[3];
        long patchDate_p[3];
    } selfmake_Library;

    typedef struct selfmake_Source {
        selfmake_Library *Library_p;
        NH_BYTE *path_p;
        NH_BOOL external;
    } selfmake_Source;

/** @} */

/** @addtogroup InstallerFunctions Functions
 *  @{
 */

    extern selfmake_Library SELFMAKE_LIBRARIES_P[SELFMAKE_MAX_LIBRARIES];
    extern selfmake_Source SELFMAKE_SOURCES_P[SELFMAKE_MAX_SOURCES];
    extern NH_BYTE *SELFMAKE_NEWS_PP[SELFMAKE_MAX_NEWS];

/** @} */

/** @addtogroup nhmakeFunctions
 *  @{
 */

    SELFMAKE_RESULT selfmake_executeFunction(
        selfmake_Function *Function_p
    ); 

    void selfmake_initFunctions(
    );

    SELFMAKE_RESULT selfmake_executeTopLevelFunctions(
    );

/** @} */

#endif
