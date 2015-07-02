/** @file
    @brief Header for inclusion inside the namespace of an OSVR library to
   implement runtime version reporting.

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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_GetRuntimeVersion_InnerImpl_h_GUID_3D1094A4_EC41_4F94_5774_8A4DA3E61F1C
#define INCLUDED_GetRuntimeVersion_InnerImpl_h_GUID_3D1094A4_EC41_4F94_5774_8A4DA3E61F1C

util::NumericVersionComponent getVersionComponent(uint8_t componentIndex) {
    switch (componentIndex) {
    case 0:
        return OSVR_CORE_VERSION_1;
    case 1:
        return OSVR_CORE_VERSION_2;
    case 2:
        return OSVR_CORE_VERSION_3;
    case 3:
        return OSVR_CORE_VERSION_4;
    default:
        return 0;
    }
}

util::RuntimeNumericVersion getRuntimeNumericVersion() {
    return util::RuntimeNumericVersion{OSVR_CORE_VERSION_1, OSVR_CORE_VERSION_2,
                                       OSVR_CORE_VERSION_3,
                                       OSVR_CORE_VERSION_4};
}
static const char COMMIT[] = OSVR_CORE_VERSION_COMMIT;
const char *getCommit() { return COMMIT; }

static const char VERSTRING[] = OSVR_CORE_VERSION_STRING;
const char *getVersionString() { return VERSTRING; }

#ifdef OSVR_CORE_BUILD_TAG
static const char BUILD_TAG[] = OSVR_CORE_BUILD_TAG;
#else
static const char BUILD_TAG[] = "";
#endif
const char *getCIBuildTag() { return BUILD_TAG; }

bool isKnownDevBuild() {
#ifdef OSVR_CORE_OFFICIAL_BUILD
    return false;
#else
    return true;
#endif
}

#ifdef OSVR_MODULE_NAME
static const char MODULENAME[] = OSVR_MODULE_NAME;
#else
static const char MODULENAME[] = "";
#endif

const char *getModuleName() { return MODULENAME; }

#endif // INCLUDED_GetRuntimeVersion_InnerImpl_h_GUID_3D1094A4_EC41_4F94_5774_8A4DA3E61F1C
