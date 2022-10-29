add_executable( chirpExp "" )
target_sources( chirpExp PRIVATE chirpExp.cpp )
target_include_directories( chirpExp PUBLIC ../src )
target_link_libraries( chirpExp ReiserRT_ChirpingPhasor )

