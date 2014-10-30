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
#ifndef INCLUDED_vrpn_QAnalogRemote_h_GUID_ab1365d4_10cb_4045_ae38_9cdc87a95088
#define INCLUDED_vrpn_QAnalogRemote_h_GUID_ab1365d4_10cb_4045_ae38_9cdc87a95088

// Internal Includes
#include "vrpn_MainloopObject.h"

// Library/third-party includes
#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <QList>

// Standard includes
// - none

class vrpn_Analog_Remote;
class vrpn_Connection;

/// A Qt-enabled wrapper object for VRPN Analog remotes.
class vrpn_QAnalogRemote : public QObject, public vrpn_MainloopObject {
		Q_OBJECT
	public:
		vrpn_QAnalogRemote(QString const& name, vrpn_Connection * c = NULL, QObject * parent = NULL);
		~vrpn_QAnalogRemote();

	signals:
		void analogReport(QList<double> channels);

	public slots:
		/// vrpn_MainloopObject interface: step through the VRPN mainloop
		void mainloop();

	protected:
		/// vrpn_MainloopObject interface: return contained pointer for comparisons.
		void * _returnContained() const;

	private:
		friend class vrpn_QAnalogRemote_impl;
		void _handleChannels(QList<double> const& channels);
		QSharedPointer<vrpn_Analog_Remote> _contained;
};

#endif // INCLUDED_vrpn_QAnalogRemote_h_GUID_ab1365d4_10cb_4045_ae38_9cdc87a95088
