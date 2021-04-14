// LICENSE NOTICE ==================================================================================

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

// INCLUDE =========================================================================================

#include "GUI.h"
#include "X11.h"

#include "../Common/Macro.h"
#include "../Common/API.h"
#include SELFMAKE_FLOW
#include SELFMAKE_DEFAULT_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// DATA ============================================================================================

NH_BOOL SELFMAKE_GUI = NH_FALSE;
static NH_BOOL run = NH_TRUE;

// BUILD ===========================================================================================

SELFMAKE_RESULT selfmake_runGUI()
{
SELFMAKE_BEGIN()

    selfmake_X11_createWindow("netzhaut-Installer", 300, 300);
    while (run) {selfmake_X11_getInput();}
    
SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_handleGUIExpose()
{
SELFMAKE_BEGIN()

    selfmake_X11_drawTextLine(5, 5, "Building via GUI is planned, but not implemented yet. Please use the command-line for now.");

SELFMAKE_END(SELFMAKE_SUCCESS)
}

void selfmake_handleGUIExit()
{
SELFMAKE_BEGIN()

    run = NH_FALSE;

SELFMAKE_SILENT_END()
}

