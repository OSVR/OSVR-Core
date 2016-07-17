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

#ifndef INCLUDED_SharedMemoryObjectWithMutex_h_GUID_8A1DE87D_04F9_4689_6CAC_6D8579B0464D
#define INCLUDED_SharedMemoryObjectWithMutex_h_GUID_8A1DE87D_04F9_4689_6CAC_6D8579B0464D

// Internal Includes
// - none

// Library/third-party includes
#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace ipc {

        namespace bip = boost::interprocess;

        /// Using it as a shared mutex - but that wasn't added until boost 1.52
        typedef bip::interprocess_upgradable_mutex mutex_type;
        typedef bip::scoped_lock<mutex_type> exclusive_lock_type;
        typedef bip::sharable_lock<mutex_type> sharable_lock_type;

        class ObjectWithMutex {
          public:
            sharable_lock_type getSharableLock() {
                return sharable_lock_type(getMutex());
            }
            exclusive_lock_type getExclusiveLock() {
                return exclusive_lock_type(getMutex());
            }
            mutex_type &getMutex() { return m_mutex; }

            /// @brief Checks to make sure we have a writer (exclusive) lock.
            void verifyWriterLock(exclusive_lock_type &lock) const {
                if (!lock || (lock.mutex() != &m_mutex)) {
                    throw std::logic_error("Lock passed must be "
                                           "exclusively locked and for the "
                                           "correct mutex!");
                }
            }
            /// @brief Checks to make sure we at least have a reader (sharable)
            /// lock.
            template <typename LockType>
            void verifyReaderLock(LockType &lock) const {
                if (!lock || (lock.mutex() != &m_mutex)) {
                    throw std::logic_error(
                        "Lock passed must be "
                        "at least sharably locked and for the "
                        "correct mutex!");
                }
            }

          private:
            mutex_type m_mutex;
        };
    } // namespace ipc

} // namespace common
} // namespace osvr

#endif // INCLUDED_SharedMemoryObjectWithMutex_h_GUID_8A1DE87D_04F9_4689_6CAC_6D8579B0464D
