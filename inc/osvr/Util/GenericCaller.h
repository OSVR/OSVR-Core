/** @file
    @brief Header providing templated functions to automate the process of
   calling a C++ member function (specifically the function call operator) from
   a facility that accepts a C function pointer and an opaque "userdata"
   pointer.

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_GenericCaller_h_GUID_C49A7DEE_89E6_478D_627A_CFFB9ED39EAC
#define INCLUDED_GenericCaller_h_GUID_C49A7DEE_89E6_478D_627A_CFFB9ED39EAC

// Internal Includes
// - none

// Library/third-party includes
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/enum.hpp>
#include <boost/preprocessor/enum_params.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/is_function.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_pointer.hpp>

#include <boost/mpl/at.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/next_prior.hpp>

#include <boost/static_assert.hpp>

// Standard includes
// - none

#ifndef OSVR_UTIL_CALLER_MAX_ARITY
#define OSVR_UTIL_CALLER_MAX_ARITY 3
#endif

namespace osvr {
namespace util {
#ifndef OSVR_DOXYGEN_EXTERNAL
    namespace functor_trampolines {
#endif // ifndef OSVR_DOXYGEN_EXTERNAL

        /// @brief Tag type indicating the last parameter of the function
        /// contains
        /// the "this" pointer.
        /// @relates GenericCaller
        struct this_last_t {};
        /// @brief Tag type indicating the first parameter of the function
        /// contains
        /// the "this" pointer.
        /// @relates GenericCaller
        struct this_first_t {};

        /// @brief Pass as an argument to a getCaller() overload to indicate the
        /// last parameter of the function contains the "this" pointer.
        /// @relates getCaller()
        BOOST_CONSTEXPR_OR_CONST this_last_t this_last = {};
        /// @brief Pass as an argument to a getCaller() overload to indicate the
        /// first parameter of the function contains the "this" pointer.
        /// @relates getCaller()
        BOOST_CONSTEXPR_OR_CONST this_first_t this_first = {};

#ifndef OSVR_DOXYGEN_EXTERNAL
        namespace detail {

            /// @brief Convenience metafunction to simplify computing the type
            /// of a particular argument to a function F
            template <typename F, int C> struct param_at {
#ifndef OSVR_DOXYGEN
                typedef typename boost::function_types::parameter_types<F>::type
                    ParamTypes;
#endif
                /// @brief Result type.
                typedef
                    typename boost::mpl::at<ParamTypes,
                                            boost::mpl::int_<C> >::type type;
            };

            /// @brief Template that will be specialized to contain callers for
            /// functors with the "this" pointer as a void * argument.
            ///
            /// @tparam ThisLocation either this_last_t or this_first_t to
            /// indicate
            /// the location of the this pointer.
            /// @tparam Arity arity of the functor (not including "this"
            /// pointer)
            /// @tparam Return 0 if void return, 1 if non-void
            template <typename ThisLocation, int Arity, int Return>
            struct Caller;

/// @brief what do we call our pass-through parameters
#define OSVR_PARAM_STEM p

/// @brief Macro for use with BOOST_PP_ENUM to generate pass-through argument
/// list
#define OSVR_MAKE_PARAMLIST(Z, N, unused)                                      \
    typename param_at<FPtr, N>::type BOOST_PP_CAT(OSVR_PARAM_STEM, N)

/// @brief Expands to the result type computation if R==1, void if R==0
///
/// @param R value of RETURNS
#define OSVR_RETURNTYPE(R)                                                     \
    BOOST_PP_IF(R, typename boost::function_types::result_type<FPtr>::type,    \
                void)

/// @brief Expands to "return" if R==1, nothing if R==0
///
/// @param R value of RETURNS
#define OSVR_RETURNSTATEMENT(R) BOOST_PP_IF(R, return, BOOST_PP_EMPTY())

/// @brief Generates specializations of CallerThisLast and CallerThisFirst for
/// the given values of ARITY and RETURNS (matching the template parameters).
///
/// Intended for use with BOOST_PP_REPEAT, passing a value (0 or 1) for RETURNS
/// in the third, "data" parameter.
#define OSVR_MAKE_CALLERS(Z, ARITY, RETURNS)                                   \
    template <> struct Caller<this_last_t, ARITY, RETURNS> {                   \
        template <typename FPtr, typename F> struct Specialized {              \
            static OSVR_RETURNTYPE(RETURNS)                                    \
                call(BOOST_PP_ENUM(ARITY, OSVR_MAKE_PARAMLIST, ~)              \
                         BOOST_PP_COMMA_IF(ARITY) void *functor) {             \
                F *o = static_cast<F *>(functor);                              \
                OSVR_RETURNSTATEMENT(RETURNS) (*o)(                            \
                    BOOST_PP_ENUM_PARAMS(ARITY, OSVR_PARAM_STEM));             \
            }                                                                  \
        };                                                                     \
    };                                                                         \
    template <> struct Caller<this_first_t, ARITY, RETURNS> {                  \
        template <typename FPtr, typename F>                                   \
        static OSVR_RETURNTYPE(RETURNS)                                        \
            call(void *functor BOOST_PP_COMMA_IF(ARITY)                        \
                     BOOST_PP_ENUM(ARITY, OSVR_MAKE_PARAMLIST, ~)) {           \
            F *o = static_cast<F *>(functor);                                  \
            OSVR_RETURNSTATEMENT(RETURNS) (*o)(                                \
                BOOST_PP_ENUM_PARAMS(ARITY, OSVR_PARAM_STEM));                 \
        }                                                                      \
    };
            // Generate specializations for void returns
            BOOST_PP_REPEAT(OSVR_UTIL_CALLER_MAX_ARITY, OSVR_MAKE_CALLERS, 0)
            // Generate specializations for  non-void returns
            BOOST_PP_REPEAT(OSVR_UTIL_CALLER_MAX_ARITY, OSVR_MAKE_CALLERS, 1)

#undef OSVR_MAKE_CALLERS
#undef OSVR_RETURNSTATEMENT
#undef OSVR_RETURNTYPE
#undef OSVR_MAKE_PARAMLIST
#undef OSVR_PARAM_STEM
            /// @brief Internal metafunction to compute which caller you need
            /// based
            /// on the pointer types and this location.
            template <typename FunctionPtr, typename FunctionObjectType,
                      typename ThisLocation>
            struct ComputeGenericCaller {
                /// @brief ThisLocation with any const& removed
                typedef typename boost::remove_cv<
                    typename boost::remove_reference<ThisLocation>::type>::type
                    UnqualifiedThisLocation;
#ifndef OSVR_DOXYGEN
                BOOST_STATIC_ASSERT_MSG(
                    (boost::is_same<UnqualifiedThisLocation,
                                    this_first_t>::value ||
                     boost::is_same<UnqualifiedThisLocation,
                                    this_last_t>::value),
                    "ThisLocation must be either this_last_t or this_first_t");
                BOOST_STATIC_ASSERT_MSG(
                    !(boost::is_pointer<FunctionObjectType>::value),
                    "FunctionObjectType must be the type of your function "
                    "object");
                BOOST_STATIC_ASSERT_MSG(
                    boost::is_function<typename boost::remove_pointer<
                        FunctionPtr>::type>::value &&
                        boost::is_pointer<FunctionPtr>::value,
                    "FunctionPtr must be a function pointer type");
#endif // OSVR_DOXYGEN
                /// @todo static assert that the argument lists are compatible

                /// @brief The function arity without the "this" parameter
                typedef typename boost::mpl::prior<
                    boost::function_types::function_arity<FunctionPtr> >::type
                    Arity;

                /// @brief Whether or not the function returns a value.
                typedef typename boost::mpl::if_<
                    boost::is_void<typename boost::function_types::result_type<
                        FunctionPtr>::type>,
                    boost::mpl::int_<0>, boost::mpl::int_<1> >::type Returns;

                /// @brief Computed intermediate result.
                typedef detail::Caller<UnqualifiedThisLocation, Arity::value,
                                       Returns::value> GeneralResult;

                /// @brief Computed result.
                typedef typename GeneralResult::template Specialized<
                    FunctionPtr, FunctionObjectType> type;
            };
        } // namespace detail
#endif    // ifndef OSVR_DOXYGEN_EXTERNAL

        /// @brief Struct containing a single static function member named
        /// "call"
        /// that serves as a converter from a function call with an opaque
        /// userdata
        /// pointer to a functor call using the userdata pointer as "this".
        ///
        /// @tparam FunctionPtr Desired function pointer type
        /// @tparam FunctionObjectType Type of your function object
        /// @tparam ThisLocation one of this_first_t or this_last_t indicating
        /// which
        /// parameter is the "userdata" this pointer.
        template <typename FunctionPtr, typename FunctionObjectType,
                  typename ThisLocation>
        struct GenericCaller
            : detail::ComputeGenericCaller<FunctionPtr, FunctionObjectType,
                                           ThisLocation>::type {};

        /// @brief Get a generic functor caller: a pointer to a function that
        /// will
        /// call an object of your specific function object type, expecting the
        /// function object address passed as a void * as a parameter.
        ///
        /// @tparam FunctionPtr Desired function pointer type
        /// @tparam FunctionObjectType Type of your function object
        /// @tparam ThisLocation one of this_first_t or this_last_t indicating
        /// which
        /// parameter is the "userdata" this pointer.
        ///
        /// Function wrapper around GenericCaller.
        template <typename FunctionPtr, typename FunctionObjectType,
                  typename ThisLocation>
        inline FunctionPtr getCaller() {
            typedef GenericCaller<FunctionPtr, FunctionObjectType, ThisLocation>
                CallerType;
            return &CallerType::call;
        }
        /// @brief Get a generic functor caller. Specify the location of "this"
        /// through the argument: results in automatic type deduction for
        /// ThisLocation.
        ///
        /// @tparam FunctionPtr Desired function pointer type
        /// @tparam FunctionObjectType Type of your function object
        ///
        /// @overload
        template <typename FunctionPtr, typename FunctionObjectType,
                  typename ThisLocation>
        inline FunctionPtr
        getCaller(/** Either this_first or this_last */ ThisLocation const &) {
            return getCaller<FunctionPtr, FunctionObjectType, ThisLocation>();
        }

        /// @brief Get a generic functor caller. Pass a pointer to a function
        /// object
        /// and specify the location of "this" through the argument: results in
        /// automatic type deduction for FunctionObjectType and ThisLocation.
        ///
        /// Note that nothing is actually done with your function object
        /// pointer:
        /// you still need to pass it along with this function pointer as
        /// userdata.
        ///
        /// @tparam FunctionPtr Desired function pointer type
        ///
        /// @overload
        template <typename FunctionPtr, typename FunctionObjectType,
                  typename ThisLocation>
        inline FunctionPtr getCaller(
            /** A pointer to your functor for type deduction only */ FunctionObjectType const *,
            /** Either this_first or this_last */ ThisLocation const &) {
            return getCaller<FunctionPtr, FunctionObjectType, ThisLocation>();
        }
#ifndef OSVR_DOXYGEN_EXTERNAL
    } // namespace functor_trampolines

    // Import symbols into namespace.
    using functor_trampolines::getCaller;
    using functor_trampolines::GenericCaller;
    using functor_trampolines::this_first;
    using functor_trampolines::this_first_t;
    using functor_trampolines::this_last;
    using functor_trampolines::this_last_t;

#endif // ifndef OSVR_DOXYGEN_EXTERNAL

} // namespace util
} // namespace osvr

#endif // INCLUDED_GenericCaller_h_GUID_C49A7DEE_89E6_478D_627A_CFFB9ED39EAC
