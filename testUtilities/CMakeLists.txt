add_library( TestUtilities STATIC "" )
target_sources( TestUtilities PRIVATE MiscTestUtilities.cpp CommandLineParser.cpp )
target_compile_options( TestUtilities PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)


