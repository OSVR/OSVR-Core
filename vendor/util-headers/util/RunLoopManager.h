/** @file
		@brief Header

		This header is maintained as a part of 'util-headers' - you can always
	find the latest version online at https://github.com/vancegroup/util-headers

	This GUID can help identify the project: d1dbc94e-e863-49cf-bc08-ab4d9f486613

	This copy of the header is from the revision that Git calls
	a1072ffcb5b18913c2a154020e16305ffc11842c

	Commit date: "2014-11-11 15:20:21 -0600"

		@date 2013

		@author
		Ryan Pavlik
		<rpavlik@iastate.edu> and <abiryan@ryand.net>
		http://academic.cleardefinition.com/
		Iowa State University Virtual Reality Applications Center
		Human-Computer Interaction Graduate Program

*/

//           Copyright Iowa State University 2013.
//  Distributed under the Boost Software License, Version 1.0.
//     (See accompanying file LICENSE_1_0.txt or copy at
//           http://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDED_RunLoopManager_h_GUID_cca4d4ff_064a_48bb_44db_b8414fb8d202
#define INCLUDED_RunLoopManager_h_GUID_cca4d4ff_064a_48bb_44db_b8414fb8d202

// Internal Includes
// - none

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
// - none

namespace util {
class StartingInterface {
public:
	virtual void signalStart() = 0;
	virtual void signalAndWaitForStart() = 0;
};

class LoopInterface {
public:
	virtual void reportStarting() = 0;
	virtual void reportRunning() = 0;
	virtual bool shouldContinue() = 0;
	virtual void reportStopped() = 0;
};

class ShutdownInterface {
public:
	virtual void signalShutdown() = 0;
	virtual void signalAndWaitForShutdown() = 0;
};

/// @brief Base class for implementations of a RunLoopManager that use
/// various synchronization libraries.
class RunLoopManagerBase : public StartingInterface,
						   public LoopInterface,
						   public ShutdownInterface,
						   boost::noncopyable {
public:
	enum RunningState { STATE_STOPPED, STATE_STARTING, STATE_RUNNING };
	RunLoopManagerBase() : shouldStop_(false) {}

	/// @name Starting Interface
	/// @{
	/// @brief Set up internal state to allow the run loop to start.
	virtual void signalStart() = 0;
	/// @brief Set up for run loop to start, then block until we have
	/// confirmation that the run loop is running.
	virtual void signalAndWaitForStart() = 0;
	/// @}

	/// @name ShutdownInterface
	/// @{
	/// @brief Send a message to the run loop that it should stop.
	virtual void signalShutdown() = 0;
	/// @brief Send a message to the run loop that it should stop, and wait
	/// until it does.
	virtual void signalAndWaitForShutdown() = 0;
	/// @}

	/// @name Loop Interface
	/// @brief Methods called from within the run loop or by a helper class.
	/// @{
	/// @brief Notify the run loop manager that the run loop is now starting but
	/// not yet running.
	void reportStarting();
	/// @brief Notify the run loop manager that the run loop is now running.
	void reportRunning();
	bool shouldContinue();
	void reportStopped();
	/// @}

protected:
	/// @brief Changes the internal state variable (under mutex protection) and
	/// notifies anyone who might be waiting on a condition change.
	virtual void reportStateChange_(RunningState s) = 0;

	/// @brief internal utility function.
	void setShouldStop_(bool value); ///< shared implementation provided

private:
	/// One-way signalling flag from outside to the runloop.
	volatile bool shouldStop_;
};

/// @brief RAII class to signal loop start and end.
class LoopGuard : boost::noncopyable {
public:
	enum StartTime { REPORT_START_IMMEDIATELY, DELAY_REPORTING_START };
	LoopGuard(LoopInterface &mgr, StartTime t = REPORT_START_IMMEDIATELY);
	~LoopGuard();

private:
	LoopInterface &mgr_;
};

inline void RunLoopManagerBase::reportStarting() {
	reportStateChange_(STATE_STARTING);
}

inline void RunLoopManagerBase::reportRunning() {
	reportStateChange_(STATE_RUNNING);
}

inline bool RunLoopManagerBase::shouldContinue() { return !shouldStop_; }

inline void RunLoopManagerBase::reportStopped() {
	reportStateChange_(STATE_STOPPED);
}

inline void RunLoopManagerBase::setShouldStop_(bool value) {
	shouldStop_ = value;
}

inline LoopGuard::LoopGuard(LoopInterface &mgr, LoopGuard::StartTime t)
	: mgr_(mgr) {
	mgr_.reportStarting();
	if (t == REPORT_START_IMMEDIATELY) {
		mgr_.reportRunning();
	}
}

inline LoopGuard::~LoopGuard() { mgr_.reportStopped(); }

} // end of namespace util

#endif // INCLUDED_RunLoopManager_h_GUID_cca4d4ff_064a_48bb_44db_b8414fb8d202
