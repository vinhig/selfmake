#ifndef SM_TOKENIZER_H
#define SM_TOKENIZER_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "../Common/Types.h"
#include "../Common/Result.h"

#include "../Core/File.h"

#endif

/** @addtogroup selfmakeEnums
 *  @{
 */

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
    
/** @} */

/** @addtogroup selfmakeStructs
 *  @{
 */

    typedef struct sm_Token {
        SM_TOKEN type;
        SM_BYTE *string_p;
    } sm_Token;

    typedef struct sm_Tokenizer {
        unsigned int tokens;
        sm_Token *Tokens_p;
    } sm_Tokenizer;

/** @} */

/** @addtogroup selfmakeFunctions
 *  @{
 */

    sm_Tokenizer sm_initTokenizer(
    );

    SM_RESULT sm_tokenizeFile(
        sm_Tokenizer *Tokenizer_p, sm_File *File_p 
    ); 

/** @} */

#endif
