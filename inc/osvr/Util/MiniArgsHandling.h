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
#include <initializer_list>
#include <iterator>
#include <string>
#include <utility>
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
        namespace detail {
            struct StringEqualityComparisonPolicy {
                template <typename T>
                static bool apply(T &&a, std::string const &b) {
                    return (std::forward<T>(a) == b);
                }
            };
#ifdef OSVR_HAVE_BOOST
            struct CaseInsensitiveComparisonPolicy {
                template <typename T>
                static bool apply(T &&a, std::string const &b) {
                    return boost::iequals(b, std::forward<T>(a));
                }
            };
#endif // OSVR_HAVE_BOOST

            template <typename ComparisonPolicy, typename SequenceType>
            inline bool generic_handle_has_switch(ArgList &c,
                                                  SequenceType &&argSwitch) {
                return 0 < handle_arg(c, [&argSwitch](std::string const &arg) {
                           return ComparisonPolicy::apply(arg, argSwitch);
                       });
            }
        } // namespace detail

#ifdef OSVR_HAVE_BOOST
        /// Takes a "sequence" to compare (case-insensitive using
        /// `boost::iequals()`) with every arg.
        ///
        /// Wraps `handle_arg()` (so it "consumes" those args it finds)
        ///
        /// @return true if at least one copy of the arg was found.
        template <typename SequenceType>
        inline bool handle_has_iswitch(ArgList &c, SequenceType &&argSwitch) {
            return detail::generic_handle_has_switch<
                detail::CaseInsensitiveComparisonPolicy>(
                c, std::forward<SequenceType>(argSwitch));
        }
#endif // OSVR_HAVE_BOOST

        /// Takes a "sequence" (something with operator== with std::string) to
        /// compare (case-sensitive) with every arg.
        ///
        /// Wraps `handle_arg()` (so it "consumes" those args it finds)
        ///
        /// @return true if at least one copy of the arg was found.
        template <typename SequenceType>
        inline bool handle_has_switch(ArgList &c, SequenceType &&argSwitch) {
            return detail::generic_handle_has_switch<
                detail::StringEqualityComparisonPolicy>(
                c, std::forward<SequenceType>(argSwitch));
        }
        /// Takes a predicate to find arguments. Found arguments not immediately
        /// followed by another argument or the end of the argument list have
        /// the following argument passed to the action and are removed from the
        /// arg list.
        /// Count of valid action invocations is returned.
        template <typename F, typename G>
        inline std::size_t handle_value_arg(ArgList &c, F &&predicate,
                                            G &&action) {

            if (c.size() < 2) {
                // Need at least two args for this to work.
                return 0;
            }
            auto origEnd = end(c);
            auto origSize = c.size();
            std::vector<bool> isCandidate(origSize, false);
            // Apply predicate
            std::transform(begin(c), origEnd, begin(isCandidate),
                           [&](std::string const &arg) {
                               return std::forward<F>(predicate)(arg);
                           });

            ArgList remainingArgs;
            auto beforeEnd = end(c);
            --beforeEnd;

            std::size_t count = 0;
            /// Loop through all but the last element (since we can't start a
            /// value arg there)
            bool lastAccepted = false;
            for (auto it = begin(c), e = end(c); it != beforeEnd && it != e;
                 ++it) {
                if (std::forward<F>(predicate)(*it)) {
                    /// found a flag, next thing is an arg.
                    ++it;
                    ++count;
                    std::forward<G>(action)(*it);
                    lastAccepted = true;
                } else {
                    /// not a flag - steal it for the remaining args list.
                    remainingArgs.emplace_back(std::move(*it));
                    lastAccepted = false;
                }
            }

            /// Move the last arg to the remaining list, if the last thing we
            /// did wasn't to accept an argument (and thus
            /// consume the last one)
            if (!lastAccepted) {
                remainingArgs.emplace_back(std::move(c.back()));
            }

            /// Remaining args are now the only args.
            c.swap(remainingArgs);
            return count;
        }

        namespace detail {
            template <typename ComparisonPolicy, typename SequenceType>
            inline bool generic_handle_has_any_switch_of(
                ArgList &c,
                std::initializer_list<SequenceType> const &argSwitches) {
                return 0 <
                       handle_arg(c, [&argSwitches](std::string const &arg) {
                           for (const auto &sw : argSwitches) {
                               if (sw == arg) {
                                   return true;
                               }
                           }
                           return false;
                       });
            }
        } // namespace detail
        /// Takes a list of  "sequences" (something with operator== with
        /// std::string) to compare (case-sensitive) with every arg.
        ///
        /// Wraps `handle_arg()` (so it "consumes" those args it finds)
        ///
        /// @return true if at least one copy of the arg was found.
        template <typename SequenceType>
        inline bool handle_has_any_switch_of(
            ArgList &c,
            std::initializer_list<SequenceType> const &argSwitches) {
            return detail::generic_handle_has_any_switch_of<
                detail::StringEqualityComparisonPolicy>(c, argSwitches);
        }

#ifdef OSVR_HAVE_BOOST
        /// Takes a "sequence" to compare (case-insensitive using
        /// `boost::iequals()`) with every arg.
        /// Takes a list of  "sequences" to compare (case-insensitive using
        /// `boost::iequals()`)  with every arg.
        ///
        /// Wraps `handle_arg()` (so it "consumes" those args it finds)
        ///
        /// @return true if at least one copy of the arg was found.
        template <typename SequenceType>
        inline bool handle_has_any_iswitch_of(
            ArgList &c, std::initializer_list<SequenceType> argSwitches) {
            return detail::generic_handle_has_any_switch_of<
                detail::CaseInsensitiveComparisonPolicy>(c, argSwitches);
        }
#endif // OSVR_HAVE_BOOST
    }  // namespace args
} // namespace util
} // namespace osvr
#endif // INCLUDED_MiniArgsHandling_h_GUID_51E5C000_C9EC_438C_FF9C_2FE43886985C
