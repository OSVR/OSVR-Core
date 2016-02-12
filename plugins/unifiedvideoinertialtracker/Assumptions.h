/** @file
    @brief Header containing preprocessor defines for various simplifying
   assumptions made for the sake of iterative development.

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_Assumptions_h_GUID_62FEFDEF_D66D_4B98_2DD6_3DC8E5847652
#define INCLUDED_Assumptions_h_GUID_62FEFDEF_D66D_4B98_2DD6_3DC8E5847652

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

/// @todo Remove when we no longer assume a single camera or monotonic camera
/// timestamps, and the build will break in a few places
/// where known "gotchas" exist
#define OSVR_UVBI_ASSUME_SINGLE_CAMERA 1
/// @todo Remove when we no longer assume that IMU reports arrive before video
/// reports with same timestamps.
#define OSVR_UVBI_ASSUME_CAMERA_ALWAYS_SLOWER 1
/// @todo Remove when we no longer assume a single IMU in the whole system.
#define OSVR_UVBI_ASSUME_SINGLE_IMU 1
/// @todo Remove when we no longer assume a single optical target per body.
#define OSVR_UVBI_ASSUME_SINGLE_TARGET_PER_BODY 1

#endif // INCLUDED_Assumptions_h_GUID_62FEFDEF_D66D_4B98_2DD6_3DC8E5847652
