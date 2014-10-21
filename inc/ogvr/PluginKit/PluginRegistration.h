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

#ifndef INCLUDED_PluginRegistration_h_GUID_4F5D6422_2977_40A9_8BA0_F86FD6245CE9
#define INCLUDED_PluginRegistration_h_GUID_4F5D6422_2977_40A9_8BA0_F86FD6245CE9

// Internal Includes
#include <ogvr/PluginKit/Export.h>
#include <ogvr/Util/UniquePtr.h>

// Library/third-party includes
// - none

// Standard includes
#include <cstddef>
#include <string>

namespace ogvr {
class PluginRegistrationContext_impl;
class PluginRegistrationContext {
  public:
    PluginRegistrationContext();
    ~PluginRegistrationContext();
    void PluginRegisterContents(std::string const &contents);
    void PluginRegisterContents(const char *contents);
    /// @overload
    /// Provides template-driven automatic handling of the size of string
    /// literals.
    template <std::size_t LEN>
    void PluginRegisterContents(const char(&contents)[LEN]) {
        PluginRegisterContents(std::string(contents, LEN));
    }

  private:
    unique_ptr<PluginRegistrationContext_impl> m_impl;
};

} // end of namespace ogvr

#endif // INCLUDED_PluginRegistration_h_GUID_4F5D6422_2977_40A9_8BA0_F86FD6245CE9
