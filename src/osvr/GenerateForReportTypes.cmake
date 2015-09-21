set(REPORT_TYPES
    Position
    Orientation
    Pose
    Button
    Analog
    Imaging
    Location2D
    Direction
    EyeTracker2D
    EyeTracker3D
    EyeTrackerBlink
    NaviVelocity
    NaviPosition)

# Generate a file using a template with the placeholder @BODY@, as well as a
# repeated template with the placeholder @TYPE@ - repeated once for each report type.
function(osvr_generate_for_report_types _fileTemplate _snippetTemplate _outFile)
    message(STATUS "Generating ${_outFile}")

    # This line forces a CMake re-run if the snippet template is modified.
    configure_file("${_snippetTemplate}" "${_outFile}.snippettemplate" @ONLY)

    file(READ "${_snippetTemplate}" SNIPPET)
    set(BODY "/* generated file - do not edit! */")
    foreach(TYPE ${REPORT_TYPES})
        string(CONFIGURE "${SNIPPET}" snippetConfigured @ONLY)
        set(BODY "${BODY}\n${snippetConfigured}")
    endforeach()
    configure_file("${_fileTemplate}" "${_outFile}" @ONLY)
endfunction()