#ifndef SM_TYPES_H
#define SM_TYPES_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

/**
 * This file contains API-level types.
 */

#include "Result.h"

#endif

/** @addtogroup selfmakeAPITypedefs
 *  @{
 */

    typedef char           SM_BYTE;
    typedef unsigned char  SM_UNSIGNED_BYTE;

    typedef struct sm_Runtime sm_Runtime;

/** @} */

/** @addtogroup selfmakeAPIEnums
 *  @{
 */ 

    /**
     * Boolean values.
     */
    typedef enum SM_BOOL {
        SM_FALSE = 0, /**<Indicates false.*/
        SM_TRUE = 1,  /**<Indicates true.*/
    } SM_BOOL;

    typedef enum SM_TOKEN {
        SM_TOKEN_UNDEFINED,
        SM_TOKEN_COMMA,
        SM_TOKEN_IDENTIFIER,
        SM_TOKEN_STRING,
        SM_TOKEN_CURLY_BRACKET_RIGHT,
        SM_TOKEN_CURLY_BRACKET_LEFT,
        SM_TOKEN_ROUND_BRACKET_RIGHT,
        SM_TOKEN_ROUND_BRACKET_LEFT,
        SM_TOKEN_ANGLE_BRACKET_RIGHT,
        SM_TOKEN_ANGLE_BRACKET_LEFT,
        SM_TOKEN_HYPHEN_MINUS,
        SM_TOKEN_EOF,
    } SM_TOKEN;

    typedef enum SM_DEFINITION {
        SM_DEFINITION_UNDEFINED,
        SM_DEFINITION_OPTION,
        SM_DEFINITION_FUNCTION,
        SM_DEFINITION_BLOCK,
    } SM_DEFINITION;

    typedef enum SM_SOURCE_CONTEXT {
        SM_SOURCE_CONTEXT_SHARED_LIBRARY,
        SM_SOURCE_CONTEXT_STATIC_LIBRARY,
        SM_SOURCE_CONTEXT_BINARY,
    } SM_SOURCE_CONTEXT;

/** @} */

/** @addtogroup selfmakeAPIStructs
 *  @{
 */

    typedef struct sm_Function {
        SM_DEFINITION type;
        SM_BYTE *name_p;
        unsigned int arguments;
        SM_BYTE **arguments_pp;
        SM_TOKEN *argumentTypes_p;
    } sm_Function;

    typedef struct sm_SourceContext {
        SM_SOURCE_CONTEXT type;
        SM_BYTE *path_p;
        SM_BYTE *name_p;
        SM_BYTE *compileArgs_p;
        SM_BYTE *linkArgs_p;
        SM_BYTE *outputPath_p;
        long api, major, minor, patch;
        long apiDate_p[3], majorDate_p[3], minorDate_p[3], patchDate_p[3];
    } sm_SourceContext;

    typedef struct sm_SourceContextArray {
        int length;
        int maxNameLength;
        sm_SourceContext *SourceContexts_p;
    } sm_SourceContextArray;

    typedef struct sm_ValueArray {
        SM_BYTE **values_pp;
        int length;
    } sm_ValueArray;

    typedef struct sm_TestArgument {
        SM_BYTE *p;
    } sm_TestArgument;

/** @} */

/** @addtogroup selfmakeAPITypedefs
 *  @{
 */

    typedef SM_RESULT (*sm_functionCallback_f)(
        sm_Runtime *Runtime_p, sm_Function *Function_p
    );

    typedef SM_RESULT (*sm_sourceContextCallback_f)(
        sm_Runtime *Runtime_p, sm_SourceContext *Context_p
    );

/** @} */

#endif
