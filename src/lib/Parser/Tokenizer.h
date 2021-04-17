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
