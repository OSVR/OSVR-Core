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
#include <osvr/Common/IPCRingBuffer.h>

// Library/third-party includes
// - none

// Standard includes
#include <thread>
#include <chrono>
#include <iostream>

int main() {
    auto opts = osvr::common::IPCRingBuffer::Options("Test");
    auto buf = osvr::common::IPCRingBuffer::find(opts);
    if (!buf) {
        std::cout << "Couldn't find it." << std::endl;
        return 1;
    }

    std::cout << "Capacity: " << buf->getEntries() << " entries.\n";
    std::cout << "Entry size: " << buf->getEntrySize() << " bytes per entry."
              << std::endl;

    osvr::common::IPCRingBuffer::sequence_type seq(0);
    while (true) {
        std::cin.ignore();
        std::cout << "Sequence number " << seq << ": ";
        auto res = buf->get(seq);
        if (res) {
            std::cout << (res.get())[0] << (res.get())[1] << (res.get())[2];
        } else {
            std::cout << "Not available";
        }
        std::cout << std::endl;
        seq++;
    }

    return 0;
}
