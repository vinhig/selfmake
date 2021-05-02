#ifndef SM_ABOUT_H
#define SM_ABOUT_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#endif

/** @defgroup selfmake selfmake internal
 *  @brief The internal code-documentation for selfmake. 
 */
/** @defgroup selfmakeAPI selfmake API 
 *  @brief The API code-documentation for selfmake. 
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

/** @defgroup selfmakeAPIEnums Enumerations
 *  @ingroup selfmakeAPI
 */
/** @defgroup selfmakeAPIStructs Data Structures
 *  @ingroup selfmakeAPI
 */
/** @defgroup selfmakeAPIFunctions Functions
 *  @ingroup selfmakeAPI
 */
/** @defgroup selfmakeAPITypedefs Typedefs
 *  @ingroup selfmakeAPI
 */
/** @defgroup selfmakeAPIVars Variables
 *  @ingroup selfmakeAPI
 */

/** @addtogroup selfmakeChangelog
 *  @{
 */

    /**
     * The API version is used for backwards-incompatible API changes.
     *
     * 2021-04-17 | v0.0.0.0
     * - [Dajo Frey](https://github.com/DajoFrey): Initial version.
     */
    #define SM_API_VERSION 0

    /**
     * The major version is used for the completion of big functionalities.
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
     * The minor version is used for the addition of small functionalities.
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
     * The patch version is used for bugfixes or non-consequential changes.
     *
     * 2021-05-01 | v0.1.2.3 <a id="v0.1.2.3"></a>
     * - [Dajo Frey](https://github.com/DajoFrey): Fix possible stack smashing for messages.
     *
     * 2021-04-28 | v0.1.2.2 <a id="v0.1.2.2"></a>
     * - [Dajo Frey](https://github.com/DajoFrey): Remove offset from argument parsing.
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
    #define SM_PATCH_VERSION 3

/** @} */

#endif
