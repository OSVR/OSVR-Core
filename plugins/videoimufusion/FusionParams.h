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

#ifndef INCLUDED_FusionParams_h_GUID_BD4F7F35_7854_4C9C_F4FF_73F62D33287D
#define INCLUDED_FusionParams_h_GUID_BD4F7F35_7854_4C9C_F4FF_73F62D33287D

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

struct VideoIMUFusionParams {
    double videoPosVariance = 3.0e-4;
    double videoOriVariance = 1.0e-1;
    double imuOriVariance = 1.0E-8;
    double imuAngVelVariance = 1.0E-10;
    double positionNoise = 0.01;
    double oriNoise = 0.1;
    double damping = 0.1;
};

#endif // INCLUDED_FusionParams_h_GUID_BD4F7F35_7854_4C9C_F4FF_73F62D33287D
