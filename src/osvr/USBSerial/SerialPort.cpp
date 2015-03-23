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
#include "osvr/USBSErial/SerialPort.h"
#include <osvr/Util/PlatformConfig.h>

// Library/third-party includes
#ifdef OSVR_WINDOWS
#define _WIN32_DCOM
#include <string>
#include <comdef.h>
#include <Wbemidl.h>
#include <tchar.h>
#include <windows.h>
#include <locale>
#include <codecvt>
#endif // OSVR_WINDOWS

// Standard includes
#include <regex>

namespace osvr {
namespace usbserial {

#if defined(OSVR_WINDOWS)

    std::string getPortNumber(USBSerialDeviceImpl *serialDevice) {

        HRESULT result;
        std::string comPort;

        // initialize COM to make lib calls, otherwise we can't proceed
        result = CoInitializeEx(0, COINIT_MULTITHREADED);

        if (FAILED(result)) {
            return comPort;
        }

        // Set COM security should be done here. It's automatically
        // called by COM when interface is marshaled/unmarshaled with default
        // settings

        // Obtain the initial locator to WMI
        IWbemLocator *locator = NULL;

        result = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                                  IID_IWbemLocator, (LPVOID *)&locator);

        if (FAILED(result)) {
            CoUninitialize();
            return comPort;
        }

        // Connect to WMI through the IWbemLocator::ConnectServer method

        IWbemServices *wbemServices = NULL;

        // Connect to the root\cimv2 namespace with
        // the current user and obtain pointer pSvc
        // to make IWbemServices calls.
        result = locator->ConnectServer(
            _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
            NULL,                    // User name. NULL = current user
            NULL,                    // User password. NULL = current
            0,                       // Locale. NULL indicates current
            NULL,                    // Security flags.
            0,                       // Authority (for example, Kerberos)
            0,                       // Context object
            &wbemServices            // pointer to IWbemServices proxy
            );

        if (FAILED(result)) {
            locator->Release();
            CoUninitialize();
        }

        // Set security levels on the proxy

        result =
            CoSetProxyBlanket(wbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                              NULL, RPC_C_AUTHN_LEVEL_CALL,
                              RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

        if (FAILED(result)) {
            wbemServices->Release();
            locator->Release();
            CoUninitialize();
            return comPort;
        }

        // Use the IWbemServices pointer to make requests of WMI

        // Get a list of serial devices
        IEnumWbemClassObject *devEnum = NULL;
        result = wbemServices->ExecQuery(
            bstr_t("WQL"), bstr_t("SELECT * FROM Win32_SerialPort"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL,
            &devEnum);

        if (FAILED(result)) {
            wbemServices->Release();
            locator->Release();
            CoUninitialize();
            return comPort;
        }

        IWbemClassObject *wbemClassObj;
        ULONG numObjRet = 0;
        std::wstring_convert<std::codecvt_utf8<wchar_t>> convStr;
        while (devEnum) {
            HRESULT hr =
                devEnum->Next(WBEM_INFINITE, 1, &wbemClassObj, &numObjRet);

            if (numObjRet == 0) {
                break;
            }

            VARIANT vtPort;
            VARIANT vtHardware;

            // Get the value of the Name property
            hr = wbemClassObj->Get(L"DeviceID", 0, &vtPort, 0, 0);
            std::string sPort = convStr.to_bytes(vtPort.bstrVal);

            hr = wbemClassObj->Get(L"PNPDeviceID", 0, &vtHardware, 0, 0);
            std::string HardwID = convStr.to_bytes(vtHardware.bstrVal);

            std::string deviceVID = serialDevice->getVID();
            std::string devicePID = serialDevice->getPID();

            std::regex vidPidRegEx("VID[\\s&\\*\\#_]?" + deviceVID +
                                   "[\\s&\\*\\#_]?PID[\\s&\\*\\#_]?" +
                                   devicePID);

            // found a match
            if (std::regex_search(HardwID, vidPidRegEx)) {
                comPort = sPort;
            }

            VariantClear(&vtPort);
            VariantClear(&vtHardware);

            wbemClassObj->Release();
        }

        // release Resources

        wbemServices->Release();
        locator->Release();
        devEnum->Release();
        if (!wbemClassObj)
            wbemClassObj->Release();
        CoUninitialize();

        return comPort;
    }

#else
#error "getBinaryLocation() not yet implemented for this platform!"
    std::string getPortNumber(USBSerialDeviceImpl *serialDevice) {}

#endif

} // namespace usbserial
} // namespace osvr
