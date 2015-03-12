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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "ClientMainloopThread.h"
#include "RecomposeTransform.h"
#include "WrapRoute.h"
#include <osvr/ClientKit/ClientKit.h>
#include <osvr/Client/ClientContext.h>
#include <osvr/ClientKit/InterfaceStateC.h>
#include <osvr/Common/RouteContainer.h>
#include <osvr/Util/EigenInterop.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <iostream>
#include <fstream>
#include <exception>

using std::cout;
using std::cerr;
using std::endl;

auto SETTLE_TIME = boost::posix_time::seconds(2);

static const double minAbsRadians = 1.5;
static const int totalDots = 10;

inline Eigen::Quaterniond
getOrientationState(osvr::clientkit::Interface &iface) {
    OSVR_OrientationState state;
    OSVR_TimeValue timestamp;
    OSVR_ReturnCode ret;
    ret = osvrGetOrientationState(iface.get(), &timestamp, &state);
    if (ret != OSVR_RETURN_SUCCESS) {
        cerr << "Sorry, no orientation state available for this route - "
                "are you sure you have a device plugged in and your "
                "path correct?" << endl;
        std::cin.ignore();
        throw std::runtime_error("No orientation state available");
    }
    return osvr::util::fromQuat(state);
}

Eigen::AngleAxisd getRotation(const char name[], ClientMainloopThread &client,
                              osvr::clientkit::Interface &iface) {
    cout << "\n\n--- " << name << "---" << endl;
    cout << "Current rotation: " << name << " - please see the image file for "
                                            "desired axis and direction of "
                                            "rotation." << endl;
    cout << "Once you're ready, put your device in a level, forward-looking "
            "orientation, then press enter." << endl;
    std::cin.ignore();
    Eigen::Quaterniond initial;
    Eigen::Quaterniond current;
    {
        ClientMainloopThread::lock_type lock(client.getMutex());
        initial = getOrientationState(iface);
        cout << "Initial axes:" << endl;
        cout << " - sensor X points "
             << (initial * Eigen::Vector3d::UnitX()).transpose() << endl;
        cout << " - sensor Y points "
             << (initial * Eigen::Vector3d::UnitY()).transpose() << endl;
        cout << " - sensor Z points "
             << (initial * Eigen::Vector3d::UnitZ()).transpose() << endl;
        cout << endl;
        cout << "Now, please rotate the device as shown in the image. " << endl;
        cout << "Keep rotating";
        double absRads = 0;
        int dots = 0;
        do {
            client.oneLoop();
            current = getOrientationState(iface);
            absRads = current.angularDistance(initial);
            int currentDots = (absRads / minAbsRadians) * totalDots;
            while (dots < currentDots) {
                cout << ".";
                dots++;
            }
        } while (absRads < minAbsRadians);
    }
    cout << "  OK!" << endl;
    auto relative = initial.conjugate() * current;
    auto angleAxis = Eigen::AngleAxisd(relative);
    cout << " - " << name << " - Rotated " << angleAxis.angle()
         << " radians about " << angleAxis.axis().transpose() << "\n" << endl;
    return angleAxis;
}

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;
    // clang-format off
    po::options_description desc("Options");
    desc.add_options()
        ("help", "produce help message")
        ("route", po::value<std::string>()->default_value("/me/head"), "route to diagnose")
        ("reset", "temporarily remove all transforms before performing diagnosis")
        ;
    // clang-format on

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    /// Deal with command line errors or requests for help
    if (vm.count("help")) {
        cout << "Usage: diagnose-rotation [options]" << endl;
        cout << desc << "\n";
        return 1;
    }

    osvr::clientkit::ClientContext ctx("com.osvr.bundled.diagnose-rotation");
    std::string const dest = vm["route"].as<std::string>();

    bool resetTransform = (vm.count("reset") != 0);

    osvr::clientkit::Interface iface = ctx.getInterface(dest);
    {

        ClientMainloopThread client(ctx);

        std::string origRouteString;
        if (resetTransform) {
            cout << "Running client mainloop briefly to get routes..." << endl;
            client.loopForDuration(boost::chrono::seconds(2));

            cout << "Removing any existing transforms..." << endl;
            Json::Value origRoute;
            origRouteString =
                ctx.get()->getRoutes().getRouteForDestination(dest);
            Json::Reader reader;
            if (!reader.parse(origRouteString, origRoute)) {
                cerr << "Error parsing existing route!" << endl;
                cerr << origRouteString << endl;
                return -1;
            }
            Json::Value origTransforms =
                origRoute[osvr::common::routing_keys::source()];
            /// Remove all transforms
            Json::Value cleanTransforms = remove_levels_if(
                origTransforms, [](Json::Value const &) { return true; });
            origRoute[osvr::common::routing_keys::source()] = cleanTransforms;
            ctx.get()->sendRoute(origRoute.toStyledString());

            cout << "Sent cleaned route: " << origRoute.toStyledString() << endl;
        }
        cout << "Running thread and waiting a few seconds "
                "for startup..." << endl;
        client.start();
        boost::this_thread::sleep(SETTLE_TIME);

        auto pitch = getRotation("pitch", client, iface);
        auto yaw = getRotation("yaw", client, iface);
        auto roll = getRotation("roll", client, iface);

        cout << "Press enter to "
             << (resetTransform ? "restore original transform and " : "")
             << "exit.";
        std::cin.ignore();
        if (resetTransform) {
            cout << "Restoring original transform/route" << endl;
            ctx.get()->sendRoute(origRouteString);
        }
        boost::this_thread::sleep(SETTLE_TIME);
    }
    return 0;
}
