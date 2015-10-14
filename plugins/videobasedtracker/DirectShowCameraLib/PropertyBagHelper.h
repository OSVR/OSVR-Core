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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_PropertyBagHelper_h_GUID_BAA9AA5B_2879_4CA0_04F3_9B90F3BAC871
#define INCLUDED_PropertyBagHelper_h_GUID_BAA9AA5B_2879_4CA0_04F3_9B90F3BAC871

// Internal Includes
#include "comutils/ComPtr.h"
#include "comutils/ComVariant.h"

// Library/third-party includes
#include <ObjIdl.h> // for IMonkider
#include <OAIdl.h>  // for IPropertyBag, IID_IPropertyBag

// Standard includes
#include <string>

///  Assists in retrieving string properties from a property bag associated with
///  a moniker.
class PropertyBagHelper {
  public:
    explicit PropertyBagHelper(IMoniker &mon) {
        mon.BindToStorage(nullptr, nullptr, IID_IPropertyBag,
                          AttachPtr(m_propBag));
    }

    /// Checks for validity
    explicit operator bool() const { return bool(m_propBag); }

    /// Accessor for underlying object reference.
    IPropertyBag &getPropertyBag() { return *m_propBag; }

    /// Const accessor for underlying object reference.
    IPropertyBag const &getPropertyBag() const { return *m_propBag; }

    /// Reads a (wide-string) property, returning an empty string if failed.
    std::wstring read(const wchar_t propName[]) const {
        using namespace comutils::variant;

        auto ret = std::wstring{};
        auto val = Variant{};
        m_propBag->Read(propName, AttachVariant(val), nullptr);
        if (contains<std::wstring>(val)) {
            ret = get<std::wstring>(val);
        }
        return ret;
    }

  private:
    comutils::Ptr<IPropertyBag> m_propBag;
};
#endif // INCLUDED_PropertyBagHelper_h_GUID_BAA9AA5B_2879_4CA0_04F3_9B90F3BAC871
