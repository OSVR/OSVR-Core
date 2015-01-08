/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include "ClientMainloop.h"
#include "WrapRoute.h"
#include <osvr/ClientKit/ClientKit.h>
#include <osvr/Client/ClientContext.h>
#include <osvr/ClientKit/InterfaceStateC.h>
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

class ClientMainloopThread : boost::noncopyable {
  public:
    ClientMainloopThread(osvr::clientkit::ClientContext &ctx)
        : m_run(true), m_mainloop(ctx) {
        m_thread = boost::thread([&] {
            while (m_run) {
                m_mainloop.mainloop();
                boost::this_thread::yield();
            }
        });
    }
    ~ClientMainloopThread() {
        m_run = false;
        m_thread.join();
    }
    boost::mutex &getMutex() { return m_mainloop.getMutex(); }

  private:
    volatile bool m_run;
    ClientMainloop m_mainloop;
    boost::thread m_thread;
};
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
        cout << "Starting client mainloop..." << endl;
        ClientMainloopThread client(ctx);
        cout << "Waiting a few seconds for startup..." << endl;
        boost::this_thread::sleep(SETTLE_TIME);

        cout << "\n\nPlease place your device for " << dest
             << "in its 'zero' orientation and press enter." << endl;
        std::cin.ignore();

        OSVR_OrientationState state;
        OSVR_TimeValue timestamp;
        OSVR_ReturnCode ret;
        osvr::routing::RouteContainer routes;
        {
            /// briefly interrupt the client mainloop so we can get stuff done
            /// with the client state.
            boost::unique_lock<boost::mutex> lock(client.getMutex());
            routes = ctx.get()->getRoutes();
            ret = osvrGetOrientationState(iface.get(), &timestamp, &state);
        }
        if (ret != OSVR_RETURN_SUCCESS) {
            cerr << "Sorry, no orientation state available for this route - "
                    "are you sure you have a device plugged in and your path "
                    "correct?" << endl;
            return -1;
        }
        Eigen::Vector3d transformedZ =
            osvr::util::fromQuat(state) * Eigen::Vector3d::UnitZ();
        transformedZ[1] = 0; // Zero out the Y to project onto the XZ plane
        auto angle =
            std::acos(transformedZ.normalized().dot(Eigen::Vector3d::UnitZ()));
        Json::Value newLayer(Json::objectValue);
        newLayer["rotate"]["radians"] = angle;
        newLayer["rotate"]["axis"] = "y";

        Json::Value origRoute;
        {
            Json::Reader reader;
            if (!reader.parse(routes.getRouteForDestination(dest), origRoute)) {
                cerr << "Error parsing existing route!" << endl;
                return -1;
            }
        }
        std::string newRoute = wrapRoute(origRoute, newLayer).toStyledString();
        cout << "New route: " << newRoute << endl;
        {
            /// briefly interrupt the client mainloop so we can get stuff done
            /// with the client state.
            boost::unique_lock<boost::mutex> lock(client.getMutex());
            ctx.get()->sendRoute(newRoute);
        }

        boost::this_thread::sleep(SETTLE_TIME);

        std::cin.ignore();
    }
    return 0;
}