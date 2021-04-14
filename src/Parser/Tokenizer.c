// LICENSE NOTICE ==================================================================================

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

// INCLUDE =========================================================================================

#include "Tokenizer.h"

#include "../Main/Main.h"
#include "../Main/Util.h"
#include "../Common/Macro.h"
#include SELFMAKE_FLOW
#include SELFMAKE_DEFAULT_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

// VARS ============================================================================================

selfmake_Tokenizer SELFMAKE_TOKENIZER;

// HELPER ==========================================================================================

static NH_BOOL selfmake_isASCIIUpperAlpha(
    NH_BYTE codepoint)
{
SELFMAKE_BEGIN()
SELFMAKE_END(codepoint >= 0x41 && codepoint <= 0x5A)
}

static NH_BOOL selfmake_isASCIILowerAlpha(
    NH_BYTE codepoint)
{
SELFMAKE_BEGIN()
SELFMAKE_END(codepoint >= 0x61 && codepoint <= 0x7A)
}

static NH_BOOL selfmake_isASCIIAlpha(
    NH_BYTE codepoint)
{
SELFMAKE_BEGIN()
SELFMAKE_END(selfmake_isASCIIUpperAlpha(codepoint) || selfmake_isASCIILowerAlpha(codepoint))
}

static NH_BOOL selfmake_isBracket(
    NH_BYTE codepoint)
{
SELFMAKE_BEGIN()
SELFMAKE_END(codepoint == '(' || codepoint == ')' || codepoint == '{' || codepoint == '}' || codepoint == '[' || codepoint == ']')
}

static NH_BOOL selfmake_isTokenBegin(
    NH_BYTE codepoint)
{
SELFMAKE_BEGIN()

    if (selfmake_isASCIIAlpha(codepoint) || selfmake_isBracket(codepoint) 
    || codepoint == ',' 
    || codepoint == '"' 
    || codepoint == '-') 

    {SELFMAKE_END(NH_TRUE)}

SELFMAKE_END(NH_FALSE)
}

// TOKENIZE ========================================================================================

static NH_BYTE *selfmake_tokenizeString(
    selfmake_Token *Token_p, NH_BYTE *bytes_p)
{
SELFMAKE_BEGIN()

    NH_BYTE *string_p = malloc(1);
    unsigned int stringLength = 0;

    NH_BYTE *stringBegin_p = bytes_p;

    NH_BOOL escape = NH_FALSE;
    while (*bytes_p && (*bytes_p != '"' || escape)) {
        escape = escape ? NH_FALSE : *bytes_p == 0x5C;
        if (!escape) {
            string_p = realloc(string_p, stringLength + 1);
            string_p[stringLength++] = *bytes_p;
        }
        bytes_p++;
    }
    if (!*bytes_p) {SELFMAKE_END(NULL)}

    *bytes_p = 0;

    string_p = realloc(string_p, stringLength + 1);
    string_p[stringLength] = 0;

    Token_p->string_p = string_p;

    *bytes_p = '"';

SELFMAKE_END(&bytes_p[1])
}

static NH_BYTE *selfmake_getToken(
    selfmake_Token *Token_p, NH_BYTE *bytes_p) 
{
SELFMAKE_BEGIN()

    if (!*bytes_p) {SELFMAKE_END(NULL)}

    Token_p->type     = SELFMAKE_TOKEN_UNDEFINED;
    Token_p->string_p = NULL;

    while (*bytes_p && !selfmake_isTokenBegin(*bytes_p)) {
        if (bytes_p[0] == '/' && bytes_p[1] == '/') {
            while (*bytes_p && *bytes_p != '\n') {bytes_p++;}
        }
        bytes_p++;
    }
    if (!*bytes_p) {SELFMAKE_END(NULL)}

    NH_BYTE *tokenBegin_p = bytes_p;

    switch (*tokenBegin_p) 
    {
        case '(' : Token_p->type = SELFMAKE_TOKEN_ROUND_BRACKET_LEFT; break;
        case ')' : Token_p->type = SELFMAKE_TOKEN_ROUND_BRACKET_RIGHT; break;
        case '{' : Token_p->type = SELFMAKE_TOKEN_CURLY_BRACKET_LEFT; break;
        case '}' : Token_p->type = SELFMAKE_TOKEN_CURLY_BRACKET_RIGHT; break;
        case '[' : Token_p->type = SELFMAKE_TOKEN_ANGLE_BRACKET_LEFT; break;
        case ']' : Token_p->type = SELFMAKE_TOKEN_ANGLE_BRACKET_RIGHT; break;
        case '-' : Token_p->type = SELFMAKE_TOKEN_HYPHEN_MINUS; break;
        case ',' : Token_p->type = SELFMAKE_TOKEN_COMMA; break;
        case '"' :
            Token_p->type = SELFMAKE_TOKEN_STRING;
            SELFMAKE_END(selfmake_tokenizeString(Token_p, &bytes_p[1]))
            break;
    }
    
    if (Token_p->type != SELFMAKE_TOKEN_UNDEFINED) {
        SELFMAKE_END(&bytes_p[1])
    }

    while (*bytes_p && selfmake_isASCIIAlpha(*bytes_p)) {bytes_p++;}
    if (!*bytes_p) {SELFMAKE_END(NULL)}

    NH_BYTE tmp = *bytes_p;
    *bytes_p = 0;    

    Token_p->type = SELFMAKE_TOKEN_IDENTIFIER;
    Token_p->string_p = malloc(strlen(tokenBegin_p) + 1);
    strcpy(Token_p->string_p, tokenBegin_p);

    *bytes_p = tmp;

SELFMAKE_END(bytes_p)
}

static void selfmake_getTokens(
    selfmake_Token *Tokens_p, NH_BYTE *bytes_p, unsigned int *tokens_p) 
{
SELFMAKE_BEGIN()

    unsigned int tokens = 0;

    while (bytes_p)
    {
        selfmake_Token Token;
        bytes_p = selfmake_getToken(&Token, bytes_p);
        if (Tokens_p) {Tokens_p[tokens] = Token;}
        tokens++;
    }

    if (Tokens_p) {
        Tokens_p[tokens - 1].type = SELFMAKE_TOKEN_EOF;        
    }

    if (tokens_p) {*tokens_p = tokens;}

SELFMAKE_SILENT_END()
}

SELFMAKE_RESULT selfmake_tokenizeInstallerFile() 
{
SELFMAKE_BEGIN()

    NH_BYTE file_p[255] = {'\0'};
    sprintf(file_p, "%s/src/bin/nhmake/Common/Data/installer.txt", SELFMAKE_PROJECT_DIR_P);

    NH_BYTE *bytes_p = selfmake_getFileData(file_p, NULL);
    SELFMAKE_CHECK_NULL(bytes_p)

    unsigned int tokens = 0;
    selfmake_getTokens(NULL, bytes_p, &tokens);

    free(bytes_p);
    bytes_p = NULL;

    bytes_p = selfmake_getFileData(file_p, NULL);
    SELFMAKE_CHECK_NULL(bytes_p)

    selfmake_Token *Tokens_p = malloc(sizeof(selfmake_Token) * tokens);
    for (int i = 0; i < tokens; ++i) {
        Tokens_p[i].string_p = NULL;
    }
    selfmake_getTokens(Tokens_p, bytes_p, NULL);

    free(bytes_p);

    SELFMAKE_TOKENIZER.tokens   = tokens;
    SELFMAKE_TOKENIZER.Tokens_p = Tokens_p;

SELFMAKE_END(SELFMAKE_SUCCESS)
}

