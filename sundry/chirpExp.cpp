#include "ChirpingPhasorToneGenerator.h"

#include <iostream>

int main()
{
    // An arbitrary epoch dwell in samples.
    constexpr size_t NUM_SAMPLES = 8192;
    constexpr double accel = M_PI / NUM_SAMPLES;
    constexpr double accelOverTwo = accel / 2.0;

    ReiserRT::Signal::ChirpingPhasorToneGenerator chirpGen{ accel };


    // Write to standard out. It can be redirected.
//    std::cout << std::scientific;
    std::cout.precision(6);
    for ( size_t n = 0; 10 != n; ++n  )
    {
        auto omegaN = chirpGen.getOmegaBar() - accelOverTwo;
        auto sample = chirpGen.getSample();
        std::cout
                << "Sample: " << n
                << ", theta = " << std::arg( sample )
                << ", omegaN = " << omegaN
                << std::endl;
    }

    return 0;
}

