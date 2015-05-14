set(DESCRIPTORS
    com_osvr_EyeTracker)
set(JSON_HEADERS)
set(JSON_DESCRIPTORS)

foreach(filestem ${DESCRIPTORS})
    set(infile ${filestem}.json)
    set(outfile "${CMAKE_CURRENT_BINARY_DIR}/${filestem}_json.h")
    osvr_convert_json(${filestem}_json
        ${infile}
        "${outfile}")
    list(APPEND JSON_HEADERS "${outfile}")
    list(APPEND JSON_DESCRIPTORS "${infile}")
endforeach()

osvr_add_plugin(NAME com_osvr_EyeTracker
    SOURCES
    com_osvr_EyeTracker.cpp
    ${JSON_HEADERS})

target_link_libraries(com_osvr_EyeTracker osvrVRPNServer osvrConnection osvrPluginHost jsoncpp_lib vendored-vrpn osvrPluginKit)

target_include_directories(com_osvr_EyeTracker PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/vrpn/submodules/hidapi" "${CMAKE_CURRENT_BINARY_DIR}")

set_target_properties(com_osvr_EyeTracker PROPERTIES
    FOLDER "OSVR Plugins")

install(FILES
    ${JSON_DESCRIPTORS}
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/device-descriptors"
    COMPONENT Doc)