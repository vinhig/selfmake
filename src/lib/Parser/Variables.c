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

// VARS ============================================================================================

const SM_BYTE *SM_VARIABLE_NAMES_PP[] = {
    "PROJECT_VERSION",
    "PROJECT_DIRECTORY",
    "LIB_DEST",
};

// INIT ============================================================================================

SM_RESULT sm_initVariableArray(
    sm_VariableArray *Array_p)
{
SM_BEGIN()

#include SM_DEFAULT_CHECK

    Array_p->length = SM_VARIABLE_COUNT;
    Array_p->Variables_p = malloc(sizeof(sm_Variable) * SM_VARIABLE_COUNT);
    SM_CHECK_NULL(Array_p->Variables_p)

    for (int i = 0; i < SM_VARIABLE_COUNT; ++i) {
        Array_p->Variables_p[i].value_p = NULL;
        Array_p->Variables_p[i].name_p  = (SM_BYTE*)SM_VARIABLE_NAMES_PP[i];
    }

#include SM_CUSTOM_CHECK

SM_DIAGNOSTIC_END(SM_SUCCESS)
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

SM_RESULT sm_updateVariable(
    sm_VariableArray *Array_p, SM_BYTE *variable_p, SM_BYTE *string_p)
{
SM_BEGIN()

#include SM_DEFAULT_CHECK

    sm_Variable *Variable_p = sm_getVariable(Array_p, variable_p);
    SM_CHECK_NULL(Variable_p)

    free(Variable_p->value_p);
    Variable_p->value_p = malloc(strlen(string_p) + 1);
    SM_CHECK_NULL(Variable_p->value_p)
    sprintf(Variable_p->value_p, string_p);

#include SM_CUSTOM_CHECK

SM_END(SM_SUCCESS)
}

// REPLACE =========================================================================================

static SM_BYTE *sm_replaceVariableCallsInString(
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

        SM_BYTE *newString_p = malloc((strlen(begin_p) - nameLength - 3) + strlen(Var_p->name_p) + 1);
        SM_CHECK_NULL(NULL, newString_p)

        *end_p = 0;
        *begin_p = 0;

        sprintf(newString_p, string_p);
        sprintf(newString_p + strlen(newString_p), Var_p->value_p);
        sprintf(newString_p + strlen(newString_p), end_p + 1); 

        free(string_p);
        SM_END(sm_replaceVariableCallsInString(Array_p, newString_p))
    }

SM_END(string_p)
}

SM_BYTE *sm_replaceVariables(
    sm_VariableArray *Array_p, SM_BYTE *string_p)
{
SM_BEGIN()

    SM_BYTE *newString_p = malloc(strlen(string_p) + 1);
    SM_CHECK_NULL(NULL, newString_p)

    sprintf(newString_p, string_p);

SM_END(sm_replaceVariableCallsInString(Array_p, newString_p)) 
}

