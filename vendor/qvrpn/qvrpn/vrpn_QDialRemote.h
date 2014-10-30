/** @file
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

#pragma once
#ifndef INCLUDED_vrpn_QDialRemote_h_GUID_fd8bb49a_d415_4d43_8466_f64226e8934f
#define INCLUDED_vrpn_QDialRemote_h_GUID_fd8bb49a_d415_4d43_8466_f64226e8934f

// Internal Includes
#include "vrpn_MainloopObject.h"

// Library/third-party includes
#include <QObject>
#include <QString>
#include <QSharedPointer>

// Standard includes
// - none

class vrpn_Dial_Remote;
class vrpn_Connection;

/// A Qt-enabled wrapper object for VRPN Dial remotes.
class vrpn_QDialRemote : public QObject, public vrpn_MainloopObject {
		Q_OBJECT
	public:
		vrpn_QDialRemote(QString const& name, vrpn_Connection * c = NULL, QObject * parent = NULL);
		~vrpn_QDialRemote();

	signals:
		void dialChanged(int id, double change);

	public slots:
		/// vrpn_MainloopObject interface: step through the VRPN mainloop
		void mainloop();

	protected:
		/// vrpn_MainloopObject interface: return contained pointer for comparisons.
		void * _returnContained() const;

	private:
		friend class vrpn_QDialRemote_impl;
		void _handleDial(int id, double change);
		QSharedPointer<vrpn_Dial_Remote> _contained;
};
#endif // INCLUDED_vrpn_QDialRemote_h_GUID_fd8bb49a_d415_4d43_8466_f64226e8934f
