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
#include "vrpn_QButtonRemote.h"

// Library/third-party includes
#include <vrpn_Button.h>

// Standard includes
// - none

/// Private class containing static callbacks for VRPN
class vrpn_QButtonRemote_impl {
	public:
		static void VRPN_CALLBACK handleButton(void * userdata, const vrpn_BUTTONCB info) {
			vrpn_QButtonRemote * qobj = static_cast<vrpn_QButtonRemote *>(userdata);
			qobj->_handleButton(info.button, info.state);
		}
};

vrpn_QButtonRemote::vrpn_QButtonRemote(QString const& name, vrpn_Connection * c, QObject * parent) :
	QObject(parent),
	_contained(new vrpn_Button_Remote(name.toStdString().c_str(), c)) {
	_contained->register_change_handler(this, &vrpn_QButtonRemote_impl::handleButton);
}

vrpn_QButtonRemote::~vrpn_QButtonRemote() {
	_contained->unregister_change_handler(this, &vrpn_QButtonRemote_impl::handleButton);
}

void vrpn_QButtonRemote::mainloop() {
	_contained->mainloop();
}

void * vrpn_QButtonRemote::_returnContained() const {
	return _contained.data();
}

void vrpn_QButtonRemote::_handleButton(int id, bool pressed) {
	emit(buttonChanged(id, pressed));
	if (pressed) {
		emit(buttonPressed(id));
	} else {
		emit(buttonReleased(id));
	}
}
