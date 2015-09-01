osvr_setup_lib_vars(TypePack)

set(API
    "${HEADER_LOCATION}/Always.h"
    "${HEADER_LOCATION}/And.h"
    "${HEADER_LOCATION}/Apply.h"
    "${HEADER_LOCATION}/ApplyList.h"
    "${HEADER_LOCATION}/Bool.h"
    "${HEADER_LOCATION}/CoerceList.h"
    "${HEADER_LOCATION}/Compose.h"
    "${HEADER_LOCATION}/Concat.h"
    "${HEADER_LOCATION}/Contains.h"
    "${HEADER_LOCATION}/Defer.h"
    "${HEADER_LOCATION}/FindFirst.h"
    "${HEADER_LOCATION}/Fold.h"
    "${HEADER_LOCATION}/HasType.h"
    "${HEADER_LOCATION}/Head.h"
    "${HEADER_LOCATION}/HomogeneousTypeMap.h"
    "${HEADER_LOCATION}/If.h"
    "${HEADER_LOCATION}/Length.h"
    "${HEADER_LOCATION}/List.h"
    "${HEADER_LOCATION}/Not.h"
    "${HEADER_LOCATION}/Or.h"
    "${HEADER_LOCATION}/PushBack.h"
    "${HEADER_LOCATION}/PushFront.h"
    "${HEADER_LOCATION}/Quote.h"
    "${HEADER_LOCATION}/QuoteTrait.h"
    "${HEADER_LOCATION}/Size.h"
    "${HEADER_LOCATION}/SizeT.h"
    "${HEADER_LOCATION}/SplitList.h"
    "${HEADER_LOCATION}/T.h"
    "${HEADER_LOCATION}/Tail.h"
    "${HEADER_LOCATION}/Transform.h"
    "${HEADER_LOCATION}/TypePack.h"
    "${HEADER_LOCATION}/Void.h")

###
# Add the dummy target to make the headers show up in IDEs
add_custom_target(${LIBNAME_FULL}_Headers SOURCES ${API})
set_target_properties(${LIBNAME_FULL}_Headers PROPERTIES
    FOLDER "${LIB_FOLDER}")

###
# Add the interface target to actually use the files.
add_library(${LIBNAME_FULL} INTERFACE)
target_include_directories(${LIBNAME_FULL}
    INTERFACE
    #$<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(${LIBNAME_FULL}
    INTERFACE
    $<BUILD_INTERFACE:osvr_cxx11_flags>)

osvr_append_target(BUILDTREE ${LIBNAME_FULL})

###
# Install things properly.
install(TARGETS ${LIBNAME_FULL}
    EXPORT osvrTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${API}
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/osvr/${LIBNAME}
    COMPONENT Devel)
