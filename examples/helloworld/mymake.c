// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "../../include/selfmake/selfmake.h"

// MAIN ============================================================================================

int main(int argc, char **argv_pp)
{
    sm_initialize();

    sm_Runtime *Runtime_p = sm_createRuntime("mymake");
    if (!Runtime_p) {return 1;}

    SM_BYTE *wrkDir_p = sm_getProcessDirectory();
    sm_setVariable(Runtime_p, "WRK_DIR", &wrkDir_p, 1);

    sm_addFile(Runtime_p, "mymake.sm");

    if (sm_run(Runtime_p, argc, argv_pp)) {return 1;}

    while (sm_isRunning()) {sm_sleepMs(100);}

    sm_destroyRuntime(Runtime_p);
    sm_terminate();
} 

