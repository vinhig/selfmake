// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Functions.h"
#include "Variables.h"

#include "../Core/Build.h"
#include "../Core/Utils.h"
#include "../UI/Message.h"
#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/types.h> 
#include <sys/stat.h>

// HELPER ==========================================================================================

static SM_BOOL sm_caseInsensitiveMatch(
    SM_BYTE *str1_p, SM_BYTE *str2_p)
{
SM_BEGIN()

    if (strlen(str1_p) != strlen(str2_p)) {SM_END(SM_FALSE)}

    for (int i = 0; i < strlen(str1_p); ++i) {
        int d = tolower((unsigned char)str1_p[i]) - tolower((unsigned char)str2_p[i]);
        if (d != 0) {
            SM_END(SM_FALSE)
        }
    }
   
SM_END(SM_TRUE)
}

// EXECUTE =========================================================================================

static SM_RESULT sm_executeCopyFunction(
    sm_Runtime *Runtime_p, sm_Function *Function_p)
{
SM_BEGIN()

    if (Function_p->arguments <= 0 || Function_p->arguments > 4) {SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)}

    switch(Function_p->arguments)
    {
        case 2 :
            sm_copy(&Runtime_p->VariableArray, Function_p->arguments_pp[0], Function_p->arguments_pp[1], SM_FALSE, SM_FALSE);
            break;
        case 3 :
            sm_copy(&Runtime_p->VariableArray, Function_p->arguments_pp[0], Function_p->arguments_pp[1], !strcmp(Function_p->arguments_pp[2], "true") ? SM_TRUE : SM_FALSE, SM_FALSE);
            break;
        case 4 :
            sm_copy(&Runtime_p->VariableArray, Function_p->arguments_pp[0], Function_p->arguments_pp[1], !strcmp(Function_p->arguments_pp[2], "true") ? SM_TRUE : SM_FALSE, !strcmp(Function_p->arguments_pp[3], "true") ? SM_TRUE : SM_FALSE);
            break;
    }

SM_END(SM_SUCCESS)
}

static SM_RESULT sm_executeChdirFunction(
    sm_Runtime *Runtime_p, sm_Function *Function_p)
{
SM_BEGIN()

    if (Function_p->arguments == 0) {
        sm_updateVariable(&Runtime_p->VariableArray, "WRK_DIR", sm_getVariable(&Runtime_p->VariableArray, "PROJ_DIR")->values_pp, 1);
    }
    else {
        sm_updateVariable(&Runtime_p->VariableArray, "WRK_DIR", Function_p->arguments_pp, 1);
    }

    sm_messagef("chdir %s", sm_getVariable(&Runtime_p->VariableArray, "WRK_DIR")->values_pp[0]);

SM_END(SM_SUCCESS)
}

static SM_RESULT sm_executeMkdirFunction(
    sm_Function *Function_p)
{
SM_BEGIN()

    if (Function_p->arguments != 1) {SM_DIAGNOSTIC_END(SM_SUCCESS)}

    struct stat st = {0};
    if (stat(Function_p->arguments_pp[0], &st) == -1) {
        int err = mkdir(Function_p->arguments_pp[0], 0755);  // 0755 -> drwxr-xr-x
        if (err != 0) {
            SM_DIAGNOSTIC_END(SM_ERROR_CANT_CREATE_DIRECTORY)
        }
    }

    sm_messagef("mkdir %s", Function_p->arguments_pp[0]);

SM_END(SM_SUCCESS)
}

static SM_RESULT sm_executeSystemFunction(
    sm_Function *Function_p)
{
SM_BEGIN()

    if (Function_p->arguments != 1) {SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)}
    sm_messagef("system %s", Function_p->arguments_pp[0]);
    system(Function_p->arguments_pp[0]);

SM_END(SM_SUCCESS)
}

SM_RESULT sm_executeFunction(
    sm_Runtime *Runtime_p, sm_Function *Function_p) 
{
SM_BEGIN()

    SM_BYTE **arguments_pp = malloc(sizeof(SM_BYTE*) * Function_p->arguments);
    memcpy(arguments_pp, Function_p->arguments_pp, sizeof(SM_BYTE*) * Function_p->arguments);

    for (int i = 0; i < Function_p->arguments; ++i) {
        Function_p->arguments_pp[i] = sm_substituteVariables(&Runtime_p->VariableArray, arguments_pp[i]);
        SM_CHECK_NULL(Function_p->arguments_pp[i])
    }
    SM_RESULT result = SM_SUCCESS;

    if (sm_caseInsensitiveMatch(Function_p->name_p, "lib")) {
        if (!strcmp(Function_p->arguments_pp[0], "SHARED")) {
            result = sm_addSourceContext(Runtime_p, Function_p, SM_SOURCE_CONTEXT_SHARED_LIBRARY, 1);
        }
        else if (!strcmp(Function_p->arguments_pp[0], "STATIC")) {
            result = sm_addSourceContext(Runtime_p, Function_p, SM_SOURCE_CONTEXT_STATIC_LIBRARY, 1);
        }
        else {
            result = sm_addSourceContext(Runtime_p, Function_p, SM_SOURCE_CONTEXT_SHARED_LIBRARY, 0);
        }
        if (!result && Runtime_p->sourceContextCallback_f) {
            result = Runtime_p->sourceContextCallback_f(
                Runtime_p, &Runtime_p->SourceContextArray.SourceContexts_p[Runtime_p->SourceContextArray.length - 1]
            );
        }
    }
    else if (sm_caseInsensitiveMatch(Function_p->name_p, "bin")) {
        result = sm_addSourceContext(Runtime_p, Function_p, SM_SOURCE_CONTEXT_BINARY, 0);
        if (!result && Runtime_p->sourceContextCallback_f) {
            result = Runtime_p->sourceContextCallback_f(
                Runtime_p, &Runtime_p->SourceContextArray.SourceContexts_p[Runtime_p->SourceContextArray.length - 1]
            );
        }
    }
    else if (sm_caseInsensitiveMatch(Function_p->name_p, "compile")) {
        result = sm_addCompileArguments(&Runtime_p->SourceContextArray, Function_p);
    }
    else if (sm_caseInsensitiveMatch(Function_p->name_p, "link")) {
        result = sm_addLinkArguments(&Runtime_p->SourceContextArray, Function_p);
    }
    else if (sm_caseInsensitiveMatch(Function_p->name_p, "source")) {
        result = sm_addSource(&Runtime_p->SourceContextArray, &Runtime_p->SourceArray, Function_p);
    }
    else if (sm_caseInsensitiveMatch(Function_p->name_p, "test")) {
        result = sm_addTest(&Runtime_p->SourceContextArray, &Runtime_p->TestArray, Function_p);
    }
    else if (sm_caseInsensitiveMatch(Function_p->name_p, "copy")) {
        result = sm_executeCopyFunction(Runtime_p, Function_p);
    }
    else if (sm_caseInsensitiveMatch(Function_p->name_p, "build")) {
        result = sm_build(Runtime_p, Function_p->arguments_pp[0]);
    }
    else if (sm_caseInsensitiveMatch(Function_p->name_p, "chdir")) {
        result = sm_executeChdirFunction(Runtime_p, Function_p);
    }
    else if (sm_caseInsensitiveMatch(Function_p->name_p, "mkdir")) {
        result = sm_executeMkdirFunction(Function_p);
    }
    else if (sm_caseInsensitiveMatch(Function_p->name_p, "system")) {
        result = sm_executeSystemFunction(Function_p);
    }
    else if (sm_caseInsensitiveMatch(Function_p->name_p, "system")) {
        result = sm_executeSystemFunction(Function_p);
    }
    else if (sm_caseInsensitiveMatch(Function_p->name_p, "set")) {
        result = sm_updateVariable(
            &Runtime_p->VariableArray, Function_p->arguments_pp[0], Function_p->arguments_pp + 1, 
            Function_p->arguments - 1
        );
    }

    if (!result && Runtime_p->functionCallback_f) {
        result = Runtime_p->functionCallback_f(Runtime_p, Function_p);
    }

    for (int i = 0; i < Function_p->arguments; ++i) {
        free(Function_p->arguments_pp[i]);
    }

    memcpy(Function_p->arguments_pp, arguments_pp, sizeof(SM_BYTE*) * Function_p->arguments);
    free(arguments_pp);

SM_DIAGNOSTIC_END(result)
}

SM_BOOL sm_compareIf(
    sm_Runtime *Runtime_p, sm_If *If_p, SM_BOOL b)
{
SM_BEGIN()

    sm_Variable *Var_p = sm_getVariable(&Runtime_p->VariableArray, If_p->string_p);
    if (Var_p && Var_p->valueCount > 0) {
        if (!strcmp(*Var_p->values_pp, "true") && b == SM_TRUE) {SM_END(SM_TRUE)}
        if (!strcmp(*Var_p->values_pp, "false") && b == SM_FALSE) {SM_END(SM_TRUE)}
    }

SM_END(SM_FALSE)
}

SM_RESULT sm_executeIf(
    sm_Runtime *Runtime_p, sm_If *If_p) 
{
SM_BEGIN()

    sm_Variable *Var_p = sm_getVariable(&Runtime_p->VariableArray, If_p->string_p);
    if (Var_p && Var_p->valueCount > 0 && !strcmp(*Var_p->values_pp, "true")) {
        SM_CHECK(sm_executeBlock(Runtime_p, &If_p->Block_p->Block))
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// EXECUTE BLOCK ===================================================================================

SM_RESULT sm_executeBlock(
    sm_Runtime *Runtime_p, sm_Block *Block_p)
{
SM_BEGIN()

    for (int d = 0; d < Block_p->definitions; ++d) {
        switch (Block_p->Definitions_p[d].type) {
            case SM_DEFINITION_FUNCTION :
                SM_CHECK(sm_executeFunction(Runtime_p, &Block_p->Definitions_p[d].Function))
            case SM_DEFINITION_IF :
                SM_CHECK(sm_executeIf(Runtime_p, &Block_p->Definitions_p[d].If))
        }
    }

SM_END(SM_SUCCESS)
}

