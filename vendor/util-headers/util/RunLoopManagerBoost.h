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

#ifndef INCLUDED_RunLoopManagerBoost_h_GUID_50f7b2f1_493e_4395_25ca_df2f010a34bd
#define INCLUDED_RunLoopManagerBoost_h_GUID_50f7b2f1_493e_4395_25ca_df2f010a34bd


// Internal Includes
#include "RunLoopManager.h"

// Library/third-party includes
#include <boost/thread.hpp>

// Standard includes
// - none


namespace util {

class RunLoopManagerBoost : public RunLoopManagerBase {
public:
    RunLoopManagerBoost()
        : currentState_(STATE_STOPPED) {}

    /// @name StartingInterface
    /// @{
    /*void signalStart();*/
    void signalAndWaitForStart();
    /// @}

    /// @name LoopInterface
    /// @{
    void reportRunning();
    /*bool shouldContinue();*/
    /// @}

    /// @name ShutdownInterface
    /// @{
    void signalShutdown();
    void signalAndWaitForShutdown();
    /// @}


	/// @name LoopGuardInterface
	/// @{
	void reportStateChange_(RunningState s);
	void reportStopped_();
	/// @}
private:
    boost::mutex mut_;
    boost::condition_variable stateCond_;
    
	typedef boost::unique_lock<boost::mutex> Lock;
    
    /// protected by condition variable
	volatile LoopGuardInterface::RunningState currentState_;
    

    /// One-way signalling flag from outside to the runloop.
    volatile bool shouldStop_;

};

inline void RunLoopManagerBoost::reportRunning() {
	reportStateChange_(LoopGuardInterface::STATE_RUNNING);
}

inline void RunLoopManagerBoost::reportStateChange_(RunLoopManagerBase::RunningState s) {
    {
        Lock condGuard(mut_);
        currentState_ = s;
    }
	stateCond_.notify_all();
}


inline void RunLoopManagerBoost::reportStopped_() {
    {
        Lock condGuard(mut_);
        setShouldStop_(false);
    }
    reportStateChange_(STATE_STOPPED);
}

inline void RunLoopManagerBoost::signalAndWaitForStart() {
    signalStart();
    {
		Lock condGuard(mut_);
        while (currentState_ != STATE_RUNNING) {
			stateCond_.wait(condGuard);
        }
    }
}

inline void RunLoopManagerBoost::signalShutdown() {
	Lock condGuard(mut_);
    if (currentState_ != STATE_STOPPED) {
        setShouldStop_(true);
    }
}


inline void RunLoopManagerBoost::signalAndWaitForShutdown() {
	Lock condGuard(mut_);

    if (currentState_ != STATE_STOPPED) {
        setShouldStop_(true);
    }

    do {
		stateCond_.wait(condGuard);
    } while (currentState_ != STATE_STOPPED);
}

} // end of namespace util

#endif // INCLUDED_RunLoopManagerBoost_h_GUID_50f7b2f1_493e_4395_25ca_df2f010a34bd

