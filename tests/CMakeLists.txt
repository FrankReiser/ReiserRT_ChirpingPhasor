add_executable( simpleChirpTest "" )
target_sources( simpleChirpTest PRIVATE simpleChirpTest.cpp )
target_include_directories( simpleChirpTest PUBLIC ../src ../testUtilities )
target_link_libraries( simpleChirpTest ReiserRT_ChirpingPhasor TestUtilities )
add_test( NAME runSimpleChirpTest COMMAND $<TARGET_FILE:simpleChirpTest> )

