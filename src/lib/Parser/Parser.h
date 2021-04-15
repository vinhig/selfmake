#ifndef SM_PARSER_H
#define SM_PARSER_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#include "Tokenizer.h"

#include "../Common/Types.h"
#include "../Common/Result.h"

#include "../Core/File.h"

#endif

/** @addtogroup selfmakeEnums
 *  @{
 */

    typedef enum SM_DEFINITION {
        SM_DEFINITION_UNDEFINED,
        SM_DEFINITION_LONG_OPTION,
        SM_DEFINITION_FUNCTION,
        SM_DEFINITION_BLOCK,
    } SM_DEFINITION;
    
/** @} */

/** @addtogroup selfmakeStructs
 *  @{
 */

    typedef struct sm_Function {
        SM_DEFINITION type;
        SM_BYTE *name_p;
        unsigned int arguments;
        SM_BYTE **arguments_pp;
        SM_TOKEN *argumentTypes_p;
    } sm_Function;

    typedef union sm_Definition sm_Definition;

    typedef struct sm_Block {
        SM_DEFINITION type;
        unsigned int definitions;
        sm_Definition *Definitions_p;
    } sm_Block;

    typedef struct sm_LongOption {
        SM_DEFINITION type;
        SM_BYTE *name_p;
        SM_BYTE *description_p;
        sm_Definition *Block_p;
    } sm_LongOption;

    typedef union sm_Definition {
        SM_DEFINITION type;
        sm_LongOption LongOption;
        sm_Function Function;
        sm_Block Block;
    } sm_Definition;

    typedef struct sm_Parser {
        SM_BOOL executed;
        unsigned int definitions;
        sm_Definition *Definitions_p;
    } sm_Parser;

    typedef struct sm_ParserArray {
        int length;
        sm_Parser *Parsers_p;
    } sm_ParserArray;

/** @} */

/** @addtogroup selfmakeFunctions
 *  @{
 */

    void sm_initParserArray(
        sm_ParserArray *Array_p
    );

    SM_RESULT sm_appendParser(
        sm_ParserArray *Array_p, sm_File *File_p, SM_BOOL showParseTree
    ); 

/** @} */

#endif
