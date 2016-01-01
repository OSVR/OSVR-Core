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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Util/CSV.h>
#include <osvr/ClientKit/Context.h>
#include <osvr/ClientKit/Interface.h>
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

osvr::util::CSV g_csvOutput;

static const auto MAX_ROWS = 1000;
static std::size_t g_markRows = 0;
static const auto MAX_SECONDS = 10;

static const auto OUTFILE = "osvrdata.csv";

using our_clock = std::chrono::system_clock;

using osvr::util::cell;

template <typename T> inline bool shouldStop(T const &deadline) {
    return (g_csvOutput.numDataRows() - g_markRows) > MAX_ROWS ||
           our_clock::now() > deadline;
}

inline osvr::util::CSV::RowProxy &&
operator<<(osvr::util::CSV::RowProxy &&row,
           osvr::util::time::TimeValue const &ts) {
    return std::move(row) << osvr::util::cell("ts:seconds", ts.seconds)
                          << osvr::util::cell("ts:microseconds",
                                              ts.microseconds);
}

static void poseCallback(void *userdata, const OSVR_TimeValue *timestamp,
                         const OSVR_PoseReport *report) {
    auto path = static_cast<char *>(userdata);
    g_csvOutput.row()
        << (*timestamp)
        << cell(path + std::string{":x"}, report->pose.translation.data[0])
        << cell(path + std::string{":y"}, report->pose.translation.data[1])
        << cell(path + std::string{":z"}, report->pose.translation.data[2])
        << cell(path + std::string{":qx"},
                osvrQuatGetX(&(report->pose.rotation)))
        << cell(path + std::string{":qy"},
                osvrQuatGetY(&(report->pose.rotation)))
        << cell(path + std::string{":qz"},
                osvrQuatGetZ(&(report->pose.rotation)))
        << cell(path + std::string{":qw"},
                osvrQuatGetW(&(report->pose.rotation)));
}

int main(int argc, char *argv[]) {
    osvr::clientkit::ClientContext context("org.osvr.tools.logtocsv");

    for (int i = 1; i < argc; ++i) {
        auto path = argv[i];
        std::cerr << "Setting up data output for " << path << std::endl;
        auto resource = context.getInterface(path);
        resource.registerCallback(&poseCallback, path);
        // will just let the context free them on exit.
    }

    if (!context.checkStatus()) {
        std::cerr << "Client context has not yet started up - waiting. Make "
                     "sure the server is running."
                  << std::endl;
        do {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            context.update();
        } while (!context.checkStatus());
        std::cerr << "OK, client context ready. Proceeding." << std::endl;
    }
    std::cerr << "Will exit after " << MAX_ROWS << " rows of data or "
              << MAX_SECONDS << " seconds of runtime, whichever comes first."
              << std::endl;

    auto begin = our_clock::now();
    auto runTimeLimit = begin + std::chrono::seconds(MAX_SECONDS);
    do {
        context.update();
    } while (!shouldStop(runTimeLimit));
    /// Client context closed by now, just output the file.
    std::cerr << "Writing " << g_csvOutput.numDataRows() << " data rows to "
              << OUTFILE << std::endl;
    {
        std::ofstream outfile(OUTFILE, std::ios::out | std::ios::binary);
        g_csvOutput.output(outfile);
        outfile.close();
    }
    std::cerr << "Done!" << std::endl;
    return 0;
}
