// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Version.h"
#include "Documents.h"

#include "../../../include/selfmake/selfmake.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// MAIN ============================================================================================

static SM_RESULT sourceContextCallback(
    sm_Runtime *Runtime_p, sm_SourceContext *SourceContext_p)
{
    if (!strcmp(SourceContext_p->name_p, "selfmake")) 
    {
        int versionNums_p[4];
        long versionDates_pp[4][3];
        getVersionData(versionNums_p, versionDates_pp);
        SourceContext_p->api   = versionNums_p[0];
        SourceContext_p->major = versionNums_p[1];
        SourceContext_p->minor = versionNums_p[2];
        SourceContext_p->patch = versionNums_p[3];
    }

    return SM_SUCCESS;
}

static SM_RESULT functionCallback(
    sm_Runtime *Runtime_p, sm_Function *Function_p)
{
    if (!strcmp(Function_p->name_p, "generateFooter")) {
        generateFooter(Runtime_p); 
    }
    else if (!strcmp(Function_p->name_p, "generateHomepage")) {
        generateHomepage(Runtime_p); 
    }

    return SM_SUCCESS;
}

int main(
    int argc, char **argv_pp)
{
    sm_initialize();

    sm_Runtime *Runtime_p = sm_createRuntime("smmake");
    if (!Runtime_p) {return 1;}

    SM_BYTE *wrkDir_p = malloc(255);
    if (!wrkDir_p) {return 1;}
    SM_BYTE *version_p = malloc(255);
    if (!version_p) {return 1;}

    sprintf(wrkDir_p, "%s/..", sm_getProcessDirectory());
    getProjectVersion(Runtime_p, version_p);

    sm_setVariable(Runtime_p, "PROJ_REV", &version_p, 1);
    sm_setVariable(Runtime_p, "WRK_DIR", &wrkDir_p, 1);

    sm_addFile(Runtime_p, "build/.sm/smmake.sm");
    sm_setFunctionCallback(Runtime_p, functionCallback);
    sm_setSourceContextCallback(Runtime_p, sourceContextCallback);

    if (sm_run(Runtime_p, argc - 1, argv_pp + 1)) {return 1;}

    while (sm_isRunning()) {sm_sleepMs(100);}

    sm_destroyRuntime(Runtime_p);
    sm_terminate();

    return 0;
} 

