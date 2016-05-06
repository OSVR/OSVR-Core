
add_executable(TrackerParameterFinder
    $<TARGET_OBJECTS:uvbi-hdkdata>
    ../MakeHDKTrackingSystem.h
    CSVTools.h
    LoadRows.h
    newuoa.h
    OptimizationBase.h
    ParameterSets.h
    ParamFindingRoutine.h
    TrackerParameterFinder.cpp
    TransformFindingRoutine.h
    UtilityFunctions.h)
set_target_properties(TrackerParameterFinder PROPERTIES
    FOLDER "${PROJ_FOLDER}")
target_link_libraries(TrackerParameterFinder
    PRIVATE
    uvbi-core
    JsonCpp::JsonCpp)
