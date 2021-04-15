// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Parser.h"

#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_CUSTOM_CHECK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

// PARSE ===========================================================================================

static sm_Token *sm_parseToken(
    sm_Token *Token_p, sm_Definition *Definition_p
); 

static sm_Token *sm_parseFunction(
    sm_Token *Token_p, sm_Definition *Definition_p)
{
SM_BEGIN()

    Definition_p->type = SM_DEFINITION_FUNCTION;
    Definition_p->Function.name_p = Token_p->string_p;
    Definition_p->Function.arguments_pp = NULL; 
    Definition_p->Function.arguments = 0;

    ++Token_p;

    if (Token_p->type != SM_TOKEN_ROUND_BRACKET_LEFT) {
        SM_END(Token_p)
    }

    Definition_p->Function.arguments_pp = malloc(sizeof(SM_BYTE*));
    SM_CHECK_NULL(NULL, Definition_p->Function.arguments_pp)

    Definition_p->Function.argumentTypes_p = malloc(sizeof(SM_TOKEN));
    SM_CHECK_NULL(NULL, Definition_p->Function.argumentTypes_p)

    while (Token_p->type != SM_TOKEN_ROUND_BRACKET_RIGHT)
    {
        switch (Token_p->type)
        {
            case SM_TOKEN_IDENTIFIER :
            case SM_TOKEN_STRING :
                Definition_p->Function.argumentTypes_p[Definition_p->Function.arguments] = Token_p->type;
                Definition_p->Function.arguments_pp[Definition_p->Function.arguments++] = Token_p->string_p;
                Definition_p->Function.arguments_pp = realloc(Definition_p->Function.arguments_pp, sizeof(SM_BYTE*) * (Definition_p->Function.arguments + 1));
                Definition_p->Function.argumentTypes_p = realloc(Definition_p->Function.argumentTypes_p, sizeof(SM_TOKEN) * (Definition_p->Function.arguments + 1));
                break;
        }
        ++Token_p;
    }

SM_END(++Token_p)
}

static sm_Token *sm_parseBlock(
    sm_Token *Token_p, sm_Definition *Definition_p)
{
SM_BEGIN()

    Definition_p->type = SM_DEFINITION_BLOCK;
    Definition_p->Block.Definitions_p = NULL;
    Definition_p->Block.definitions   = 0;

    if (Token_p->type == SM_TOKEN_CURLY_BRACKET_RIGHT) {
        SM_END(Token_p)
    }

    Definition_p->Block.Definitions_p = malloc(sizeof(sm_Definition));
    SM_CHECK_NULL(NULL, Definition_p->Block.Definitions_p)

    while (Token_p->type != SM_TOKEN_CURLY_BRACKET_RIGHT)
    {        
        Token_p = sm_parseToken(Token_p, &Definition_p->Block.Definitions_p[Definition_p->Block.definitions++]);
        Definition_p->Block.Definitions_p = realloc(Definition_p->Block.Definitions_p, sizeof(sm_Definition) * (Definition_p->Block.definitions + 1));
    }

SM_END(Token_p)
}

static sm_Token *sm_parseLongOption(
    sm_Token *Token_p, sm_Definition *Definition_p)
{
SM_BEGIN()

    Definition_p->type = SM_DEFINITION_LONG_OPTION;

    if (Token_p->type != SM_TOKEN_IDENTIFIER) {
        SM_END(Token_p)
    }

    Definition_p->LongOption.name_p = Token_p->string_p;
    Definition_p->LongOption.description_p = NULL;
    Definition_p->LongOption.Block_p = malloc(sizeof(sm_Block));
    SM_CHECK_NULL(NULL, Definition_p->LongOption.Block_p)

    ++Token_p;

    if (Token_p->type == SM_TOKEN_STRING) {
        Definition_p->LongOption.description_p = Token_p->string_p;
        ++Token_p;
    }
    if (Token_p->type != SM_TOKEN_CURLY_BRACKET_LEFT) {
        SM_END(Token_p)
    }

SM_END(sm_parseBlock(++Token_p, Definition_p->LongOption.Block_p))
}

static sm_Token *sm_parseToken(
    sm_Token *Token_p, sm_Definition *Definition_p) 
{
SM_BEGIN()

    Definition_p->type = SM_DEFINITION_UNDEFINED;

    switch (Token_p->type)
    {
        case SM_TOKEN_CURLY_BRACKET_RIGHT :
        case SM_TOKEN_CURLY_BRACKET_LEFT  :
        case SM_TOKEN_ROUND_BRACKET_RIGHT :
        case SM_TOKEN_ROUND_BRACKET_LEFT  :
        case SM_TOKEN_ANGLE_BRACKET_RIGHT :
        case SM_TOKEN_ANGLE_BRACKET_LEFT  :
        case SM_TOKEN_COMMA               :
        case SM_TOKEN_STRING              :
            break;

        case SM_TOKEN_IDENTIFIER :
            SM_END(sm_parseFunction(Token_p, Definition_p))

        case SM_TOKEN_HYPHEN_MINUS :
            if (Token_p[1].type == SM_TOKEN_HYPHEN_MINUS) {
                SM_END(sm_parseLongOption(Token_p + 2, Definition_p))
            }
            break;
    }

SM_END(++Token_p)
}

static SM_RESULT sm_parseFile(
    sm_Parser *Parser_p, sm_File *File_p) 
{
SM_BEGIN()

#include SM_DEFAULT_CHECK

    sm_Tokenizer Tokenizer = sm_initTokenizer();
    SM_CHECK(sm_tokenizeFile(&Tokenizer, File_p))

    sm_Token *Token_p = Tokenizer.Tokens_p;

    Parser_p->definitions = 0;
    Parser_p->Definitions_p = malloc(sizeof(sm_Definition));
    SM_CHECK_NULL(Parser_p->Definitions_p)

    while (Token_p->type != SM_TOKEN_EOF) {
        Token_p = sm_parseToken(Token_p, &Parser_p->Definitions_p[Parser_p->definitions]);
        if (Parser_p->Definitions_p[Parser_p->definitions].type != SM_DEFINITION_UNDEFINED) {
            Parser_p->definitions++;
            Parser_p->Definitions_p = realloc(Parser_p->Definitions_p, sizeof(sm_Definition) * (Parser_p->definitions + 1));
        }
    }

#include SM_CUSTOM_CHECK

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// SHOW ============================================================================================

static void sm_showParserDefinition(
    sm_Definition *Definition_p, unsigned int depth)
{
SM_BEGIN()

    SM_BYTE depth_p[255];
    memset(depth_p, 0, 255);

    for (int i = 0; i < (depth * 2); ++i) {depth_p[i] = ' ';}

    switch (Definition_p->type)
    {
        case SM_DEFINITION_FUNCTION :
            sm_messagef("%s%s", depth_p, Definition_p->Function.name_p);
            for (int j = 0; j < Definition_p->Function.arguments; ++j) {
                sm_messagef("%s  %s", depth_p, Definition_p->Function.arguments_pp[j]);
            }
            break;
        case SM_DEFINITION_BLOCK :
            for (int j = 0; j < Definition_p->Block.definitions; ++j) {
                sm_showParserDefinition(&Definition_p->Block.Definitions_p[j], depth + 1);
            }
            break;
        case SM_DEFINITION_LONG_OPTION :
            sm_messagef("%s--%s", depth_p, Definition_p->LongOption.name_p);
            sm_showParserDefinition(Definition_p->LongOption.Block_p, depth + 1);
            break;
    }

SM_SILENT_END()
}

static void sm_showParseTree(
    sm_Parser *Parser_p)
{
SM_BEGIN()

    for (int i = 0; i < Parser_p->definitions; ++i) {
        sm_showParserDefinition(&Parser_p->Definitions_p[i], 0);
    }

SM_SILENT_END()
}

// PARSER ARRAY ====================================================================================

void sm_initParserArray(
    sm_ParserArray *Array_p)
{
SM_BEGIN()

    Array_p->length = 0;
    Array_p->Parsers_p = NULL;

SM_SILENT_END()
}

static sm_Parser sm_initParser()
{
SM_BEGIN()

    sm_Parser Parser;
    Parser.executed = SM_FALSE;
    Parser.Definitions_p = NULL;
    Parser.definitions = 0;

SM_END(Parser)
}

SM_RESULT sm_appendParser(
    sm_ParserArray *Array_p, sm_File *File_p, SM_BOOL showParseTree)
{
SM_BEGIN()

#include SM_DEFAULT_CHECK

    if (!Array_p->Parsers_p) {
        Array_p->Parsers_p = malloc(sizeof(sm_Parser));
        SM_CHECK_NULL(Array_p->Parsers_p)
    }
    else {
        Array_p->Parsers_p = realloc(Array_p->Parsers_p, sizeof(sm_Parser) * (Array_p->length + 1));
        SM_CHECK_NULL(Array_p->Parsers_p)
    }

    sm_Parser *Parser_p = &Array_p->Parsers_p[Array_p->length];

    *Parser_p = sm_initParser();
    SM_CHECK(sm_parseFile(Parser_p, File_p))

    if (showParseTree) {
        sm_showParseTree(Parser_p);
    }

    Array_p->length++;

#include SM_CUSTOM_CHECK

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

