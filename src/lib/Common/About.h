#ifndef SM_ABOUT_H
#define SM_ABOUT_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#endif

/** @defgroup selfmake selfmake
 *  @brief TODO 
 */

/** @defgroup selfmakeChangelog Changelog
 *  @ingroup selfmake
 */
/** @defgroup selfmakeMacros Macros 
 *  @ingroup selfmake
 */
/** @defgroup selfmakeEnums Enumerations
 *  @ingroup selfmake
 */
/** @defgroup selfmakeStructs Data Structures
 *  @ingroup selfmake
 */
/** @defgroup selfmakeFunctions Functions
 *  @ingroup selfmake
 */
/** @defgroup selfmakeTypedefs Typedefs
 *  @ingroup selfmake
 */
/** @defgroup selfmakeVars Variables
 *  @ingroup selfmake
 */

/** @addtogroup selfmakeChangelog
 *  @{
 */

    /**
     * API version.
     *
     * 2021-04-17 | v0.0.0.0
     * - [Dajo Frey](https://github.com/DajoFrey): Initial version.
     */
    #define SM_API_VERSION 0

    /**
     * Major version.
     *
     * 2021-04-21 | v0.1.0.0 <a id="v0.1.0.0"></a>
     * - [Dajo Frey](https://github.com/DajoFrey): Add first iteration of binary building, mostly implemented in sm_build().
     * - [Dajo Frey](https://github.com/DajoFrey): Add parameter to sm_addSourceContext() to offset argument parsing.
     * - [Dajo Frey](https://github.com/DajoFrey): Add sm_setBeforeBuildCallback() and sm_setAfterBuildCallback().
     * - [Dajo Frey](https://github.com/DajoFrey): Improve option parsing and language syntax. For this, sm_parseOption() is introduced.
     *
     * 2021-04-17 | v0.0.0.0
     * - [Dajo Frey](https://github.com/DajoFrey): Initial version.
     */
    #define SM_MAJOR_VERSION 1

    /**
     * Minor version.
     *
     * 2021-04-25 | v0.1.2.0 <a id="v0.1.2.0"></a>
     * - [Dajo Frey](https://github.com/DajoFrey): Add sm_messagef() to API. 
     *
     * 2021-04-22 | v0.1.1.0 <a id="v0.1.1.0"></a>
     * - [Dajo Frey](https://github.com/DajoFrey): Add custom output path parameter for source contexts, stored in \ref sm_SourceContext.outputPath_p
     *
     * 2021-04-17 | v0.0.1.0 <a id="v0.0.1.0"></a>
     * - [Dajo Frey](https://github.com/DajoFrey): Fix invalid memory writes in sm_replaceVariables().
     * - [Dajo Frey](https://github.com/DajoFrey): Add sm_terminate() and sm_destroyRuntime(). Not implemented yet though.
     * - [Dajo Frey](https://github.com/DajoFrey): Rename sm_init() to sm_initialize().
     * - [Dajo Frey](https://github.com/DajoFrey): Add sm_getSourceContextArray() and sm_getSourceContext().
     *
     * 2021-04-17 | v0.0.0.0
     * - [Dajo Frey](https://github.com/DajoFrey): Initial version.
     */
    #define SM_MINOR_VERSION 2

    /**
     * Patch version.
     *
     * 2021-04-26 | v0.1.2.1 <a id="v0.1.2.1"></a>
     * - [Dajo Frey](https://github.com/DajoFrey): Remove inserted project-prefixes in sm_copy(). 
     *
     * 2021-04-22 | v0.1.1.1 <a id="v0.1.1.1"></a>
     * - [Dajo Frey](https://github.com/DajoFrey): Fix missing initialization of \ref sm_SourceContext.api and \ref sm_SourceContext.apiDate_p.
     *
     * 2021-04-17 | v0.0.0.0
     * - [Dajo Frey](https://github.com/DajoFrey): Initial version.
     */
    #define SM_PATCH_VERSION 1

/** @} */

#endif
