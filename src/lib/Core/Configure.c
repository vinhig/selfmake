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

// INIT ============================================================================================

SM_RESULT sm_init()
{
    sm_initThreadPool();

    static SM_BYTE *defaultName_p = "selfmake";
    SM_DEFAULT_RUNTIME.name_p = defaultName_p;
    SM_DEFAULT_RUNTIME.showParseTree = SM_TRUE;
    SM_DEFAULT_RUNTIME.quiet = SM_FALSE;
    SM_DEFAULT_RUNTIME.GUI = SM_FALSE;
}

// FILES ===========================================================================================

SM_RESULT sm_addFile(
    sm_Runtime *Runtime_p, SM_BYTE *path_p)
{
SM_BEGIN()

    SM_CHECK_NULL(Runtime_p)
    SM_CHECK_NULL(path_p)

    chdir(Runtime_p->projectDirectory_p);

    SM_CHECK(sm_appendFile(&Runtime_p->FileArray, path_p))

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// SET =============================================================================================

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

