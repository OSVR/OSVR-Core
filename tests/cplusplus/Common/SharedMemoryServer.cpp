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
#include <osvr/Common/SharedMemoryRingBuffer.h>

// Library/third-party includes
// - none

// Standard includes
#include <thread>
#include <chrono>
#include <iostream>

int main() {

    auto opts = osvr::common::SharedMemoryRingBuffer::Options("Test");
    auto buf = osvr::common::SharedMemoryRingBuffer::create(opts);
    if (!buf) {
        std::cout << "Couldn't create it." << std::endl;
        return 1;
    }
    std::cout << "Using backend: " << int(buf->getBackend()) << std::endl;

    std::cout << "Capacity: " << buf->getEntries() << " entries.\n";
    std::cout << "Entry size: " << buf->getEntrySize() << " bytes per entry."
              << std::endl;

    while (true) {
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::string data;
        std::getline(std::cin, data);
        auto seq =
            buf->put(reinterpret_cast<const unsigned char *>(data.data()),
                     data.length());
        std::cout << "Sequence number " << seq << std::endl;
        // auto proxy = buf->put();
    }

    return 0;
}
