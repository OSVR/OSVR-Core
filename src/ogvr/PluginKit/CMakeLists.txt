include_directories("${HEADER_BASE}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../..")

if(BUILD_SHARED_LIBS)
    set(OGVR_PLUGINKIT_STATIC FALSE)
else()
    set(OGVR_PLUGINKIT_STATIC TRUE)
endif()
configure_file(Export.h.in "${CMAKE_CURRENT_BINARY_DIR}/Export.h")

set(HEADER_LOCATION "${HEADER_BASE}/ogvr/PluginKit")

set(API
    "${HEADER_LOCATION}/PluginRegistration.h"
    "${HEADER_LOCATION}/PluginRegistrationC.h"
    "${CMAKE_CURRENT_BINARY_DIR}/Export.h")

set(SOURCE
    "${HEADER_BASE}/ogvr/Util/UniquePtr.h"
    Export.h.in
    PluginRegistration.cpp
    PluginRegistrationC.cpp)

add_library(ogvrPluginKit ${API} ${SOURCE})
