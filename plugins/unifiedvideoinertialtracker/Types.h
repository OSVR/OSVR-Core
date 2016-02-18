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

#ifndef INCLUDED_Types_h_GUID_819757A3_DE89_4BAD_3BF5_6FE152F1EA08
#define INCLUDED_Types_h_GUID_819757A3_DE89_4BAD_3BF5_6FE152F1EA08

// Internal Includes
#include "BasicTypes.h"
#include "ConfigParams.h"
#include "Assumptions.h"

// Library/third-party includes
#include <opencv2/features2d/features2d.hpp>

// Standard includes
#include <vector>
#include <list>
#include <string>
#include <memory>
#include <functional>

namespace osvr {
namespace vbtracker {

    class Led;
    class LedIdentifier;

    typedef std::vector<std::string> PatternStringList;

    typedef std::vector<LedPatternWrapped>
        PatternList; //< Ordered set of patterns to search

    typedef std::vector<cv::KeyPoint> KeyPointList;
    typedef KeyPointList::iterator KeyPointIterator;

    typedef std::unique_ptr<LedIdentifier> LedIdentifierPtr;

    typedef std::list<Led> LedGroup;
    using LedPtrList = std::vector<Led *>;

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_Types_h_GUID_819757A3_DE89_4BAD_3BF5_6FE152F1EA08
