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
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#define _WIN32_DCOM
#include <string>
#include <comdef.h>
#include <Wbemidl.h>
#include <tchar.h>
#include <windows.h>

#include <comutils/ComVariant.h>
#include <intrusive_ptr_COM.h>

#include <boost/intrusive_ptr.hpp>
#include <boost/noncopyable.hpp>

// Standard includes
#include <regex>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm> // for std::transform
#include <string>

namespace osvr {
namespace usbserial {

    namespace {
        /// @brief RAII wrapper for initializing/uninitializing COM.
        class ComRAII : boost::noncopyable {
          public:
            ComRAII() {
                auto result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
                if (FAILED(result)) {
                    m_failed = true;
                }
            }
            ~ComRAII() {
                // Must call, even if it failed.
                CoUninitialize();
            }
            bool failed() const { return m_failed; }

          private:
            bool m_failed = false;
        };

        /// Class to contain the workings of extracting VID and PID from a
        /// hardware ID, which are similar and repetitive.
        ///
        /// Can be (intended to be) used for multiple hardware ID strings.
        class USBVidPidExtractor {
          public:
            USBVidPidExtractor()
                : m_vidPidRegex(
                      "USB.VID_([[:xdigit:]]{4})&PID_([[:xdigit:]]{4})") {}

            boost::optional<std::pair<uint16_t, uint16_t>>
            getVidPid(std::string const &hardwareId) {
                std::smatch m;
                if (!std::regex_search(hardwareId, m, m_vidPidRegex)) {
                    // Couldn't find it - that's weird.
                    // Might not be a USB serial port (could be a "regular" one)
                    return boost::none;
                }
                // first submatch is vid, second submatch is pid.
                return std::make_pair(m_hexToInt(m.str(1)),
                                      m_hexToInt(m.str(2)));
            }

          private:
            /// Convert hex, such as that found in a VID:PID string, to the
            /// corresponding 16-bit unsigned int.
            uint16_t m_hexToInt(std::string const &str) {
                m_ss.clear();
                uint16_t ret;
                m_ss << std::hex << str;
                m_ss >> ret;
                return ret;
            }

            /// @name Regex to extract the hex VID and PID from a string.
            std::regex m_vidPidRegex;

            /// String stream used to convert the hex string to an int.
            std::stringstream m_ss;
        };
    } // namespace

    inline std::string
    getPNPDeviceIdSearchString(boost::optional<uint16_t> const &vendorID,
                               boost::optional<uint16_t> const &productID) {
        /// The "haystack" these will be used to search through looks like this:
        /// USB\VID_1532&PID_0B00&MI_00\...
        std::ostringstream searchTerms;
        if (vendorID) {
            searchTerms << "VID_" << std::setfill('0') << std::setw(4)
                        << std::hex << *vendorID;
        }
        if (vendorID && productID) {
            searchTerms << "&";
        }
        if (productID) {
            searchTerms << "PID_" << std::setfill('0') << std::setw(4)
                        << std::hex << *productID;
        }
        // Convert to all upper-case, in place.
        std::string ret{searchTerms.str()};
        std::transform(begin(ret), end(ret), begin(ret),
                       [](char c) { return ::toupper(c); });

        return ret;
    }

    std::vector<USBSerialDevice>
    getSerialDeviceList(boost::optional<uint16_t> vendorID,
                        boost::optional<uint16_t> productID) {
        using boost::intrusive_ptr;

        auto searchString = getPNPDeviceIdSearchString(vendorID, productID);

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

        result =
            CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
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
            nullptr,                        // User name. nullptr = current user
            nullptr,                        // User password. nullptr = current
            nullptr,                        // Locale. nullptr indicates current
            WBEM_FLAG_CONNECT_USE_MAX_WAIT, // Security flags - here, requesting
                                            // a timeout.
            0,                              // Authority (for example, Kerberos)
            nullptr,                        // Context object
            AttachPtr(wbemServices)         // pointer to IWbemServices proxy
            );

        if (FAILED(result)) {
            return devices;
        }

        // Set security levels on the proxy

        result =
            CoSetProxyBlanket(wbemServices.get(), RPC_C_AUTHN_WINNT,
                              RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL,
                              RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

        if (FAILED(result)) {
            return devices;
        }

        // Use the IWbemServices pointer to make requests of WMI

        // Get a list of serial devices
        intrusive_ptr<IEnumWbemClassObject> devEnum;
        result = wbemServices->ExecQuery(
            bstr_t(L"WQL"), bstr_t(L"SELECT * FROM Win32_SerialPort"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr,
            AttachPtr(devEnum));

        if (FAILED(result)) {
            return devices;
        }

        USBVidPidExtractor vidPidExtractor;

        intrusive_ptr<IWbemClassObject> wbemClassObj;
        ULONG numObjRet = 0;
        while (devEnum) {
            HRESULT hr = devEnum->Next(WBEM_INFINITE, 1,
                                       AttachPtr(wbemClassObj), &numObjRet);

            if (FAILED(hr) || numObjRet == 0) {
                break;
            }

            using comutils::Variant;
            using comutils::get;

            /// DeviceID is the COM port name
            Variant varPort;
            hr = wbemClassObj->Get(L"DeviceID", 0, AttachVariant(varPort),
                                   nullptr, nullptr);
            std::string port =
                util::wideToUTF8String(get<std::wstring>(varPort));

            /// PNPDeviceID is the hardware instance ID that has the VID:PID
            /// embedded in it.
            Variant varHardwareID;
            hr = wbemClassObj->Get(L"PNPDeviceID", 0,
                                   AttachVariant(varHardwareID), nullptr,
                                   nullptr);
            std::string hardwareID =
                util::wideToUTF8String(get<std::wstring>(varHardwareID));

            if (!searchString.empty()) {
                if (hardwareID.find(searchString) == std::string::npos) {
                    /// We had something to search for, and we didn't find it,
                    /// go on.
                    continue;
                }
            }

            auto vidPid = vidPidExtractor.getVidPid(hardwareID);
            if (!vidPid) {
                /// Couldn't parse the VID/PID - may not be a USB serial port.
                continue;
            }

            /// On Windows (NT-based), the "full path" to a serial port with a
            /// COM port name is \\.\COMx (where x is a number)
            std::string path = "\\\\.\\" + port;

            devices.emplace_back(vidPid->first, vidPid->second, path, port);
        }

        return devices;
    }

} // namespace usbserial
} // namespace osvr
