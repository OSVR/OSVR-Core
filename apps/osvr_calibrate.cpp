/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>

*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/Server/ConfigureServerFromFile.h>
#include <osvr/Server/RegisterShutdownHandler.h>

#include <osvr/ClientKit/ClientKit.h>
#include <osvr/ClientKit/InterfaceStateC.h>

#include <osvr/Util/EigenInterop.h>

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

static osvr::server::ServerWeakPtr g_server;

auto SETTLE_TIME = boost::posix_time::seconds(2);

/// @brief Shutdown handler function - forcing the server pointer to be global.
void handleShutdown() {
    osvr::server::ServerPtr server(g_server.lock());
    if (server) {
        cout << "Received shutdown signal..." << endl;
        server->signalStop();
    } else {
        cout << "Received shutdown signal but server already stopped..."
             << endl;
    }
}

void waitForEnter() { std::cin.ignore(); }

/// @brief Simple class to handle running a client mainloop in another thread,
/// but easily pausable.
class ClientMainloop : boost::noncopyable {
  public:
    ClientMainloop(osvr::clientkit::ClientContext &ctx) : m_ctx(ctx) {}
    void mainloop() {
        boost::unique_lock<boost::mutex> lock(m_mutex, boost::try_to_lock);
        if (lock) {
            m_ctx.update();
        }
    }
    boost::mutex &getMutex() { return m_mutex; }

  private:
    osvr::clientkit::ClientContext &m_ctx;
    boost::mutex m_mutex;
};

int main(int argc, char *argv[]) {
    std::string configName;
    std::string outputName;
    namespace po = boost::program_options;
    // clang-format off
    po::options_description desc("Options");
    desc.add_options()
        ("help", "produce help message")
        ("route", po::value<std::string>()->default_value("/me/head"), "route to calibrate")
        ("output,O", po::value<std::string>(&outputName), "output file (defaults to same as config file)")
        ;
    po::options_description hidden("Hidden (positional-only) options");
    hidden.add_options()
        ("config", po::value<std::string>(&configName)->default_value(std::string(osvr::server::getDefaultConfigFilename())))
        ;
    // clang-format on

    po::positional_options_description p;
    p.add("config", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
                  .options(po::options_description().add(desc).add(hidden))
                  .positional(p)
                  .run(),
              vm);
    po::notify(vm);

    {
        /// Deal with command line errors or requests for help
        bool usage = false;

        if (vm.count("help")) {
            usage = true;
        } else if (vm.count("route") != 1) {
            cout << "Error: --route is a required argument\n" << endl;
            usage = true;
        }

        if (usage) {
            cout << "Usage: osvr_calibrate [config file name] [options]"
                 << endl;
            cout << desc << "\n";
            return 1;
        }
    }

    if (outputName.empty()) {
        outputName = configName;
    }

    osvr::server::ServerPtr srv =
        osvr::server::configureServerFromFile(configName);
    if (!srv) {
        return -1;
    }
    g_server = srv;

    cout << "Registering shutdown handler..." << endl;
    osvr::server::registerShutdownHandler<&handleShutdown>();

    std::string dest = vm["route"].as<std::string>();
    std::string route = srv->getSource(dest);
    if (route.empty()) {
        cerr << "Error: No route found for provided destination: " << dest
             << endl;
        return -1;
    }
    cout << dest << " -> " << route << endl;
    cout << "Starting client..." << endl;
    osvr::clientkit::ClientContext ctx("com.osvr.bundled.osvr_calibrate");
    osvr::clientkit::Interface iface = ctx.getInterface(dest);

    ClientMainloop client(ctx);
    srv->registerMainloopMethod(
        std::bind(&ClientMainloop::mainloop, std::ref(client)));
    {
        // Take ownership of the server inside this nested scope
        // We want to ensure that the client parts outlive the server.
        osvr::server::ServerPtr server(srv);
        srv.reset();

        cout << "Starting server and client mainloop..." << endl;
        server->start();
        cout << "Waiting a few seconds for the server to settle..." << endl;
        boost::this_thread::sleep(SETTLE_TIME);

        cout << "\n\nPlease place your device in its 'zero' orientation and "
                "press enter." << endl;
        waitForEnter();

        OSVR_OrientationState state;
        OSVR_TimeValue timestamp;
        OSVR_ReturnCode ret;
        {
            /// briefly interrupt the client mainloop so we can get stuff done
            /// with the client state.
            boost::unique_lock<boost::mutex> lock(client.getMutex());
            ret = osvrGetOrientationState(iface.get(), &timestamp, &state);
        }
        if (ret != OSVR_RETURN_SUCCESS) {
            cerr << "Sorry, no orientation state available for this route - "
                    "are you sure you have a device plugged in and your path "
                    "correct?" << endl;
            return -1;
        }
        Eigen::AngleAxisd rotation(osvr::util::fromQuat(state).inverse());

        cout << "Angle: " << rotation.angle()
             << " Axis: " << rotation.axis().transpose() << endl;

        Json::Value newRoute(Json::objectValue);
        {

            Json::Value axis(Json::arrayValue);
            axis.append(rotation.axis()[0]);
            axis.append(rotation.axis()[1]);
            axis.append(rotation.axis()[2]);

            newRoute["destination"] = dest;
            newRoute["source"]["rotate"]["radians"] = rotation.angle();
            newRoute["source"]["rotate"]["axis"] = axis;
            std::istringstream(route) >> newRoute["source"]["child"];

            bool isNew = server->addRoute(newRoute.toStyledString());
            BOOST_ASSERT_MSG(
                !isNew,
                "Server claims this is a new, rather than a replacement, "
                "route... should not happen!");
        }

        cout << "\n\nNew calibration applied: please inspect it with the "
                "Tracker Viewer." << endl;
        if (configName == outputName) {
            cout << "If you are satisfied and want to OVERWRITE your existing "
                    "config file with this update, press enter." << endl;
            cout << "Otherwise, press Ctrl-C to break out of this program."
                 << endl;
            cout << "(If rotations appear incorrect, you may first need to add "
                    "a basisChange transform layer to the route.)" << endl;
            waitForEnter();
        }
        Json::Value root;
        {
            std::ifstream config(configName);
            if (!config.good()) {
                cerr << "Could not read the original config file again!"
                     << endl;
                return -1;
            }

            Json::Reader reader;
            if (!reader.parse(config, root)) {
                cerr << "Could not parse the original config file again! "
                        "Should never happen!" << endl;
                return -1;
            }
        }
        auto &routes = root["routes"];
        for (auto &fileRoute : routes) {
            if (fileRoute["destination"] == dest) {
                fileRoute = newRoute;
            }
        }
        {
            cout << "\n\nWriting updated config file to " << outputName << endl;
            std::ofstream outfile(outputName);
            outfile << root.toStyledString();
        }

        cout << "Awaiting Ctrl-C to trigger server shutdown..." << endl;
        server->awaitShutdown();
    }
    cout << "Server mainloop exited." << endl;

    return 0;
}