add_subdirectory(multiserver)
if(BUILD_OPENCV_CAMERA_PLUGIN)
	add_subdirectory(opencv)
endif()
if(BUILD_VIDEOTRACKER_PLUGIN)
	# This directory contains source files but doesn't compile them,
	# just sets OSVR_VIDEOTRACKERSHARED_SOURCES_* internal cache variables
	add_subdirectory(videotrackershared)

	add_subdirectory(videobasedtracker)
	add_subdirectory(unifiedvideoinertialtracker)
endif()
if(BUILD_ANALYSISPLUGINKIT)
	add_subdirectory(oneeurofilter)
	add_subdirectory(videoimufusion)
	add_subdirectory(deadreckoningorientation)
endif()
