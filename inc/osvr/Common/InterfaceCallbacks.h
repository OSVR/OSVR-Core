/** @file
    @brief Header

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

#ifndef INCLUDED_InterfaceCallbacks_h_GUID_0CE1EE79_D74A_4EAA_CF16_3AABDA3A1B6A
#define INCLUDED_InterfaceCallbacks_h_GUID_0CE1EE79_D74A_4EAA_CF16_3AABDA3A1B6A

// Internal Includes
#include <osvr/Common/ReportMap.h>
#include <osvr/Common/ReportTypes.h>
#include <osvr/Common/ReportFromCallback.h>
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/optional.hpp>

// Standard includes
#include <vector>
#include <functional>

namespace osvr {
namespace common {
    /// @brief Metafunction computing the storage for callbacks for a report
    /// type.
    template <typename ReportType> struct CallbackStorageType {
        using type = std::vector<
            std::function<void(const OSVR_TimeValue *, const ReportType *)> >;
    };

    typedef traits::GenerateReportMap<CallbackStorageType<boost::mpl::_> >::type
        CallbackMap;

    /// @brief Class to maintain callbacks for an interface for each report type
    /// explicitly enumerated.
    class InterfaceCallbacks {
      public:
        template <typename CallbackType>
        void addCallback(CallbackType cb, void *userdata) {
            using ReportType = traits::ReportFromCallback_t<CallbackType>;
            boost::fusion::at_key<ReportType>(m_callbacks)
                .push_back(
                    [cb, userdata](util::time::TimeValue const *timestamp,
                                   ReportType const *report) {
                        cb(userdata, timestamp, report);
                    });
        }

        template <typename ReportType>
        void triggerCallbacks(util::time::TimeValue const &timestamp,
                              ReportType const &report) const {
            for (auto const &f :
                 boost::fusion::at_key<ReportType>(m_callbacks)) {
                f(&timestamp, &report);
            }
            /// @todo do we fail silently or throw exception if we are asked for
            /// state we don't have?
        }

      private:
        CallbackMap m_callbacks;
    };

} // namespace common
} // namespace osvr

#endif // INCLUDED_InterfaceCallbacks_h_GUID_0CE1EE79_D74A_4EAA_CF16_3AABDA3A1B6A
