/** @file
    @brief Implementation

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

// Internal Includes
#include "USBSerialDevInfo.h"
#include <osvr/Util/WideToUTF8.h>

// Library/third-party includes
#define _WIN32_DCOM
#include <string>
#include <comdef.h>
#include <Wbemidl.h>
#include <tchar.h>
#include <windows.h>
#include "intrusive_ptr_COM.h"
#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>

// Standard includes
#include <regex>
#include <iostream>

namespace osvr {
namespace usbserial {

    namespace {
        /// @brief RAII wrapper for initializing/uninitializing COM.
        class ComRAII : boost::noncopyable {
          public:
            ComRAII() : m_failed(false) {
                auto result = CoInitializeEx(0, COINIT_MULTITHREADED);
                if (FAILED(result)) {
                    m_failed = true;
                }
            }
            ~ComRAII() {
                if (!m_failed) {
                    CoUninitialize();
                }
            }
            bool failed() const { return m_failed; }

          private:
            bool m_failed;
        };
    } // namespace

    std::vector<USBSerialDevice>
    getSerialDeviceList(boost::optional<uint16_t> vendorID,
                        boost::optional<uint16_t> productID) {
        using boost::intrusive_ptr;

        HRESULT result;
        std::vector<USBSerialDevice> devices;

        // initialize COM to make lib calls, otherwise we can't proceed
        ComRAII comSetup;
        if (comSetup.failed()) {
            return devices;
        }

        // Set COM security should be done here. It's automatically
        // called by COM when interface is marshaled/unmarshaled with default
        // settings

        // Obtain the initial locator to WMI
        intrusive_ptr<IWbemLocator> locator;

        result = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                                  IID_IWbemLocator, AttachPtr(locator));

        if (FAILED(result)) {
            return devices;
        }

        // Connect to WMI through the IWbemLocator::ConnectServer method

        intrusive_ptr<IWbemServices> wbemServices;

        // Connect to the root\cimv2 namespace with
        // the current user and obtain pointer pSvc
        // to make IWbemServices calls.
        result = locator->ConnectServer(
            bstr_t(L"ROOT\\CIMV2"),         // Object path of WMI namespace
            NULL,                           // User name. NULL = current user
            NULL,                           // User password. NULL = current
            0,                              // Locale. NULL indicates current
            WBEM_FLAG_CONNECT_USE_MAX_WAIT, // Security flags - here, requesting
                                            // a timeout.
            0,                              // Authority (for example, Kerberos)
            0,                              // Context object
            AttachPtr(wbemServices)         // pointer to IWbemServices proxy
            );

        if (FAILED(result)) {
            return devices;
        }

        // Set security levels on the proxy

        result =
            CoSetProxyBlanket(wbemServices.get(), RPC_C_AUTHN_WINNT,
                              RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL,
                              RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

        if (FAILED(result)) {
            return devices;
        }

        // Use the IWbemServices pointer to make requests of WMI

        // Get a list of serial devices
        intrusive_ptr<IEnumWbemClassObject> devEnum;
        result = wbemServices->ExecQuery(
            bstr_t(L"WQL"), bstr_t(L"SELECT * FROM Win32_SerialPort"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL,
            AttachPtr(devEnum));

        if (FAILED(result)) {
            return devices;
        }

        intrusive_ptr<IWbemClassObject> wbemClassObj;
        ULONG numObjRet = 0;
        while (devEnum) {
            HRESULT hr = devEnum->Next(WBEM_INFINITE, 1,
                                       AttachPtr(wbemClassObj), &numObjRet);

            if (FAILED(hr) || numObjRet == 0) {
                break;
            }

            VARIANT vtPort;
            VARIANT vtHardware;
            VARIANT vtPath;

            // Get the value of the Name property
            hr = wbemClassObj->Get(L"DeviceID", 0, &vtPort, 0, 0);
            std::string sPort = util::wideToUTF8String(vtPort.bstrVal);

            hr = wbemClassObj->Get(L"PNPDeviceID", 0, &vtHardware, 0, 0);
            std::string HardwID = util::wideToUTF8String(vtHardware.bstrVal);

            hr = wbemClassObj->Get(L"__PATH", 0, &vtPath, 0, 0);
            std::string devPath = util::wideToUTF8String(vtPath.bstrVal);

            if ((vendorID) && (productID)) {

                std::string deviceVID = std::to_string(*vendorID);
                std::string devicePID = std::to_string(*productID);

                std::regex vidPidRegEx("VID[\\s&\\*\\#_]?" + deviceVID +
                                       "[\\s&\\*\\#_]?PID[\\s&\\*\\#_]?" +
                                       devicePID);

                // found a match
                if (std::regex_search(HardwID, vidPidRegEx)) {

                    USBSerialDevice newDevice(*vendorID, *productID, devPath,
                                              sPort);
                    devices.push_back(newDevice);
                }
            } else {
                USBSerialDevice newDevice(*vendorID, *productID, devPath,
                                          sPort);
                devices.push_back(newDevice);
            }

            VariantClear(&vtPort);
            VariantClear(&vtHardware);
            VariantClear(&vtPath);
        }

        return devices;
    }

} // namespace usbserial
} // namespace osvr
