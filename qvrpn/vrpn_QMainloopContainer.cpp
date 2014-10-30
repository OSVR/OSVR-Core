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
#include "vrpn_QMainloopContainer.h"

// Library/third-party includes
#include <QMutexLocker>

// Standard includes
// - none

vrpn_QMainloopContainer::vrpn_QMainloopContainer(int msInterval, QObject *parent) :
	QObject(parent),
	_msInterval(msInterval) {
}

vrpn_QMainloopContainer::~vrpn_QMainloopContainer() {
	clear();
}

vrpn_MainloopObject * vrpn_QMainloopContainer::add(vrpn_MainloopObject * o) {
	if (!o) {
		return o;
	}
	{
		QMutexLocker locker(&_vectorMutex);
		_vrpn.add(o);
	}
	emit added();
	return o;
}

void vrpn_QMainloopContainer::clear() {
	stop();
	{
		QMutexLocker locker(&_vectorMutex);
		_vrpn.clear();
	}
	emit cleared();
}

void vrpn_QMainloopContainer::start() {
	if (!_timer.isNull()) {
		return;
	}
	QSharedPointer<QTimer> tmr(new QTimer(this));
	_timer = tmr;
	connect(_timer.data(), SIGNAL(timeout()), this, SLOT(mainloop()));

	emit started();
	_timer->start(_msInterval);
}

void vrpn_QMainloopContainer::mainloop() {
	emit beforeMainloop();
	{
		QMutexLocker locker(&_vectorMutex);
		_vrpn.mainloop();
	}
	emit afterMainloop();
}

void vrpn_QMainloopContainer::stop() {
	if (_timer.isNull()) {
		return;
	}
	_timer->stop();
	_timer.clear();

	emit stopped();
}
