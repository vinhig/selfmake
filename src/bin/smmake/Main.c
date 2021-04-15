// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "../../../include/selfmake/selfmake.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// MAIN ============================================================================================

int main(
    int argc, char **argv_pp)
{
    sm_init();

    SM_BYTE dir_p[255];
    sm_getProcessDirectory(dir_p, 255);
    sprintf(dir_p, "%s/..", dir_p);

    sm_Runtime *Runtime_p = sm_createRuntime("smmake", dir_p);

    sm_addFile(Runtime_p, "src/bin/smmake/smmake.sm");

    sm_run(Runtime_p, 0, NULL);

    while (sm_isRunning()) {sm_sleepMs(100);}

    return 0;
} 

