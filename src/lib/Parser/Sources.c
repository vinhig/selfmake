// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Sources.h"

#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

// INIT ============================================================================================

void sm_initSourceArray(
    sm_SourceArray *Array_p)
{
SM_BEGIN()

    Array_p->length = 0;
    Array_p->Sources_p = NULL;

SM_SILENT_END()
}

static sm_Source sm_initSource()
{
SM_BEGIN()

    sm_Source Source;
    Source.Context_p = NULL;
    Source.path_p = NULL;

SM_END(Source)
}

// ADD =============================================================================================

SM_RESULT sm_addSource(
    sm_SourceContextArray *ContextArray_p, sm_SourceArray *SourceArray_p, sm_Function *Function_p)
{
SM_BEGIN()

    sm_SourceContext *Context_p = NULL;

    for (int i = 0; i < Function_p->arguments; ++i) 
    {
        if (Function_p->argumentTypes_p[i] == SM_TOKEN_IDENTIFIER) {
            for (int j = 0; j < ContextArray_p->length; ++j) {
                if (!strcmp(ContextArray_p->SourceContexts_p[j].name_p, Function_p->arguments_pp[i])) {
                    Context_p = &ContextArray_p->SourceContexts_p[j];
                }
            }
        }
        else {

            if (!Context_p) {
                if (!ContextArray_p->length) {SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)}
                Context_p = ContextArray_p->SourceContexts_p;
            }

            if (!SourceArray_p->Sources_p) {
                SourceArray_p->Sources_p = malloc(sizeof(sm_Source));
                SM_CHECK_NULL(SourceArray_p->Sources_p)
            }
            else {
                SourceArray_p->Sources_p = realloc(SourceArray_p->Sources_p, sizeof(sm_Source) * (SourceArray_p->length + 1));
                SM_CHECK_NULL(SourceArray_p->Sources_p)
            }
        
            sm_Source *Source_p = &SourceArray_p->Sources_p[SourceArray_p->length];
            *Source_p = sm_initSource();

            Source_p->Context_p = Context_p;
            Source_p->path_p = malloc(strlen(Function_p->arguments_pp[i]) + 1);
            SM_CHECK_NULL(Source_p->path_p)
            sprintf(Source_p->path_p, Function_p->arguments_pp[i]);
        
            SourceArray_p->length++;

            SM_BYTE offset_p[64];
            sm_getIndentAfterSourceContext(Context_p->name_p, offset_p, 64, ContextArray_p);
            sm_messagef("[%s]%s Add source file \"%s\"", Context_p->name_p, offset_p, Source_p->path_p);
        }
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

