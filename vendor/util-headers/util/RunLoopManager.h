/** @file
        @brief Header

        This header is maintained as a part of 'util-headers' - you can always
	find the latest version online at https://github.com/vancegroup/util-headers

	This GUID can help identify the project: d1dbc94e-e863-49cf-bc08-ab4d9f486613

	This copy of the header is from the revision that Git calls
	94f7934d19090d7f76e3160c49c0dbafdc3ecf29

	Commit date: "2014-10-31 15:21:32 -0500"

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
    virtual void reportRunning() = 0;
    virtual bool shouldContinue() = 0;
};

class ShutdownInterface {
public:
    virtual void signalShutdown() = 0;
    virtual void signalAndWaitForShutdown() = 0;
};

class LoopGuardInterface {
public:
	enum RunningState {
		STATE_STOPPED,
		STATE_STARTING,
		STATE_RUNNING
	};
	virtual void reportStateChange_(RunningState s) = 0;
	virtual void reportStopped_() = 0;
};

/// @brief Base class for implementations of a RunLoopManager that use
/// various synchronization libraries.
class RunLoopManagerBase : public StartingInterface,
	public LoopInterface,
	public ShutdownInterface,
	public LoopGuardInterface {
public:
	RunLoopManagerBase()
		: shouldStop_(false) {}

	/// @name StartingInterface
	/// @{
	virtual void signalStart(); ///< shared implementation provided
	virtual void signalAndWaitForStart() = 0;
	/// @}

	/// @name LoopInterface
	/// @{
	virtual void reportRunning() = 0;
	virtual bool shouldContinue(); ///< shared implementation provided
	/// @}

	/// @name ShutdownInterface
	/// @{
	virtual void signalShutdown() = 0;
	virtual void signalAndWaitForShutdown() = 0;
	/// @}


	/// @name LoopGuardInterface
	/// @{
	void reportStateChange_(RunningState s) = 0;
	void reportStopped_() = 0;
	/// @}

protected:
	/// @brief internal utility function.
	void setShouldStop_(bool value); ///< shared implementation provided

private:
	/// One-way signalling flag from outside to the runloop.
	volatile bool shouldStop_;
};

/// @brief RAII class to signal loop start and end.
class LoopGuard : boost::noncopyable {
public:
    enum StartTime {
        REPORT_START_IMMEDIATELY,
        DELAY_REPORTING_START
    };
    LoopGuard(LoopGuardInterface &mgr, StartTime t = REPORT_START_IMMEDIATELY);
    ~LoopGuard();

private:
	LoopGuardInterface &mgr_;
};


inline bool RunLoopManagerBase::shouldContinue() {
	return !shouldStop_;
}

inline void RunLoopManagerBase::signalStart() {
	/// @todo how?
}
inline void RunLoopManagerBase::setShouldStop_(bool value) {
	shouldStop_ = value;
}

inline LoopGuard::LoopGuard(LoopGuardInterface &mgr, LoopGuard::StartTime t)
    : mgr_(mgr) {
	mgr_.reportStateChange_(LoopGuardInterface::STATE_STARTING);
    if (t == REPORT_START_IMMEDIATELY) {
		mgr_.reportStateChange_(LoopGuardInterface::STATE_RUNNING);
    }
}

inline LoopGuard::~LoopGuard() {
    mgr_.reportStopped_();
}

} // end of namespace util

#endif // INCLUDED_RunLoopManager_h_GUID_cca4d4ff_064a_48bb_44db_b8414fb8d202
