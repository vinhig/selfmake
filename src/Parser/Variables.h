#ifndef SELFMAKE_VARIABLES_H
#define SELFMAKE_VARIABLES_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "../Common/API.h"
#include "../Common/Result.h"

#endif

/** @addtogroup nhmakeEnums
 *  @{
 */

    typedef enum SELFMAKE_VARIABLE {
        SELFMAKE_VARIABLE_PROJECT_VERSION,
        SELFMAKE_VARIABLE_PROJECT_DIRECTORY,
        SELFMAKE_VARIABLE_COUNT,
    } SELFMAKE_VARIABLE;

/** @} */

/** @addtogroup nhmakeStructs
 *  @{
 */

    typedef struct selfmake_Variable {
        NH_BYTE *name_p;
        NH_BYTE *value_p;
    } selfmake_Variable;

/** @} */

/** @addtogroup nhmakeVars
 *  @{
 */

    extern selfmake_Variable SELFMAKE_VARIABLES_P[SELFMAKE_VARIABLE_COUNT];
    extern const NH_BYTE *SELFMAKE_VARIABLE_NAMES_PP[];

/** @} */

/** @addtogroup nhmakeFunctions
 *  @{
 */

    void selfmake_initVariables(
    );

    SELFMAKE_RESULT selfmake_updateVariable(
        SELFMAKE_VARIABLE type, NH_BYTE *string_p
    );

    NH_BYTE *selfmake_replaceVariables(
        NH_BYTE *string_p
    );

/** @} */

#endif
