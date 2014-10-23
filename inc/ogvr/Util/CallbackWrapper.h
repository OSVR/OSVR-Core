/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_CallbackWrapper_h_GUID_6169ADE2_5BA1_4A81_47C9_9E492F6405ED
#define INCLUDED_CallbackWrapper_h_GUID_6169ADE2_5BA1_4A81_47C9_9E492F6405ED

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
template <typename FunctionType> class CallbackWrapper {

  private:
    FunctionType m_f;
};
} // end of namespace ogvr
#endif // INCLUDED_CallbackWrapper_h_GUID_6169ADE2_5BA1_4A81_47C9_9E492F6405ED
