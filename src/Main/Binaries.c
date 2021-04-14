// LICENSE NOTICE ==================================================================================

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

// INCLUDE =========================================================================================

#include "Binaries.h"
#include "Util.h"
#include "Main.h"

#include "../Common/Macro.h"

#include SELFMAKE_FLOW
#include SELFMAKE_CUSTOM_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// BUILD BINARY ====================================================================================

SELFMAKE_RESULT selfmake_buildnhterminal()
{
SELFMAKE_BEGIN()

    char projDir_p[2048] = {'\0'};
    SELFMAKE_CHECK(SELFMAKE_ERROR_GET_PROJECT_DIRECTORY, selfmake_getProjectDir(projDir_p, 2048))

    // set -no-pie because of https://stackoverflow.com/questions/41398444/gcc-creates-mime-type-application-x-sharedlib-instead-of-application-x-applicati
    static char command_p[2048] = {'\0'};
    sprintf(command_p, "gcc -std=gnu99 -Wl,-rpath=%s/lib:/usr/local/lib -o%s/bin/nhterminal -no-pie -L%s/lib -lnetzhaut %s/src/bin/nhterminal/Terminal.c", projDir_p, projDir_p, projDir_p, projDir_p);

    int status = system(command_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_GCC_EXECUTION_FAILED)}

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_buildnhwebbrowser()
{
SELFMAKE_BEGIN()

    char projDir_p[2048] = {'\0'};
    SELFMAKE_CHECK(SELFMAKE_ERROR_GET_PROJECT_DIRECTORY, selfmake_getProjectDir(projDir_p, 2048))

    // set -no-pie because of https://stackoverflow.com/questions/41398444/gcc-creates-mime-type-application-x-sharedlib-instead-of-application-x-applicati
    static char command_p[2048] = {'\0'};
    sprintf(command_p, "gcc -std=gnu99 -Wl,-rpath=%s/lib:/usr/local/lib -o%s/bin/nhwebbrowser -no-pie -L%s/lib -lnetzhaut %s/src/bin/nhwebbrowser/WebBrowser.c", projDir_p, projDir_p, projDir_p, projDir_p);

    int status = system(command_p);
    if (WEXITSTATUS(status) || WIFSIGNALED(status)) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_GCC_EXECUTION_FAILED)}

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

// INSTALL BINARY ==================================================================================

SELFMAKE_RESULT selfmake_installnhterminal()
{
SELFMAKE_BEGIN()

    char wrkDir_p[2048] = {'\0'};
    SELFMAKE_CHECK_NULL(SELFMAKE_ERROR_GET_WORK_DIRECTORY, getcwd(wrkDir_p, 2048))

    char projDir_p[2048] = {'\0'};
    SELFMAKE_CHECK(SELFMAKE_ERROR_GET_PROJECT_DIRECTORY, selfmake_getProjectDir(projDir_p, 2048))

    chdir(projDir_p);

    const char *homedir_p = selfmake_getHomeDir();
    char dest_p[512] = {'\0'};

    sprintf(dest_p, "%s/.local/share/applications/", homedir_p);
    SELFMAKE_CHECK(SELFMAKE_ERROR_COPY_FAILED, selfmake_copy("src/bin/nhterminal/Common/Data/nhterminal.desktop", dest_p, NH_FALSE, NH_FALSE))
    SELFMAKE_CHECK(SELFMAKE_ERROR_COPY_FAILED, selfmake_copy("bin/nhterminal", "/usr/local/bin", NH_FALSE, NH_TRUE))

    chdir(wrkDir_p);

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_installnhwebbrowser()
{
SELFMAKE_BEGIN()

    char wrkDir_p[2048] = {'\0'};
    SELFMAKE_CHECK_NULL(SELFMAKE_ERROR_GET_WORK_DIRECTORY, getcwd(wrkDir_p, 2048))

    char projDir_p[2048] = {'\0'};
    SELFMAKE_CHECK(SELFMAKE_ERROR_GET_PROJECT_DIRECTORY, selfmake_getProjectDir(projDir_p, 2048))

    chdir(projDir_p);

    const char *homedir_p = selfmake_getHomeDir();
    char dest_p[512] = {'\0'};

    sprintf(dest_p, "%s/.local/share/applications/", homedir_p);
    SELFMAKE_CHECK(SELFMAKE_ERROR_COPY_FAILED, selfmake_copy("src/bin/nhwebbrowser/Common/Data/nhwebbrowser.desktop", dest_p, NH_FALSE, NH_FALSE))
    SELFMAKE_CHECK(SELFMAKE_ERROR_COPY_FAILED, selfmake_copy("bin/nhwebbrowser", "/usr/local/bin", NH_FALSE, NH_TRUE))

    chdir(wrkDir_p);

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

// INSTALL LOGO ====================================================================================

SELFMAKE_RESULT selfmake_installLogo()
{
SELFMAKE_BEGIN()

    char wrkDir_p[2048] = {'\0'};
    SELFMAKE_CHECK_NULL(SELFMAKE_ERROR_GET_WORK_DIRECTORY, getcwd(wrkDir_p, 2048))

    char projDir_p[2048] = {'\0'};
    SELFMAKE_CHECK(SELFMAKE_ERROR_GET_PROJECT_DIRECTORY, selfmake_getProjectDir(projDir_p, 2048))

    chdir(projDir_p);

    const char *homedir_p = selfmake_getHomeDir();
    char dest_p[512] = {'\0'};

    sprintf(dest_p, "%s/.local/share/icons/hicolor/32x32/apps", homedir_p);
    SELFMAKE_CHECK(SELFMAKE_ERROR_COPY_FAILED, selfmake_copy("docs/Logo/32x32/netzhaut.png", dest_p, NH_FALSE, NH_TRUE))

    memset(dest_p, '\0', sizeof(char) * 512);
    sprintf(dest_p, "%s/.local/share/icons/hicolor/64x64/apps", homedir_p);
    SELFMAKE_CHECK(SELFMAKE_ERROR_COPY_FAILED, selfmake_copy("docs/Logo/64x64/netzhaut.png", dest_p, NH_FALSE, NH_TRUE))

    memset(dest_p, '\0', sizeof(char) * 512);
    sprintf(dest_p, "%s/.local/share/icons/hicolor/128x128/apps", homedir_p);
    SELFMAKE_CHECK(SELFMAKE_ERROR_COPY_FAILED, selfmake_copy("docs/Logo/128x128/netzhaut.png", dest_p, NH_FALSE, NH_TRUE))

    memset(dest_p, '\0', sizeof(char) * 512);
    sprintf(dest_p, "%s/.local/share/icons/hicolor/256x256/apps", homedir_p);
    SELFMAKE_CHECK(SELFMAKE_ERROR_COPY_FAILED, selfmake_copy("docs/Logo/256x256/netzhaut.png", dest_p, NH_FALSE, NH_TRUE))

    memset(dest_p, '\0', sizeof(char) * 512);
    sprintf(dest_p, "%s/.local/share/icons/hicolor/512x512/apps", homedir_p);
    SELFMAKE_CHECK(SELFMAKE_ERROR_COPY_FAILED, selfmake_copy("docs/Logo/512x512/netzhaut.png", dest_p, NH_FALSE, NH_TRUE))

    chdir(wrkDir_p);

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

