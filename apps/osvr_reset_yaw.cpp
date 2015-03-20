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
#include <osvr/Common/ClientContext.h>
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

/// @brief A flag we set in transform levels we create.
static const char FLAG_KEY[] = "resetYaw";

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;
    // clang-format off
    po::options_description desc("Options");
    desc.add_options()
        ("help", "produce help message")
        ("route", po::value<std::string>()->default_value("/me/head"), "route to calibrate")
        ;
    // clang-format on

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    {
        /// Deal with command line errors or requests for help
        bool usage = false;

        if (vm.count("help")) {
            cout << "Usage: osvr_reset_yaw [options]" << endl;
            cout << desc << "\n";
            return 1;
        }
    }
    osvr::clientkit::ClientContext ctx("com.osvr.bundled.resetyaw");
    std::string const dest = vm["route"].as<std::string>();

    osvr::clientkit::Interface iface = ctx.getInterface(dest);
    {

        ClientMainloopThread client(ctx);

        cout << "Running client mainloop briefly to get routes..." << endl;
        client.loopForDuration(boost::chrono::seconds(2));
        cout << "Removing any previous yaw-reset transforms..." << endl;
        Json::Value origRoute;
        {
            Json::Reader reader;
            auto routeString =
                ctx.get()->getRoutes().getRouteForDestination(dest);
            if (!reader.parse(routeString, origRoute)) {
                cerr << "Error parsing existing route!" << endl;
                cerr << routeString << endl;
                return -1;
            }
        }
        Json::Value origTransforms =
            origRoute[osvr::common::routing_keys::source()];
        Json::Value cleanTransforms =
            remove_levels_if(origTransforms, [](Json::Value const &current) {
                return current.isMember(FLAG_KEY) &&
                       current[FLAG_KEY].isBool() && current[FLAG_KEY].asBool();
            });
        origRoute[osvr::common::routing_keys::source()] = cleanTransforms;
        ctx.get()->sendRoute(origRoute.toStyledString());

        cout << "Sent cleaned route, starting again and waiting a few seconds "
                "for startup..." << endl;
        client.start();
        boost::this_thread::sleep(SETTLE_TIME);

        cout << "\n\nPlease place your device for " << dest
             << " in its 'zero' orientation and press enter." << endl;
        std::cin.ignore();

        OSVR_OrientationState state;
        OSVR_TimeValue timestamp;
        OSVR_ReturnCode ret;
        {
            /// briefly interrupt the client mainloop so we can get stuff done
            /// with the client state.
            ClientMainloopThread::lock_type lock(client.getMutex());
            ret = osvrGetOrientationState(iface.get(), &timestamp, &state);
            if (ret != OSVR_RETURN_SUCCESS) {
                cerr
                    << "Sorry, no orientation state available for this route - "
                       "are you sure you have a device plugged in and your "
                       "path correct?" << endl;
                std::cin.ignore();
                return -1;
            }
            auto q = osvr::util::fromQuat(state);

            // see
            // http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
            double yaw = std::atan2(2 * (q.y() * q.w() - q.x() * q.z()),
                                    1 - 2 * q.y() * q.y() - 2 * q.z() * q.z());
            cout << "Correction: " << -yaw << " radians about Y" << endl;

            Json::Value newLayer(Json::objectValue);
            newLayer["rotate"]["radians"] = -yaw;
            newLayer["rotate"]["axis"] = "y";
            newLayer[FLAG_KEY] = true;

            std::string newRoute =
                wrapRoute(origRoute, newLayer).toStyledString();
            cout << "New route: " << newRoute << endl;
            ctx.get()->sendRoute(newRoute);
            boost::this_thread::sleep(SETTLE_TIME / 2);
        }

        boost::this_thread::sleep(SETTLE_TIME);

        cout << "Press enter to exit.";
        std::cin.ignore();
    }
    return 0;
}
