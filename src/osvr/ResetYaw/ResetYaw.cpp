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
#include <osvr/ResetYaw/ResetYaw.h>
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

auto SETTLE_TIME = boost::posix_time::microseconds(1);

/// @brief A flag we set in transform levels we create.
static const char FLAG_KEY[] = "resetYaw";
static bool gotNewPose = false;
static bool gotFirstPose = false;

static OSVR_TimeValue firstPoseTime;
static OSVR_TimeValue newPoseTime;
static OSVR_TimeValue timeNow;
static OSVR_TimeValue lastResetTime;
static const double DELAY_TIME = 0.5;


void headOrientationCallback(void* userdata, const OSVR_TimeValue* timestamp, const OSVR_OrientationReport *report)
{
	if (!gotFirstPose && osvrTimeValueDurationSeconds(timestamp, &newPoseTime) > 0.1)
	{
		gotFirstPose = true;
		firstPoseTime = *(timestamp);
	}
	else if (gotFirstPose && !gotNewPose && firstPoseTime != *(timestamp))
	{
		gotNewPose = true;
		newPoseTime = *(timestamp);
	}
}

OSVR_ReturnCode osvrResetYaw() {
	//avoid spamming calls
	if (lastResetTime.seconds != 0)
	{
		osvrTimeValueGetNow(&timeNow);
		float duration = osvrTimeValueDurationSeconds(&timeNow, &lastResetTime);
		if (duration <= DELAY_TIME) return OSVR_RETURN_FAILURE;
	}	

	osvr::clientkit::ClientContext ctx("com.osvr.bundled.resetyaw");
	std::string const path = "/me/head";
	// Get the interface associated with the destination route we
	// are looking for.
	osvr::clientkit::Interface iface = ctx.getInterface(path);
	{
		iface.registerCallback(&headOrientationCallback, NULL);

		ClientMainloopThread client(ctx);

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
		osvr::common::ParsedAlias origAlias{ elt->getSource() };
		if (!origAlias.isValid()) {
			cerr << "Couldn't parse the alias!" << endl;
			return OSVR_RETURN_FAILURE;
		}

		osvr::common::GeneralizedTransform xforms{ origAlias.getAliasValue() };
		osvr::common::remove_if(xforms, [](Json::Value const &current) {
			return current.isMember(FLAG_KEY) && current[FLAG_KEY].isBool() &&
				current[FLAG_KEY].asBool();
		});

		elt->setSource(
			osvr::common::jsonToCompactString(xforms.get(origAlias.getLeaf())));
		ctx.get()->sendRoute(createJSONAlias(path, *elt));

		
		client.start();
		while (!gotFirstPose)
		{
			boost::this_thread::sleep(SETTLE_TIME);
		}

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
				return OSVR_RETURN_FAILURE;
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

			gotNewPose = false;
			gotFirstPose = false;	
		}
		while (!gotNewPose)
		{
			boost::this_thread::sleep(SETTLE_TIME);
		}
		gotNewPose = false;
		gotFirstPose = false;
		osvrTimeValueGetNow(&lastResetTime);
	}

	

	return OSVR_RETURN_SUCCESS;
}
