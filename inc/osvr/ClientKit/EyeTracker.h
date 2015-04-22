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

#ifndef INCLUDED_EyeTracker_h_GUID_0A75A14B_0F60_4384_A630_0548995F60D3
#define INCLUDED_EyeTracker_h_GUID_0A75A14B_0F60_4384_A630_0548995F60D3


// Internal Includes
#include <osvr/ClientKit/EyeTracker_decl.h>
#include <osvr/ClientKit/Interface.h>
#include <osvr/ClientKit/EyeTrackerC.h>
#include <osvr/Util/EyeTrackerReportTypesC.h>
#include <osvr/Util/TimeValue.h>
#include <osvr/Util/Deletable.h>

// Library/third-party includes
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

// Standard includes
// - none

namespace osvr {

	namespace clientkit {
		void registerEyeTrackerCallback(Interface &iface, EyeTrackerCallback cb,
			void *userdata);
		/// @brief Implementation details
		namespace detail {
			/// @brief Class serving to maintain the registration of and wrap a
			/// friendly eye tracker callback
			class EyeTrackerCallbackRegistration : public util::Deletable,
				boost::noncopyable {
			public:
				virtual ~EyeTrackerCallbackRegistration() {}

			private:
				/// @brief Constructor - private so that only the factory
				/// (registration) function can create it.
				EyeTrackerCallbackRegistration(osvr::clientkit::Interface iface,
					EyeTrackerCallback cb,
					void *userdata)
					: m_cb(cb), m_userdata(userdata),
					m_ctx(iface.getContext().get()) {
					osvrRegisterEyeTrackerCallback(
						iface.get(),
						&EyeTrackerCallbackRegistration::handleEyeTrackerCallback,
						this);
				}
				/// @brief Custom deleter for shared pointer to image buffer
				class EyeDataDeleter {
				public:
					EyeDataDeleter(OSVR_ClientContext ctx) : m_ctx(ctx) {}
					void operator()(OSVR_ImageBufferElement *buf) {
						osvrClientFreeEyeData(m_ctx, buf);
					}

				private:
					OSVR_ClientContext m_ctx;
				};

				/// @brief Raw callback that processes the data and calls the user's
				/// callback.
				static void
					handleEyeTrackerCallback(void *userdata,
											const struct OSVR_TimeValue *timestamp,
											const struct OSVR_EyeTrackerReport *report) {
					EyeTrackerCallbackRegistration *self =
						static_cast<EyeTrackerCallbackRegistration *>(userdata);
					EyeTrackerReport newReport;
					newReport.sensor = report->sensor;
					//newReport.buffer.reset(report->gaze,
					//	EyeDataDeleter(self->m_ctx));
					self->m_cb(self->m_userdata, *timestamp, newReport);
				}

				EyeTrackerCallback m_cb;
				void *m_userdata;
				OSVR_ClientContext m_ctx;
				friend void osvr::clientkit::registerEyeTrackerCallback(
					Interface &iface, EyeTrackerCallback cb, void *userdata);
			};
		} // namespace detail

		inline void registerImagingCallback(Interface &iface,
			EyeTrackerCallback cb,
			void *userdata) {
			util::boost_util::DeletablePtr ptr(
				new detail::EyeTrackerCallbackRegistration(iface, cb, userdata));
			iface.takeOwnership(ptr);
		}

	} // end namespace clientkit

} // end namespace osvr

#endif // INCLUDED_EyeTracker_h_GUID_0A75A14B_0F60_4384_A630_0548995F60D3

