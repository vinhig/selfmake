// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Tokenizer.h"

#include "../Core/Utils.h"
#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

// HELPER ==========================================================================================

static SM_BOOL sm_isASCIIUpperAlpha(
    SM_BYTE codepoint)
{
SM_BEGIN()
SM_END(codepoint >= 0x41 && codepoint <= 0x5A)
}

static SM_BOOL sm_isASCIILowerAlpha(
    SM_BYTE codepoint)
{
SM_BEGIN()
SM_END(codepoint >= 0x61 && codepoint <= 0x7A)
}

static SM_BOOL sm_isASCIIAlpha(
    SM_BYTE codepoint)
{
SM_BEGIN()
SM_END(sm_isASCIIUpperAlpha(codepoint) || sm_isASCIILowerAlpha(codepoint))
}

static SM_BOOL sm_isBracket(
    SM_BYTE codepoint)
{
SM_BEGIN()
SM_END(codepoint == '(' || codepoint == ')' || codepoint == '{' || codepoint == '}' || codepoint == '[' || codepoint == ']')
}

static SM_BOOL sm_isTokenBegin(
    SM_BYTE codepoint)
{
SM_BEGIN()

    if (sm_isASCIIAlpha(codepoint) || sm_isBracket(codepoint) 
    || codepoint == ',' 
    || codepoint == '"' 
    || codepoint == '-') 

    {SM_END(SM_TRUE)}

SM_END(SM_FALSE)
}

// TOKENIZE ========================================================================================

static SM_BYTE *sm_tokenizeString(
    sm_Token *Token_p, SM_BYTE *bytes_p)
{
SM_BEGIN()

    SM_BYTE *string_p = malloc(1);
    unsigned int stringLength = 0;

    SM_BYTE *stringBegin_p = bytes_p;

    SM_BOOL escape = SM_FALSE;
    while (*bytes_p && (*bytes_p != '"' || escape)) {
        escape = escape ? SM_FALSE : *bytes_p == 0x5C;
        if (!escape) {
            string_p = realloc(string_p, stringLength + 1);
            string_p[stringLength++] = *bytes_p;
        }
        bytes_p++;
    }
    if (!*bytes_p) {SM_END(NULL)}

    *bytes_p = 0;

    string_p = realloc(string_p, stringLength + 1);
    string_p[stringLength] = 0;

    Token_p->string_p = string_p;

    *bytes_p = '"';

SM_END(&bytes_p[1])
}

static SM_BYTE *sm_getToken(
    sm_Token *Token_p, SM_BYTE *bytes_p) 
{
SM_BEGIN()

    if (!*bytes_p) {SM_END(NULL)}

    Token_p->type     = SM_TOKEN_UNDEFINED;
    Token_p->string_p = NULL;

    while (*bytes_p && !sm_isTokenBegin(*bytes_p)) {
        if (bytes_p[0] == '/' && bytes_p[1] == '/') {
            while (*bytes_p && *bytes_p != '\n') {bytes_p++;}
        }
        bytes_p++;
    }
    if (!*bytes_p) {SM_END(NULL)}

    SM_BYTE *tokenBegin_p = bytes_p;

    switch (*tokenBegin_p) 
    {
        case '(' : Token_p->type = SM_TOKEN_ROUND_BRACKET_LEFT; break;
        case ')' : Token_p->type = SM_TOKEN_ROUND_BRACKET_RIGHT; break;
        case '{' : Token_p->type = SM_TOKEN_CURLY_BRACKET_LEFT; break;
        case '}' : Token_p->type = SM_TOKEN_CURLY_BRACKET_RIGHT; break;
        case '[' : Token_p->type = SM_TOKEN_ANGLE_BRACKET_LEFT; break;
        case ']' : Token_p->type = SM_TOKEN_ANGLE_BRACKET_RIGHT; break;
        case '-' : Token_p->type = SM_TOKEN_HYPHEN_MINUS; break;
        case ',' : Token_p->type = SM_TOKEN_COMMA; break;
        case '"' :
            Token_p->type = SM_TOKEN_STRING;
            SM_END(sm_tokenizeString(Token_p, &bytes_p[1]))
            break;
    }
    
    if (Token_p->type != SM_TOKEN_UNDEFINED) {
        SM_END(&bytes_p[1])
    }

    while (*bytes_p && sm_isASCIIAlpha(*bytes_p)) {bytes_p++;}
    if (!*bytes_p) {SM_END(NULL)}

    SM_BYTE tmp = *bytes_p;
    *bytes_p = 0;    

    Token_p->type = SM_TOKEN_IDENTIFIER;
    Token_p->string_p = malloc(strlen(tokenBegin_p) + 1);
    strcpy(Token_p->string_p, tokenBegin_p);

    *bytes_p = tmp;

SM_END(bytes_p)
}

static void sm_getTokens(
    sm_Token *Tokens_p, SM_BYTE *bytes_p, unsigned int *tokens_p) 
{
SM_BEGIN()

    unsigned int tokens = 0;

    while (bytes_p)
    {
        sm_Token Token;
        bytes_p = sm_getToken(&Token, bytes_p);
        if (Tokens_p) {Tokens_p[tokens] = Token;}
        tokens++;
    }

    if (Tokens_p) {
        Tokens_p[tokens - 1].type = SM_TOKEN_EOF;        
    }

    if (tokens_p) {*tokens_p = tokens;}

SM_SILENT_END()
}

SM_RESULT sm_tokenizeFile(
    sm_Tokenizer *Tokenizer_p, sm_File *File_p) 
{
SM_BEGIN()

    SM_BYTE *bytes_p = malloc(strlen(File_p->data_p) + 1);
    SM_CHECK_NULL(bytes_p)
    strcpy(bytes_p, File_p->data_p);

    unsigned int tokens = 0;
    sm_getTokens(NULL, bytes_p, &tokens);

    free(bytes_p);
    bytes_p = NULL;

    bytes_p = malloc(strlen(File_p->data_p) + 1);
    SM_CHECK_NULL(bytes_p)
    strcpy(bytes_p, File_p->data_p);

    sm_Token *Tokens_p = malloc(sizeof(sm_Token) * tokens);
    for (int i = 0; i < tokens; ++i) {
        Tokens_p[i].string_p = NULL;
    }
    sm_getTokens(Tokens_p, bytes_p, NULL);

    free(bytes_p);

    Tokenizer_p->tokens   = tokens;
    Tokenizer_p->Tokens_p = Tokens_p;

SM_END(SM_SUCCESS)
}

// INIT ============================================================================================

sm_Tokenizer sm_initTokenizer()
{
SM_BEGIN()

    sm_Tokenizer Tokenizer;
    Tokenizer.tokens = 0;
    Tokenizer.Tokens_p = NULL;

SM_END(Tokenizer)
}

