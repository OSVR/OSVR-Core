# 2009-2010 Ryan Pavlik <rpavlik@iastate.edu> <abiryan@ryandnet>
# http://academic.cleardefinition.com/
# Iowa State University HCI Graduate Program/VRAC

find_package(Qt4 COMPONENTS QtCore REQUIRED)
include(${QT_USE_FILE})

if(NOT VRPN_FOUND)
	find_package(VRPN REQUIRED)
endif()

###
# Build the project
###
set(EXTRA_LIBS ${QT_LIBRARIES} ${VRPN_LIBRARIES})
include_directories(${VRPN_INCLUDE_DIRS})

set(SOURCES
	vrpn_QMainloopContainer.cpp
	vrpn_QAnalogRemote.cpp
	vrpn_QButtonRemote.cpp
	vrpn_QDialRemote.cpp
	vrpn_QTrackerRemote.cpp)

set(HEADERS
	vrpn_ConnectionPtr.h)

set(MOCS
	vrpn_QMainloopContainer.h
	vrpn_QAnalogRemote.h
	vrpn_QButtonRemote.h
	vrpn_QDialRemote.h
	vrpn_QTrackerRemote.h)

# Let QT pre-process the files and add the generated files to the source list
qt4_wrap_ui(GENERATED_SOURCES ${UIS})
qt4_add_resources(GENERATED_SOURCES ${RESOURCES})
qt4_wrap_cpp(GENERATED_SOURCES ${MOCS})


source_group("Generated Sources - Do Not Edit" FILES ${GENERATED_SOURCES})

# Make sure the compiler can find the pre-processed files from qt4_wrap_ui
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

# Build and link the lib!
add_library(qvrpn
	${SOURCES}
	${GENERATED_SOURCES}
	${HEADERS}
	${MOCS})

target_link_libraries(qvrpn ${QT_LIBRARIES} ${VRPN_LIBRARIES})

