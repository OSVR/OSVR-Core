/** @file
    @brief A simple application that lists all detected USB serial devices.

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com>

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
#include <osvr/USBSerial/USBSerialEnum.h>
#include <osvr/USBSerial/USBSerialDevice.h>

// Library/third-party includes
// - none

// Standard includes
#include <cstdlib>  // for EXIT_SUCCESS
#include <iostream> // for cout
#include <ios>      // for hex
#include <iomanip>  // for setfill, setw

int main(int argc, char *argv[]) {
    using namespace std;
    for (auto &&dev : osvr::usbserial::enumerate()) {
        cout << setfill('0') << setw(4) << hex << dev.getVID() << ":"
             << setfill('0') << setw(4) << hex << dev.getPID() << " "
             << dev.getPlatformSpecificPath() << endl;
    }

    return EXIT_SUCCESS;
}
