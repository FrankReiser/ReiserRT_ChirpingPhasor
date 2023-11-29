add_executable( streamChirpingPhasor "" )
target_sources( streamChirpingPhasor PRIVATE streamChirpingPhasor.cpp)
target_include_directories( streamChirpingPhasor PUBLIC ../src ../testUtilities )
target_link_libraries( streamChirpingPhasor ReiserRT_ChirpingPhasor TestUtilities )
target_compile_options( streamChirpingPhasor PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_executable( streamExpoChirpingPhasor "" )
target_sources( streamExpoChirpingPhasor PRIVATE streamExpoChirpingPhasor.cpp)
target_include_directories( streamExpoChirpingPhasor PUBLIC ../src ../testUtilities )
target_link_libraries( streamExpoChirpingPhasor ReiserRT_ChirpingPhasor TestUtilities )
#[[
target_compile_options( streamExpoChirpingPhasor PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
#]]

# What this does is set up a relative path where we expect our custom libraries to be
# It will be used to patch the installation to find libraries relative to the binary.
file( RELATIVE_PATH _rel ${CMAKE_INSTALL_PREFIX}/${INSTALL_BINDIR} ${CMAKE_INSTALL_PREFIX})
set( _rpath "\$ORIGIN/${_rel}" )
file( TO_NATIVE_PATH "${_rpath}/${INSTALL_LIBDIR}" MY_LIBRARY_RPATH )
message( STATUS "MY_LIBRARY_RPATH: ${MY_LIBRARY_RPATH}" )

# Install streamFlyingPhasorGen and streamLegacyPhasorGen with RPATH setting
set_target_properties( streamChirpingPhasor
        PROPERTIES
        SKIP_BUILD_RPATH OFF
        BUILD_WITH_INSTALL_RPATH OFF
        INSTALL_RPATH "${MY_LIBRARY_RPATH}"
        INSTALL_RPATH_USE_LINK_PATH ON
)

# Installation of our binary executable component.
install(
        TARGETS streamChirpingPhasor
        RUNTIME DESTINATION ${INSTALL_BINDIR} COMPONENT bin
)
