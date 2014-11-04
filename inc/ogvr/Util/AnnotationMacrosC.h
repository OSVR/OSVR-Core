/** @file
    @brief Header containing macros for source-level annotation.

    In theory, supporting MSVC SAL, as well as compatible GCC and
    Clang attributes. In practice, expanded as time allows and requires.

    Must be c-safe!

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

/*
// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)
*/

#ifndef INCLUDED_AnnotationMacrosC_h_GUID_48538D9B_35E3_4E9A_D2B0_D83D51DD5900
#define INCLUDED_AnnotationMacrosC_h_GUID_48538D9B_35E3_4E9A_D2B0_D83D51DD5900

#ifndef OGVR_DISABLE_ANALYSIS

#if defined(_MSC_VER) && (_MSC_VER >= 1700)
/* Visual C++ (2012 and newer) */
/* Using SAL attribute format:
 * http://msdn.microsoft.com/en-us/library/ms182032(v=vs.120).aspx */

#include <CodeAnalysis/SourceAnnotations.h>

#define OGVR_IN _In_
#define OGVR_IN_PTR _In_
#define OGVR_IN_OPT _In_opt_
#define OGVR_IN_STRZ _In_z_
#define OGVR_IN_READS(NUM_ELEMENTS) _In_reads_(NUM_ELEMENTS)

#define OGVR_OUT _Out_
#define OGVR_OUT_PTR _Out_
#define OGVR_OUT_OPT _Out_opt_

#define OGVR_INOUT _Inout_
#define OGVR_INOUT_PTR _Inout_

#endif
/* end of msvc section */

#endif
/* end of ndef disable analysis */

/* Fallback declarations */
/** @defgroup annotation_macros Utilities: Static analysis annotation macros
    @brief Wrappers for Microsoft's SAL annotations and others

    Use of these is optional, but recommended particularly for C APIs,
    as well as any methods handling a buffer with a length.
@{
*/
/** @name Parameter annotations

    These indicate the role and valid values for parameters to functions.

    At most one of these should be placed before a parameter's type name in the
   function parameter list, in both the declaration and definition. (They must
   match!)
   @{
*/
#ifndef OGVR_IN
/** @brief Indicates a required function parameter that serves only as input.

    Place before the parameter's type name in a function parameter list.
*/
#define OGVR_IN
#endif

#ifndef OGVR_IN_PTR
/** @brief Indicates a required pointer (non-null) function parameter that
    serves only as input.
*/
#define OGVR_IN_PTR
#endif

#ifndef OGVR_IN_OPT
/** @brief Indicates a function parameter that serves only as input, but is
    optional and might be NULL

    Place before the parameter's type name in a function parameter list.
*/
#define OGVR_IN_OPT
#endif

#ifndef OGVR_IN_STRZ
/** @brief Indicates a null-terminated string function parameter that serves
   only as input.

    Place before the parameter's type name in a function parameter list.
*/
#define OGVR_IN_STRZ
#endif

#ifndef OGVR_IN_READS
/** @brief Indicates a buffer containing input with the specified number of
   elements.

    The specified number of elements is typically the name of another parameter.

    Place before the parameter's type name in a function parameter list.
*/
#define OGVR_IN_READS(NUM_ELEMENTS)
#endif

#ifndef OGVR_OUT
/** @brief Indicates a required function parameter that serves only as output.
    In C code, since this usually means "pointer", you probably want
   OGVR_OUT_PTR.
*/
#define OGVR_OUT
#endif

#ifndef OGVR_OUT_PTR
/** @brief Indicates a required pointer (non-null) function parameter that
    serves only as output.
*/
#define OGVR_OUT_PTR
#endif

#ifndef OGVR_OUT_OPT
/** @brief Indicates a function parameter that serves only as output, but is
    optional and might be NULL

    Place before the parameter's type name in a function parameter list.
*/
#define OGVR_OUT_OPT
#endif

#ifndef OGVR_INOUT
/** @brief Indicates a required function parameter that is both read and written
   to.

    In C code, since this usually means "pointer", you probably want
   OGVR_INOUT_PTR.
*/
#define OGVR_INOUT
#endif

#ifndef OGVR_INOUT_PTR
/** @brief Indicates a required pointer (non-null) function parameter that is
    both read and written to.
*/
#define OGVR_INOUT_PTR
#endif

/* End of parameter annotations. */

/** @} */

/* End of annotation group. */
/** @} */
#endif