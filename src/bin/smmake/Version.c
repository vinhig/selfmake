// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Version.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// VERSION =========================================================================================

static long getVersion(
    SM_BYTE *data_p, SM_BYTE *macro_p)
{
    SM_BYTE *p = strstr(data_p, macro_p); 
    if (!p) {return 0;}
    p += strlen(macro_p);
    long val = strtol(p, &p, 10);

    return val;
}

static void getVersionDate(
    SM_BYTE *data_p, SM_BYTE *macro_p, long date_p[3])
{
    date_p[0] = 0;
    date_p[1] = 0;
    date_p[2] = 0;

    SM_BYTE *p = strstr(data_p, macro_p); 
    if (!p) {return;}

    while (p[0] != '/' || p[1] != '*' || p[2] != '*') {p--;}
    while (*p && !isdigit(*p++)) {}
    if (!*p) {return;}

    p--;

    date_p[0] = strtol(p, &p, 10);
    p++;
    date_p[1] = strtol(p, &p, 10);
    p++;
    date_p[2] = strtol(p, &p, 10);
    p++;
}

SM_RESULT getVersionData(
    int version_p[4], long versionDates_pp[4][3])
{
    SM_BYTE *api_p   = "SM_API_VERSION"; 
    SM_BYTE *major_p = "SM_MAJOR_VERSION"; 
    SM_BYTE *minor_p = "SM_MINOR_VERSION";
    SM_BYTE *patch_p = "SM_PATCH_VERSION";

    SM_BYTE path_p[512]; 
    sprintf(path_p, "src/lib/Common/About.h");

    SM_BYTE *data_p = sm_getFileData(path_p, NULL);
    if (!data_p) {return SM_ERROR_BAD_STATE;}

    version_p[0] = getVersion(data_p, api_p);
    version_p[1] = getVersion(data_p, major_p);
    version_p[2] = getVersion(data_p, minor_p);
    version_p[3] = getVersion(data_p, patch_p);

    getVersionDate(data_p, api_p, versionDates_pp[0]);
    getVersionDate(data_p, major_p, versionDates_pp[1]);
    getVersionDate(data_p, minor_p, versionDates_pp[2]);
    getVersionDate(data_p, patch_p, versionDates_pp[3]);

    free(data_p);

    return SM_SUCCESS;
}

void getProjectVersion(
    sm_Runtime *Runtime_p, SM_BYTE *version_p)
{
    sm_ValueArray Stage = sm_getVariableValues(Runtime_p, "PROJ_STAGE");

    int versionNums_p[4];
    long versionDates_pp[4][3];
    getVersionData(versionNums_p, versionDates_pp);

    if (Stage.length) {
        SM_BYTE *stage_p = Stage.values_pp[0];
        sprintf(version_p, "v%d.%d.%d.%d %s", versionNums_p[0], versionNums_p[1], versionNums_p[2], versionNums_p[3], stage_p);
    }
    else {
        sprintf(version_p, "v%d.%d.%d.%d", versionNums_p[0], versionNums_p[1], versionNums_p[2], versionNums_p[3]);
    }
}

