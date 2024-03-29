# We are creating a build environment for a shared library named the same as
# our project in the top level CMakeLists.txt file.
add_library( ${PROJECT_NAME} SHARED "" )

# We are dependent on ReiserRT_FlyingPhasor shared library package.
# Ensure we specify the Major.Minor version we want.
find_package( ReiserRT_FlyingPhasor 3.0 CONFIG REQUIRED )
if ( ReiserRT_FlyingPhasor_FOUND )
	message( STATUS "Found ReiserRT_FlyingPhasor!" )
endif()

# Specify all of our public headers for easy reference.
set( _publicHeaders
    ChirpingPhasorToneGenerator.h
    )

# Specify all of our private headers for easy reference.
set( _privateHeaders
      ""
    )

# Specify our source files
set( _sourceFiles
    ChirpingPhasorToneGenerator.cpp
    )

# Specify Sources to be built into our library
target_sources( ${PROJECT_NAME} PRIVATE ${_sourceFiles} )

# Specify our target interfaces for ourself and external clients post installation
target_include_directories( ${PROJECT_NAME}
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_BINARY_DIR}/${INSTALL_INCLUDEDIR}>"
        "$<INSTALL_INTERFACE:${INSTALL_INCLUDEDIR}>"
        )

# We do not actually link at this time but, this creates a requirement that will eventually have to be satisfied.
# Anything that links to 'Us', needs these libraries also.
target_link_libraries( ${PROJECT_NAME} ReiserRT_FlyingPhasor::ReiserRT_FlyingPhasor )

# Specify Shared Object used Position Independent Code Major, the Major Version and Debug Prefix.
# NOTE: Additional properties set after Export Header Generated below.
set_target_properties( ${PROJECT_NAME}
        PROPERTIES
        POSITION_INDEPENDENT_CODE 1
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        DEBUG_POSTFIX "_d"
)

target_compile_options( ${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Generate Export Header File
include(GenerateExportHeader)
generate_export_header( ${PROJECT_NAME}
        BASE_NAME "${PROJECT_NAME}"
        EXPORT_MACRO_NAME "${PROJECT_NAME}_EXPORT"
        EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/${INSTALL_INCLUDEDIR}/${PROJECT_NAME}Export.h"
        DEPRECATED_MACRO_NAME "${PROJECT_NAME}_DEPRECATED"
        NO_EXPORT_MACRO_NAME "${PROJECT_NAME}_NO_EXPORT"
        STATIC_DEFINE "${PROJECT_NAME}_STATIC_DEFINE"
        NO_DEPRECATED_MACRO_NAME "${PROJECT_NAME}_NO_DEPRECATED"
        DEFINE_NO_DEPRECATED
        )

# Since we have an EXPORT file, hide everything that isn't explicitly exported.
# Additionally, specify all the public headers. These will be part of the installation.
set_target_properties( ${PROJECT_NAME}
        PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN 1
        PUBLIC_HEADER "${_publicHeaders};${CMAKE_BINARY_DIR}/${INSTALL_INCLUDEDIR}/${PROJECT_NAME}Export.h"
        )

# Process CMake configuration input file which dynamically generates the output CMake configuration files.
# This aides the integration with other CMake client projects that use this project.
# The first part specifies version compatibility for clients and the the second part generates the CMake
# configuration for the development purposes.
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${INSTALL_CMAKEDIR}
)

# Installation of Versioned Shared object library and CMake configuration files.
install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        ARCHIVE DESTINATION ${INSTALL_LIBDIR} COMPONENT lib
        LIBRARY DESTINATION ${INSTALL_LIBDIR} COMPONENT lib
        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT bin
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDEDIR}" COMPONENT dev
)

install(
        EXPORT ${PROJECT_NAME}Targets
        NAMESPACE "${PROJECT_NAME}::"
        DESTINATION ${INSTALL_CMAKEDIR}
        COMPONENT dev
)

install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${INSTALL_CMAKEDIR}
)

