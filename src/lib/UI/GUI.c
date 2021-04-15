// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "GUI.h"
#include "X11.h"

#include "../Common/Macro.h"
#include "../Common/API.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// DATA ============================================================================================

SM_BOOL SM_GUI = SM_FALSE;
static SM_BOOL run = SM_TRUE;

// BUILD ===========================================================================================

SM_RESULT sm_runGUI()
{
SM_BEGIN()

    sm_X11_createWindow("netzhaut-Installer", 300, 300);
    while (run) {sm_X11_getInput();}
    
SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_RESULT sm_handleGUIExpose()
{
SM_BEGIN()

    sm_X11_drawTextLine(5, 5, "Building via GUI is planned, but not implemented yet. Please use the command-line for now.");

SM_END(SM_SUCCESS)
}

void sm_handleGUIExit()
{
SM_BEGIN()

    run = SM_FALSE;

SM_SILENT_END()
}

