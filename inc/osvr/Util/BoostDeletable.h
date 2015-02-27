/** @file
    @brief Header for boost::shared_ptr-based management of Deletables

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

#ifndef INCLUDED_Deletable_h_GUID_66D6C45A_7AB7_4198_F8EE_202F0FCE682C
#define INCLUDED_Deletable_h_GUID_66D6C45A_7AB7_4198_F8EE_202F0FCE682C

// Internal Includes
#include <osvr/Util/Deletable.h>

// Library/third-party includes
#include <boost/shared_ptr.hpp>

// Standard includes
#include <vector>

namespace osvr {
namespace util {
    namespace boost_util {
        /// @brief Shared-ownership smart pointer to a Deletable, using Boost's
        /// shared pointers.
        typedef ::boost::shared_ptr<Deletable> DeletablePtr;
        /// @brief Vector of shared-ownership smart pointers
        typedef ::std::vector<DeletablePtr> DeletableList;
    } // namespace boost_util
} // namespace util
} // namespace osvr

#endif // INCLUDED_Deletable_h_GUID_66D6C45A_7AB7_4198_F8EE_202F0FCE682C
