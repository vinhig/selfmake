// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Test.h"
#include "Channel.h"
#include "Process.h"
#include "Library.h"

#include "../Core/Runtime.h"
#include "../Parser/Variables.h"

#include "../Common/Types.h"
#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#ifdef __unix__
    #include <sys/wait.h>
#endif

// TEST ============================================================================================

typedef int (*sm_initializer_f)();
typedef void *(*sm_customLoader_f)(SM_BYTE *libraryName_p, SM_BYTE *functionName_p);
typedef int (*sm_test_f)(int arguments, sm_TestArgument *Arguments_p);

static SM_RESULT sm_runTests(
    sm_TestEnvironment *TestEnvironment_p)
{
SM_BEGIN()

    void *initLib_p = NULL;
    void *loadLib_p = NULL;

    if (TestEnvironment_p->Initializer_p) 
    {
        initLib_p = sm_openLibrary(TestEnvironment_p->Initializer_p->libraryName_p);
        SM_CHECK_NULL(initLib_p)

        sm_initializer_f initializer_f = sm_loadSymbol(initLib_p, TestEnvironment_p->Initializer_p->functionName_p);
        SM_CHECK_NULL(initializer_f)

        // run custom initializer
        if (initializer_f()) {SM_END(SM_ERROR_BAD_STATE)}
    }

    sm_customLoader_f customLoader_f = NULL;

    if (TestEnvironment_p->Loader_p) 
    {
        loadLib_p = sm_openLibrary(TestEnvironment_p->Loader_p->libraryName_p);
        SM_CHECK_NULL(loadLib_p)

        customLoader_f = sm_loadSymbol(loadLib_p, TestEnvironment_p->Loader_p->functionName_p);
        SM_CHECK_NULL(customLoader_f)
    }

    SM_BOOL match = SM_FALSE;

    for (int i = 0; i < TestEnvironment_p->TestArray_p->length; ++i) 
    {
        sm_Test *Test_p = &TestEnvironment_p->TestArray_p->Tests_p[i];
        sm_test_f test_f = NULL;

        if (strcmp(Test_p->Context_p->name_p, TestEnvironment_p->target_p)) {continue;}

        if (customLoader_f) {
            test_f = customLoader_f(Test_p->Context_p->name_p, Test_p->name_p);
        }
        else {
            Test_p->dl_p = sm_openLibrary(Test_p->Context_p->name_p);
            SM_CHECK_NULL(Test_p->dl_p)
            test_f = sm_loadSymbol(Test_p->dl_p, Test_p->name_p);
        }

        SM_CHECK_NULL(test_f)

        // perform the test
        int result = test_f(Test_p->arguments, Test_p->Arguments_p);

        if (result) {
            sm_messagef("Test [%s]: FAIL", Test_p->name_p);
        }
        else {sm_messagef("Test [%s]: SUCCESS", Test_p->name_p);}

        match = SM_TRUE;
    }

    if (match) {
        sm_messagef("");
    }

SM_END(SM_SUCCESS)
}

static sm_TestEnvironment sm_initializeTestEnvironment(
    sm_Runtime *Runtime_p, sm_ExternalFunction *Initializer_p, sm_ExternalFunction *Loader_p,
    SM_BYTE *target_p)
{
SM_BEGIN()

    sm_TestEnvironment TestEnvironment;
    TestEnvironment.target_p = target_p;

    sm_Variable *InitLibrary_p = sm_getVariable(&Runtime_p->VariableArray, "TEST_INITIALIZER_SOURCE");
    sm_Variable *InitFunction_p = sm_getVariable(&Runtime_p->VariableArray, "TEST_INITIALIZER");

    if (InitLibrary_p && InitFunction_p) {
        Initializer_p->libraryName_p = InitLibrary_p->values_pp[0];
        Initializer_p->functionName_p = InitFunction_p->values_pp[0];
        TestEnvironment.Initializer_p = Initializer_p;
    }
    else {TestEnvironment.Initializer_p = NULL;}
  
    sm_Variable *LoadLibrary_p = sm_getVariable(&Runtime_p->VariableArray, "TEST_LOADER_SOURCE");
    sm_Variable *LoadFunction_p = sm_getVariable(&Runtime_p->VariableArray, "TEST_LOADER");

    if (LoadLibrary_p && LoadFunction_p) {
        Loader_p->libraryName_p = LoadLibrary_p->values_pp[0];
        Loader_p->functionName_p = LoadFunction_p->values_pp[0];
        TestEnvironment.Loader_p = Loader_p;
    }
    else {TestEnvironment.Loader_p = NULL;}

SM_END(TestEnvironment)
}

static SM_RESULT sm_runTestEnvironment(
    sm_TestEnvironment *TestEnvironment_p, sm_TestArray *TestArray_p)
{
SM_BEGIN()

    TestEnvironment_p->Process = sm_fork();
    TestEnvironment_p->TestArray_p = TestArray_p;

    if (TestEnvironment_p->Process.id == 0) {
        exit(sm_runTests(TestEnvironment_p));
    }

    pid_t w;
    int wstatus;

    do {
        w = waitpid(TestEnvironment_p->Process.id, &wstatus, WUNTRACED | WCONTINUED);
        if (w == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
//        if (WIFEXITED(wstatus)) {
//            printf("exited, status=%d\n", WEXITSTATUS(wstatus));
//        } else if (WIFSIGNALED(wstatus)) {
//            printf("killed by signal %d\n", WTERMSIG(wstatus));
//        } else if (WIFSTOPPED(wstatus)) {
//            printf("stopped by signal %d\n", WSTOPSIG(wstatus));
//        } else if (WIFCONTINUED(wstatus)) {
//            printf("continued\n");
//        }
    } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));

SM_END(SM_SUCCESS)
}

SM_RESULT sm_test(
    sm_Runtime *Runtime_p, SM_BYTE *name_p)
{
SM_BEGIN()

    sm_ExternalFunction Initializer;
    sm_ExternalFunction Loader;

    sm_TestEnvironment TestEnvironment = 
        sm_initializeTestEnvironment(Runtime_p, &Initializer, &Loader, name_p);

    SM_CHECK(sm_runTestEnvironment(&TestEnvironment, &Runtime_p->TestArray))

SM_END(SM_SUCCESS)
}

//SM_BYTE *sm_getTestResult(
//    sm_Process *Fork_p)
//{
//SM_BEGIN()
//
//    if (Fork_p->id != 0) {
//        SM_BYTE *receive_p = sm_readFromChannel(&Fork_p->IPC.Out, NULL);
//        if (receive_p != NULL) {
//            SM_CHECK(sm_handleIPCReceive(receive_p))
//        }
//// TODO free?
//    }
//
//SM_END(SM_SIGNAL_OK)
//}
//
//SM_RESULT sm_destroyTestEnvironment(
//    sm_TestEnvironment *TestEnvironment_p)
//{
//SM_BEGIN()
//
//SM_END(SM_SUCCESS)
//}


// TEST ARRAY ======================================================================================

void sm_initTestArray(
    sm_TestArray *Array_p)
{
SM_BEGIN()

    Array_p->length = 0;
    Array_p->Tests_p = NULL;

SM_SILENT_END()
}

static sm_Test sm_initTest()
{
SM_BEGIN()

    sm_Test Test;
    Test.Context_p = NULL;
    Test.name_p = NULL;
    Test.dl_p = NULL;
    Test.arguments = 0;
    Test.Arguments_p = NULL;

SM_END(Test)
}

SM_RESULT sm_addTest(
    sm_SourceContextArray *ContextArray_p, sm_TestArray *TestArray_p, sm_Function *Function_p)
{
SM_BEGIN()

    sm_SourceContext *Context_p = NULL;
    sm_Test *Test_p = NULL;

    for (int i = 0; i < Function_p->arguments; ++i) 
    {
        if (Function_p->argumentTypes_p[i] == SM_TOKEN_IDENTIFIER) {
            for (int j = 0; j < ContextArray_p->length; ++j) {
                if (!strcmp(ContextArray_p->SourceContexts_p[j].name_p, Function_p->arguments_pp[i])) {
                    Context_p = &ContextArray_p->SourceContexts_p[j];
                }
            }
        }
        else {

            if (!Context_p) {
                SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)
            }

            if (!Test_p)
            {
                if (!TestArray_p->Tests_p) {
                    TestArray_p->Tests_p = malloc(sizeof(sm_Test));
                    SM_CHECK_NULL(TestArray_p->Tests_p)
                }
                else {
                    TestArray_p->Tests_p = realloc(TestArray_p->Tests_p, sizeof(sm_Test) * (TestArray_p->length + 1));
                    SM_CHECK_NULL(TestArray_p->Tests_p)
                }
        
                Test_p = &TestArray_p->Tests_p[TestArray_p->length];
                *Test_p = sm_initTest();

                Test_p->Context_p = Context_p;
                Test_p->name_p = malloc(strlen(Function_p->arguments_pp[i]) + 1);
                SM_CHECK_NULL(Test_p->name_p)
                sprintf(Test_p->name_p, Function_p->arguments_pp[i]);
        
                TestArray_p->length++;
                
                SM_BYTE offset_p[64];
                sm_getIndentAfterSourceContext(Context_p->name_p, offset_p, 64, ContextArray_p);
                sm_messagef("[%s]%s Add test \"%s\"", Context_p->name_p, offset_p, Test_p->name_p);
            }
            else
            {
                if (!Test_p->Arguments_p) {
                    Test_p->Arguments_p = malloc(sizeof(sm_TestArgument));
                    SM_CHECK_NULL(Test_p->Arguments_p)
                }
                else {
                    Test_p->Arguments_p = realloc(Test_p->Arguments_p, sizeof(sm_TestArgument) * (Test_p->arguments + 1));
                    SM_CHECK_NULL(Test_p->Arguments_p)
                }
        
                sm_TestArgument *TestArgument_p = &Test_p->Arguments_p[Test_p->arguments];

                TestArgument_p->p = malloc(strlen(Function_p->arguments_pp[i]) + 1);
                SM_CHECK_NULL(TestArgument_p->p)
                sprintf(TestArgument_p->p, Function_p->arguments_pp[i]);
        
                Test_p->arguments++;
            }
        }
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

