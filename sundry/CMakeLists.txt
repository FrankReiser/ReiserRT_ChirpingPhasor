add_executable( chirpExp "" )
target_sources( chirpExp PRIVATE chirpExp.cpp )
target_include_directories( chirpExp PUBLIC ../src )
target_link_libraries( chirpExp ReiserRT_ChirpingPhasor )
target_compile_options( chirpExp PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

