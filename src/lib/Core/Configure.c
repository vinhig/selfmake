// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Runtime.h"
#include "Thread.h"
#include "File.h"
#include "../Parser/Functions.h"
#include "../Parser/Variables.h"

#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

// ADD =============================================================================================

SM_RESULT sm_addFile(
    sm_Runtime *Runtime_p, SM_BYTE *path_p)
{
SM_BEGIN()

    SM_CHECK_NULL(Runtime_p)
    SM_CHECK_NULL(path_p)

    SM_CHECK(sm_appendFile(&Runtime_p->FileArray, path_p))

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// SET =============================================================================================

void sm_setFunctionCallback(
    sm_Runtime *Runtime_p, sm_functionCallback_f functionCallback_f)
{
SM_BEGIN()

    Runtime_p->functionCallback_f = functionCallback_f;

SM_SILENT_END()
}

void sm_setSourceContextCallback(
    sm_Runtime *Runtime_p, sm_sourceContextCallback_f sourceContextCallback_f)
{
SM_BEGIN()

    Runtime_p->sourceContextCallback_f = sourceContextCallback_f;

SM_SILENT_END()
}

sm_SourceContextArray *sm_getSourceContextArray(
    sm_Runtime *Runtime_p)
{
SM_BEGIN()
SM_END(&Runtime_p->SourceContextArray)
}

sm_SourceContext *sm_getSourceContext(
    sm_Runtime *Runtime_p, SM_BYTE *name_p)
{
SM_BEGIN()

    for (int i = 0; i < Runtime_p->SourceContextArray.length; ++i) {
        sm_SourceContext *Context_p = &Runtime_p->SourceContextArray.SourceContexts_p[i];
        if (!strcmp(Context_p->name_p, name_p)) {SM_END(Context_p)}
    }

SM_END(NULL)
}

void sm_setQuiet(
    sm_Runtime *Runtime_p, SM_BOOL quiet)
{
SM_BEGIN()

    Runtime_p->quiet = quiet;

SM_SILENT_END()
}

void sm_setShowParseTree(
    sm_Runtime *Runtime_p, SM_BOOL showParseTree)
{
SM_BEGIN()

    Runtime_p->showParseTree = showParseTree;

SM_SILENT_END()
}

SM_RESULT sm_setVariable(
    sm_Runtime *Runtime_p, SM_BYTE *variable_p, SM_BYTE **values_pp, int valueCount)
{
SM_BEGIN()
SM_END(sm_updateVariable(&Runtime_p->VariableArray, variable_p, values_pp, valueCount))
}

sm_ValueArray sm_getVariableValues(
    sm_Runtime *Runtime_p, SM_BYTE *variable_p)
{
SM_BEGIN()

    sm_Variable *Variable_p = sm_getVariable(&Runtime_p->VariableArray, variable_p);

    sm_ValueArray Values;
    Values.values_pp = NULL;
    Values.length = 0;

    if (Variable_p) {
        Values.values_pp = Variable_p->values_pp;
        Values.length = Variable_p->valueCount;
    }

SM_END(Values)
}

