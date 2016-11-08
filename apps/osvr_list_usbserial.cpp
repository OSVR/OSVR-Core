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
#include <osvr/USBSerial/USBSerialDevice.h>
#include <osvr/USBSerial/USBSerialEnum.h>

// Library/third-party includes
// - none

// Standard includes
#include <cstdint>
#include <cstdlib> // for EXIT_SUCCESS
#include <functional>
#include <iomanip>  // for setfill, setw
#include <ios>      // for hex
#include <iostream> // for cout
#include <sstream>

/// Returns a hex value always padded out to 4 digits.
std::string padHex(std::uint16_t v) {
    std::ostringstream os;
    os << std::setfill('0') << std::setw(4) << std::hex << v;
    return os.str();
}

int main(int argc, char *argv[]) {
    using namespace std;

    function<osvr::usbserial::Enumerator()> enumerate = [] {
        return osvr::usbserial::enumerate();
    };

    if (argc == 3) {

        uint16_t vID;
        {
            stringstream ss;
            ss << hex << argv[1];
            if (!(ss >> vID)) {
                cerr << "Could not parse first command line argument as a "
                        "hex vendor ID!"
                     << endl;
                return -1;
            }
        }
        uint16_t pID;
        {
            stringstream ss;
            ss << hex << argv[2];
            if (!(ss >> pID)) {
                cerr << "Could not parse second command line argument as "
                        "a hex product ID!"
                     << endl;
                return -1;
            }
        }
        cout << "Will enumerate USB Serial devices with hex VID:PID "
             << padHex(vID) << ":" << padHex(pID) << "\n"
             << endl;
        enumerate = [vID, pID] { return osvr::usbserial::enumerate(vID, pID); };

    } else if (argc != 1) {
        cerr << "Usage: either pass no arguments to enumerate all USB "
                "Serial devices, or pass two arguments: a hex VID and PID "
                "to search for."
             << endl;
        return -1;

    } else {
        cout << "Will enumerate all USB Serial devices.\n" << endl;
    }

    for (auto &&dev : enumerate()) {
        cout << setfill('0') << setw(4) << hex << dev.getVID() << ":"
             << setfill('0') << setw(4) << hex << dev.getPID() << " "
             << dev.getPlatformSpecificPath() << endl;
    }

    return EXIT_SUCCESS;
}
