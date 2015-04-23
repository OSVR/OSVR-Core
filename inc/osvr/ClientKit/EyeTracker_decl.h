/** @file
    @brief Header

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

#ifndef INCLUDED_EyeTracker_decl_h_GUID_8AAE3E5A_60D6_4E87_8952_3AF0A1F3C6F1
#define INCLUDED_EyeTracker_decl_h_GUID_8AAE3E5A_60D6_4E87_8952_3AF0A1F3C6F1


// Internal Includes
#include <osvr/Util/EyeTrackerReportTypesC.h>
#include <osvr/Util/TimeValue_fwd.h>
#include <osvr/Util/Deletable.h>

// Library/third-party includes
#include <boost/shared_ptr.hpp>

// Standard includes
// - none

namespace osvr {

	namespace clientkit {
		/// @brief A smart pointer controlling deletion of the eye data buffer. The
		/// buffer is automatically wrapped in one of these before your callback
		/// gets it.

		//typedef boost::shared_ptr<OSVR_EyeBufferElement> EyeBufferPtr;

		/// @brief The user-friendly eye tracker report.
		struct EyeTrackerReport {
			/// @brief The device sensor number this data came from.
			OSVR_ChannelCount sensor;

			/// @brief A shared pointer with custom deleter that owns the underlying
			/// eye data buffer for the frame.

			//EyeBufferPtr buffer;

			OSVR_EyeGazeDirection gaze;
		};

		/// @brief The user-friendly eye tracker callback type
		typedef void(*EyeTrackerCallback)(
			void *userdata, util::time::TimeValue const &timestamp,
			EyeTrackerReport report);

		/// @brief Register a callback to receive each new eye data
	} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_EyeTracker_decl_h_GUID_8AAE3E5A_60D6_4E87_8952_3AF0A1F3C6F1

