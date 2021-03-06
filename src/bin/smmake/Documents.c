// LICENSE NOTICE ==================================================================================

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

// INCLUDE =========================================================================================

#include "Documents.h"
#include "Version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#include <sys/types.h> 
#include <sys/stat.h>

// HELPER ==========================================================================================

static long *getLatestDate(
    long date1_p[3], long date2_p[3], long date3_p[3])
{
    if (date1_p[0] > date2_p[0] && date1_p[0] > date3_p[0]) {
        return date1_p;
    }
    else if (date1_p[0] < date2_p[0] || date1_p[0] < date3_p[0]) {
        return NULL;
    }
    if (date1_p[1] > date2_p[1] && date1_p[1] > date3_p[1]) {
        return date1_p;
    }
    else if (date1_p[1] < date2_p[1] || date1_p[1] < date3_p[1]) {
        return NULL;
    }
    else if (date1_p[2] > date2_p[2] && date1_p[2] > date3_p[2]) {
        return date1_p;
    }
    else if (date1_p[2] < date2_p[2] || date1_p[2] < date3_p[2]) {
        return NULL;
    }

    return NULL;
}

// INSERT ==========================================================================================

static SM_BYTE *insertFullVersion(
    sm_Runtime *Runtime_p, SM_BYTE *data_p, size_t size)
{
    SM_BYTE *new_p = malloc(size + 10000);
    memset(new_p, 0, size + 10000);

    SM_BYTE *before_p = data_p;
    SM_BYTE *after_p = strstr(data_p, "SM_MAKE_INSERT_FULL_VERSION_BEGIN"); 
    if (!after_p) {return NULL;}

    after_p += 37;
    *after_p = 0;

    sprintf(new_p, before_p);

    SM_BYTE version_p[255];
    getProjectVersion(Runtime_p, version_p);
    sprintf(new_p + strlen(new_p), "\n%s", version_p);

    after_p = strstr(after_p + 1, "<!-- SM_MAKE_INSERT_FULL_VERSION_END"); 
    if (!after_p) {return NULL;}

    sprintf(new_p + strlen(new_p), "\n%s", after_p);

    free(data_p);

    return new_p;
}

static SM_BYTE *insertChangelogs(
    sm_Runtime *Runtime_p, SM_BYTE *data_p, size_t size)
{
    SM_BYTE *new_p = malloc(size + 10000);
    memset(new_p, 0, size + 10000);

    SM_BYTE *before_p = data_p;
    SM_BYTE *after_p = strstr(data_p, "SM_MAKE_INSERT_CHANGELOGS_BEGIN"); 
    if (!after_p) {return NULL;}
    
    after_p += 35;
    *after_p = 0;

    sprintf(new_p, before_p);
        
    int versionNums_p[4];
    long versionDates_pp[4][3];
    getVersionData(versionNums_p, versionDates_pp);

    long *date_p = getLatestDate(versionDates_pp[1], versionDates_pp[2], versionDates_pp[3]);
    if (!date_p) {date_p = getLatestDate(versionDates_pp[2], versionDates_pp[1], versionDates_pp[3]);}
    if (!date_p) {date_p = getLatestDate(versionDates_pp[3], versionDates_pp[1], versionDates_pp[2]);}
    if (!date_p) {date_p = versionDates_pp[1];} 

    SM_BYTE line_p[512];
    sprintf(line_p, "\n%d-%02d-%02d <a href=\"impl/html/group__selfmakeChangelog.html#v%d.%d.%d.%d\">selfmakeLibrary v%d.%d.%d.%d</a><br>\n", 
        date_p[0], date_p[1], date_p[2], versionNums_p[0], versionNums_p[1], versionNums_p[2], versionNums_p[3], 
        versionNums_p[0], versionNums_p[1], versionNums_p[2], versionNums_p[3]);

    sprintf(new_p + strlen(new_p), line_p);

    sm_ValueArray LangVer = sm_getVariableValues(Runtime_p, "LANG_VER");
    sm_ValueArray LangDate = sm_getVariableValues(Runtime_p, "LANG_DATE");

    memset(line_p, 0, 512);
    sprintf(line_p, "\n%s <a href=\"lang/html/changelog.html#v%s\">selfmakeLanguage v%s</a><br>", 
        LangDate.values_pp[0], LangVer.values_pp[0], LangVer.values_pp[0]);

    sprintf(new_p + strlen(new_p), line_p);

    after_p = strstr(after_p + 1, "<!-- SM_MAKE_INSERT_CHANGELOGS_END"); 
    if (!after_p) {return NULL;}

    sprintf(new_p + strlen(new_p), "\n%s", after_p);

    free(data_p);

    return new_p;
}

static SM_BYTE *insertNews(
    sm_Runtime *Runtime_p, SM_BYTE *data_p, size_t size)
{
    SM_BYTE *new_p = malloc(size + 10000);
    memset(new_p, 0, size + 10000);

    SM_BYTE *before_p = data_p;
    SM_BYTE *after_p = strstr(data_p, "SM_MAKE_INSERT_NEWS_BEGIN"); 
    if (!after_p) {return NULL;}
    
    after_p += 29;
    *after_p = 0;

    sprintf(new_p, before_p);

    sm_ValueArray News = sm_getVariableValues(Runtime_p, "NEWS");

    for (int i = 0; i < News.length; ++i) 
    {
        sprintf(new_p + strlen(new_p), "\n");
        sprintf(new_p + strlen(new_p), News.values_pp[i]);
    }

    after_p = strstr(after_p + 1, "<!-- SM_MAKE_INSERT_NEWS_END"); 
    if (!after_p) {return NULL;}

    sprintf(new_p + strlen(new_p), "\n%s", after_p);

    free(data_p);

    return new_p;
}

static SM_BYTE *insertTime(
    sm_Runtime *Runtime_p, SM_BYTE *data_p, size_t size)
{
    SM_BYTE *new_p = malloc(size + 10000);
    memset(new_p, 0, size + 10000);

    SM_BYTE *before_p = data_p;
    SM_BYTE *after_p = strstr(data_p, "SM_MAKE_INSERT_TIME_BEGIN"); 
    if (!after_p) {return NULL;}
    
    after_p += 29;
    *after_p = 0;

    sprintf(new_p, before_p);

    time_t t; struct tm* tm;
    SM_BYTE date_p[11];
    
    time(&t);
    tm = localtime(&t);
    
    strftime(date_p, sizeof(date_p), "%Y-%m-%d", tm);
    sprintf(new_p + strlen(new_p), "\n");
    sprintf(new_p + strlen(new_p), date_p);

    after_p = strstr(after_p + 1, "<!-- SM_MAKE_INSERT_TIME_END"); 
    if (!after_p) {return NULL;}

    sprintf(new_p + strlen(new_p), "\n%s", after_p);

    free(data_p);

    return new_p;
}

// GENERATE ========================================================================================

SM_RESULT generateHomepage(
    sm_Runtime *Runtime_p)
{
    long size;
    SM_BYTE *data_p = sm_getFileData("external/selfmake.netzwerkz.org/docs/index.html", &size);
    if (!data_p) {return SM_ERROR_BAD_STATE;}

    data_p = insertChangelogs(Runtime_p, data_p, size);
    data_p = insertFullVersion(Runtime_p, data_p, size);
    data_p = insertNews(Runtime_p, data_p, size);
    data_p = insertTime(Runtime_p, data_p, size);

    sm_writeBytesToFile("external/selfmake.netzwerkz.org/docs/index.html", data_p);

    free(data_p);

    return SM_SUCCESS;
}

SM_RESULT generateFooter(
    sm_Runtime *Runtime_p)
{
    long size;
    SM_BYTE *data_p = sm_getFileData("external/selfmake.netzwerkz.org/docs/theme/footer1.html", &size);
    if (!data_p) {return SM_ERROR_BAD_STATE;}
    data_p = insertFullVersion(Runtime_p, data_p, size);
    sm_writeBytesToFile("external/selfmake.netzwerkz.org/docs/theme/footer1.html", data_p);
    free(data_p);

    data_p = sm_getFileData("external/selfmake.netzwerkz.org/docs/theme/footer2.html", &size);
    if (!data_p) {return SM_ERROR_BAD_STATE;}
    data_p = insertFullVersion(Runtime_p, data_p, size);
    sm_writeBytesToFile("external/selfmake.netzwerkz.org/docs/theme/footer2.html", data_p);
    free(data_p);

    return SM_SUCCESS;
}

