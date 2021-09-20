// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Message.h"
#include "GUI.h"

#include "../Core/Thread.h"
#include "../Core/Runtime.h"
#include "../Core/Utils.h"
#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// DATA ============================================================================================

static unsigned int messages = 0;

// MESSAGE =========================================================================================

SM_RESULT sm_operationf(
    SM_BYTE *format_p, ...)
{
SM_BEGIN()

    sm_Thread *Thread_p = _sm_getThread();
    sm_Runtime *Runtime_p = Thread_p && Thread_p->Runtime_p ? Thread_p->Runtime_p : &SM_DEFAULT_RUNTIME;
    SM_BYTE *name_p = sm_getVariable(&Runtime_p->VariableArray, "NAME")->values_pp[0];

    if (Runtime_p->quiet) {SM_DIAGNOSTIC_END(SM_SUCCESS)}

    if (Runtime_p->GUI)
    {

    }
    else 
    {
        va_list args;
        va_start(args, format_p);
    
        SM_BYTE message_p[4096] = {'\0'};
        vsprintf(message_p, format_p, args);
    
        va_end(args);
    
        if (messages == 0) {
            printf("%s: -> \e[1;32m%s\e[0m\n%s:\n", name_p, message_p, name_p);
        }
        else {printf("%s:\n%s: -> \e[1;32m%s\e[0m\n%s:\n", name_p, name_p, message_p, name_p);}
    
        messages = 0;
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_RESULT sm_messagef(
    SM_BYTE *format_p, ...)
{
SM_BEGIN()

    sm_Thread *Thread_p = _sm_getThread();
    sm_Runtime *Runtime_p = Thread_p && Thread_p->Runtime_p ? Thread_p->Runtime_p : &SM_DEFAULT_RUNTIME;
    SM_BYTE *name_p = sm_getVariable(&Runtime_p->VariableArray, "NAME")->values_pp[0];

    if (Runtime_p->quiet) {SM_DIAGNOSTIC_END(SM_SUCCESS)}

    if (Runtime_p->GUI)
    {

    }
    else 
    {
        va_list args;

        va_start(args, format_p);
        int size = vsnprintf(NULL, 0, format_p, args);
        va_end(args);

        SM_BYTE *message_p = malloc(size + 1);
        SM_CHECK_NULL(message_p)
        memset(message_p, 0, size + 1);

        va_start(args, format_p);
        vsnprintf(message_p, size + 1, format_p, args);
        va_end(args);

        printf("%s: %s\n", name_p, message_p);
        messages++;

        free(message_p);
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_RESULT sm_noticef(
    SM_BYTE *format_p, ...)
{
SM_BEGIN()

    sm_Thread *Thread_p = _sm_getThread();
    sm_Runtime *Runtime_p = Thread_p && Thread_p->Runtime_p ? Thread_p->Runtime_p : &SM_DEFAULT_RUNTIME;
    SM_BYTE *name_p = sm_getVariable(&Runtime_p->VariableArray, "NAME")->values_pp[0];

    if (Runtime_p->quiet) {SM_DIAGNOSTIC_END(SM_SUCCESS)}

    if (Runtime_p->GUI)
    {

    }
    else 
    {
        va_list args;

        va_start(args, format_p);
        int size = vsnprintf(NULL, 0, format_p, args);
        va_end(args);

        SM_BYTE *message_p = malloc(size + 1);
        SM_CHECK_NULL(message_p)
        memset(message_p, 0, size + 1);

        va_start(args, format_p);
        vsnprintf(message_p, size + 1, format_p, args);
        va_end(args);

        printf("%s: \e[1;35mNOTICE:\e[0m %s\n", name_p, message_p);
        messages++;

        free(message_p);
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_RESULT sm_externalMessage(
    SM_BYTE *prepend_p, SM_BYTE *message_p)
{
SM_BEGIN()

    sm_Thread *Thread_p = _sm_getThread();
    sm_Runtime *Runtime_p = Thread_p && Thread_p->Runtime_p ? Thread_p->Runtime_p : &SM_DEFAULT_RUNTIME;
    SM_BYTE *name_p = sm_getVariable(&Runtime_p->VariableArray, "NAME")->values_pp[0];

    if (Runtime_p->quiet) {SM_DIAGNOSTIC_END(SM_SUCCESS)}

    if (Runtime_p->GUI)
    {

    }
    else 
    {
        SM_BYTE *p = message_p;
        for (int i = 0; i < strlen(message_p); ++i) 
        {
            if (i + 1 == strlen(message_p)) {i++;}

            if (message_p[i] == '\n' || message_p[i] == '\0') 
            {
                SM_BYTE replace = message_p[i];
                message_p[i] = '\0';
                printf("%s%s\n", prepend_p, p);
                p = &message_p[i + 1];
                message_p[i] = replace;
                messages++;
            }
        }
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_RESULT sm_exitMessage(
    SM_RESULT result)
{
SM_BEGIN()

    sm_Thread *Thread_p = _sm_getThread();
    sm_Runtime *Runtime_p = Thread_p && Thread_p->Runtime_p ? Thread_p->Runtime_p : &SM_DEFAULT_RUNTIME;
    SM_BYTE *name_p = sm_getVariable(&Runtime_p->VariableArray, "NAME")->values_pp[0];

    if (Runtime_p->quiet) {SM_DIAGNOSTIC_END(SM_SUCCESS)}

    if (Runtime_p->GUI)
    {

    }
    else 
    {
        if (result) {
            printf("%s: -> \e[1;31mERROR\e[0m %s\n", name_p, SM_RESULTS_PP[result]);
            printf("%s: -> \e[1;31mEXECUTION ERROR\e[0m -> \e[1;31mEXITING\e[0m\n", name_p);
        }
        else {printf("%s:\n%s: -> \e[1;32mEXECUTION SUCCESS\e[0m -> \e[1;32mEXITING\e[0m\n", name_p, name_p);}
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

