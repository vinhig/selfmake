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

static sm_Token *sm_parseOption(
    sm_Token *Token_p, sm_Definition *Definition_p)
{
SM_BEGIN()

    Definition_p->type = SM_DEFINITION_OPTION;

    sm_Token *Description_p = Token_p->type == SM_TOKEN_STRING ? Token_p : NULL;
    SM_BOOL longOption = SM_FALSE;

    if (Token_p->type == SM_TOKEN_STRING) {
        if (Token_p[2].type == SM_TOKEN_HYPHEN_MINUS) {
            longOption = SM_TRUE; 
            Token_p = Token_p + 3;
        }
        else {Token_p = Token_p + 2;}
    }
    else {
        if (Token_p[1].type == SM_TOKEN_HYPHEN_MINUS) {
            longOption = SM_TRUE;
            Token_p = Token_p + 2;
        }
        else {Token_p = Token_p + 1;}
    }

    Definition_p->Option.arguments = 0;
    Definition_p->Option.arguments_pp = NULL;
    Definition_p->Option.longOption = longOption;
    Definition_p->Option.name_p = Token_p->string_p;
    Definition_p->Option.description_p = Description_p ? Description_p->string_p : NULL;
    Definition_p->Option.Block_p = malloc(sizeof(sm_Block));
    SM_CHECK_NULL(NULL, Definition_p->Option.Block_p)

    ++Token_p;

    sm_Token *Argument_p = Token_p;
    int arguments = 0;
    while (Argument_p->type == SM_TOKEN_IDENTIFIER || Argument_p->type == SM_TOKEN_STRING) {
        arguments++;
        Argument_p++;
    }

    if (arguments) {
        Definition_p->Option.arguments = arguments;
        Definition_p->Option.arguments_pp = malloc(sizeof(SM_BYTE*) * arguments);
        SM_CHECK_NULL(NULL, Definition_p->Option.arguments_pp)
        for (int i = 0; i < arguments; ++i) {
            Definition_p->Option.arguments_pp[i] = Token_p->string_p;
            Token_p++;
        }
    }

    if (Token_p->type != SM_TOKEN_CURLY_BRACKET_LEFT) {
        SM_END(Token_p)
    }

SM_END(sm_parseBlock(++Token_p, Definition_p->Option.Block_p))
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
            break;

        case SM_TOKEN_STRING :
            if (Token_p[1].type == SM_TOKEN_HYPHEN_MINUS) {
                SM_END(sm_parseOption(Token_p, Definition_p))
            }
            break;

        case SM_TOKEN_IDENTIFIER :
            SM_END(sm_parseFunction(Token_p, Definition_p))

        case SM_TOKEN_HYPHEN_MINUS :
            SM_END(sm_parseOption(Token_p, Definition_p))
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
            sm_messagef("%s{", depth_p);
            for (int j = 0; j < Definition_p->Block.definitions; ++j) {
                sm_showParserDefinition(&Definition_p->Block.Definitions_p[j], depth + 1);
            }
            sm_messagef("%s}", depth_p);
            break;

        case SM_DEFINITION_OPTION :
            if (Definition_p->Option.longOption) {
                sm_messagef("%s--%s", depth_p, Definition_p->Option.name_p);
            }
            else {
                sm_messagef("%s-%s", depth_p, Definition_p->Option.name_p);
            }
            if (Definition_p->Option.description_p) {
                sm_messagef("%s  description: %s", depth_p, Definition_p->Option.description_p);
            }
            for (int i = 0; i < Definition_p->Option.arguments; ++i) {
                sm_messagef("%s  argument %d: %s", depth_p, i, Definition_p->Option.arguments_pp[i]);
            }
            sm_showParserDefinition(Definition_p->Option.Block_p, depth + 1);
            break;
    }

SM_SILENT_END()
}

static void sm_showParseTree(
    sm_Parser *Parser_p)
{
SM_BEGIN()

    sm_operationf("SHOW_PARSE_TREE");

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

