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

// Internal Includes
#include "vrpn_QTrackerRemote.h"

// Library/third-party includes
#include <vrpn_Tracker.h>
#include <quat.h>

// Standard includes
// - none

/// Private class containing static callbacks for VRPN
class vrpn_QTrackerRemote_impl {
	public:
		static void VRPN_CALLBACK handleTracker(void * userdata, const vrpn_TRACKERCB t) {
			vrpn_QTrackerRemote * tkr = static_cast<vrpn_QTrackerRemote *>(userdata);
			QVector3D pos(t.pos[0], t.pos[1], t.pos[2]);
			QQuaternion quat(t.quat[Q_W], t.quat[Q_X], t.quat[Q_Y], t.quat[Q_Z]);
			tkr->_handleTracker(t.sensor, pos, quat);
		}
};

vrpn_QTrackerRemote::vrpn_QTrackerRemote(QString const& name, vrpn_Connection * c, QObject * parent) :
	QObject(parent),
	_contained(new vrpn_Tracker_Remote(name.toStdString().c_str(), c)) {
	_contained->register_change_handler(this, &vrpn_QTrackerRemote_impl::handleTracker);
}

vrpn_QTrackerRemote::~vrpn_QTrackerRemote() {
	_contained->unregister_change_handler(this, &vrpn_QTrackerRemote_impl::handleTracker);
}

void vrpn_QTrackerRemote::mainloop() {
	_contained->mainloop();
}

void * vrpn_QTrackerRemote::_returnContained() const {
	return _contained.data();
}

void vrpn_QTrackerRemote::_handleTracker(int sensor, QVector3D position, QQuaternion orientation) {
	emit(poseReport(sensor, position, orientation));
}
