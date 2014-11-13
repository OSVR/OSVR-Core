/** @file
    @brief a sample application to demonstrate lib init, configuration load and,
   tracker handling

    @date 2014
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
// - none

// Library/third-party includes
#include <ogvr/Util/ReturnCodesC.h>

// Standard includes
// - none

/**
 * @brief a quick macro to test the returned result
 */
#define CHECK_RES(res, txt)                                                    \
    if (res != OGVR_RESULT_OK) {                                               \
        std::cerr << "Error: " << txt << "failed" << ogvrGetErrorText(res)     \
                  << std::endl;                                                \
        return -1;                                                             \
    }

/**
 * @brief Interface callback for the head tracker
 */
OGVR_Result MyHeadTrackerCallback(OGVR_InterfaceHandle itf, OGVR_Data data,
                                  OGVR_ReadInfo *readInfo, void *userParam) {
    OGVR_Point *pt;
    OGVR_Result res = ogvrData_UnpackPoint(data, 0, &pt);

    std::cout << "Head at " << pt->x << "," << pt->y << "," << pt->z
              << std::endl;

    return OGVR_RESULT_OK;
}

/**
 * @brief Interface callback for the button (this example uses same callback for
 * all buttons)
 */

OGVR_Result MyButtonCallback(OGVR_InterfaceHandle itf, OGVR_Data data,
                             OGVR_ReadInfo *readInfo, void *userParam) {
    OGVR_ButtonState *buttonState;
    OGVR_Result res = ogvrData_UnpackButtonState(data, 0, &buttonState);

    std::cout << "Button " << buttonState->state << std::endl;

    return OGVR_RESULT_OK;
}

/**
 * @brief The global callback for system-wide events
 */
OGVR_Result MyGlobalCallback(OGVR_DeviceHandle dev, OGVR_InterfaceHandle itf,
                             OGVR_Event evt, void *eventParam,
                             void *userParam) {
    switch (evt) {
    case OGVR_EVENT_ITF_DISCONNECTED:
        // handle interface disconnect here
        break;
    case OGVR_EVENT_ERROR:
        // handle system error here
        break;
    }

    return OGVR_RESULT_OK;
}

int main(void) {
    OGVR_Result res;
    uint32_t nInitFlags = 0;
    OGVR_Context ctx;

    res = ogvrInitLib(nInitFlags, &ctx);
    CHECK_RES(res, "ogvrInitLib");

    // use global interface callback - for disconnect, error conditions, etc
    res = ogvrRegisterGlobalCallback(ctx, MyGlobalCallback, 0);
    CHECK_RES(res, "ogvrRegisterGlobalCallback");

    OGVR_InterfaceHandle left_hand, right_hand, left_hand_btn;
    OGVR_InterfaceHandle head;
    OGVR_InterfaceHandle hmd;

    OGVR_InterfaceConfig cfg;
    res = ogvrLoadInterfaceConfig(ctx, "ces_demo.cnf", &cfg);
    CHECK_RES(res, "ogvrLoadConfig");

    res = ogvrConfig_GetInterface(cfg, "left_hand/pos", 0, &left_hand);
    CHECK_RES(res, "ogvrConfig_GetInterface");

    res = ogvrConfig_GetInterface(cfg, "left_hand/buttons/fire", 0,
                                  &left_hand_btn);
    CHECK_RES(res, "ogvrConfig_GetInterface");

    res = ogvrConfig_GetInterface(cfg, "right_hand/pos", 0, &right_hand);
    CHECK_RES(res, "ogvrConfig_GetInterface");

    res = ogvrConfig_GetInterface(cfg, "head/pos", 0, &head);
    CHECK_RES(res, "ogvrConfig_GetInterface");

    // Demonstrate get and set interface params

    // query refresh rate
    int refreshRate;
    res = ogvrInterface_GetParamInt(left_hand, OGVR_PARAM_NAME_REFRESH_RATE, 0,
                                    &refreshRate);
    CHECK_RES(res, "ogvrInterface_GetParamInt");

    // set resolution mode
    res = ogvrConfig_GetInterface(cfg, "display", 0, &hmd);
    CHECK_RES(res, "ogvrConfig_GetInterface");

    int numResModes;
    res = ogvrInterface_GetParamInt(hmd, OGVR_PARAM_NAME_DISPLAY_NUM_RES_MODES,
                                    0, &numResModes);

    // this (hypothetical) device supports <numResModes> resolution modes. We
    // request the 3rd:
    res =
        ogvrInterface_SetParamInt(hmd, OGVR_PARAM_NAME_DISPLAY_RES_MODE, 0, 3);

    // for head and hand buttons, use callback

    res = ogvrInterface_RegisterCallback(head, MyHeadTrackerCallback, 0);
    res = ogvrInterface_RegisterCallback(left_hand_btn, MyButtonCallback, 0);
    res = ogvrInterface_RegisterCallback(right_hand, MyButtonCallback,
                                         0); // same callback for both hands

    // main loop.
    while (true) {
        OGVR_Point *ptLeftHand, *ptRightHand;
        OGVR_ReadInfo *ri;

        // gets the most recent state
        ogvrInterface_Read(left_hand, &ptLeftHand, &ri);
        ogvrInterface_Read(right_hand, &ptRightHand, &ri);
    }

    ogvrDeinitLib(ctx);
}