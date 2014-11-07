/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

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

#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/type_traits/is_void.hpp>

#include <boost/mpl/at.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/next_prior.hpp>

// Standard includes
// - none

#ifndef OGVR_UTIL_CALLER_MAX_ARITY
#define OGVR_UTIL_CALLER_MAX_ARITY 3
#endif

namespace ogvr {
namespace functor_trampolines {

    namespace detail {
        /// @brief Convenience metafunction to simplify computing the type of a
        /// particular argument to a function F
        template <typename F, int C> struct param_at {
            typedef typename boost::function_types::parameter_types<F>::type
                ParamTypes;
            typedef typename boost::mpl::at<ParamTypes,
                                            boost::mpl::int_<C> >::type type;
        };
        /// @brief Template that will be specialized to contain callers for
        /// functors with the "this" pointer as the last argument.
        ///
        /// @tparam Arity arity of the functor (not including "this" pointer)
        /// @tparam Return 0 if void return, 1 if non-void
        template <int Arity, int Return> struct CallerThisLast;

        /// @brief Template that will be specialized to contain callers for
        /// functors with the "this" pointer as the first argument.
        ///
        /// @tparam Arity arity of the functor (not including "this" pointer)
        /// @tparam Return 0 if void return, 1 if non-void
        template <int Arity, int Return> struct CallerThisFirst;

/// @brief what do we call our pass-through parameters
#define OGVR_PARAM_STEM p

/// @brief Macro for use with BOOST_PP_ENUM to generate pass-through argument
/// list
#define OGVR_MAKE_PARAMLIST(Z, N, unused)                                      \
    typename param_at<FPtr, N>::type BOOST_PP_CAT(OGVR_PARAM_STEM, N)

/// @brief Expands to the result type computation if R==1, void if R==0
///
/// @param R value of RETURNS
#define OGVR_RETURNTYPE(R)                                                     \
    BOOST_PP_IF(R, typename boost::function_types::result_type<FPtr>::type,    \
                void)

/// @brief Expands to "return" if R==1, nothing if R==0
///
/// @param R value of RETURNS
#define OGVR_RETURNSTATEMENT(R) BOOST_PP_IF(R, return, )

/// @brief Generates specializations of CallerThisLast and CallerThisFirst for
/// the given values of ARITY and RETURNS (matching the template parameters).
///
/// Intended for use with BOOST_PP_REPEAT, passing a value (0 or 1) for RETURNS
/// in the third, "data" parameter.
#define OGVR_MAKE_CALLERS(Z, ARITY, RETURNS)                                   \
    template <> struct CallerThisLast<ARITY, RETURNS> {                        \
        template <typename FPtr, typename F>                                   \
        static OGVR_RETURNTYPE(RETURNS)                                        \
            call(BOOST_PP_ENUM(ARITY, OGVR_MAKE_PARAMLIST, ~)                  \
                     BOOST_PP_COMMA_IF(ARITY) void *functor) {                 \
            F *o = static_cast<F *>(functor);                                  \
            OGVR_RETURNSTATEMENT(RETURNS) (*o)(                                \
                BOOST_PP_ENUM_PARAMS(ARITY, OGVR_PARAM_STEM));                 \
        }                                                                      \
    };                                                                         \
    template <> struct CallerThisFirst<ARITY, RETURNS> {                       \
        template <typename FPtr, typename F>                                   \
        static OGVR_RETURNTYPE(RETURNS)                                        \
            call(void *functor BOOST_PP_COMMA_IF(ARITY)                        \
                     BOOST_PP_ENUM(ARITY, OGVR_MAKE_PARAMLIST, ~)) {           \
            F *o = static_cast<F *>(functor);                                  \
            OGVR_RETURNSTATEMENT(RETURNS) (*o)(                                \
                BOOST_PP_ENUM_PARAMS(ARITY, OGVR_PARAM_STEM));                 \
        }                                                                      \
    };
        // Generate specializations for void returns
        BOOST_PP_REPEAT(OGVR_UTIL_CALLER_MAX_ARITY, OGVR_MAKE_CALLERS, 0)
        // Generate specializations for  non-void returns
        BOOST_PP_REPEAT(OGVR_UTIL_CALLER_MAX_ARITY, OGVR_MAKE_CALLERS, 1)

#undef OGVR_MAKE_CALLERS
#undef OGVR_RETURNSTATEMENT
#undef OGVR_RETURNTYPE
#undef OGVR_MAKE_PARAMLIST
#undef OGVR_PARAM_STEM
    } // end of namespace detail

    /// @brief Get the pointer to a function that will call an object of your
    /// specific function object type, expecting the function object address
    /// passed as a void * as the first parameter.
    template <typename FunctionPtr, typename FunctionObjectPtr>
    inline FunctionPtr getCallerThisFirst() {
        /// @todo static assert that the argument lists are compatible
        /// @todo make this compile time?
        typedef typename boost::mpl::prior<
            boost::function_arity<FunctionPtr> >::type Arity;
        typedef typename boost::mpl::if_<
            boost::is_void<
                typename boost::function_types::result_type<FPtr>::type>,
            boost::mpl::int_<0>, boost::mpl::int_<1> >::type Returns;
        return &detail::CallerThisFirst<Arity::value, Returns::value>::call;
    }

    /// @brief Overload for type deduction on the functor.
    template <typename FunctionPtr, typename FunctionObjectPtr>
    inline FunctionPtr getCallerThisFirst(FunctionObjectPtr /*functor*/) {
        return getCallerThisFirst<FunctionPtr, FunctionObjectPtr>();
    }

    /// @brief Get the pointer to a function that will call an object of your
    /// specific function object type, expecting the function object address
    /// passed as a void * as the last parameter.
    template <typename FunctionPtr, typename FunctionObjectPtr>
    inline FunctionPtr getCallerThisLast() {
        /// @todo static assert that the argument lists are compatible
        /// @todo make this compile time?
        using namespace boost::mpl;
        using namespace boost::function_types;
        using boost::is_void;
        typedef typename prior<function_arity<FunctionPtr> >::type Arity;
        typedef typename if_<is_void<typename result_type<FunctionPtr>::type>,
                             int_<0>, int_<1> >::type Returns;
        return &detail::CallerThisLast<Arity::value, Returns::value>::call;
    }

    /// @brief Overload for type deduction on the functor.
    template <typename FunctionPtr, typename FunctionObjectPtr>
    inline FunctionPtr getCallerThisLast(FunctionObjectPtr /*functor*/) {
        return getCallerThisLast<FunctionPtr, FunctionObjectPtr>();
    }
} // end of namespace functor_trampolines
} // end of namespace ogvr

#endif // INCLUDED_GenericCaller_h_GUID_C49A7DEE_89E6_478D_627A_CFFB9ED39EAC
