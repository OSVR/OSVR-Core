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

#ifndef INCLUDED_USBSerialEnum_h_GUID_B17561E7_ADC2_438C_87B9_D05321AF6BD6
#define INCLUDED_USBSerialEnum_h_GUID_B17561E7_ADC2_438C_87B9_D05321AF6BD6


// Internal Includes
#include <osvr/USBSerial/Export.h>

// Library/third-party includes
// - none

// Standard includes
#include <memory>
#include <string>

namespace osvr {
namespace usbserial {
	class Enumerator;
	class EnumeratedDevice {
	public:
		std::string getPlatformSpecificPath();
		uint16_t getVID();
		uint16_t getPID();
	private:
		class Impl;
		EnumeratedDevice(std::unique_ptr<Impl> impl);
	};
	class EnumeratorIterator {
		/// stuff
	};
	class Enumerator {
		class Impl;
	public:
		OSVR_USBSERIAL_EXPORT Enumerator();
		OSVR_USBSERIAL_EXPORT ~Enumerator();

		OSVR_USBSERIAL_EXPORT EnumeratorIterator begin();
		OSVR_USBSERIAL_EXPORT EnumeratorIterator end();
	private:
		std::unique_ptr<Impl> m_impl;
	};

} // namespace usbserial
} // namespace osvr
#endif // INCLUDED_USBSerialEnum_h_GUID_B17561E7_ADC2_438C_87B9_D05321AF6BD6