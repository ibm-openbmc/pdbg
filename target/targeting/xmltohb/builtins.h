// IBM_PROLOG_BEGIN_TAG 
// This is an automatically generated prolog. 
//  
// fips1110 src/hbfw/fsp/targeting/adapters/builtins.h 1.2 
//  
// IBM CONFIDENTIAL 
//  
// OBJECT CODE ONLY SOURCE MATERIALS 
//  
// COPYRIGHT International Business Machines Corp. 2012 
// All Rights Reserved 
//  
// The source code for this program is not published or otherwise 
// divested of its trade secrets, irrespective of what has been 
// deposited with the U.S. Copyright Office. 
//  
// IBM_PROLOG_END_TAG 

#ifndef __TARGETING_BUILTINS_H
#define __TARGETING_BUILTINS_H

/**
 *  @file targeting/adapters/builtins.h
 *
 *  @brief Defines platform specific "builtin" macros useful for controlling
 *      the output of the GCC compiler
 */
/**
 *  @page ChangeLogs Change Logs
 *  @section BUILTINS_H builtins.h
 *  @verbatim
 *
 *  Flag   Defect/Feature User     Date         Description
 *  ------ -------------- -------- ------------ --------------------------------
 *         F 831072       bofferdn Mar 01, 2012 Created
 *
 *  @endverbatim
 */

//******************************************************************************
// Includes
//******************************************************************************

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *  @brief This macro ensures that object code never gets generated without
 *      being inlined
 */
#define ALWAYS_INLINE __attribute__((always_inline))

/**
 *  @brief This macro tells the comipler the function will never return
 */
#define NO_RETURN __attribute__((noreturn))

/**
 *  @brief This macro forces the compiler not to pad structures to convenient
 *      data type boundaries
 */
#define PACKED __attribute__((packed))

/**
 *  @brief This macro gives the compiler a hint that the condition is likely to
 *      be true (for optimization)
 */
#define likely(expr) __builtin_expect((expr),1)

/**
 *  @brief This macro gives the compiler a hint that the condition is likely to
 *      be false (for optimization)
 */
#define unlikely(expr) __builtin_expect((expr),0)

#ifdef __cplusplus
};
#endif

#endif // __TARGETING_BUILTINS_H
