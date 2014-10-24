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

#ifndef INCLUDED_RegistrationContext_h_GUID_9FF83E64_B2E1_40B7_E072_929AC0F94A10
#define INCLUDED_RegistrationContext_h_GUID_9FF83E64_B2E1_40B7_E072_929AC0F94A10

// Internal Includes
#include <ogvr/Util/SharedPtr.h>
#include <ogvr/PluginKit/Export.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <vector>

namespace ogvr {
class PluginSpecificRegistrationContext;
/// @brief Internal class responsible for the registration and destruction of
/// plugins.
class RegistrationContext : boost::noncopyable {
  public:
    /// @brief basic constructor
    OGVR_PLUGINKIT_EXPORT RegistrationContext();

    /// @brief Destructor responsible for destroying plugins in reverse order.
    OGVR_PLUGINKIT_EXPORT ~RegistrationContext();

    /// @name Host-side (internal) API
    /// @{
    /// @brief load a plugin in this context
    OGVR_PLUGINKIT_EXPORT void loadPlugin(std::string const &pluginName);

    /// @}

  private:
    /// @brief Pointer with ownership semantics for cleanup of plugins.
    /// @todo why did unique_ptr not work here?
    typedef shared_ptr<PluginSpecificRegistrationContext> PluginRegPtr;
    /// @brief List of owning pointers for plugin registration.
    typedef std::vector<PluginRegPtr> PluginRegList;

    PluginRegList m_regList;
};
} // end of namespace ogvr

#endif // INCLUDED_RegistrationContext_h_GUID_9FF83E64_B2E1_40B7_E072_929AC0F94A10
