/**
	@file
	@brief

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
#include "vrpn_QDialRemote.h"

// Library/third-party includes
#include <vrpn_Dial.h>

// Standard includes
// - none

/// Private class containing static callbacks for VRPN
class vrpn_QDialRemote_impl {
	public:
		static void VRPN_CALLBACK handleDial(void * userdata, const vrpn_DIALCB info) {
			vrpn_QDialRemote * qobj = static_cast<vrpn_QDialRemote *>(userdata);
			qobj->_handleDial(info.dial, info.change);
		}
};

vrpn_QDialRemote::vrpn_QDialRemote(QString const& name, vrpn_Connection * c, QObject * parent) :
	QObject(parent),
	_contained(new vrpn_Dial_Remote(name.toStdString().c_str(), c)) {
	_contained->register_change_handler(this, &vrpn_QDialRemote_impl::handleDial);
}

vrpn_QDialRemote::~vrpn_QDialRemote() {
	_contained->unregister_change_handler(this, &vrpn_QDialRemote_impl::handleDial);
}

void vrpn_QDialRemote::mainloop() {
	_contained->mainloop();
}

void * vrpn_QDialRemote::_returnContained() const {
	return _contained.data();
}

void vrpn_QDialRemote::_handleDial(int id, double change) {
	emit(dialChanged(id, change));
}
