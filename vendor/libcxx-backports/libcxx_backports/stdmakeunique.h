/** @file
    @brief Header containing a std::make_unique implementation using extracted
   code from libc++ file `include/memory`

    @author
    LLVM Team
    University of Illinois at Urbana-Champaign
    <http://llvm.org>

    Extracted from `libc++/include/memory which is dual-licensed under the MIT
    and the University of Illinois "BSD-Like" licenses.

    Modifications: Portion of header extracted, `_LIBCPP_INLINE_VISIBILITY`
   macro instantiations removed and `_VSTD` macro instantations replaced with
   `::std`, and code placed in its own header file with appropriate includes and
   namespace. Some additional namespace qualification of `::std::` added as
   required. Automatic clang-format applied.

    Original license header follows.
*/
// -*- C++ -*-
//===-------------------------- memory ------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef INCLUDED_stdmakeunique_h_GUID_555E4284_D6E5_4384_C523_67411C391068
#define INCLUDED_stdmakeunique_h_GUID_555E4284_D6E5_4384_C523_67411C391068

#include <cstddef>
#include <memory>      // for std::unique_ptr
#include <type_traits> // for std::remove_extent
#include <utility>     // for std::forward

namespace libcxx_backports {

template <class _Tp> struct __unique_if {
    typedef ::std::unique_ptr<_Tp> __unique_single;
};

template <class _Tp> struct __unique_if<_Tp[]> {
    typedef ::std::unique_ptr<_Tp[]> __unique_array_unknown_bound;
};

template <class _Tp, size_t _Np> struct __unique_if<_Tp[_Np]> {
    typedef void __unique_array_known_bound;
};

template <class _Tp, class... _Args>
inline typename __unique_if<_Tp>::__unique_single
make_unique(_Args &&... __args) {
    return ::std::unique_ptr<_Tp>(new _Tp(::std::forward<_Args>(__args)...));
}

template <class _Tp>
inline typename __unique_if<_Tp>::__unique_array_unknown_bound
make_unique(size_t __n) {
    typedef typename ::std::remove_extent<_Tp>::type _Up;
    return ::std::unique_ptr<_Tp>(new _Up[__n]());
}

template <class _Tp, class... _Args>
typename __unique_if<_Tp>::__unique_array_known_bound
make_unique(_Args &&...) = delete;

} // namespace libcxx_backports
#endif // INCLUDED_stdmakeunique_h_GUID_555E4284_D6E5_4384_C523_67411C391068
