// LICENSE NOTICE ==================================================================================

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

// INCLUDE =========================================================================================

#include "Parser.h"
#include "Tokenizer.h"

#include "../Common/Macro.h"
#include SELFMAKE_FLOW
#include SELFMAKE_CUSTOM_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

// VARS ============================================================================================

selfmake_Parser SELFMAKE_PARSER;

// PARSE ===========================================================================================

static selfmake_Token *selfmake_parseToken(
    selfmake_Token *Token_p, selfmake_Definition *Definition_p
); 

static selfmake_Token *selfmake_parseFunction(
    selfmake_Token *Token_p, selfmake_Definition *Definition_p)
{
SELFMAKE_BEGIN()

    Definition_p->type = SELFMAKE_DEFINITION_FUNCTION;
    Definition_p->Function.name_p = Token_p->string_p;
    Definition_p->Function.arguments_pp = NULL; 
    Definition_p->Function.arguments = 0;

    ++Token_p;

    if (Token_p->type != SELFMAKE_TOKEN_ROUND_BRACKET_LEFT) {
        SELFMAKE_END(Token_p)
    }

    Definition_p->Function.arguments_pp = malloc(sizeof(NH_BYTE*));
    SELFMAKE_CHECK_NULL(NULL, Definition_p->Function.arguments_pp)

    while (Token_p->type != SELFMAKE_TOKEN_ROUND_BRACKET_RIGHT)
    {
        switch (Token_p->type)
        {
            case SELFMAKE_TOKEN_IDENTIFIER :
            case SELFMAKE_TOKEN_STRING :
                Definition_p->Function.arguments_pp[Definition_p->Function.arguments++] = Token_p->string_p;
                Definition_p->Function.arguments_pp = realloc(Definition_p->Function.arguments_pp, sizeof(NH_BYTE*) * (Definition_p->Function.arguments + 1));
                break;
        }
        ++Token_p;
    }

SELFMAKE_END(++Token_p)
}

static selfmake_Token *selfmake_parseBlock(
    selfmake_Token *Token_p, selfmake_Definition *Definition_p)
{
SELFMAKE_BEGIN()

    Definition_p->type = SELFMAKE_DEFINITION_BLOCK;
    Definition_p->Block.Definitions_p = NULL;
    Definition_p->Block.definitions   = 0;

    if (Token_p->type == SELFMAKE_TOKEN_CURLY_BRACKET_RIGHT) {
        SELFMAKE_END(Token_p)
    }

    Definition_p->Block.Definitions_p = malloc(sizeof(selfmake_Definition));
    SELFMAKE_CHECK_NULL(NULL, Definition_p->Block.Definitions_p)

    while (Token_p->type != SELFMAKE_TOKEN_CURLY_BRACKET_RIGHT)
    {        
        Token_p = selfmake_parseToken(Token_p, &Definition_p->Block.Definitions_p[Definition_p->Block.definitions++]);
        Definition_p->Block.Definitions_p = realloc(Definition_p->Block.Definitions_p, sizeof(selfmake_Definition) * (Definition_p->Block.definitions + 1));
    }

SELFMAKE_END(Token_p)
}

static selfmake_Token *selfmake_parseLongOption(
    selfmake_Token *Token_p, selfmake_Definition *Definition_p)
{
SELFMAKE_BEGIN()

    Definition_p->type = SELFMAKE_DEFINITION_LONG_OPTION;

    if (Token_p->type != SELFMAKE_TOKEN_IDENTIFIER) {
        SELFMAKE_END(Token_p)
    }

    Definition_p->LongOption.name_p = Token_p->string_p;
    Definition_p->LongOption.description_p = NULL;
    Definition_p->LongOption.Block_p = malloc(sizeof(selfmake_Block));
    SELFMAKE_CHECK_NULL(NULL, Definition_p->LongOption.Block_p)

    ++Token_p;

    if (Token_p->type == SELFMAKE_TOKEN_STRING) {
        Definition_p->LongOption.description_p = Token_p->string_p;
        ++Token_p;
    }
    if (Token_p->type != SELFMAKE_TOKEN_CURLY_BRACKET_LEFT) {
        SELFMAKE_END(Token_p)
    }

SELFMAKE_END(selfmake_parseBlock(++Token_p, Definition_p->LongOption.Block_p))
}

static selfmake_Token *selfmake_parseToken(
    selfmake_Token *Token_p, selfmake_Definition *Definition_p) 
{
SELFMAKE_BEGIN()

    Definition_p->type = SELFMAKE_DEFINITION_UNDEFINED;

    switch (Token_p->type)
    {
        case SELFMAKE_TOKEN_CURLY_BRACKET_RIGHT :
        case SELFMAKE_TOKEN_CURLY_BRACKET_LEFT  :
        case SELFMAKE_TOKEN_ROUND_BRACKET_RIGHT :
        case SELFMAKE_TOKEN_ROUND_BRACKET_LEFT  :
        case SELFMAKE_TOKEN_ANGLE_BRACKET_RIGHT :
        case SELFMAKE_TOKEN_ANGLE_BRACKET_LEFT  :
        case SELFMAKE_TOKEN_COMMA               :
        case SELFMAKE_TOKEN_STRING              :
            break;

        case SELFMAKE_TOKEN_IDENTIFIER :
            SELFMAKE_END(selfmake_parseFunction(Token_p, Definition_p))

        case SELFMAKE_TOKEN_HYPHEN_MINUS :
            if (Token_p[1].type == SELFMAKE_TOKEN_HYPHEN_MINUS) {
                SELFMAKE_END(selfmake_parseLongOption(Token_p + 2, Definition_p))
            }
            break;
    }

SELFMAKE_END(++Token_p)
}

SELFMAKE_RESULT selfmake_parseInstallerFile() 
{
SELFMAKE_BEGIN()

#include SELFMAKE_DEFAULT_CHECK

    SELFMAKE_CHECK(selfmake_tokenizeInstallerFile())

    selfmake_Token *Token_p = SELFMAKE_TOKENIZER.Tokens_p;

    SELFMAKE_PARSER.definitions = 0;
    SELFMAKE_PARSER.Definitions_p = malloc(sizeof(selfmake_Definition));
    SELFMAKE_CHECK_NULL(SELFMAKE_PARSER.Definitions_p)

    while (Token_p->type != SELFMAKE_TOKEN_EOF) {
        Token_p = selfmake_parseToken(Token_p, &SELFMAKE_PARSER.Definitions_p[SELFMAKE_PARSER.definitions]);
        if (SELFMAKE_PARSER.Definitions_p[SELFMAKE_PARSER.definitions].type != SELFMAKE_DEFINITION_UNDEFINED) {
            SELFMAKE_PARSER.definitions++;
            SELFMAKE_PARSER.Definitions_p = realloc(SELFMAKE_PARSER.Definitions_p, sizeof(selfmake_Definition) * (SELFMAKE_PARSER.definitions + 1));
        }
    }

#include SELFMAKE_CUSTOM_CHECK

SELFMAKE_DIAGNOSTIC_END(SELFMAKE_SUCCESS)
}

// SHOW ============================================================================================

static void selfmake_showParserDefinition(
    selfmake_Definition *Definition_p, unsigned int depth)
{
SELFMAKE_BEGIN()

    NH_BYTE depth_p[255];
    memset(depth_p, 0, 255);

    for (int i = 0; i < (depth * 2); ++i) {depth_p[i] = ' ';}

    switch (Definition_p->type)
    {
        case SELFMAKE_DEFINITION_FUNCTION :
            selfmake_messagef("%s%s", depth_p, Definition_p->Function.name_p);
            for (int j = 0; j < Definition_p->Function.arguments; ++j) {
                selfmake_messagef("%s  %s", depth_p, Definition_p->Function.arguments_pp[j]);
            }
            break;
        case SELFMAKE_DEFINITION_BLOCK :
            for (int j = 0; j < Definition_p->Block.definitions; ++j) {
                selfmake_showParserDefinition(&Definition_p->Block.Definitions_p[j], depth + 1);
            }
            break;
        case SELFMAKE_DEFINITION_LONG_OPTION :
            selfmake_messagef("%s--%s", depth_p, Definition_p->LongOption.name_p);
            selfmake_showParserDefinition(Definition_p->LongOption.Block_p, depth + 1);
            break;
    }

SELFMAKE_SILENT_END()
}

void selfmake_showParserOutput()
{
SELFMAKE_BEGIN()

    for (int i = 0; i < SELFMAKE_PARSER.definitions; ++i) {
        selfmake_showParserDefinition(&SELFMAKE_PARSER.Definitions_p[i], 0);
    }

SELFMAKE_SILENT_END()
}

