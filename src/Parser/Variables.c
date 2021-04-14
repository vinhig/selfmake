// LICENSE NOTICE ==================================================================================

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

// INCLUDE =========================================================================================

#include "Variables.h"
#include "Tokenizer.h"

#include "../Main/Documents.h"
#include "../Common/Macro.h"
#include SELFMAKE_FLOW
#include SELFMAKE_CUSTOM_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

// VARS ============================================================================================

selfmake_Variable SELFMAKE_VARIABLES_P[SELFMAKE_VARIABLE_COUNT];

const NH_BYTE *SELFMAKE_VARIABLE_NAMES_PP[] = {
    "PROJECT_VERSION",
    "PROJECT_DIRECTORY",
};

// INIT ============================================================================================

void selfmake_initVariables()
{
SELFMAKE_BEGIN()

    for (int i = 0; i < SELFMAKE_VARIABLE_COUNT; ++i) {
        SELFMAKE_VARIABLES_P[i].value_p = NULL;
        SELFMAKE_VARIABLES_P[i].name_p  = (NH_BYTE*)SELFMAKE_VARIABLE_NAMES_PP[i];
    }

SELFMAKE_SILENT_END()
}

// UPDATE ==========================================================================================

SELFMAKE_RESULT selfmake_updateVariable(
    SELFMAKE_VARIABLE type, NH_BYTE *string_p)
{
SELFMAKE_BEGIN()

#include SELFMAKE_DEFAULT_CHECK

    free(SELFMAKE_VARIABLES_P[type].value_p);
    SELFMAKE_VARIABLES_P[type].value_p = malloc(strlen(string_p) + 1);
    SELFMAKE_CHECK_NULL(SELFMAKE_VARIABLES_P[type].value_p)
    sprintf(SELFMAKE_VARIABLES_P[type].value_p, string_p);

#include SELFMAKE_CUSTOM_CHECK

SELFMAKE_END(SELFMAKE_SUCCESS)
}

// REPLACE =========================================================================================

static selfmake_Variable *selfmake_getMatchingVariable(
    NH_BYTE *name_p)
{
SELFMAKE_BEGIN()

    for (int i = 0; i < SELFMAKE_VARIABLE_COUNT; ++i) {
        if (!strcmp(name_p, SELFMAKE_VARIABLE_NAMES_PP[i])) {
            SELFMAKE_END(&SELFMAKE_VARIABLES_P[i])
        }
    }

SELFMAKE_END(NULL)
}

static NH_BYTE *selfmake_replaceVariableCallsInString(
    NH_BYTE *string_p)
{
SELFMAKE_BEGIN()

    NH_BYTE *begin_p = NULL;

    for (int i = 0; i < strlen(string_p); ++i) {
        if (string_p[i] == '$' && i + 1 < strlen(string_p) && string_p[i + 1] == '{') {
            begin_p = &string_p[i];
            break;
        }
    }

    if (begin_p) 
    {
        NH_BYTE *end_p = begin_p;
        while (*end_p != '}') {++end_p;}
        *end_p = 0;

        int nameLength = strlen(begin_p + 2);
        selfmake_Variable *Var_p = selfmake_getMatchingVariable(begin_p + 2);
        *end_p = '}';

        if (!Var_p) {SELFMAKE_END(string_p)}

        NH_BYTE *newString_p = malloc((strlen(begin_p) - nameLength - 3) + strlen(Var_p->name_p) + 1);
        SELFMAKE_CHECK_NULL(NULL, newString_p)

        *end_p = 0;
        *begin_p = 0;

        sprintf(newString_p, string_p);
        sprintf(newString_p + strlen(newString_p), Var_p->value_p);
        sprintf(newString_p + strlen(newString_p), end_p + 1); 

        free(string_p);
        SELFMAKE_END(selfmake_replaceVariableCallsInString(newString_p))
    }

SELFMAKE_END(string_p)
}

NH_BYTE *selfmake_replaceVariables(
    NH_BYTE *string_p)
{
SELFMAKE_BEGIN()

    NH_BYTE *newString_p = malloc(strlen(string_p) + 1);
    SELFMAKE_CHECK_NULL(NULL, newString_p)

    sprintf(newString_p, string_p);

SELFMAKE_END(selfmake_replaceVariableCallsInString(newString_p)) 
}

