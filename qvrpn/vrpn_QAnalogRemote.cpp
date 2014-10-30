/**
	@file
	@brief Implementation

	@date 2011

	@author
	Ryan Pavlik
	<rpavlik@iastate.edu> and <abiryan@ryand.net>
	http://academic.cleardefinition.com/
	Iowa State University Virtual Reality Applications Center
	Human-Computer Interaction Graduate Program
*/

//          Copyright Iowa State University 2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Internal Includes
#include "vrpn_QAnalogRemote.h"

// Library/third-party includes
#include <vrpn_Analog.h>

// Standard includes
// - none

/// Private class containing static callbacks for VRPN
class vrpn_QAnalogRemote_impl {
	public:
		static void VRPN_CALLBACK handleAnalog(void * userdata, const vrpn_ANALOGCB info) {
			vrpn_QAnalogRemote * qobj = static_cast<vrpn_QAnalogRemote *>(userdata);
			QList<double> channels;
			for (int i = 0; i < info.num_channel; ++i) {
				channels.append(info.channel[i]);
			}
			qobj->_handleChannels(channels);
		}
};

vrpn_QAnalogRemote::vrpn_QAnalogRemote(QString const& name, vrpn_Connection * c, QObject * parent) :
	QObject(parent),
	_contained(new vrpn_Analog_Remote(name.toStdString().c_str(), c)) {
	_contained->register_change_handler(this, &vrpn_QAnalogRemote_impl::handleAnalog);
}

vrpn_QAnalogRemote::~vrpn_QAnalogRemote() {
	_contained->unregister_change_handler(this, &vrpn_QAnalogRemote_impl::handleAnalog);
}

void vrpn_QAnalogRemote::mainloop() {
	_contained->mainloop();
}

void * vrpn_QAnalogRemote::_returnContained() const {
	return _contained.data();
}

void vrpn_QAnalogRemote::_handleChannels(QList<double> const& channels) {
	emit(analogReport(channels));
}
