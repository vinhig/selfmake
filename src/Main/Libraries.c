// LICENSE NOTICE ==================================================================================

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

// INCLUDE =========================================================================================

#include "Libraries.h"
#include "Util.h"
#include "Unicode.h"
#include "Main.h"
#include "WebIDL.h"

#include "../UI/Message.h"
#include "../Common/Macro.h"

#include SELFMAKE_FLOW
#include SELFMAKE_CUSTOM_CHECK

#include "../../../lib/netzhaut/netzhaut.h"
#include "../../../lib/nhloader/Common/About.h"
#include "../../../lib/nhcore/Common/About.h"
#include "../../../lib/nhtty/Common/About.h"
#include "../../../lib/nhhtml/Common/About.h"
#include "../../../lib/nhdom/Common/About.h"
#include "../../../lib/nhwebidl/Common/About.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>

// CREATE ==========================================================================================

static SELFMAKE_RESULT selfmake_createLibrary(
    NH_BYTE *wrkDir_p, NH_BYTE *objPath_p, NH_BYTE *extra_p, NH_BYTE *libName_p, int major, int minor, int patch)
{
SELFMAKE_BEGIN()

#include SELFMAKE_DEFAULT_CHECK

    chdir(objPath_p);

#ifdef __unix__

    int index = 0;
    NH_BYTE objects_p[16384] = {'\0'};

    DIR *dir;
    struct dirent *ent;
    
    if ((dir = opendir(objPath_p)) != NULL) 
    {
        while ((ent = readdir(dir)) != NULL) {
            for (int i = 0; ent->d_name[i] != '\0' && ent->d_name[0] != '.'; ++i) {
                objects_p[index++] = ent->d_name[i];
            }
            objects_p[index++] = ' ';
        }
        closedir(dir);
    } 
    else {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_CANT_OPEN_DIR)}

    NH_BYTE libPath_p[256] = {'\0'};
    sprintf(libPath_p, "../../lib%s.so.%d.%d.%d", libName_p, major, minor, patch);
    NH_BYTE symPath1_p[256] = {'\0'};
    sprintf(symPath1_p, "../../lib%s.so.%d", libName_p, major);
    NH_BYTE symPath2_p[256] = {'\0'};
    sprintf(symPath2_p, "../../lib%s.so", libName_p);
  
    SELFMAKE_CHECK(selfmake_createSharedLibraryUsingGCC(objects_p, libPath_p, extra_p))

    memset(libPath_p, 0, 256); 
    sprintf(libPath_p, "lib%s.so.%d.%d.%d", libName_p, major, minor, patch);
    selfmake_createSymLink(libPath_p, symPath1_p, NH_FALSE);
    selfmake_createSymLink(libPath_p, symPath2_p, NH_FALSE);

#endif 

    chdir(wrkDir_p);

#include SELFMAKE_CUSTOM_CHECK

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

// INSTALL_ALL =========================================================================================

static SELFMAKE_RESULT selfmake_installLibrary(
    NH_BYTE *libName_p, int major, int minor, int patch)
{
SELFMAKE_BEGIN()

    NH_BYTE wrkDir_p[2048] = {'\0'};
    SELFMAKE_CHECK_NULL(SELFMAKE_ERROR_GET_WORK_DIRECTORY, getcwd(wrkDir_p, 2048))

    NH_BYTE projDir_p[2048] = {'\0'};
    SELFMAKE_CHECK(SELFMAKE_ERROR_GET_PROJECT_DIRECTORY, selfmake_getProjectDir(projDir_p, 2048))

    chdir(projDir_p);

    NH_BYTE libPath_p[512] = {'\0'};
    sprintf(libPath_p, "lib/lib%s.so.%d.%d.%d", libName_p, major, minor, patch);

    SELFMAKE_CHECK(SELFMAKE_ERROR_COPY_FAILED, selfmake_copy(libPath_p, "/usr/local/lib", NH_FALSE, NH_TRUE))

    chdir("/usr/local/lib");

    NH_BYTE cpyPath_p[256] = {'\0'};
    sprintf(cpyPath_p, "lib%s.so.%d.%d.%d", libName_p, major, minor, patch);
    NH_BYTE symPath1_p[256] = {'\0'};
    sprintf(symPath1_p, "lib%s.so.%d", libName_p, major);
    NH_BYTE symPath2_p[256] = {'\0'};
    sprintf(symPath2_p, "lib%s.so", libName_p);

    selfmake_createSymLink(cpyPath_p, symPath1_p, NH_TRUE);
    selfmake_createSymLink(cpyPath_p, symPath2_p, NH_TRUE);

    chdir(wrkDir_p);

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

// PREPARE BUILD ===================================================================================

static SELFMAKE_RESULT selfmake_createIncludeFileUsingXXD(
    const NH_BYTE *in_p, const NH_BYTE *out_p, const NH_BYTE *name_p)
{
SELFMAKE_BEGIN()

    selfmake_messagef("CREATE FILE %s/%s", SELFMAKE_PROJECT_DIR_P, out_p);

    NH_BYTE command_p[1024] = {'\0'};
    sprintf(command_p, "./external/xxd/xxd -i %s/%s %s/%s %s", SELFMAKE_PROJECT_DIR_P, in_p, SELFMAKE_PROJECT_DIR_P, out_p, name_p);

    int status = system(command_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_XXD_EXECUTION_FAILED)}

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

// BUILD ===========================================================================================

static void selfmake_getObjectFileName(
    selfmake_Source *Source_p, NH_BYTE *name_p)
{
SELFMAKE_BEGIN()

    NH_BYTE path_p[255] = {0};
    strcpy(path_p, Source_p->path_p);

    int index = strlen(path_p) - 1;
    while (path_p[index] != '/') {index--;}

    NH_BYTE fileName_p[255] = {0};
    strcpy(fileName_p, path_p + index + 1);

    int tmp = index - 1;
    while (path_p[tmp] != '/' && tmp > 0) {tmp--;}

    if (tmp > 0) {
        path_p[index] = '\0';
        sprintf(name_p, "%s%s.o", path_p + tmp, fileName_p);
    }
    else {
        sprintf(name_p, "%s.o", fileName_p);
    }

SELFMAKE_SILENT_END()
}

static SELFMAKE_RESULT selfmake_compileLibraryFiles(
    char *wrkDir_p, selfmake_Library *Library_p)
{
SELFMAKE_BEGIN()

    NH_BYTE objPath_p[255] = {0};
    sprintf(objPath_p, "lib/objects/%s", Library_p->name_p);

    SELFMAKE_CHECK(SELFMAKE_ERROR_CANT_CREATE_BIN_OBJECT_DIRECTORY, selfmake_createDir("lib/objects/"))
    SELFMAKE_CHECK(SELFMAKE_ERROR_CANT_CREATE_BIN_OBJECT_DIRECTORY, selfmake_createDir(objPath_p))

    NH_BYTE path_p[2048] = {'\0'};

    NH_BOOL external = !strcmp(Library_p->name_p, "nhexternal");

    if (external) {
        sprintf(path_p, "%s%s", SELFMAKE_PROJECT_DIR_P, "/external");
    }
    else {
        sprintf(path_p, "%s%s", SELFMAKE_PROJECT_DIR_P, "/src/lib");
    }

    chdir(path_p);

    for (int i = 0; i < SELFMAKE_MAX_SOURCES; ++i) 
    {
        selfmake_Source *Source_p = &SELFMAKE_SOURCES_P[i];

        if (Source_p->Library_p == Library_p) 
        {
            NH_BYTE outPath_p[255] = {0};
            NH_BYTE fileName_p[255] = {0};
            selfmake_getObjectFileName(Source_p, fileName_p);
            sprintf(outPath_p, external ? "../lib/objects/%s/%s" : "../../lib/objects/%s/%s", Library_p->name_p, fileName_p);
            NH_BYTE empty = 0;

            SELFMAKE_CHECK(SELFMAKE_ERROR_CANT_CREATE_OBJECT_FILE, 
                selfmake_createPICObjectFileUsingGCC(Source_p->path_p, outPath_p, Library_p->compileArgs_p ? Library_p->compileArgs_p : &empty))
        }
    }

    chdir(wrkDir_p);

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_buildLibrary(
    selfmake_Library *Library_p, NH_BOOL install) 
{
SELFMAKE_BEGIN()

    selfmake_operationf("BUILD %s LIBRARY", Library_p->name_p);

    NH_BYTE wrkDir_p[2048] = {'\0'};
    SELFMAKE_CHECK_NULL(SELFMAKE_ERROR_GET_WORK_DIRECTORY, getcwd(wrkDir_p, 2048))

    if (!strcmp(Library_p->name_p, "nhwebidl")) {
        chdir(SELFMAKE_PROJECT_DIR_P);
        SELFMAKE_CHECK(SELFMAKE_ERROR_BAD_STATE, selfmake_processWebIDL())
        chdir(wrkDir_p);
    }
    else if (!strcmp(Library_p->name_p, "nhcore")) {
        SELFMAKE_CHECK(SELFMAKE_ERROR_BAD_STATE, selfmake_createUnicodeLookup())
    }
    else if (!strcmp(Library_p->name_p, "nhcss")) {
        chdir(SELFMAKE_PROJECT_DIR_P);
        SELFMAKE_CHECK(SELFMAKE_ERROR_BAD_STATE, selfmake_createIncludeFileUsingXXD("src/lib/nhcss/Common/Data/default.css", "src/lib/nhcss/Common/Data/default.css.h", "default_css_h"))
        chdir(wrkDir_p);
    }
    
    chdir(SELFMAKE_PROJECT_DIR_P);

    NH_BYTE empty = 0;
    NH_BYTE objPath_p[2048] = {'\0'};
    sprintf(objPath_p, "%s%s%s", SELFMAKE_PROJECT_DIR_P, "/lib/objects/", Library_p->name_p);

    SELFMAKE_CHECK(SELFMAKE_ERROR_CANT_CREATE_OBJECTS, selfmake_compileLibraryFiles(
        wrkDir_p, Library_p
    ))
    SELFMAKE_CHECK(SELFMAKE_ERROR_CANT_CREATE_LIBRARY, selfmake_createLibrary(
        wrkDir_p, objPath_p, Library_p->linkArgs_p ? Library_p->linkArgs_p : &empty, Library_p->name_p, 0, 0, 0 
    ))

    if (install) {
        SELFMAKE_CHECK(SELFMAKE_ERROR_BAD_STATE, selfmake_installLibrary(
            Library_p->name_p, 0, 0, 0 
        ))
    }

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

