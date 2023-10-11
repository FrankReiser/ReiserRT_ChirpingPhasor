add_executable( chirpPurityTest "" )
target_sources( chirpPurityTest PRIVATE chirpPurityTest.cpp)
target_include_directories( chirpPurityTest PUBLIC ../src ../testUtilities )
target_link_libraries( chirpPurityTest ReiserRT_ChirpingPhasor TestUtilities )
target_compile_options( chirpPurityTest PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
add_test( NAME runChirpPurityTest COMMAND $<TARGET_FILE:chirpPurityTest> )

add_executable( detailedChirpTest "" )
target_sources( detailedChirpTest PRIVATE detailedChirpTest.cpp )
target_include_directories( detailedChirpTest PUBLIC ../src ../testUtilities )
target_link_libraries( detailedChirpTest ReiserRT_ChirpingPhasor TestUtilities )
target_compile_options( detailedChirpTest PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
add_test( NAME runDetailedChirpTest1 COMMAND $<TARGET_FILE:detailedChirpTest> )
add_test( NAME runDetailedChirpTest2 COMMAND $<TARGET_FILE:detailedChirpTest> --omegaZero=0.5 )
add_test( NAME runDetailedChirpTest3 COMMAND $<TARGET_FILE:detailedChirpTest> --phi=0.5 )
add_test( NAME runDetailedChirpTest4 COMMAND $<TARGET_FILE:detailedChirpTest> --omegaZero=0.5 --phi=0.5 )
add_test( NAME runDetailedChirpTest5 COMMAND $<TARGET_FILE:detailedChirpTest> --omegaZero=0.5 --phi=-0.5 )
add_test( NAME runDetailedChirpTest6 COMMAND $<TARGET_FILE:detailedChirpTest> --omegaZero=-0.5 --phi=0.5 )
add_test( NAME runDetailedChirpTest7 COMMAND $<TARGET_FILE:detailedChirpTest> --omegaZero=-0.5 --phi=-0.5 )

add_executable( modifyAccelTest "" )
target_sources( modifyAccelTest PRIVATE modifyAccelTest.cpp)
target_include_directories( modifyAccelTest PUBLIC ../src ../testUtilities )
target_link_libraries( modifyAccelTest ReiserRT_ChirpingPhasor TestUtilities )
target_compile_options( modifyAccelTest PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
add_test( NAME runModifyAccelTest COMMAND $<TARGET_FILE:modifyAccelTest> )
