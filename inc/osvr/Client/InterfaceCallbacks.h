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

#ifndef INCLUDED_InterfaceCallbacks_h_GUID_0CE1EE79_D74A_4EAA_CF16_3AABDA3A1B6A
#define INCLUDED_InterfaceCallbacks_h_GUID_0CE1EE79_D74A_4EAA_CF16_3AABDA3A1B6A

// Internal Includes
#include <osvr/Client/ReportMap.h>
#include <osvr/Client/ReportTypes.h>
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
namespace client {
    /// @brief Metafunction computing the storage for callbacks for a report
    /// type.
    template <typename ReportType> struct CallbackStorageType {
        typedef std::vector<std::function<void(const OSVR_TimeValue *,
                                               const ReportType *)> > type;
    };

    typedef traits::GenerateReportMap<CallbackStorageType<boost::mpl::_> >::type
        CallbackMap;

    /// @brief Class to maintain callbacks for an interface for each report type
    /// explicitly enumerated.
    class InterfaceCallbacks {
      public:
        template <typename CallbackType>
        void addCallback(CallbackType cb, void *userdata) {
            typedef typename traits::ReportTypeFromCallback<CallbackType>::type
                ReportType;
            using namespace std::placeholders;
            boost::fusion::at_key<ReportType>(m_callbacks)
                .push_back(std::bind(cb, userdata, _1, _2));
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

} // namespace client
} // namespace osvr

#endif // INCLUDED_InterfaceCallbacks_h_GUID_0CE1EE79_D74A_4EAA_CF16_3AABDA3A1B6A
