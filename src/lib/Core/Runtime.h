#ifndef SM_RUNTIME_H
#define SM_RUNTIME_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../Common/Result.h"
#include "../Common/Types.h"

#include "../Parser/Parser.h"
#include "../Parser/Variables.h"
#include "../Parser/Sources.h"
#include "../Parser/SourceContexts.h"

#include "../Test/Test.h"

#endif

/** @addtogroup selfmakeStructs
 *  @{
 */

    typedef struct sm_Runtime {
        SM_BOOL quiet;
        SM_BOOL GUI;
        SM_BOOL showParseTree;
        SM_BYTE *prefix_p;
        sm_ParserArray ParserArray;
        sm_SourceArray SourceArray; 
        sm_TestArray TestArray;
        sm_SourceContextArray SourceContextArray; 
        sm_VariableArray VariableArray; 
        sm_FileArray FileArray; 
        sm_functionCallback_f functionCallback_f;
        sm_sourceContextCallback_f sourceContextCallback_f;
        sm_sourceContextCallback_f beforeBuildCallback_f;
        sm_sourceContextCallback_f afterBuildCallback_f;
    } sm_Runtime;

/** @} */

/** @addtogroup selfmakeVars
 *  @{
 */

    extern sm_Runtime SM_DEFAULT_RUNTIME;

/** @} */

/** @addtogroup selfmakeFunctions
 *  @{
 */

    SM_RESULT sm_executeRuntime(
        sm_Runtime *Runtime_p, SM_BYTE **args_pp, int args
    );

/** @} */

#endif
