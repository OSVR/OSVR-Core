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

#ifndef INCLUDED_JSONResolvePossibleRef_h_GUID_64F277A1_33F0_4986_6EEE_09E17C09AA52
#define INCLUDED_JSONResolvePossibleRef_h_GUID_64F277A1_33F0_4986_6EEE_09E17C09AA52

// Internal Includes
// - none

// Library/third-party includes
#include <json/value.h>

// Standard includes
#include <vector>
#include <string>

namespace osvr {
namespace server {
    enum class FileLoadStatus {
        CouldNotOpenFile,
        CouldNotParseFile,
        FileOpenedAndParsed
    };
    enum class ValueHandledAs { Filename, String, JsonRefToFile, Other };
    struct FileLoadAttempt {
        std::string path;
        FileLoadStatus status;
        std::string details;
    };
    using FileLoadAttempts = std::vector<FileLoadAttempt>;

    struct ResolveRefResult {
        Json::Value result;
        bool resolved = false;
        ValueHandledAs handledAs = ValueHandledAs::Other;
        FileLoadAttempts fileAttempts;
    };

    const char *fileLoadStatusToString(FileLoadStatus status);

    /// @brief Given an input that might be a filename, might be a JSON
    /// Pointer-style $ref object, and might just directly be an object, return
    /// the object desired.
    ///
    /// If just a string (suggesting it was intended to be a reference), returns
    /// null.
    ///
    /// @param stringAcceptableResult Determines whether a string that we can't
    /// resolve to a loadable JSON reference should be returned as itself
    /// (valid, true), or should be signaled as an error by returning null
    /// (false, default)
    /// @param searchPath Optional list of directories to look for any mentioned
    /// files.
    Json::Value resolvePossibleRef(Json::Value const &input,
                                   bool stringAcceptableResult = false,
                                   std::vector<std::string> const &searchPath =
                                       std::vector<std::string>());

    ResolveRefResult
    resolvePossibleRefWithDetails(Json::Value const &input,
                                  bool stringAcceptableResult = false,
                                  std::vector<std::string> const &searchPath =
                                      std::vector<std::string>());

} // namespace server
} // namespace osvr

#endif // INCLUDED_JSONResolvePossibleRef_h_GUID_64F277A1_33F0_4986_6EEE_09E17C09AA52
