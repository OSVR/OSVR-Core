/** @file
    @brief Header defining a simple C++11 template-parameter-pack-based
   metaprogramming library inspired by/based on Eric Niebler's `meta`

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

    @author
    Eric Niebler
*/

// Copyright 2015 Sensics, Inc.
// Copyright Eric Niebler 2014-2015
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/meta
//

#ifndef INCLUDED_TypePack_h_GUID_3431E1BF_7DF4_429A_0C80_2D14E1948181
#define INCLUDED_TypePack_h_GUID_3431E1BF_7DF4_429A_0C80_2D14E1948181

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <cstddef>
#include <type_traits>

namespace osvr {
/// @brief A simple argument-pack-based metaprogramming library, inspired by
/// and based partially on https://ericniebler.github.io/meta
namespace typepack {

    template <bool V> using bool_ = std::integral_constant<bool, V>;
    template <std::size_t V>
    using size_t_ = std::integral_constant<std::size_t, V>;

    template <typename T> using t_ = typename T::type;

    namespace detail {
        struct list_base_ {};
    } // namespace detail

    template <typename... Ts> struct list : detail::list_base_ {
        using type = list;
    };
    template <typename... Ts> struct list<list<Ts...>> : list<Ts...>::type {};

    /// @brief Will turn whatever is passed into it into the simplest list.
    template <typename... Ts> using coerce_list = t_<list<Ts...>>;

    namespace detail {
        template <typename... Ts> struct size;

        // The following fails with clang due to a bug.
        // <https://llvm.org/bugs/show_bug.cgi?id=14858>
        // template <typename... Ts> using size_impl =
        // size_t_<sizeof...(Ts)>;
        // template <typename... Ts>
        // struct size<list<Ts...>> : size_impl<Ts...> {};
        template <typename... Ts>
        struct size<list<Ts...>> : size_t_<sizeof...(Ts)> {};
    } // namespace detail

    template <typename... Ts> using size = detail::size<coerce_list<Ts...>>;
    template <typename... Ts> using length = size<coerce_list<Ts...>>;

    /// Apply an alias class
    template <typename F, typename... Args>
    using apply = typename F::template apply<Args...>;

    namespace detail {
        /// General/dummy case.
        template <typename F, typename List> struct apply_list_ {};
        template <typename F, typename... Args>
        struct apply_list_<F, list<Args...>> {
            typedef typename F::template apply<Args...>::type type;
        };
    } // namespace detail

    /// Apply an alias class, exploding the list of args
    template <typename F, typename Args>
    using apply_list = t_<detail::apply_list_<F, coerce_list<Args>>>;

    /// A Alias Class that always returns \p T.
    template <typename T> struct always {
      private:
        // Redirect through a class template for compilers that have not
        // yet implemented CWG 1558:
        // <http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1558>
        template <typename...> struct impl { using type = T; };

      public:
        template <typename... Ts> using apply = t_<impl<Ts...>>;
    };

    /// An alias for `void`.
    template <typename... Ts> using void_ = apply<always<void>, Ts...>;

    /// \cond
    namespace detail {
        template <typename, typename = void> struct has_type_ {
            using type = std::false_type;
        };

        template <typename T> struct has_type_<T, void_<typename T::type>> {
            using type = std::true_type;
        };

        template <template <typename...> class, typename> struct defer_ {};

        template <template <typename...> class C, typename... Ts>
        struct defer_<C, list<Ts...>> {
            using type = C<Ts...>;
        };
    } // namespace detail
      /// \endcond

    /// An alias for `std::true_type` if `T::type` exists and names a type;
    /// otherwise, it's an alias for `std::false_type`.
    template <typename T> using has_type = t_<detail::has_type_<T>>;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // defer
    /// A wrapper that defers the instantiation of a template \p C with type
    /// parameters \p Ts in
    /// a \c lambda or \c let expression.
    ///
    /// In the code below, the lambda would ideally be written as
    /// `lambda<_a,_b,push_back<_a,_b>>`, however this fails since
    /// `push_back` expects its first
    /// argument to be a list, not a placeholder. Instead, we express it
    /// using \c defer as
    /// follows:
    ///
    /// \code
    /// template<typename List>
    /// using reverse = reverse_fold<List, list<>, lambda<_a, _b,
    /// defer<push_back, _a, _b>>>;
    /// \endcode
    template <template <typename...> class C, typename... Ts>
    struct defer : detail::defer_<C, list<Ts...>> {};

    /// Compose the Alias Classes \p Fs in the parameter pack \p Ts.
    template <typename... Fs> struct compose {};

    template <typename F0> struct compose<F0> {
        template <typename... Ts> using apply = apply<F0, Ts...>;
    };

    template <typename F0, typename... Fs> struct compose<F0, Fs...> {
        template <typename... Ts>
        using apply =
            typepack::apply<F0, typepack::apply<compose<Fs...>, Ts...>>;
    };
    /// Turn a class template or alias template \p C into a Alias Class.
    template <template <typename...> class C> struct quote {
        // Indirection through defer here needed to avoid Core issue 1430
        // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
        template <typename... Ts> using apply = t_<defer<C, Ts...>>;
    };

    /// Turn a trait \p C into a Alias Class.
    /// \ingroup composition
    template <template <typename...> class C>
    using quote_trait = compose<quote<t_>, quote<C>>;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // if_
    /// \cond
    namespace detail {
        template <typename...> struct if_impl {};

        template <typename If>
        struct if_impl<If> : std::enable_if<If::type::value> {};

        template <typename If, typename Then>
        struct if_impl<If, Then> : std::enable_if<If::type::value, Then> {};

        template <typename If, typename Then, typename Else>
        struct if_impl<If, Then, Else>
            : std::conditional<If::type::value, Then, Else> {};
    } // namespace detail
    /// \endcond

    /// Select one type or another depending on a compile-time Boolean.
    template <typename... Args> using if_ = t_<detail::if_impl<Args...>>;

    /// Select one type or another depending on a compile-time Boolean.
    template <bool If, typename... Args>
    using if_c = t_<detail::if_impl<bool_<If>, Args...>>;

    /// \cond
    namespace detail {
        template <typename... Bools> struct and_impl;

        template <> struct and_impl<> : std::true_type {};

        template <typename Bool, typename... Bools>
        struct and_impl<Bool, Bools...>
            : if_<bool_<!Bool::type::value>, std::false_type,
                  and_impl<Bools...>> {};

        template <typename... Bools> struct or_impl;

        template <> struct or_impl<> : std::false_type {};

        template <typename Bool, typename... Bools>
        struct or_impl<Bool, Bools...>
            : if_c<Bool::type::value, std::true_type, or_impl<Bools...>> {};
    } // namespace detail
    /// \endcond

    /// Logically and together all the integral constant-wrapped Boolean
    /// parameters, \e with short-circuiting.
    template <typename... Bools> using and_ = t_<detail::and_impl<Bools...>>;

    /// Logically or together all the integral constant-wrapped Boolean
    /// parameters, \e with short-circuiting.
    template <typename... Bools> using or_ = t_<detail::or_impl<Bools...>>;

    /// Logical not on a single boolean.
    template <typename Bool> using not_ = bool_<!Bool::value>;

    namespace detail {
        /// General/dummy case.
        template <typename... List> struct split_list_ {
            using head = void;
            using tail = list<>;
        };

        /// Unwrap type pack.
        template <typename... List>
        struct split_list_<list<List...>> : split_list_<List...> {};

        /// Primary case
        template <typename Head, typename... Tail>
        struct split_list_<Head, Tail...> {
            using head = Head;
            using tail = list<Tail...>;
        };
    } // namespace detail

    /// Get the first element of a list.
    template <typename... List>
    using head = typename detail::split_list_<List...>::head;

    /// Get the list without its first element
    template <typename... List>
    using tail = typename detail::split_list_<List...>::tail;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // concat
    /// \cond
    namespace detail {
        template <typename... Lists> struct concat_ {};

        template <> struct concat_<> { using type = list<>; };

        template <typename... List1> struct concat_<list<List1...>> {
            using type = list<List1...>;
        };

        template <typename... List1, typename... List2>
        struct concat_<list<List1...>, list<List2...>> {
            using type = list<List1..., List2...>;
        };

        template <typename... List1, typename... List2, typename... List3>
        struct concat_<list<List1...>, list<List2...>, list<List3...>> {
            using type = list<List1..., List2..., List3...>;
        };

        template <typename... List1, typename... List2, typename... List3,
                  typename... Rest>
        struct concat_<list<List1...>, list<List2...>, list<List3...>, Rest...>
            : concat_<list<List1..., List2..., List3...>, Rest...> {};
    } // namespace detail
    /// \endcond

    /// Concatenates several lists into a single list.
    /// \pre The parameters must all be instantiations of \c meta::list.
    /// \par Complexity
    /// \f$ O(L) \f$ where \f$ L \f$ is the number of lists in the list of
    /// lists.
    template <typename... Lists> using concat = t_<detail::concat_<Lists...>>;

    namespace detail {
        template <typename T> struct push_back_impl {
            template <typename... Ts> using apply = list<Ts..., T>;
        };

        template <typename T> struct push_front_impl {
            template <typename... Ts> struct apply {
                using type = list<T, Ts...>;
            };
        };
    }

    template <typename List, typename T>
    using push_back = apply_list<detail::push_back_impl<T>, List>;
    template <typename List, typename T>
    using push_front = apply_list<detail::push_front_impl<T>, List>;

    namespace detail {
        // Fold: Forward declaration of general form
        template <typename List, typename State, typename Fun> struct fold_;

        // Fold: Recurse
        template <typename List, typename State, typename Fun>
        struct fold_
            : fold_<tail<List>, t_<apply<Fun, State, head<List>>>, Fun> {};

        // Fold: base case
        template <typename State, typename Fun>
        struct fold_<list<>, State, Fun> {
            using type = State;
        };

    } // namespace detail

    template <typename List, typename State, typename Fun>
    using fold = t_<detail::fold_<List, State, Fun>>;

    namespace detail {

        template <typename, typename> struct transform_ {};
        template <typename... Ts, typename Fun>
        struct transform_<list<Ts...>, Fun> {
            /// required for MSVC2013 to avoid "there are no parameter packs
            /// available to expand"
            template <typename T>
            struct apply_workaround : apply<Fun, T>::type {};
            typedef coerce_list<typename apply_workaround<Ts>::type...> type;
        };
    } // namespace detail
    template <typename List, typename Fun>
    using transform = t_<detail::transform_<List, Fun>>;

    namespace detail {

        template <typename T> struct is_ {
            template <typename Elt> using apply = std::is_same<T, Elt>;
        };
    }
    template <typename Haystack, typename Needle>
    using contains =
        apply_list<quote<or_>, transform<Haystack, detail::is_<Needle>>>;

} // namespace typepack
} // namespace osvr

#endif // INCLUDED_TypePack_h_GUID_3431E1BF_7DF4_429A_0C80_2D14E1948181
