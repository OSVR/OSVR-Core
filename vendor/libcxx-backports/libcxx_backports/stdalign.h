/** @file
    @brief Header containing a std::align implementation using extracted code
    from libc++ file `src/memory.cpp`

    Source is from r245463, aka commit 00f4a49 in canonical git mirrors:
    <https://github.com/llvm-mirror/libcxx/blob/00f4a49b0b76dea78a68ea2dd7333408f52caa5b/src/memory.cpp>

    Modifications: implementation extracted, made `inline`, and placed in its
    own header file with appropriate includes and namespace. Automatic
    clang-format applied.

    Original license header follows.
*/

//===------------------------ memory.cpp ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef INCLUDED_stdalign_h_GUID_0F960B99_26D5_44B3_4431_8CA8074C6C23
#define INCLUDED_stdalign_h_GUID_0F960B99_26D5_44B3_4431_8CA8074C6C23

#include <stddef.h> // for size_t

namespace libcxx_backports {
static inline void *align(size_t alignment, size_t size, void *&ptr,
                          size_t &space) {
    void *r = nullptr;
    if (size <= space) {
        char *p1 = static_cast<char *>(ptr);
        char *p2 = reinterpret_cast<char *>(
            reinterpret_cast<size_t>(p1 + (alignment - 1)) & -alignment);
        size_t d = static_cast<size_t>(p2 - p1);
        if (d <= space - size) {
            r = p2;
            ptr = r;
            space -= d;
        }
    }
    return r;
}
} // namespace libcxx_backports

#endif // INCLUDED_stdalign_h_GUID_0F960B99_26D5_44B3_4431_8CA8074C6C23
