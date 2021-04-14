#ifndef SELFMAKE_PARSER_H
#define SELFMAKE_PARSER_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#include "../Common/API.h"
#include "../Common/Result.h"

#endif

/** @addtogroup nhmakeEnums
 *  @{
 */

    typedef enum SELFMAKE_DEFINITION {
        SELFMAKE_DEFINITION_UNDEFINED,
        SELFMAKE_DEFINITION_LONG_OPTION,
        SELFMAKE_DEFINITION_FUNCTION,
        SELFMAKE_DEFINITION_BLOCK,
    } SELFMAKE_DEFINITION;
    
/** @} */

/** @addtogroup nhmakeStructs
 *  @{
 */

    typedef struct selfmake_Function {
        SELFMAKE_DEFINITION type;
        NH_BYTE *name_p;
        unsigned int arguments;
        NH_BYTE **arguments_pp;
    } selfmake_Function;

    typedef union selfmake_Definition selfmake_Definition;

    typedef struct selfmake_Block {
        SELFMAKE_DEFINITION type;
        unsigned int definitions;
        selfmake_Definition *Definitions_p;
    } selfmake_Block;

    typedef struct selfmake_LongOption {
        SELFMAKE_DEFINITION type;
        NH_BYTE *name_p;
        NH_BYTE *description_p;
        selfmake_Definition *Block_p;
    } selfmake_LongOption;

    typedef union selfmake_Definition {
        SELFMAKE_DEFINITION type;
        selfmake_LongOption LongOption;
        selfmake_Function Function;
        selfmake_Block Block;
    } selfmake_Definition;

    typedef struct selfmake_Parser {
        unsigned int definitions;
        selfmake_Definition *Definitions_p;
    } selfmake_Parser;

/** @} */

/** @addtogroup nhmakeVars
 *  @{
 */

    extern selfmake_Parser SELFMAKE_PARSER;

/** @} */

/** @addtogroup nhmakeFunctions
 *  @{
 */

    SELFMAKE_RESULT selfmake_parseInstallerFile(
    ); 

    void selfmake_showParserOutput(
    );

/** @} */

#endif
