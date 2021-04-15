#ifndef SM_TYPES_H
#define SM_TYPES_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#endif

/** @addtogroup selfmakeTypedefs
 *  @{
 */

    typedef char           SM_BYTE;
    typedef unsigned char  SM_UNSIGNED_BYTE;

/** @} */

/** @addtogroup selfmakeEnums
 *  @{
 */ 

    /**
     * Boolean values.
     */
    typedef enum SM_BOOL {
        SM_FALSE = 0, /**<Indicates false.*/
        SM_TRUE = 1,  /**<Indicates true.*/
    } SM_BOOL;

/** } */

#endif
