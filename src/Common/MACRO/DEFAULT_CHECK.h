/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "../../UI/Message.h"
#include "../Result.h"

#undef  SELFMAKE_CHECK
#define SELFMAKE_CHECK(checkable)                                                            \
{                                                                                      \
    SELFMAKE_RESULT checkResult = checkable;                                            \
    if (checkResult != SELFMAKE_SUCCESS) {                                              \
        selfmake_messagef("-> \e[1;31mERROR\e[0m %s", SELFMAKE_RESULTS_PP[checkResult]); \
        return checkResult;                                                            \
    }                                                                                  \
}

#undef  SELFMAKE_CHECK_NULL
#define SELFMAKE_CHECK_NULL(checkable)                                                                      \
{                                                                                                     \
    void *checkResult_p = checkable;                                                                  \
    if (checkResult_p == NULL) {                                                                      \
        selfmake_messagef("-> \e[1;31mERROR\e[0m %s", SELFMAKE_RESULTS_PP[SELFMAKE_ERROR_NULL_POINTER]); \
        return SELFMAKE_ERROR_NULL_POINTER;                                                            \
    }                                                                                                 \
}

