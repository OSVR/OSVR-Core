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

#ifndef INCLUDED_GetRuntimeVersion_h_GUID_487199D9_CA87_4E85_52A2_D1BDBC119163
#define INCLUDED_GetRuntimeVersion_h_GUID_487199D9_CA87_4E85_52A2_D1BDBC119163

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Util/RuntimeNumericVersion.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {
    /// @brief Returns the value of the version component (zero-indexed)
    /// specified, in the inclusive range [0,3].
    /// @return version component value, or 0 if component out of range.
    OSVR_CLIENT_EXPORT util::NumericVersionComponent
    getVersionComponent(uint8_t componentIndex);

    /// @brief Gets all numeric components of the runtime version in a tuple.
    OSVR_CLIENT_EXPORT util::RuntimeNumericVersion getRuntimeNumericVersion();

    /// @brief Gets the Git commit hash corresponding to this library build.
    /// @returns an empty string if the commit hash could not be detected.
    OSVR_CLIENT_EXPORT const char *getCommit();

    /// @brief Gets the full descriptive version string, typically including the
    /// numerical version components and the commit hash.
    OSVR_CLIENT_EXPORT const char *getVersionString();

    /// @brief If this was built by the CI, return the "Build Tag" the CI
    /// associated with the build job. (Unrelated to git tags.)
    /// @returns empty string if we don't have a CI build tag recorded.
    OSVR_CLIENT_EXPORT const char *getCIBuildTag();

    /// @brief Does this build identify itself as a dev build, rather than a
    /// CI-build "production" build? This is not necessarily 100% accurate - for
    /// informational purposes only.
    OSVR_CLIENT_EXPORT bool isKnownDevBuild();

    /// @brief Gets the name of this library/module, or an empty string if the
    /// build system did not incorporate this information into the build.
    OSVR_CLIENT_EXPORT const char *getModuleName();
} // namespace client
} // namespace osvr

#endif // INCLUDED_GetRuntimeVersion_h_GUID_487199D9_CA87_4E85_52A2_D1BDBC119163
