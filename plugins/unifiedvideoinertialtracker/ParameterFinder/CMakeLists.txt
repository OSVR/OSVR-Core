
add_executable(TrackerParameterFinder
    $<TARGET_OBJECTS:uvbi-hdkdata>
    ../MakeHDKTrackingSystem.h
    CSVTools.h
    LoadRows.h
    newuoa.h
    TrackerParameterFinder.cpp)
set_target_properties(TrackerParameterFinder PROPERTIES
    FOLDER "${PROJ_FOLDER}")
target_link_libraries(TrackerParameterFinder
    PRIVATE
    uvbi-core)
