// LICENSE NOTICE ==================================================================================

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

// INCLUDE =========================================================================================

#include "Message.h"
#include "GUI.h"

#include "../Main/Util.h"
#include "../Common/Macro.h"
#include "../Common/Version.h"
#include SELFMAKE_FLOW
#include SELFMAKE_DEFAULT_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// DATA ============================================================================================

NH_BOOL SELFMAKE_QUIET = NH_FALSE;
static unsigned int messages = 0;

// HELP ============================================================================================

SELFMAKE_RESULT selfmake_printHelp()
{
SELFMAKE_BEGIN()

#include SELFMAKE_CUSTOM_CHECK

    NH_BYTE projDir_p[2048] = {'\0'};
    if (selfmake_getProjectDir(projDir_p, 2048) != SELFMAKE_SUCCESS) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_ERROR_BAD_STATE)}

#include SELFMAKE_DEFAULT_CHECK

    NH_BYTE helpPath_p[2048] = {'\0'};
    sprintf(helpPath_p, "%s%s", projDir_p, "/data/installer/help.txt");
    NH_BYTE *help_p = selfmake_getFileData(helpPath_p, NULL); 

    printf("\n%s\n", help_p);

    free(help_p);

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

// VERSION =========================================================================================

SELFMAKE_RESULT selfmake_printVersion()
{
SELFMAKE_BEGIN()

    puts("");
    puts("  NETZHAUT-INSTALLER VERSION");
    puts("");

#ifdef __unix__
    NH_BYTE *target_p = "LINUX";
#elif defined(_WIN32)
    NH_BYTE *target_p = "WINDOWS";
#endif

    printf("  %s %d.%d.%d\n", target_p, SELFMAKE_MAJOR_VERSION, SELFMAKE_MINOR_VERSION, SELFMAKE_PATCH_VERSION);
    printf("  %s %s\n", __DATE__, __TIME__);

    switch (__STDC_VERSION__)
    {
        case 199409L : puts("  GNU C95"); break;
        case 199901L : puts("  GNU C99"); break;
        case 201112L : puts("  GNU C11"); break;
        case 201710L : puts("  GNU C18"); break;
    }

    puts("");

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

// MESSAGE =========================================================================================

SELFMAKE_RESULT selfmake_operationf(
    NH_BYTE *format_p, ...)
{
SELFMAKE_BEGIN()

    if (SELFMAKE_QUIET) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)}

    if (SELFMAKE_GUI)
    {

    }
    else 
    {
        va_list args;
        va_start(args, format_p);
    
        NH_BYTE message_p[1024] = {'\0'};
        vsprintf(message_p, format_p, args);
    
        va_end(args);
    
        if (messages == 0) {
            printf("nhmake: -> \e[1;32m%s\e[0m\nnhmake:\n", message_p);
        }
        else {printf("nhmake:\nnhmake: -> \e[1;32m%s\e[0m\nnhmake:\n", message_p);}
    
        messages = 0;
    }

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_messagef(
    NH_BYTE *format_p, ...)
{
SELFMAKE_BEGIN()

    if (SELFMAKE_QUIET) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)}

    if (SELFMAKE_GUI)
    {

    }
    else 
    {
        va_list args;
        va_start(args, format_p);

        NH_BYTE message_p[1024] = {'\0'};
        vsprintf(message_p, format_p, args);

        va_end(args);

        printf("nhmake: %s\n", message_p);
        messages++;
    }

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_noticef(
    NH_BYTE *format_p, ...)
{
SELFMAKE_BEGIN()

    if (SELFMAKE_QUIET) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)}

    if (SELFMAKE_GUI)
    {

    }
    else 
    {
        va_list args;
        va_start(args, format_p);

        NH_BYTE message_p[1024] = {'\0'};
        vsprintf(message_p, format_p, args);

        va_end(args);

        printf("nhmake: \e[1;35mNOTICE:\e[0m %s\n", message_p);
        messages++;
    }

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_externalMessage(
    NH_BYTE *prepend_p, NH_BYTE *message_p)
{
SELFMAKE_BEGIN()

    if (SELFMAKE_QUIET) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)}

    if (SELFMAKE_GUI)
    {

    }
    else 
    {
        NH_BYTE *p = message_p;
        for (int i = 0; i < strlen(message_p); ++i) 
        {
            if (i + 1 == strlen(message_p)) {i++;}

            if (message_p[i] == '\n' || message_p[i] == '\0') 
            {
                NH_BYTE replace = message_p[i];
                message_p[i] = '\0';
                printf("%s%s\n", prepend_p, p);
                p = &message_p[i + 1];
                message_p[i] = replace;
                messages++;
            }
        }
    }

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

SELFMAKE_RESULT selfmake_exitMessage(
    SELFMAKE_RESULT result)
{
SELFMAKE_BEGIN()

    if (SELFMAKE_QUIET) {SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)}

    if (SELFMAKE_GUI)
    {

    }
    else 
    {
        if (result) {
            printf("nhmake: -> \e[1;31mERROR\e[0m %s\n", SELFMAKE_RESULTS_PP[result]);
            printf("nhmake: -> \e[1;31mEXECUTION ERROR\e[0m -> \e[1;31mEXITING\e[0m\n");
        }
        else {printf("nhmake:\nnhmake: -> \e[1;32mEXECUTION SUCCESS\e[0m -> \e[1;32mEXITING\e[0m\n");}
    }

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

