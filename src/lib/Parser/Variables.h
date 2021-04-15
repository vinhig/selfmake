#ifndef SM_VARIABLES_H
#define SM_VARIABLES_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../Common/Types.h"
#include "../Common/Result.h"

#endif

/** @addtogroup selfmakeEnums
 *  @{
 */

    typedef enum SM_VARIABLE {
        SM_VARIABLE_PROJECT_VERSION,
        SM_VARIABLE_PROJECT_DIRECTORY,
        SM_VARIABLE_COUNT,
    } SM_VARIABLE;

/** @} */

/** @addtogroup selfmakeStructs
 *  @{
 */

    typedef struct sm_Variable {
        SM_BYTE *name_p;
        SM_BYTE *value_p;
    } sm_Variable;

    typedef struct sm_VariableArray {
        int length;
        sm_Variable *Variables_p;
    } sm_VariableArray;

/** @} */

/** @addtogroup selfmakeFunctions
 *  @{
 */

    SM_RESULT sm_initVariableArray(
        sm_VariableArray *Array_p
    );

    sm_Variable *sm_getVariable(
        sm_VariableArray *Array_p, SM_BYTE *name_p
    );

    SM_RESULT sm_updateVariable(
        sm_VariableArray *Array_p, SM_BYTE *variable_p, SM_BYTE *string_p
    );

    SM_BYTE *sm_replaceVariables(
        sm_VariableArray *Array_p, SM_BYTE *string_p
    );

/** @} */

#endif
