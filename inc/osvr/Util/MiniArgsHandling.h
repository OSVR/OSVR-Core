/** @file
    @brief Header containing some simple args-handling routines: nothing
   sophisticated, just enough to keep the hand-written stuff from taking over
   main().

   Define OSVR_HAVE_BOOST before including if you want to be able to use the
   case-insensitive comparisons, etc.

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_MiniArgsHandling_h_GUID_51E5C000_C9EC_438C_FF9C_2FE43886985C
#define INCLUDED_MiniArgsHandling_h_GUID_51E5C000_C9EC_438C_FF9C_2FE43886985C

// Internal Includes
// - none

// Library/third-party includes
#if defined(OSVR_HAVE_BOOST) || defined(BOOST_VERSION)
#include <boost/algorithm/string/predicate.hpp> // for argument handling
#endif

// Standard includes
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <vector>

namespace osvr {
namespace util {
    namespace args {
        using ArgList = std::vector<std::string>;

        /// Constructs a vector containing the arguments excluding argv[0].
        inline ArgList makeArgList(int argc, char *argv[]) {
            return std::vector<std::string>(argv + 1, argv + argc);
        }
        /// Takes a predicate to find arguments. Any found arguments are counted
        /// and removed from the arg list. Count is returned.
        template <typename F> inline std::size_t handle_arg(ArgList &c, F &&f) {
            auto origSize = c.size();
            auto origEnd = end(c);
            auto newEnd = std::remove_if(begin(c), end(c), std::forward<F>(f));
            if (newEnd != origEnd) {
                std::size_t newSize = std::distance(begin(c), newEnd);
                c.resize(newSize);
                return origSize - newSize;
            }
            return 0;
        }

#ifdef OSVR_HAVE_BOOST
        /// Takes a "sequence" to compare (case-insensitive using
        /// `boost::iequals()`) with every arg.
        ///
        /// Wraps `handle_arg()` (so it "consumes" those args it finds)
        ///
        /// @return true if at least one copy of the arg was found.
        template <typename SequenceType>
        inline bool handle_has_iswitch(ArgList &c, SequenceType &&argSwitch) {
            return 0 < handle_arg(c, [&argSwitch](std::string const &arg) {
                       return boost::iequals(arg, argSwitch);
                   });
        }
#endif

        /// Takes a "sequence" (something with operator== with std::string) to
        /// compare (case-sensitive) with every arg.
        ///
        /// Wraps `handle_arg()` (so it "consumes" those args it finds)
        ///
        /// @return true if at least one copy of the arg was found.
        template <typename SequenceType>
        inline bool handle_has_switch(ArgList &c, SequenceType &&argSwitch) {
            return 0 < handle_arg(c, [&argSwitch](std::string const &arg) {
                       return arg == argSwitch;
                   });
        }
    } // namespace args
} // namespace util
} // namespace osvr
#endif // INCLUDED_MiniArgsHandling_h_GUID_51E5C000_C9EC_438C_FF9C_2FE43886985C
