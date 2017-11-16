/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_CheckFirmwareVersion_h_GUID_C1356250_0CF2_4595_83A5_1522F8AC6795
#define INCLUDED_CheckFirmwareVersion_h_GUID_C1356250_0CF2_4595_83A5_1522F8AC6795

// Internal Includes
// - none

// Library/third-party includes
#include <boost/assert.hpp>
#ifdef _WIN32
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#define _WIN32_DCOM
#include <Wbemidl.h>
#include <comdef.h>
#include <comutils/ComPtr.h>
#include <comutils/ComVariant.h>
#include <tchar.h>

#ifdef OSVR_HAVE_CODECVT
#include <codecvt>
#include <locale>
#else // !OSVR_HAVE_CODECVT
#include <boost/locale/encoding_utf.hpp>
#endif // OSVR_HAVE_CODECVT
#endif // _WIN32

// Standard includes
#include <iostream>

namespace osvr {
namespace vbtracker {
    static const auto CAMERA_FIRMWARE_UPGRADE_REQUIRED = 6;
    static const auto CURRENT_CAMERA_FIRMWARE_VERSION = 7;

    template <typename T> inline std::string wideToUTF8String(T input) {
#ifdef OSVR_HAVE_CODECVT
        std::wstring_convert<std::codecvt_utf8<wchar_t> > strCvt;
        return strCvt.to_bytes(input);
#else  // !OSVR_HAVE_CODECVT
        return boost::locale::conv::utf_to_utf<char>(input);
#endif // OSVR_HAVE_CODECVT
    }

    template <typename T> inline std::wstring narrowToWideString(T input) {
#ifdef OSVR_HAVE_CODECVT
        std::wstring_convert<std::codecvt_utf8<wchar_t> > strCvt;
        return strCvt.from_bytes(input);
#else  // !OSVR_HAVE_CODECVT
        return boost::locale::conv::utf_to_utf<wchar_t>(input);
#endif // OSVR_HAVE_CODECVT
    }

    enum class FirmwareStatus {
        Good,
        Future,
        UpgradeUseful,
        UpgradeRequired,
        Unknown
    };

#ifdef _WIN32
    inline FirmwareStatus checkCameraFirmwareRevision(std::string path) {

        FirmwareStatus ret = FirmwareStatus::Unknown;
        // Remove the end of the path: #{GUID}\global
        {
            auto endOfPath = path.find("#{");
            if (endOfPath != std::string::npos) {
                path.erase(endOfPath);
            }
        }
        // Remove the system prefix thing
        {
            static const auto prefixString = "\\\\?\\";
            static const auto prefixLength = 4;
            BOOST_ASSERT(std::strlen(prefixString) == prefixLength);

            if (boost::algorithm::starts_with(path, prefixString)) {
                boost::algorithm::erase_head(path, prefixLength);
            }
        }
        // Convert # to \\

        boost::algorithm::ireplace_all(path, "#", "\\\\");
        std::wstring wpath = narrowToWideString(path);

        /// OK, now path should be sanitized for use in the query.

        // Set COM security should be done here. It's automatically
        // called by COM when interface is marshaled/unmarshaled with default
        // settings

        // Obtain the initial locator to WMI
        comutils::Ptr<IWbemLocator> locator;
        auto result =
            CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER,
                             IID_IWbemLocator, AttachPtr(locator));
        if (FAILED(result)) {
            return ret;
        }

        // Connect to WMI through the IWbemLocator::ConnectServer method

        comutils::Ptr<IWbemServices> wbemServices;

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
            return ret;
        }

        // Set security levels on the proxy

        result =
            CoSetProxyBlanket(wbemServices.get(), RPC_C_AUTHN_WINNT,
                              RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL,
                              RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);

        if (FAILED(result)) {
            return ret;
        }

        // Use the IWbemServices pointer to make requests of WMI

        std::wstring query =
            L"SELECT * FROM Win32_PnPEntity WHERE DeviceID=\"" + wpath + L"\"";
        // Get a list of serial devices
        comutils::Ptr<IEnumWbemClassObject> devEnum;
        result = wbemServices->ExecQuery(bstr_t(L"WQL"), bstr_t(query.c_str()),
                                         WBEM_FLAG_FORWARD_ONLY |
                                             WBEM_FLAG_RETURN_IMMEDIATELY,
                                         nullptr, AttachPtr(devEnum));
        if (FAILED(result)) {
            std::cout << "Query failed!" << std::endl;
            return ret;
        }
        comutils::Ptr<IWbemClassObject> wbemClassObj;
        ULONG numObjRet = 0;
        while (devEnum) {
            HRESULT hr = devEnum->Next(WBEM_INFINITE, 1,
                                       AttachPtr(wbemClassObj), &numObjRet);

            if (FAILED(hr) || numObjRet == 0) {
                break;
            }

            using comutils::Variant;
            using comutils::get;
            using comutils::containsArray;
            using comutils::getArray;

            // This is an array of hardware IDs - one of them should have
            // REV_xxxx
            Variant varHardwareID;
            hr = wbemClassObj->Get(L"HardwareID", 0,
                                   AttachVariant(varHardwareID), nullptr,
                                   nullptr);
            if (!containsArray<std::wstring>(varHardwareID)) {
#if 0
                std::cout << "HardwareID doesn't contain a string array! VT is "
                          << varHardwareID.get().vt << std::endl;
#endif
                break;
            }

            for (auto &&hwid : getArray<std::wstring>(varHardwareID)) {
                auto narrow = wideToUTF8String(hwid);
                // std::cout << "hwid: " << narrow << std::endl;
                auto loc = narrow.find("REV_");
                if (loc != std::string::npos) {
                    auto revString = narrow.substr(loc);
                    // remove REV_
                    boost::algorithm::erase_head(revString, 4);
                    // Keep the four digit revision.
                    revString.resize(4);
                    std::istringstream iss(revString);
                    unsigned rev = 0;
                    if ((iss >> rev)) {
                        // OK, we could parse it as a number
                        if (rev < CAMERA_FIRMWARE_UPGRADE_REQUIRED) {
                            ret = FirmwareStatus::UpgradeRequired;

                        } else if (rev < CURRENT_CAMERA_FIRMWARE_VERSION) {
                            ret = FirmwareStatus::UpgradeUseful;
                        } else if (rev > CURRENT_CAMERA_FIRMWARE_VERSION) {
                            ret = FirmwareStatus::Future;
                            std::cout
                                << "\n[Video-based Tracking] Detected camera "
                                   "firmware version "
                                << rev << std::endl;
                        } else {
                            // they're on the current version.
                            ret = FirmwareStatus::Good;
                        }
                        return ret;
                    }
                    // Hmm, couldn't parse as a number, weird.
                }
            }
        }
        return ret;
    }
#endif
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_CheckFirmwareVersion_h_GUID_C1356250_0CF2_4595_83A5_1522F8AC6795
