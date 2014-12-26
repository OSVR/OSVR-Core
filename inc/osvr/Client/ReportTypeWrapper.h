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

#ifndef INCLUDED_ReportTypeWrapper_h_GUID_D9BF9925_4EA6_4CB5_4035_332F8D5307F2
#define INCLUDED_ReportTypeWrapper_h_GUID_D9BF9925_4EA6_4CB5_4035_332F8D5307F2

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {
    namespace traits {
        /// @brief A template type used as a function argument for type
        /// deduction or overloading.
        template <typename ReportType> struct ReportTypeWrapper;

        /// @brief Creates and returns a null pointer to a given report type
        /// wrapper, for use as a function argument for type deduction or
        /// overloading.
        template <typename ReportType>
        inline ReportTypeWrapper<ReportType> *report() {
            return static_cast<ReportTypeWrapper<ReportType> *>(nullptr);
        }
    } // namespace traits

} // namespace client
} // namespace osvr

#endif // INCLUDED_ReportTypeWrapper_h_GUID_D9BF9925_4EA6_4CB5_4035_332F8D5307F2
