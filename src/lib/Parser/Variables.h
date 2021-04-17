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

/** @addtogroup selfmakeStructs
 *  @{
 */

    typedef struct sm_Variable {
        SM_BYTE *name_p;
        int valueCount;
        SM_BYTE **values_pp;
    } sm_Variable;

    typedef struct sm_VariableArray {
        int length;
        sm_Variable *Variables_p;
    } sm_VariableArray;

/** @} */

/** @addtogroup selfmakeFunctions
 *  @{
 */

    void sm_initVariableArray(
        sm_VariableArray *Array_p
    );

    sm_Variable *sm_getVariable(
        sm_VariableArray *Array_p, SM_BYTE *name_p
    );

    SM_RESULT sm_updateVariable(
        sm_VariableArray *Array_p, SM_BYTE *variable_p, SM_BYTE **values_pp, int valueCount
    );

    SM_BYTE *sm_replaceVariables(
        sm_VariableArray *Array_p, SM_BYTE *string_p
    );

/** @} */

#endif
