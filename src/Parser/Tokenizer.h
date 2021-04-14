#ifndef SELFMAKE_TOKENIZER_H
#define SELFMAKE_TOKENIZER_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "../Common/API.h"
#include "../Common/Result.h"

#endif

/** @addtogroup nhmakeEnums
 *  @{
 */

    typedef enum SELFMAKE_TOKEN {
        SELFMAKE_TOKEN_UNDEFINED,
        SELFMAKE_TOKEN_COMMA,
        SELFMAKE_TOKEN_IDENTIFIER,
        SELFMAKE_TOKEN_STRING,
        SELFMAKE_TOKEN_CURLY_BRACKET_RIGHT,
        SELFMAKE_TOKEN_CURLY_BRACKET_LEFT,
        SELFMAKE_TOKEN_ROUND_BRACKET_RIGHT,
        SELFMAKE_TOKEN_ROUND_BRACKET_LEFT,
        SELFMAKE_TOKEN_ANGLE_BRACKET_RIGHT,
        SELFMAKE_TOKEN_ANGLE_BRACKET_LEFT,
        SELFMAKE_TOKEN_HYPHEN_MINUS,
        SELFMAKE_TOKEN_EOF,
    } SELFMAKE_TOKEN;
    
/** @} */

/** @addtogroup nhmakeStructs
 *  @{
 */

    typedef struct selfmake_Token {
        SELFMAKE_TOKEN type;
        NH_BYTE *string_p;
    } selfmake_Token;

    typedef struct selfmake_Tokenizer {
        unsigned int tokens;
        selfmake_Token *Tokens_p;
    } selfmake_Tokenizer;

/** @} */

/** @addtogroup nhmakeVars
 *  @{
 */

    extern selfmake_Tokenizer SELFMAKE_TOKENIZER;

/** @} */

/** @addtogroup nhmakeFunctions
 *  @{
 */

    SELFMAKE_RESULT selfmake_tokenizeInstallerFile(
    ); 

/** @} */

#endif
