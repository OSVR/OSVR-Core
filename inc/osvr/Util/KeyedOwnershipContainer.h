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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_KeyedOwnershipContainer_h_GUID_002CD118_DF06_45AC_44D8_C37BA99E0E93
#define INCLUDED_KeyedOwnershipContainer_h_GUID_002CD118_DF06_45AC_44D8_C37BA99E0E93

// Internal Includes
// - none

// Library/third-party includes
#include <boost/any.hpp>

// Standard includes
#include <map>

namespace osvr {
namespace util {
    class SingleOwnershipPolicy {
      protected:
        SingleOwnershipPolicy() {}
        void *doInsert(void *rawPtr, boost::any smartPtr) {
            m_container[rawPtr] = smartPtr;
            return rawPtr;
        }

        bool doReleaseOne(void *rawPtr) {

            size_t found = m_container.erase(rawPtr);
            return (0 != found);
        }

      private:
        typedef std::map<void *, boost::any> Container;
        Container m_container;
    };

    class MultipleReferenceOwnershipPolicy {
      protected:
        MultipleReferenceOwnershipPolicy() {}

        void *doInsert(void *rawPtr, boost::any smartPtr) {
            m_container.insert(std::make_pair(rawPtr, smartPtr));
            return rawPtr;
        }

        bool doReleaseOne(void *rawPtr) {
            auto it = m_container.find(rawPtr);
            if (m_container.end() != it) {
                m_container.erase(it);
                return true;
            }
            return false;
        }

      private:
        typedef std::multimap<void *, boost::any> Container;
        Container m_container;
    };
    /// @brief Holds on to smart pointers by value, and lets you free them by
    /// providing the corresponding void *.
    template <typename Policy = SingleOwnershipPolicy>
    class BasicKeyedOwnershipContainer : private Policy {
      public:
        /// @brief Adds an object held by a smart pointer to our ownership,
        /// returning its void * usable to release it before the destruction of
        /// this object.
        template <typename T> void *acquire(T ptr) {
            return Policy::doInsert(ptr.get(), ptr);
        }

        /// @brief Releases the indicated smart pointer in our ownership, if we
        /// have it.
        ///
        /// Note that this just releases the smart pointer object that we have -
        /// the underlying memory may not be freed if your smart pointer is
        /// still referenced elsewhere, for instance.
        ///
        /// @returns true if we found it and released it
        bool release(void *ptr) { return Policy::doReleaseOne(ptr); }
    };
    typedef BasicKeyedOwnershipContainer<SingleOwnershipPolicy>
        KeyedOwnershipContainer;

    typedef BasicKeyedOwnershipContainer<MultipleReferenceOwnershipPolicy>
        MultipleKeyedOwnershipContainer;
} // namespace util
} // namespace osvr

#endif // INCLUDED_KeyedOwnershipContainer_h_GUID_002CD118_DF06_45AC_44D8_C37BA99E0E93
