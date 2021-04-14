// LICENSE NOTICE ==================================================================================

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

// INCLUDE =========================================================================================

#include "Documents.h"
#include "Util.h"

#include "../Parser/Functions.h"
#include "../Common/Macro.h"
#include SELFMAKE_FLOW
#include SELFMAKE_DEFAULT_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/types.h> 
#include <sys/stat.h>

// VERSION =========================================================================================

static long *selfmake_getLatestDate(
    long date1_p[3], long date2_p[3], long date3_p[3])
{
SELFMAKE_BEGIN()

    if (date1_p[0] > date2_p[0] && date1_p[0] > date3_p[0]) {
        SELFMAKE_END(date1_p)
    }
    else if (date1_p[0] < date2_p[0] || date1_p[0] < date3_p[0]) {
        SELFMAKE_END(NULL)
    }
    if (date1_p[1] > date2_p[1] && date1_p[1] > date3_p[1]) {
        SELFMAKE_END(date1_p)
    }
    else if (date1_p[1] < date2_p[1] || date1_p[1] < date3_p[1]) {
        SELFMAKE_END(NULL)
    }
    else if (date1_p[2] > date2_p[2] && date1_p[2] > date3_p[2]) {
        SELFMAKE_END(date1_p)
    }
    else if (date1_p[2] < date2_p[2] || date1_p[2] < date3_p[2]) {
        SELFMAKE_END(NULL)
    }

SELFMAKE_END(NULL)
}

static void selfmake_getVersionSum(
    long version_p[3])
{
SELFMAKE_BEGIN()

    version_p[0] = 0;
    version_p[1] = 0;
    version_p[2] = 0;

    for (int i = 0; i < SELFMAKE_MAX_LIBRARIES; ++i) 
    {
        selfmake_Library *Lib_p = &SELFMAKE_LIBRARIES_P[i];
        if (!Lib_p->name_p) {continue;}

        version_p[0] += Lib_p->major; 
        version_p[1] += Lib_p->minor;
        version_p[2] += Lib_p->patch;
    }

SELFMAKE_SILENT_END()
}

static selfmake_Library *selfmake_getLibrary(
    NH_BYTE *name_p)
{
SELFMAKE_BEGIN()

    for (int i = 0; i < SELFMAKE_MAX_LIBRARIES; ++i)
    {
        selfmake_Library *Lib_p = &SELFMAKE_LIBRARIES_P[i];
        if (!Lib_p->name_p) {continue;}

        if (!strcmp(Lib_p->name_p, name_p)) {SELFMAKE_END(Lib_p)}
    }

SELFMAKE_END(NULL)
}

void selfmake_getProjectVersion(
    NH_BYTE *version_p)
{
SELFMAKE_BEGIN()

    long versionSum_p[3];
    selfmake_getVersionSum(versionSum_p);
    selfmake_Library *Lib_p = selfmake_getLibrary("netzhaut");
    if (Lib_p) {
        sprintf(version_p, "v%d.%d.%d-%d.%d.%d pre-alpha-1", Lib_p->major, Lib_p->minor, Lib_p->patch, versionSum_p[0], versionSum_p[1], versionSum_p[2]);
    }

SELFMAKE_SILENT_END()
}

// INSERT ==========================================================================================

static NH_BYTE *selfmake_insertFullVersion(
    NH_BYTE *data_p, size_t size)
{
SELFMAKE_BEGIN()

    NH_BYTE *new_p = malloc(size + 10000);
    memset(new_p, 0, size + 10000);

    NH_BYTE *before_p = data_p;
    NH_BYTE *after_p = strstr(data_p, "SELFMAKE_INSERT_FULL_VERSION_BEGIN"); 
    if (!after_p) {SELFMAKE_END(NULL)}

    after_p += 42;
    *after_p = 0;

    sprintf(new_p, before_p);

    NH_BYTE version_p[255];
    selfmake_getProjectVersion(version_p);
    sprintf(new_p + strlen(new_p), "\n%s", version_p);

    after_p = strstr(after_p + 1, "<!-- SELFMAKE_INSERT_FULL_VERSION_END"); 
    if (!after_p) {SELFMAKE_END(NULL)}

    sprintf(new_p + strlen(new_p), "\n%s", after_p);

    free(data_p);

SELFMAKE_END(new_p)
}

static NH_BYTE *selfmake_insertChangelogs(
    NH_BYTE *data_p, size_t size)
{
SELFMAKE_BEGIN()

    NH_BYTE *new_p = malloc(size + 10000);
    memset(new_p, 0, size + 10000);

    NH_BYTE *before_p = data_p;
    NH_BYTE *after_p = strstr(data_p, "SELFMAKE_INSERT_CHANGELOGS_BEGIN"); 
    if (!after_p) {SELFMAKE_END(NULL)}
    
    after_p += 40;
    *after_p = 0;

    sprintf(new_p, before_p);

    for (int i = 0; i < SELFMAKE_MAX_LIBRARIES; ++i) 
    {
        selfmake_Library *Lib_p = &SELFMAKE_LIBRARIES_P[i];
        if (!Lib_p->name_p || !strcmp(Lib_p->name_p, "nhexternal")) {continue;}
        
        long *date_p = selfmake_getLatestDate(Lib_p->majorDate_p, Lib_p->minorDate_p, Lib_p->patchDate_p);
        if (!date_p) {date_p = selfmake_getLatestDate(Lib_p->minorDate_p, Lib_p->majorDate_p, Lib_p->patchDate_p);}
        if (!date_p) {date_p = selfmake_getLatestDate(Lib_p->patchDate_p, Lib_p->majorDate_p, Lib_p->minorDate_p);}
        if (!date_p) {date_p = Lib_p->majorDate_p;} 
        
        NH_BYTE line_p[512];
        sprintf(line_p, "\n%d-%02d-%02d <a href=\"Dev/HTML/group__%sChangelog.html\">%s v%d.%d.%d</a><br>", date_p[0], date_p[1], date_p[2], Lib_p->name_p, Lib_p->name_p, Lib_p->major, Lib_p->minor, Lib_p->patch);

        sprintf(new_p + strlen(new_p), line_p);
    }

    after_p = strstr(after_p + 1, "<!-- SELFMAKE_INSERT_CHANGELOGS_END"); 
    if (!after_p) {SELFMAKE_END(NULL)}

    sprintf(new_p + strlen(new_p), "\n%s", after_p);

    free(data_p);

SELFMAKE_END(new_p)
}

static NH_BYTE *selfmake_insertNews(
    NH_BYTE *data_p, size_t size)
{
SELFMAKE_BEGIN()

    NH_BYTE *new_p = malloc(size + 10000);
    memset(new_p, 0, size + 10000);

    NH_BYTE *before_p = data_p;
    NH_BYTE *after_p = strstr(data_p, "SELFMAKE_INSERT_NEWS_BEGIN"); 
    if (!after_p) {SELFMAKE_END(NULL)}
    
    after_p += 40;
    *after_p = 0;

    sprintf(new_p, before_p);

    for (int i = 0; i < SELFMAKE_MAX_NEWS; ++i) 
    {
        if (SELFMAKE_NEWS_PP[i]) {
            sprintf(new_p + strlen(new_p), "\n");
            sprintf(new_p + strlen(new_p), SELFMAKE_NEWS_PP[i]);
        }
        else {break;}
    }

    after_p = strstr(after_p + 1, "<!-- SELFMAKE_INSERT_NEWS_END"); 
    if (!after_p) {SELFMAKE_END(NULL)}

    sprintf(new_p + strlen(new_p), "\n%s", after_p);

    free(data_p);

SELFMAKE_END(new_p)
}

// GENERATE ========================================================================================

SELFMAKE_RESULT selfmake_generateHomepage()
{
SELFMAKE_BEGIN()

    SELFMAKE_CHECK(selfmake_executeTopLevelFunctions())

    long size;
    NH_BYTE *data_p = selfmake_getFileData("external/DOWNLOADS/docs/docs/index.html", &size);
    if (!data_p) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

    data_p = selfmake_insertChangelogs(data_p, size);
    data_p = selfmake_insertFullVersion(data_p, size);
    data_p = selfmake_insertNews(data_p, size);

    SELFMAKE_CHECK(selfmake_writeCharsToFile("external/DOWNLOADS/docs/docs/index.html", data_p))

    free(data_p);

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_generateFooter()
{
SELFMAKE_BEGIN()

    SELFMAKE_CHECK(selfmake_executeTopLevelFunctions())

    long size;
    NH_BYTE *data_p = selfmake_getFileData("external/DOWNLOADS/docs/docs/DoxygenTheme/Footer1.html", &size);
    if (!data_p) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}
    data_p = selfmake_insertFullVersion(data_p, size);
    SELFMAKE_CHECK(selfmake_writeCharsToFile("external/DOWNLOADS/docs/docs/DoxygenTheme/Footer1.html", data_p))
    free(data_p);

    data_p = selfmake_getFileData("external/DOWNLOADS/docs/docs/DoxygenTheme/Footer2.html", &size);
    if (!data_p) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}
    data_p = selfmake_insertFullVersion(data_p, size);
    SELFMAKE_CHECK(selfmake_writeCharsToFile("external/DOWNLOADS/docs/docs/DoxygenTheme/Footer2.html", data_p))
    free(data_p);

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

