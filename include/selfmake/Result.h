#ifndef SM_RESULT_H
#define SM_RESULT_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#endif

/** @addtogroup selfmakeAPIEnums
 *  @{
 */ 

    /**
     * Return values for functions.
     */
    typedef enum SM_RESULT 
    {
        SM_SUCCESS,
        SM_ERROR_INVALID_OPTION,
        SM_ERROR_NULL_POINTER,
        SM_ERROR_BAD_STATE,
        SM_ERROR_GENERATE_DOCUMENTS_FAILURE,
        SM_ERROR_DOWNLOAD_FAILED,
        SM_ERROR_BUILD_EXTERNAL_FAILED,
        SM_ERROR_COPY_EXTERNAL_HEADER_FAILED,
        SM_ERROR_COPY_PUBLIC_LIBRARY_HEADER_FAILED,
        SM_ERROR_CREATE_LIBRARY_INCLUDES_FAILED,
        SM_ERROR_BUILD_LIBRARY_FAILED,
        SM_ERROR_WGET_EXECUTION_FAILED,
        SM_ERROR_UNZIP_EXECUTION_FAILED,
        SM_ERROR_DOXYGEN_EXECUTION_FAILED,
        SM_ERROR_GCC_EXECUTION_FAILED,
        SM_ERROR_AR_EXECUTION_FAILED,
        SM_ERROR_XXD_EXECUTION_FAILED,
        SM_ERROR_CP_EXECUTION_FAILED,
        SM_ERROR_LIBRARY_NOT_FOUND,
        SM_ERROR_WGET_NOT_FOUND,
        SM_ERROR_UNZIP_NOT_FOUND,
        SM_ERROR_DOXYGEN_NOT_FOUND,
        SM_ERROR_GCC_NOT_FOUND,
        SM_ERROR_AR_NOT_FOUND,
        SM_ERROR_CP_NOT_FOUND,
        SM_ERROR_GENERATE_VERSION_DEPENDENT_DOCS,
        SM_ERROR_GENERATE_DOCS_USING_DOXYGEN,
        SM_ERROR_GET_PROJECT_DIRECTORY,
        SM_ERROR_GET_WORK_DIRECTORY,
        SM_ERROR_CANT_OPEN_README,
        SM_ERROR_CANT_OPEN_HEADER,
        SM_ERROR_CANT_OPEN_DIR,
        SM_ERROR_CANT_DOWNLOAD_VULKAN_HEADERS,
        SM_ERROR_CANT_DOWNLOAD_VOLK,
        SM_ERROR_CANT_DOWNLOAD_FREETYPE,
        SM_ERROR_CANT_DOWNLOAD_OPENSSL,
        SM_ERROR_CANT_DOWNLOAD_PROJECT,
        SM_ERROR_BUILD_XXD_FAILED,
        SM_ERROR_BUILD_HELPER_LIBRARY_FAILED,
        SM_ERROR_NOT_IMPLEMENTED,
        SM_ERROR_CANT_CREATE_DIRECTORY,
        SM_ERROR_CANT_CREATE_OBJECT_FILE,
        SM_ERROR_CANT_CREATE_OBJECTS,
        SM_ERROR_CANT_CREATE_LIBRARY,
        SM_ERROR_CANT_CREATE_EXTERNAL_BIN_DIRECTORY,
        SM_ERROR_CANT_CREATE_EXTERNAL_BIN_OBJECT_DIRECTORY,
        SM_ERROR_CANT_CREATE_BIN_OBJECT_DIRECTORY,
        SM_ERROR_CANT_CREATE_BIN_DIRECTORY,
        SM_ERROR_CANT_CREATE_SHARED_LIB,
        SM_ERROR_CANT_CREATE_STATIC_LIB,
        SM_ERROR_CANT_CREATE_INCLUDE_FILE,
        SM_ERROR_CANT_CREATE_INCLUDE_DIRECTORY,
        SM_ERROR_CANT_CREATE_EXTERNAL_DIRECTORY,
        SM_ERROR_COPY_EXTERNAL_HEADER_USING_CP_FAILED,
        SM_ERROR_COPY_PUBLIC_LIBRARY_HEADER_USING_CP_FAILED,
        SM_ERROR_COPY_FAILED,
        SM_ERROR_CANT_CREATE_NETZHAUT_OBJECT_FILES,
        SM_ERROR_CANT_CREATE_IO_OBJECT_FILES,
        SM_ERROR_CANT_CREATE_GRAPHICS_OBJECT_FILES,
        SM_ERROR_CANT_CREATE_CSS_OBJECT_FILES,
        SM_ERROR_CANT_CREATE_HTML_OBJECT_FILES,
        SM_ERROR_CANT_CREATE_JAVASCRIPT_OBJECT_FILES,
        SM_ERROR_CANT_CREATE_TTY_OBJECT_FILES,
        SM_ERROR_CANT_CREATE_NETWORK_OBJECT_FILES,
        SM_ERROR_CANT_CREATE_API_OBJECT_FILES,
        SM_ERROR_THREAD_CREATION_FAILED,
        SM_ERROR_INSTALL_ALL_LIBRARY_FAILED,
        SM_ERROR_INSTALL_ALL_EXTERNAL_LIBRARY_FAILED,
        SM_ERROR_INSTALL_ALL_WEB_BROWSER_FAILED,
        SM_ERROR_INSTALL_ALL_LOGO_FAILED,
        SM_ERROR_BUILD_WEB_BROWSER_FAILED,

    } SM_RESULT;

/** @} */

/** @addtogroup selfmakeAPIVars 
 *  @{
 */ 

    extern const char *SM_RESULTS_PP[];
    extern unsigned int SM_RESULTS_PP_COUNT;

/** @} */

#endif
