# Manually-run script for generating the GetRuntimeVersion headers/implementations.
# Manually generated simply because they're identical except for module name.

set(INPUT_H "${CMAKE_CURRENT_LIST_DIR}/GetRuntimeVersion.h.in")
set(INPUT_CPP "${CMAKE_CURRENT_LIST_DIR}/GetRuntimeVersion.cpp.in")
foreach(guidmod
    487199D9_CA87_4E85_52A2_D1BDBC119163:Client
    5BF26574_26A9_4DB1_27B9_B5B8E69F2AE1:Common
    6FCB818C_A087_4CB8_E0C4_29B472CCE703:Connection
    4A6A0FF9_4A87_4F9E_1192_82AEA1E1DF58:PluginHost
    8513C123_CF38_4F9F_586F_1A1FDA5463BA:Server
    4458E735_A9DC_4F94_AE78_433B41E5147F:USBSerial
    FF76C4D7_2239_4DA3_0216_19D9473788E8:Util
    D58CA30A_7011_4BA6_509B_CF2A61DDF191:VRPNServer)

    string(SUBSTRING "${guidmod}" 0 36 GUID)
    string(SUBSTRING "${guidmod}" 37 -1 Modname)
    message(STATUS "Module: '${Modname}'")
    string(TOUPPER "${Modname}" MODNAME)
    string(TOLOWER "${Modname}" modname)
    configure_file("${INPUT_H}"
        "${CMAKE_CURRENT_LIST_DIR}/../../inc/osvr/${Modname}/GetRuntimeVersion.h"
        @ONLY
        NEWLINE_STYLE LF)
    configure_file("${INPUT_CPP}"
        "${CMAKE_CURRENT_LIST_DIR}/${Modname}/GetRuntimeVersion.cpp"
        @ONLY
        NEWLINE_STYLE LF)
endforeach()
