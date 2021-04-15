#ifndef SM_FUNCTIONS_H
#define SM_FUNCTIONS_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "Parser.h"

#include "../Core/Runtime.h"

#include "../Common/Types.h"
#include "../Common/Result.h"

#endif

/** @addtogroup selfmakeFunctions
 *  @{
 */

    SM_RESULT sm_executeFunction(
        sm_Runtime *Runtime_p, sm_Function *Function_p
    ); 

    SM_RESULT sm_executeFunctions(
        sm_Runtime *Runtime_p, sm_Parser *Parser_p
    );

/** @} */

#endif
