/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_GenerateCompoundServer_h_GUID_55F870FA_C398_49B8_9907_A007B3C6F8CA
#define INCLUDED_GenerateCompoundServer_h_GUID_55F870FA_C398_49B8_9907_A007B3C6F8CA

// Internal Includes
#include "GenerateVrpnDynamicServer.h"

// Library/third-party includes
#include <boost/mpl/begin_end.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/next.hpp>
#include <boost/type_traits/is_same.hpp>

// Standard includes
// - none

namespace osvr {
namespace connection {
    namespace server_generation {

        /// @brief Metaprogramming to generate a VRPN server object out of an
        /// MPL
        /// sequence of types.
        template <typename Types> struct GenerateServer {

            template <typename Iter>
            struct IsLast
                : boost::is_same<typename boost::mpl::next<Iter>::type,
                                 typename boost::mpl::end<Types>::type> {};

            /// Forward declare the main class
            template <typename Iter = typename boost::mpl::begin<Types>::type,
                      bool Last = IsLast<Iter>::value>
            class ServerElement {};

            /// Compute base class: the current class from the sequence
            template <typename Iter> struct LeafBase {
                typedef typename boost::mpl::deref<Iter>::type type;
            };

            /// Compute base class: the recursive step to the next element in
            /// the sequence.
            template <typename Iter> struct ChainBase {
                typedef ServerElement<typename boost::mpl::next<Iter>::type>
                    type;
            };

            /// Base case: this is the last type in the sequence, just inherit
            /// from it
            template <typename Iter>
            class ServerElement<Iter, true> : public LeafBase<Iter>::type {
              public:
                typedef typename LeafBase<Iter>::type LeafBase;
                /// @brief Constructor passing along.
                ServerElement(ConstructorArgument &init) : LeafBase(init) {}
                void mainloop() { LeafBase::server_mainloop(); }
                vrpn_Connection *connectionPtr() {
                    return LeafBase::connectionPtr();
                }
            };

            /// Normal case: inherit from the current sequence element (leaf
            /// base) and from the next step in the chain
            template <typename Iter>
            class ServerElement<Iter, false> : public LeafBase<Iter>::type,
                                               public ChainBase<Iter>::type {
              public:
                typedef typename LeafBase<Iter>::type LeafBase;
                typedef typename ChainBase<Iter>::type ChainBase;

                /// @brief Constructor passing along to both base classes.
                ServerElement(ConstructorArgument &init)
                    : LeafBase(init), ChainBase(init) {}

                /// @brief Mainloop calls server_mainloop in the leaf.
                void mainloop() { LeafBase::server_mainloop(); }

                vrpn_Connection *connectionPtr() {
                    return LeafBase::connectionPtr();
                }
            };

            /// @brief Computed server type
            typedef ServerElement<> type;

            /// @brief Create a server
            static type *make(ConstructorArgument &init) {
                return new type(init);
            }
        };
    } // namespace server_generation
} // namespace connection
} // namespace osvr

#endif // INCLUDED_GenerateCompoundServer_h_GUID_55F870FA_C398_49B8_9907_A007B3C6F8CA
