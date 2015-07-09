/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_SharedMemory_h_GUID_4EAFB440_5023_439B_9413_0E54BC240DBD
#define INCLUDED_SharedMemory_h_GUID_4EAFB440_5023_439B_9413_0E54BC240DBD

/// required because CircularBuffer's debug mode uses raw pointers.
/// see http://stackoverflow.com/a/2335693/265522
#define BOOST_CB_DISABLE_DEBUG

/// Allows header-only usage of Interprocess
#define BOOST_DATE_TIME_NO_LIB

/// @todo This is a workaround for Clang and Boost pre-1.50, to fix a build
/// error caused by a syntax error in Boost.Containers allocator traits.
#include <boost/version.hpp>
#if defined(__clang__) && (BOOST_VERSION < 105000) &&                          \
    !defined(BOOST_NO_TEMPLATE_ALIASES)
#define OSVR_NEEDS_BOOST_CLANG_WORKAROUND
#endif

// Internal Includes
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#ifdef OSVR_NEEDS_BOOST_CLANG_WORKAROUND
#define BOOST_NO_TEMPLATE_ALIASES
#endif
#include <boost/interprocess/mem_algo/rbtree_best_fit.hpp>
#ifdef OSVR_NEEDS_BOOST_CLANG_WORKAROUND
#undef BOOST_NO_TEMPLATE_ALIASES
#endif

#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/offset_ptr.hpp>

// Standard includes
#include <cctype>
#include <string>

#undef OSVR_USE_PLATFORM_SPECIALIZED_SHM

#if defined(BOOST_INTERPROCESS_WINDOWS)
// Currently only supporting mixing 32/64 on Windows
#define OSVR_SHM_3264
#define OSVR_HAVE_WINDOWS_SHM
#include <boost/interprocess/managed_windows_shared_memory.hpp>
#elif defined(BOOST_INTERPROCESS_XSI_SHARED_MEMORY_OBJECTS)
// Disabled - can't compute a correct key at this time.
#undef OSVR_HAVE_XSI_SHM
#ifdef OSVR_USE_PLATFORM_SPECIALIZED_SHM
#undef OSVR_USE_PLATFORM_SPECIALIZED_SHM
#endif
#include <boost/interprocess/managed_xsi_shared_memory.hpp>
#else
#ifdef OSVR_USE_PLATFORM_SPECIALIZED_SHM
#undef OSVR_USE_PLATFORM_SPECIALIZED_SHM
#endif
#endif

#include <boost/interprocess/managed_shared_memory.hpp>

namespace osvr {
namespace common {

    namespace ipc {
#ifdef OSVR_SHM_3264
        /// @brief Types related to sharing memory between processes of
        /// different
        /// pointer bitness on the same system.
        namespace detail {
            typedef int64_t my_ptrdiff;
            typedef uint64_t my_uintptr;
            static_assert(
                sizeof(my_ptrdiff) == sizeof(my_uintptr),
                "These types must be the same size - the largest "
                "possible pointer size on a given platform that you want "
                "to interoperate!");
            static_assert(sizeof(my_ptrdiff) >= sizeof(::ptrdiff_t),
                          "my_ptrdiff must be at least as big as ptrdiff_t!");
            static_assert(sizeof(my_uintptr) >= sizeof(::uintptr_t),
                          "my_uintptr must be at least as big as uintptr_t!");

            template <typename T>
            using my_offset_ptr =
                boost::interprocess::offset_ptr<T, my_ptrdiff, my_uintptr,
                                                sizeof(my_ptrdiff)>;

        } // namespace detail
        template <typename T> using ipc_offset_ptr = detail::my_offset_ptr<T>;

#else
        template <typename T>
        using ipc_offset_ptr = boost::interprocess::offset_ptr<T>;
#endif

        template <class MutexFamily>
        using my_mem_alloc_algo =
            boost::interprocess::rbtree_best_fit<MutexFamily,
                                                 ipc_offset_ptr<void> >;

        template <class IndexConfig>
        using index_type = boost::interprocess::iset_index<IndexConfig>;

        /// @brief Helper function to be able to remove shared memory.
        /// Specializations allow this to turn into a no-op for Win shared
        /// memory.
        template <typename ManagedMemory> struct device_type {
            static void remove(const char *name) {
                typedef typename ManagedMemory::device_type device_type;
                device_type::remove(name);
            }
        };

        typedef uint8_t SharedMemoryBackendType;
        static const SharedMemoryBackendType BASIC_MANAGED_SHM_ID = 0;
        static const SharedMemoryBackendType WINDOWS_MANAGED_SHM_ID = 1;
        static const SharedMemoryBackendType SYSV_MANAGED_SHM_ID = 2;

        using basic_managed_shm =
            boost::interprocess::basic_managed_shared_memory<
                char, my_mem_alloc_algo<boost::interprocess::mutex_family>,
                index_type>;

#if defined(OSVR_HAVE_WINDOWS_SHM)
        using windows_managed_shm =
            boost::interprocess::basic_managed_windows_shared_memory<
                char, my_mem_alloc_algo<boost::interprocess::mutex_family>,
                index_type>;
        template <> struct device_type<windows_managed_shm> {
            static void remove(const char *) {}
        };
#endif

#if defined(OSVR_HAVE_XSI_SHM)

        using sysv_managed_shm =
            boost::interprocess::basic_managed_xsi_shared_memory<
                char, my_mem_alloc_algo<boost::interprocess::mutex_family>,
                index_type>;
#endif

#ifdef OSVR_USE_PLATFORM_SPECIALIZED_SHM
#if defined(OSVR_HAVE_WINDOWS_SHM)
        using default_managed_shm = windows_managed_shm;
        static const SharedMemoryBackendType DEFAULT_MANAGED_SHM_ID =
            WINDOWS_MANAGED_SHM_ID;
#elif defined(OSVR_HAVE_XSI_SHM)
        using default_managed_shm = sysv_managed_shm;
        static const SharedMemoryBackendType DEFAULT_MANAGED_SHM_ID =
            SYSV_MANAGED_SHM_ID;
#else
#error                                                                         \
    "Unknown kind of platform-specialized shared memory - this set of conditionals must match the one with the includes above!"
#endif
#else  // !OSVR_USE_PLATFORM_SPECIALIZED_SHM
        using default_managed_shm = basic_managed_shm;
        static const SharedMemoryBackendType DEFAULT_MANAGED_SHM_ID =
            BASIC_MANAGED_SHM_ID;
#endif // OSVR_USE_PLATFORM_SPECIALIZED_SHM

        /// @brief Turns all "unsafe" characters in a name into underscores.
        inline std::string make_name_safe(std::string const &input) {
            static const char UNDERSCORE = '_';
            std::string ret{input};

            /// Must start with an alphabetic character.
            if (ret.empty() || !std::isalpha(ret[0])) {
                ret.insert(0u, "Z");
            }

            for (auto &c : ret) {
                if (std::isalpha(c) || std::isdigit(c) || UNDERSCORE == c) {
                    continue;
                }
                c = UNDERSCORE;
            }
            return ret;
        }
    } // namespace ipc
    using ipc::ipc_offset_ptr;
} // namespace common
} // namespace osvr

#endif // INCLUDED_SharedMemory_h_GUID_4EAFB440_5023_439B_9413_0E54BC240DBD
