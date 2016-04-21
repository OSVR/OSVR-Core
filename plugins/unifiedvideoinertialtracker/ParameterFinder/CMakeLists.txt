
add_executable(TrackerParameterFinder
    CSVTools.h
    ../MakeHDKTrackingSystem.h
    $<TARGET_OBJECTS:uvbi-hdkdata>
    TrackerParameterFinder.cpp)
set_target_properties(TrackerParameterFinder PROPERTIES
    FOLDER "${PROJ_FOLDER}")
target_link_libraries(TrackerParameterFinder
    PRIVATE
    uvbi-core)
