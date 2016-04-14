/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com>

*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_LogSinks_h_GUID_C582966D_EA04_42D2_83FF_19483537D704
#define INCLUDED_LogSinks_h_GUID_C582966D_EA04_42D2_83FF_19483537D704

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <mutex>
#include <iostream>

namespace spdlog {
namespace sinks {

    template <typename Mutex> class ostream_sink;

} // end namespace sinks

namespace details {

    struct null_mutex;

} // end namespace details
} // end namespace spdlog

namespace osvr {
namespace util {
namespace log {

    /**
     * @brief A sink which sends its output to std::cout. This sink is a drop-in
     * replacement for spdlog::stdout_sink and differs only in that this sink isn't
     * a singleton.
     */
    template <typename Mutex>
    class stdout_sink : public ::spdlog::sinks::ostream_sink<Mutex> {
    public:
        stdout_sink() : ::spdlog::sinks::ostream_sink<Mutex>(std::cout, true)
        {
            // do nothing
        }
    };

    typedef stdout_sink<::spdlog::details::null_mutex> stdout_sink_st;
    typedef stdout_sink<std::mutex> stdout_sink_mt;


    /**
     * @brief A sink which sends its output to std::cerr. This sink is a drop-in
     * replacement for spdlog::stderr_sink and differs only in that this sink isn't
     * a singleton.
     */
    template <typename Mutex>
    class stderr_sink : public ::spdlog::sinks::ostream_sink<Mutex> {
    public:
        stderr_sink() : ::spdlog::sinks::ostream_sink<Mutex>(std::cerr, true)
        {
            // do nothing
        }
    };

    typedef stderr_sink<std::mutex> stderr_sink_mt;
    typedef stderr_sink<::spdlog::details::null_mutex> stderr_sink_st;

} // end namespace log
} // end namespace util
} // end namespace osvr

#endif // INCLUDED_LogSinks_h_GUID_C582966D_EA04_42D2_83FF_19483537D704

