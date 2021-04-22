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
    sm_Runtime *Runtime_p, sm_Parser *Parser_p, SM_BYTE *name_p)
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

static sm_Option *sm_getOptionFromParser(
    sm_Runtime *Runtime_p, sm_Parser *Parser_p, SM_BYTE *name_p, SM_BYTE **argv_pp, int args)
{
SM_BEGIN()

    for (int d = 0; d < Parser_p->definitions; ++d) {
        if (Parser_p->Definitions_p[d].type == SM_DEFINITION_OPTION) {
            if (!strcmp(Parser_p->Definitions_p[d].Option.name_p, name_p)
            &&  Parser_p->Definitions_p[d].Option.arguments == args) {
                SM_BOOL sameArguments = SM_TRUE;
                for (int i = 0; i < args; ++i) {
                    SM_BYTE *argument_p = Parser_p->Definitions_p[d].Option.arguments_pp[i];
                    SM_BYTE *offsetArgument_p = argument_p;
                    if (!strcmp(Parser_p->Definitions_p[d].Option.name_p, "b")) {
                        offsetArgument_p = sm_offsetBuildArgumentPrefix(Runtime_p, Parser_p, argument_p);
                    }
                    if (strcmp(argument_p, argv_pp[i]) && strcmp(offsetArgument_p, argv_pp[i])) {
                        sameArguments = SM_FALSE; 
                        break;
                    }
                }
                if (sameArguments) {
                    SM_END(&Parser_p->Definitions_p[d].Option)
                }
            }
        }
    }

SM_END(NULL)
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

static SM_BOOL sm_optionNameExists(
    sm_Runtime *Runtime_p, SM_BYTE *name_p)
{
SM_BEGIN()

    for (int j = 0; j < Runtime_p->ParserArray.length; ++j) {
        sm_Parser *Parser_p = &Runtime_p->ParserArray.Parsers_p[j];
        for (int d = 0; d < Parser_p->definitions; ++d) {
            if (Parser_p->Definitions_p[d].type == SM_DEFINITION_OPTION) {
                if (!strcmp(Parser_p->Definitions_p[d].Option.name_p, name_p)) {
                    SM_END(SM_TRUE)
                }
            }
        } 
    }

SM_END(SM_FALSE)
}

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
        else {SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)}
    }
    else {
        for (int d = 0; d < Option_p->Block_p->Block.definitions; ++d) {
            if (Option_p->Block_p->Block.Definitions_p[d].type == SM_DEFINITION_FUNCTION) {
                SM_CHECK(sm_executeFunction(Runtime_p, &Option_p->Block_p->Block.Definitions_p[d].Function))
            }
        }
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
            &&  (!strcmp(Definition_p->Option.arguments_pp[0], name_p) || !strcmp(sm_offsetBuildArgumentPrefix(Runtime_p, Parser_p, Definition_p->Option.arguments_pp[0]), name_p))) {
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

// PARSE ===========================================================================================

SM_RESULT sm_parseOption(
    sm_Runtime *Runtime_p, int argc, SM_BYTE **argv_pp, int *advance_p)
{
SM_BEGIN()

    int expect = 0;
    sm_Option *Option_p = NULL;

    for (int i = 0; i < argc; ++i) 
    {
        if (argv_pp[i][0] == '-' && expect) {
            sm_noticef("Invalid option \"%s\"", argv_pp[i - expect]);
            SM_DIAGNOSTIC_END(SM_ERROR_INVALID_OPTION)
        }
        if (argv_pp[i][0] == '-' && Option_p) {
            *advance_p = i;
            SM_END(sm_executeOption(Runtime_p, Option_p))
        }

        if (argv_pp[i][0] == '-' && argv_pp[i][1] == '-')
        {
            Option_p = sm_getOption(Runtime_p, argv_pp[i] + 2, NULL, 0);

            if (!Option_p) {
                if (!sm_optionNameExists(Runtime_p, argv_pp[i] + 2)) {
                    sm_noticef("Invalid option \"%s\"", argv_pp[i]);
                    SM_DIAGNOSTIC_END(SM_ERROR_INVALID_OPTION)
                }
                expect = 1;
            }
        }
        else if (argv_pp[i][0] == '-' && argv_pp[i][1] != '-')
        {
            Option_p = sm_getOption(Runtime_p, argv_pp[i] + 1, NULL, 0);

            if (!Option_p) {
                if (!sm_optionNameExists(Runtime_p, argv_pp[i] + 1)) {
                    sm_noticef("Invalid option \"%s\"", argv_pp[i]);
                    SM_DIAGNOSTIC_END(SM_ERROR_INVALID_OPTION)
                }
                expect = 1;
            }
        }
        else if (expect) {
            sm_Option *Match_p = sm_getOption(Runtime_p, argv_pp[i - expect][1] == '-' ? argv_pp[i - expect] + 2 : argv_pp[i - expect] + 1, &argv_pp[i - (expect - 1)], expect);
            if (Match_p) {
                Option_p = Match_p;
                expect = 0;
            }
        }
        else if (Option_p) {
            sm_Option *Match_p = sm_getOption(Runtime_p, Option_p->name_p, &argv_pp[i - Option_p->arguments], Option_p->arguments + 1);
            if (!Match_p) {
                SM_CHECK(sm_executeOption(Runtime_p, Option_p))
                Match_p = sm_getOption(Runtime_p, Option_p->name_p, &argv_pp[i], 1);
                if (!Match_p) {
                    sm_noticef("Invalid option \"%s\"", argv_pp[i]);
                    SM_DIAGNOSTIC_END(SM_ERROR_INVALID_OPTION)
                }
                Option_p = Match_p;
            }
            else {
                SM_CHECK(sm_executeOption(Runtime_p, Option_p))
                Option_p = Match_p;
            }
        }
    }

    if (Option_p) {
        SM_CHECK(sm_executeOption(Runtime_p, Option_p))
        *advance_p = argc;
    }
    else {SM_DIAGNOSTIC_END(SM_ERROR_BAD_STATE)}

SM_DIAGNOSTIC_END(SM_SUCCESS)
}

