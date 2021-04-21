// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Variables.h"
#include "Tokenizer.h"

#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_CUSTOM_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

// INIT ============================================================================================

void sm_initVariableArray(
    sm_VariableArray *Array_p)
{
SM_BEGIN()

    Array_p->length = 0;
    Array_p->Variables_p = NULL;

SM_SILENT_END()
}

// UPDATE ==========================================================================================

sm_Variable *sm_getVariable(
    sm_VariableArray *Array_p, SM_BYTE *name_p)
{
SM_BEGIN()

    for (int i = 0; i < Array_p->length; ++i) {
        if (!strcmp(name_p, Array_p->Variables_p[i].name_p)) {
            SM_END(&Array_p->Variables_p[i])
        }
    }

SM_END(NULL)
}

static sm_Variable *sm_createVariable(
    sm_VariableArray *Array_p, SM_BYTE *name_p)
{
SM_BEGIN()

    if (!Array_p->Variables_p) {
        Array_p->Variables_p = malloc(sizeof(sm_Variable));
        SM_CHECK_NULL(NULL, Array_p->Variables_p)
    }
    else {
        Array_p->Variables_p = realloc(Array_p->Variables_p, sizeof(sm_Variable) * (Array_p->length + 1));
        SM_CHECK_NULL(NULL, Array_p->Variables_p)
    }
 
    sm_Variable *Variable_p = &Array_p->Variables_p[Array_p->length];

    Variable_p->values_pp = NULL;
    Variable_p->valueCount = 0;

    Variable_p->name_p = malloc(strlen(name_p) + 1);
    SM_CHECK_NULL(NULL, Variable_p->name_p)
    sprintf(Variable_p->name_p, name_p);

    Array_p->length++;

SM_END(Variable_p)
}

SM_RESULT sm_appendToVariable(
    sm_VariableArray *Array_p, SM_BYTE *variable_p, SM_BYTE **values_pp, int valueCount)
{
SM_BEGIN()

#include SM_DEFAULT_CHECK

    sm_Variable *Variable_p = sm_getVariable(Array_p, variable_p);
    if (!Variable_p) {
        Variable_p = sm_createVariable(Array_p, variable_p);
    }
    SM_CHECK_NULL(Variable_p)

    Variable_p->values_pp = realloc(Variable_p->values_pp, sizeof(SM_BYTE*) * (Variable_p->valueCount + valueCount));
    SM_CHECK_NULL(Variable_p->values_pp)

    for (int i = Variable_p->valueCount, j = 0; i < valueCount + Variable_p->valueCount; ++i, ++j) {
        Variable_p->values_pp[i] = malloc(strlen(values_pp[j]) + 1);
        SM_CHECK_NULL(Variable_p->values_pp[i])
        sprintf(Variable_p->values_pp[i], values_pp[j]);
    }

    Variable_p->valueCount += valueCount;

#include SM_CUSTOM_CHECK

SM_END(SM_SUCCESS)
}

SM_RESULT sm_updateVariable(
    sm_VariableArray *Array_p, SM_BYTE *variable_p, SM_BYTE **values_pp, int valueCount)
{
SM_BEGIN()

#include SM_DEFAULT_CHECK

    sm_Variable *Variable_p = sm_getVariable(Array_p, variable_p);
    if (!Variable_p) {
        Variable_p = sm_createVariable(Array_p, variable_p);
    }
    SM_CHECK_NULL(Variable_p)

    if (Variable_p->values_pp) {
        for (int i = 0; i < Variable_p->valueCount; ++i) {
            free(Variable_p->values_pp[i]);
        }
        free(Variable_p->values_pp);
    }

    Variable_p->values_pp = malloc(sizeof(SM_BYTE*) * valueCount);
    SM_CHECK_NULL(Variable_p->values_pp)

    for (int i = 0; i < valueCount; ++i) {
        Variable_p->values_pp[i] = malloc(strlen(values_pp[i]) + 1);
        SM_CHECK_NULL(Variable_p->values_pp[i])
        sprintf(Variable_p->values_pp[i], values_pp[i]);
    }

    Variable_p->valueCount = valueCount;

    if (!strcmp(Variable_p->name_p, "WRK_DIR")) {
        chdir(Variable_p->values_pp[0]);
    }

#include SM_CUSTOM_CHECK

SM_END(SM_SUCCESS)
}

// REPLACE =========================================================================================

static SM_BYTE *sm_substituteVariableCallsInString(
    sm_VariableArray *Array_p, SM_BYTE *string_p)
{
SM_BEGIN()

    SM_BYTE *begin_p = NULL;

    for (int i = 0; i < strlen(string_p); ++i) {
        if (string_p[i] == '$' && i + 1 < strlen(string_p) && string_p[i + 1] == '{') {
            begin_p = &string_p[i];
            break;
        }
    }

    if (begin_p) 
    {
        SM_BYTE *end_p = begin_p;
        while (*end_p != '}') {++end_p;}
        *end_p = 0;

        int nameLength = strlen(begin_p + 2);
        sm_Variable *Var_p = sm_getVariable(Array_p, begin_p + 2);
        *end_p = '}';

        if (!Var_p) {SM_END(string_p)}

        SM_BYTE *newString_p = malloc((strlen(string_p) - strlen(Var_p->name_p) - 3) + strlen(Var_p->values_pp[0]) + 1);
        SM_CHECK_NULL(NULL, newString_p)

        *end_p = 0;
        *begin_p = 0;

        sprintf(newString_p, string_p);
        sprintf(newString_p + strlen(newString_p), Var_p->values_pp[0]);
        sprintf(newString_p + strlen(newString_p), end_p + 1); 

        free(string_p);
        SM_END(sm_substituteVariableCallsInString(Array_p, newString_p))
    }

SM_END(string_p)
}

SM_BYTE *sm_substituteVariables(
    sm_VariableArray *Array_p, SM_BYTE *string_p)
{
SM_BEGIN()

    SM_BYTE *newString_p = malloc(strlen(string_p) + 1);
    SM_CHECK_NULL(NULL, newString_p)

    sprintf(newString_p, string_p);

SM_END(sm_substituteVariableCallsInString(Array_p, newString_p)) 
}

