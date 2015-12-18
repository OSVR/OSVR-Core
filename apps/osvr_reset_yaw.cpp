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
#include <osvr/ClientKit/ClientKit.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/ClientKit/InterfaceStateC.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/JSONHelpers.h>
#include <osvr/Common/AliasProcessor.h>
#include <osvr/Common/ParseAlias.h>
#include <osvr/Common/GeneralizedTransform.h>
#include <osvr/Util/EigenInterop.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Util/ExtractYaw.h>

// Library/third-party includes
#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/variant/get.hpp>
#include <boost/optional.hpp>
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <iostream>
#include <fstream>
#include <exception>

using std::cout;
using std::cerr;
using std::endl;

inline std::string
createJSONAlias(std::string const &path,
                osvr::common::elements::AliasElement const &elt) {
    return osvr::common::applyPriorityToAlias(
               osvr::common::createJSONAlias(
                   path, osvr::common::jsonParse(elt.getSource())),
               elt.priority())
        .toStyledString();
}

boost::optional<osvr::common::elements::AliasElement>
getAliasElement(osvr::clientkit::ClientContext &ctx, std::string const &path) {
    osvr::common::PathNode const *node = nullptr;
    try {
        node = &(ctx.get()->getPathTree().getNodeByPath(path));
    } catch (std::exception &e) {
        cerr << "Could not get node at path '" << path
             << "' - exception: " << e.what() << endl;
        return boost::none;
    }
    auto elt = boost::get<osvr::common::elements::AliasElement>(&node->value());
    if (!elt) {
        return boost::none;
    }
    return *elt;
}

auto SETTLE_TIME = boost::posix_time::seconds(2);

/// @brief A flag we set in transform levels we create.
static const char FLAG_KEY[] = "resetYaw";

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;
    // clang-format off
    po::options_description desc("Options");
    desc.add_options()
        ("help", "produce help message")
        ("path", po::value<std::string>()->default_value("/me/head"), "path to reset-yaw on")
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
    std::string const path = vm["path"].as<std::string>();

    // Get the interface associated with the destination route we
    // are looking for.
    osvr::clientkit::Interface iface = ctx.getInterface(path);
    {
        ClientMainloopThread client(ctx);

        cout << "Running client mainloop briefly to start up..." << endl;
        client.loopForDuration(boost::chrono::seconds(2));
        cout << "Removing any previous yaw-reset transforms..." << endl;

        // Get the alias element corresponding to the desired path, if possible.
        auto elt = getAliasElement(ctx, path);
        if (!elt) {
            // No luck, sorry.
            cerr << "Couldn't get the alias at " << path << endl;
            return -1;
        }

        // Get a reference to the source associated with the portion
        // of the tree that has this destination.  Then clean out
        // any prior instance of our meddling by checking for an
        // entry that has our flag key in it.  Then replace the
        // original source tree with the cleaned tree.  Send this
        // cleaned alias back to the server.
        osvr::common::ParsedAlias origAlias{elt->getSource()};
        if (!origAlias.isValid()) {
            cerr << "Couldn't parse the alias!" << endl;
            return -1;
        }
        cout << "Original transform: "
             << origAlias.getAliasValue().toStyledString() << "\n" << endl;
        osvr::common::GeneralizedTransform xforms{origAlias.getAliasValue()};
        osvr::common::remove_if(xforms, [](Json::Value const &current) {
            return current.isMember(FLAG_KEY) && current[FLAG_KEY].isBool() &&
                   current[FLAG_KEY].asBool();
        });
        cout << "Cleaned transform: "
             << xforms.get(origAlias.getLeaf()).toStyledString() << "\n"
             << endl;
        elt->setSource(
            osvr::common::jsonToCompactString(xforms.get(origAlias.getLeaf())));
        ctx.get()->sendRoute(createJSONAlias(path, *elt));

        cout << "Sent cleaned transform, starting again and waiting a few "
                "seconds for startup..."
             << endl;
        client.start();
        boost::this_thread::sleep(SETTLE_TIME);

        cout << "\n\nPlease place your device for " << path
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
                cerr << "Sorry, no orientation state available for this path - "
                        "are you sure you have a device plugged in and your "
                        "path correct?"
                     << endl;
                std::cin.ignore();
                return -1;
            }
            auto q = osvr::util::eigen_interop::map(state);
            auto yaw = osvr::util::extractYaw(q);
            cout << "Correction: " << -yaw << " radians about Y" << endl;

            Json::Value newLayer(Json::objectValue);
            newLayer["postrotate"]["radians"] = -yaw;
            newLayer["postrotate"]["axis"] = "y";
            newLayer[FLAG_KEY] = true;
            xforms.wrap(newLayer);
            cout << "New source: "
                 << xforms.get(origAlias.getLeaf()).toStyledString() << endl;

            elt->setSource(osvr::common::jsonToCompactString(
                xforms.get(origAlias.getLeaf())));
            ctx.get()->sendRoute(createJSONAlias(path, *elt));
            boost::this_thread::sleep(SETTLE_TIME / 2);
        }

        boost::this_thread::sleep(SETTLE_TIME);

        cout << "Press enter to exit.";
        std::cin.ignore();
    }
    return 0;
}
