// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "SourceContexts.h"

#include "../Core/Runtime.h"
#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

// INIT ============================================================================================

void sm_initSourceContextArray(
    sm_SourceContextArray *Array_p)
{
SM_BEGIN()

    Array_p->length = 0;
    Array_p->maxNameLength = 0;
    Array_p->SourceContexts_p = NULL;

SM_SILENT_END()
}

static sm_SourceContext sm_initSourceContext(
    SM_SOURCE_CONTEXT type)
{
SM_BEGIN()

    sm_SourceContext SourceContext;

    SourceContext.type = type;
    SourceContext.path_p = NULL;
    SourceContext.compileArgs_p = NULL;
    SourceContext.linkArgs_p = NULL;
    SourceContext.major = 0;
    SourceContext.minor = 0;
    SourceContext.patch = 0;
    memset(SourceContext.majorDate_p, 0, sizeof(long) * 3);
    memset(SourceContext.minorDate_p, 0, sizeof(long) * 3);
    memset(SourceContext.patchDate_p, 0, sizeof(long) * 3);

SM_END(SourceContext)
}

// INDENT ==========================================================================================

void sm_getIndentAfterSourceContext(
    SM_BYTE *name_p, SM_BYTE *buffer_p, int size, sm_SourceContextArray *Array_p)
{
SM_BEGIN()

    memset(buffer_p, 0, size);
    int diff = Array_p->maxNameLength - strlen(name_p);
    for (int i = 0; i < diff; ++i) {buffer_p[i] = ' ';}

SM_SILENT_END()
}

// ADD =============================================================================================

SM_RESULT sm_addSourceContext(
    sm_Runtime *Runtime_p, sm_Function *Function_p, SM_SOURCE_CONTEXT type, int offset)
{
SM_BEGIN()

    sm_SourceContextArray *Array_p = &Runtime_p->SourceContextArray;

    if (Function_p->arguments < 1) {SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)}

    if (!Array_p->SourceContexts_p) {
        Array_p->SourceContexts_p = malloc(sizeof(sm_SourceContext));
        SM_CHECK_NULL(Array_p->SourceContexts_p)
    }
    else {
        Array_p->SourceContexts_p = realloc(Array_p->SourceContexts_p, sizeof(sm_SourceContext) * (Array_p->length + 1));
        SM_CHECK_NULL(Array_p->SourceContexts_p)
    }

    sm_SourceContext *SourceContext_p = &Array_p->SourceContexts_p[Array_p->length];
    *SourceContext_p = sm_initSourceContext(type);

    int index = offset;

    SourceContext_p->name_p = malloc(strlen(Function_p->arguments_pp[index]) + 1);
    SM_CHECK_NULL(SourceContext_p->name_p)
    strcpy(SourceContext_p->name_p, Function_p->arguments_pp[index++]);

    if (Function_p->arguments > index) {
        SourceContext_p->path_p = malloc(strlen(Function_p->arguments_pp[index]) + 1);
        SM_CHECK_NULL(SourceContext_p->path_p)
        strcpy(SourceContext_p->path_p, Function_p->arguments_pp[index++]);
    }

    if (Function_p->arguments > index) {
        SourceContext_p->major = strtol(Function_p->arguments_pp[index++], NULL, 10);
    }

    if (Function_p->arguments > index) {
        SourceContext_p->minor = strtol(Function_p->arguments_pp[index++], NULL, 10);
    }

    if (Function_p->arguments > index) {
        SourceContext_p->patch = strtol(Function_p->arguments_pp[index++], NULL, 10);
    }

    if (strlen(SourceContext_p->name_p) > Array_p->maxNameLength) {
        Array_p->maxNameLength = strlen(SourceContext_p->name_p);
    }

    Array_p->length++;

    SM_CHECK(sm_appendToVariable(&Runtime_p->VariableArray, "ALL", &Function_p->arguments_pp[offset], 1))

    if (type == SM_SOURCE_CONTEXT_BINARY) {
        SM_CHECK(sm_appendToVariable(&Runtime_p->VariableArray, "BINS", &Function_p->arguments_pp[offset], 1))
    }
    else {
        SM_CHECK(sm_appendToVariable(&Runtime_p->VariableArray, "LIBS", &Function_p->arguments_pp[offset], 1))
    }

    switch (type)
    {
        case SM_SOURCE_CONTEXT_BINARY :
            sm_messagef("Add binary [%s]", SourceContext_p->name_p);
            break;
        case SM_SOURCE_CONTEXT_SHARED_LIBRARY :
            sm_messagef("Add shared library [%s]", SourceContext_p->name_p);
            break;
        case SM_SOURCE_CONTEXT_STATIC_LIBRARY :
            sm_messagef("Add static library [%s]", SourceContext_p->name_p);
            break;
    }

SM_END(SM_SUCCESS)
}

// ARGUMENTS =======================================================================================

SM_RESULT sm_addCompileArguments(
    sm_SourceContextArray *ContextArray_p, sm_Function *Function_p)
{
SM_BEGIN()

    if (Function_p->arguments != 2) {SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)}

    sm_SourceContext *Context_p = NULL;
    for (int j = 0; j < ContextArray_p->length; ++j) {
        if (!strcmp(ContextArray_p->SourceContexts_p[j].name_p, Function_p->arguments_pp[0])) {
            Context_p = &ContextArray_p->SourceContexts_p[j];
        }
    }

    if (!Context_p) {SM_DIAGNOSTIC_END(SM_ERROR_LIBRARY_NOT_FOUND)}

    Context_p->compileArgs_p = malloc(strlen(Function_p->arguments_pp[1]) + 1);
    SM_CHECK_NULL(Context_p->compileArgs_p)
    sprintf(Context_p->compileArgs_p, Function_p->arguments_pp[1]);

    SM_BYTE offset_p[64];
    sm_getIndentAfterSourceContext(Context_p->name_p, offset_p, 64, ContextArray_p);
    sm_messagef("[%s]%s Set compile arguments \"%s\"", Context_p->name_p, offset_p, Context_p->compileArgs_p);

SM_END(SM_SUCCESS)
}

SM_RESULT sm_addLinkArguments(
    sm_SourceContextArray *ContextArray_p, sm_Function *Function_p)
{
SM_BEGIN()

    if (Function_p->arguments != 2) {SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)}

    sm_SourceContext *Context_p = NULL;
    for (int j = 0; j < ContextArray_p->length; ++j) {
        if (!strcmp(ContextArray_p->SourceContexts_p[j].name_p, Function_p->arguments_pp[0])) {
            Context_p = &ContextArray_p->SourceContexts_p[j];
        }
    }

    if (!Context_p) {SM_DIAGNOSTIC_END(SM_ERROR_LIBRARY_NOT_FOUND)}

    Context_p->linkArgs_p = malloc(strlen(Function_p->arguments_pp[1]) + 1);
    SM_CHECK_NULL(Context_p->linkArgs_p)
    sprintf(Context_p->linkArgs_p, Function_p->arguments_pp[1]);

    SM_BYTE offset_p[64];
    sm_getIndentAfterSourceContext(Context_p->name_p, offset_p, 64, ContextArray_p);
    sm_messagef("[%s]%s Set link arguments \"%s\"", Context_p->name_p, offset_p, Context_p->linkArgs_p);

SM_END(SM_SUCCESS)
}

