/** @file
	@brief Header

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

#pragma once
#ifndef INCLUDED_vrpn_QMainloopContainer_h_GUID_f7a94be6_2c69_476a_acf1_2d904a3d8a1d
#define INCLUDED_vrpn_QMainloopContainer_h_GUID_f7a94be6_2c69_476a_acf1_2d904a3d8a1d

// Internal Includes
// - none

// Library/third-party includes
#include <QObject>
#include <QTimer>
#include <QSharedPointer>
#include <QMutex>

#include <vrpn_MainloopContainer.h>

// Standard includes
// - none

/// A class supporting the interaction of Qt and the VRPN mainloop:
/// uses vrpn_MainloopContainer to hold arbitrary vrpn objects and
/// mainloop them at a regular basis.
class vrpn_QMainloopContainer : public QObject {
		Q_OBJECT
	public:
		explicit vrpn_QMainloopContainer(int msInterval = 20, QObject *parent = 0);
		~vrpn_QMainloopContainer();

		/// Add a vrpn_MainloopObject. The mainloop container takes ownership
		/// of the passed object and will delete it when destroyed or cleared.
		vrpn_MainloopObject * add(vrpn_MainloopObject * o);

		/// Add an object wrappable into a vrpn_MainloopObject. The mainloop
		/// container takes ownership of the passed object and will delete it
		/// when destroyed or cleared.
		template<class T>
		T add(T o) {
			add(vrpn_MainloopObject::wrap(o));
			return o;
		}

	signals:
		void added();
		void started();
		void stopped();
		void beforeMainloop();
		void afterMainloop();
		void cleared();

	public slots:
		void start();
		void mainloop();
		void stop();
		void clear();

	private:
		int _msInterval;
		QMutex _vectorMutex;
		QSharedPointer<QTimer> _timer;
		vrpn_MainloopContainer _vrpn;
};

#endif // INCLUDED_vrpn_QMainloopContainer_h_GUID_f7a94be6_2c69_476a_acf1_2d904a3d8a1d
