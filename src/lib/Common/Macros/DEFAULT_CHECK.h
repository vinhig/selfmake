/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../../UI/Message.h"
#include "../Result.h"

#undef  SM_CHECK
#define SM_CHECK(checkable)                                                            \
{                                                                                      \
    SM_RESULT checkResult = checkable;                                            \
    if (checkResult != SM_SUCCESS) {                                              \
        sm_messagef("-> \e[1;31mERROR\e[0m %s", SM_RESULTS_PP[checkResult]); \
        return checkResult;                                                            \
    }                                                                                  \
}

#undef  SM_CHECK_NULL
#define SM_CHECK_NULL(checkable)                                                                      \
{                                                                                                     \
    void *checkResult_p = checkable;                                                                  \
    if (checkResult_p == NULL) {                                                                      \
        sm_messagef("-> \e[1;31mERROR\e[0m %s", SM_RESULTS_PP[SM_ERROR_NULL_POINTER]); \
        return SM_ERROR_NULL_POINTER;                                                            \
    }                                                                                                 \
}

