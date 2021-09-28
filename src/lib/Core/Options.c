// LICENSE NOTICE ==================================================================================

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

// INCLUDE =========================================================================================

#include "Options.h"
#include "Thread.h"
#include "File.h"
#include "Build.h"

#include "../Parser/Functions.h"
#include "../Parser/Variables.h"

#include "../Common/Functions.h"
#include "../Common/Macros/Macros.h"
#include SM_FLOW
#include SM_DEFAULT_CHECK

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// HELPER ==========================================================================================

static SM_BYTE *sm_offsetBuildArgumentPrefix(
    sm_Runtime *Runtime_p, SM_BYTE *name_p)
{
SM_BEGIN()

    sm_Variable *Prefix_p = sm_getVariable(&Runtime_p->VariableArray, "PREFIX");

    if (Prefix_p) {
        int offset = 0;
        for (offset = 0; offset < strlen(Prefix_p->values_pp[0]) && offset < strlen(name_p) 
            && Prefix_p->values_pp[0][offset] == name_p[offset]; ++offset) {};
        if (offset == strlen(Prefix_p->values_pp[0])) {
            SM_END(name_p + offset)
        }
    }

SM_END(name_p)
}

// GET OPTION ======================================================================================

static sm_Option *sm_getOptionFromBlock(
    sm_Runtime *Runtime_p, sm_Block *Block_p, SM_BYTE *name_p, SM_BYTE **argv_pp, int args)
{
SM_BEGIN()

    for (int d = 0; d < Block_p->definitions; ++d) 
    {
        switch (Block_p->Definitions_p[d].type)
        {
            case SM_DEFINITION_IF :
                if (sm_compareIf(Runtime_p, &Block_p->Definitions_p[d].If, SM_TRUE)) {
                    sm_Option *Option_p = sm_getOptionFromBlock(Runtime_p, &Block_p->Definitions_p[d].If.Block_p->Block, name_p, argv_pp, args);
                    if (Option_p) {SM_END(Option_p)}
                }
                break;

            case SM_DEFINITION_OPTION :
                if (!strcmp(Block_p->Definitions_p[d].Option.name_p, name_p)
                &&  Block_p->Definitions_p[d].Option.arguments == args) {
                    SM_BOOL sameArguments = SM_TRUE;
                    for (int i = 0; i < args; ++i) {
                        SM_BYTE *argument_p = Block_p->Definitions_p[d].Option.arguments_pp[i];
                        SM_BYTE *offsetArgument_p = argument_p;
                        if (Block_p->Definitions_p[d].Option.name_p[0] == 'b' || Block_p->Definitions_p[d].Option.name_p[0] == 't') {
                            offsetArgument_p = sm_offsetBuildArgumentPrefix(Runtime_p, argument_p);
                        }
                        if (strcmp(argument_p, argv_pp[i]) && strcmp(offsetArgument_p, argv_pp[i])) {
                            sameArguments = SM_FALSE; 
                            break;
                        }
                    }
                    if (sameArguments) {
                        SM_END(&Block_p->Definitions_p[d].Option)
                    }
                }
                break;
        }
    }

SM_END(NULL)
}

static sm_Option *sm_getOptionFromParser(
    sm_Runtime *Runtime_p, sm_Parser *Parser_p, SM_BYTE *name_p, SM_BYTE **argv_pp, int args)
{
SM_BEGIN()

    sm_Block Block;
    Block.definitions = Parser_p->definitions;
    Block.Definitions_p = Parser_p->Definitions_p;

SM_END(sm_getOptionFromBlock(Runtime_p, &Block, name_p, argv_pp, args))
}

static sm_Option *sm_getShortOptions(
    sm_Runtime *Runtime_p, SM_BYTE *name_p, SM_BYTE **argv_pp, int args, int *options_p)
{
SM_BEGIN()

    *options_p = 0;

    sm_Option *Options_p = NULL;
    SM_BYTE c = 0;

    for (int i = 0; i < strlen(name_p); ++i) 
    {
        if (i + 1 < strlen(name_p)) {
            c = name_p[i+1];
            name_p[i+1] = 0;
        }

        sm_Option *Option_p = NULL;
        for (int j = 0; j < Runtime_p->ParserArray.length; ++j) {
            sm_Parser *Parser_p = &Runtime_p->ParserArray.Parsers_p[j];
            Option_p = sm_getOptionFromParser(Runtime_p, Parser_p, name_p+i, argv_pp, args);
            if (Option_p) {break;}
        }

        if (Option_p) {
            if (*options_p == 0) {
                Options_p = Option_p;
            }
            else if (*options_p == 1) {
                sm_Option *Tmp_p = malloc(sizeof(sm_Option)*2);
                Tmp_p[0] = *Options_p;
                Tmp_p[1] = *Option_p;
                Options_p = Tmp_p;
            }
            else {
                Options_p = realloc(Options_p, sizeof(sm_Option)*(*options_p+1));
                Options_p[*options_p] = *Option_p;
            }
            *options_p += 1;
        }

        if (c != 0) {
            name_p[i+1] = c;
            c = 0;
        }
    }

SM_END(Options_p)
}

static sm_Option *sm_getOption(
    sm_Runtime *Runtime_p, SM_BYTE *name_p, SM_BYTE **argv_pp, int args)
{
SM_BEGIN()

    sm_Option *Option_p = NULL;

    for (int j = 0; j < Runtime_p->ParserArray.length; ++j) {
        sm_Parser *Parser_p = &Runtime_p->ParserArray.Parsers_p[j];
        Option_p = sm_getOptionFromParser(Runtime_p, Parser_p, name_p, argv_pp, args);
        if (Option_p) {break;}
    }

SM_END(Option_p)
}

// OPTION NAME =====================================================================================

static SM_BOOL sm_optionNameExistsInBlock(
    sm_Runtime *Runtime_p, sm_Block *Block_p, SM_BYTE *name_p)
{
SM_BEGIN()

    for (int d = 0; d < Block_p->definitions; ++d) {
        switch (Block_p->Definitions_p[d].type) {
            case SM_DEFINITION_OPTION :
                if (!strcmp(Block_p->Definitions_p[d].Option.name_p, name_p)) {
                    SM_END(SM_TRUE)
                }
                break;
            case SM_DEFINITION_IF :
                if (sm_compareIf(Runtime_p, &Block_p->Definitions_p[d].If, SM_TRUE)) {
                    if (sm_optionNameExistsInBlock(Runtime_p, &Block_p->Definitions_p[d].If.Block_p->Block, name_p)) {
                        SM_END(SM_TRUE)
                    }
                }
                break;
        }
          
    } 

SM_END(SM_FALSE)
}

static SM_BOOL sm_optionNameExists(
    sm_Runtime *Runtime_p, SM_BYTE *name_p)
{
SM_BEGIN()

    for (int i = 0; i < Runtime_p->ParserArray.length; ++i) {
        sm_Parser *Parser_p = &Runtime_p->ParserArray.Parsers_p[i];
        sm_Block Block;
        Block.definitions = Parser_p->definitions;
        Block.Definitions_p = Parser_p->Definitions_p;
        if (sm_optionNameExistsInBlock(Runtime_p, &Block, name_p)) {
            SM_END(SM_TRUE)
        } 
    }

SM_END(SM_FALSE)
}

// EXECUTE OPTION ==================================================================================

static SM_RESULT sm_printOption(
    sm_Option *Option_p)
{
SM_BEGIN()

    SM_BYTE args_p[1024]; // TODO malloc
    memset(args_p, 0, 1024);
    for (int i = 0; i < Option_p->arguments; ++i) {
        sprintf(args_p + strlen(args_p), "%s ", Option_p->arguments_pp[i]); 
    }

    sm_operationf(Option_p->longOption ? "--%s %s'%s'" : "-%s %s'%s'", Option_p->name_p, args_p, Option_p->description_p);

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

static SM_RESULT sm_executeOption(
    sm_Runtime *Runtime_p, sm_Option *Option_p)
{
SM_BEGIN()

    SM_CHECK(sm_printOption(Option_p))

    if (!Option_p->Block_p) {
        if (!strcmp(Option_p->name_p, "b")) {
            SM_CHECK(sm_build(Runtime_p, Option_p->arguments_pp[0]))
        }
        else if (!strcmp(Option_p->name_p, "i")) {
        }
        else if (!strcmp(Option_p->name_p, "t")) {
            SM_CHECK(sm_test(Runtime_p, Option_p->arguments_pp[0]))
        }
        else {SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)}
    }
    else {
        SM_CHECK(sm_executeBlock(Runtime_p, &Option_p->Block_p->Block))
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// BUILD OPTIONS ===================================================================================

static SM_BOOL sm_buildOptionOverride(
    sm_Runtime *Runtime_p, SM_BYTE *name_p)
{
SM_BEGIN()

    for (int j = 0; j < Runtime_p->ParserArray.length; ++j)
    {
        sm_Parser *Parser_p = &Runtime_p->ParserArray.Parsers_p[j];

        for (int i = 0; i < Parser_p->definitions; ++i) 
        {
            sm_Definition *Definition_p = &Parser_p->Definitions_p[i];

            if (Definition_p->type == SM_DEFINITION_OPTION 
            &&  !Definition_p->Option.longOption
            &&  !strcmp(Definition_p->Option.name_p, "b") 
            &&  Definition_p->Option.arguments == 1 
            &&  (!strcmp(Definition_p->Option.arguments_pp[0], name_p) || !strcmp(sm_offsetBuildArgumentPrefix(Runtime_p, Definition_p->Option.arguments_pp[0]), name_p))) {
                SM_END(SM_TRUE)
            }
        }
    }

SM_END(SM_FALSE)
}

SM_RESULT sm_addBuildOption(
    sm_Runtime *Runtime_p, SM_BYTE *name_p)
{
SM_BEGIN()

    if (sm_buildOptionOverride(Runtime_p, name_p)) {SM_DIAGNOSTIC_END(SM_SUCCESS)}

    static SM_BYTE *option_p = "b";

    sm_Parser *Parser_p = &Runtime_p->ParserArray.Parsers_p[0];

    Parser_p->Definitions_p = realloc(Parser_p->Definitions_p, sizeof(sm_Definition) * (Parser_p->definitions + 1));
    SM_CHECK_NULL(Parser_p->Definitions_p)

    sm_Definition *Definition_p = &Parser_p->Definitions_p[Parser_p->definitions++];

    SM_BYTE *description_p = malloc(strlen(name_p) + 7);
    SM_CHECK_NULL(description_p)
    sprintf(description_p, "build %s", name_p);

    SM_BYTE *argument_p = malloc(strlen(name_p) + 1);
    SM_CHECK_NULL(argument_p)
    strcpy(argument_p, name_p);

    SM_BYTE **arguments_pp = malloc(sizeof(SM_BYTE*));
    SM_CHECK_NULL(arguments_pp)
    arguments_pp[0] = argument_p;

    Definition_p->type = SM_DEFINITION_OPTION;
    Definition_p->Option.arguments     = 1;
    Definition_p->Option.arguments_pp  = arguments_pp;
    Definition_p->Option.longOption    = SM_FALSE;
    Definition_p->Option.name_p        = option_p;
    Definition_p->Option.description_p = description_p;
    Definition_p->Option.Block_p       = NULL;

    sm_messagef("Generate Option '-b %s'", name_p);

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_RESULT sm_addTestOption(
    sm_Runtime *Runtime_p, SM_BYTE *name_p)
{
SM_BEGIN()

    static SM_BYTE *option_p = "t";

    sm_Parser *Parser_p = &Runtime_p->ParserArray.Parsers_p[0];

    Parser_p->Definitions_p = realloc(Parser_p->Definitions_p, sizeof(sm_Definition) * (Parser_p->definitions + 1));
    SM_CHECK_NULL(Parser_p->Definitions_p)

    sm_Definition *Definition_p = &Parser_p->Definitions_p[Parser_p->definitions++];

    SM_BYTE *description_p = malloc(strlen(name_p) + 7);
    SM_CHECK_NULL(description_p)
    sprintf(description_p, "test %s", name_p);

    SM_BYTE *argument_p = malloc(strlen(name_p) + 1);
    SM_CHECK_NULL(argument_p)
    strcpy(argument_p, name_p);

    SM_BYTE **arguments_pp = malloc(sizeof(SM_BYTE*));
    SM_CHECK_NULL(arguments_pp)
    arguments_pp[0] = argument_p;

    Definition_p->type = SM_DEFINITION_OPTION;
    Definition_p->Option.arguments     = 1;
    Definition_p->Option.arguments_pp  = arguments_pp;
    Definition_p->Option.longOption    = SM_FALSE;
    Definition_p->Option.name_p        = option_p;
    Definition_p->Option.description_p = description_p;
    Definition_p->Option.Block_p       = NULL;

    sm_messagef("Generate Option '-t %s'", name_p);

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

// PARSE ===========================================================================================

static SM_RESULT sm_parseShortOption(
    sm_Runtime *Runtime_p, int argc, SM_BYTE **argv_pp, int *advance_p)
{
SM_BEGIN()

    SM_BOOL match = SM_FALSE;
    int options = 0;
    sm_Option *Options_p = NULL;

    *advance_p = 1;

    for (int i = 1; i < argc; ++i) 
    {
        if (argv_pp[i][0] == '-') {
            break;
        }

        Options_p = sm_getShortOptions(Runtime_p, &argv_pp[0][1], &argv_pp[i], 1, &options);
        if (Options_p) {
            for (int j = 0; j < options; ++j) {
                SM_CHECK(sm_executeOption(Runtime_p, Options_p+j))
            }
            if (options > 1) {free(Options_p);}
            Options_p = NULL;
            match = SM_TRUE;
        }

        *advance_p += 1;
    }

    if (Options_p) {
        for (int i = 0; i < options; ++i) {
            SM_CHECK(sm_executeOption(Runtime_p, Options_p+i))
        }
        if (options > 1) {free(Options_p);}
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

static SM_RESULT sm_parseLongOption(
    sm_Runtime *Runtime_p, int argc, SM_BYTE **argv_pp, int *advance_p)
{
SM_BEGIN()

    SM_BOOL match = SM_FALSE;
    sm_Option *Option_p = NULL;

    for (int i = 0; i < argc; ++i) 
    {
        if (i > 0 && argv_pp[i][0] == '-') {
            break;
        }

        sm_Option *NewOption_p = 
            sm_getOption(Runtime_p, &argv_pp[0][2], i == 0 ? NULL : &argv_pp[1], *advance_p);

        if (NewOption_p) {
            Option_p = NewOption_p;
        }
        else {
            sm_noticef("Invalid option \"%s\"", argv_pp[i]);
            SM_DIAGNOSTIC_END(SM_ERROR_INVALID_OPTION)
        }

        *advance_p += 1;
    }

    if (Option_p) {
        SM_CHECK(sm_executeOption(Runtime_p, Option_p))
    }
 
SM_DIAGNOSTIC_END(SM_SUCCESS)
}

SM_RESULT sm_parseOption(
    sm_Runtime *Runtime_p, int argc, SM_BYTE **argv_pp, int *advance_p)
{
SM_BEGIN()

    int advance = 0;

    for (int i = 0; i < argc; i += advance) 
    {
        advance = 0;

        if (argv_pp[i][0] == '-' && argv_pp[i][1] != '-') {
            SM_CHECK(sm_parseShortOption(Runtime_p, argc-i, &argv_pp[i], &advance))
        }
        else if (argv_pp[i][0] == '-' && argv_pp[i][1] == '-') {
            SM_CHECK(sm_parseLongOption(Runtime_p, argc-i, &argv_pp[i], &advance))
        }

        *advance_p += advance;
    }

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

