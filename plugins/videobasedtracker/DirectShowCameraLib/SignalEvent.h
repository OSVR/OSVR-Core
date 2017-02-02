/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_SignalEvent_h_GUID_DD338B8E_2CC9_43B8_0699_13169941BD2E
#define INCLUDED_SignalEvent_h_GUID_DD338B8E_2CC9_43B8_0699_13169941BD2E

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/// Handy little low-level sync primitive
/// An automatic reset event restores to "un-signalled" after releasing one
/// thread, while a manual reset event must be manually reset.
///
/// inspired by https://msdn.microsoft.com/en-us/magazine/jj721588.aspx
class SignalEvent {
  public:
    /// Constructor.
    explicit SignalEvent(bool manual = false)
        : h_(CreateEvent(nullptr, manual, false, nullptr)) {
        if (nullptr == h_) {
            throw std::runtime_error("Could not create Windows event handle!");
        }
    }

    /// Destructor
    ~SignalEvent() { CloseHandle(h_); }
    /// Set or signal the event.
    void set() { SetEvent(h_); }

    /// Un-signal the event (not usually necessary for auto=reset events)
    void clear() { ResetEvent(h_); }
    /// Wait for the event, infinitely.
    /// Returns true if we were, in fact, signalled (should always be true)
    bool wait() { return WAIT_OBJECT_0 == WaitForSingleObject(h_, INFINITE); }

    /// Wait for the event, with a timeout.
    /// Returns true if we were, in fact, signalled (false if we hit our
    /// timeout)
    bool wait(DWORD milliseconds) {
        return WAIT_OBJECT_0 == WaitForSingleObject(h_, milliseconds);
    }
    SignalEvent(SignalEvent const &) = delete;
    SignalEvent &operator=(SignalEvent const &) = delete;

  private:
    HANDLE h_;
};

#endif // INCLUDED_SignalEvent_h_GUID_DD338B8E_2CC9_43B8_0699_13169941BD2E
