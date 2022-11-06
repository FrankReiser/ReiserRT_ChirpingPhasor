#include "ChirpingPhasorToneGenerator.h"

#include <iostream>

int main()
{
    // An arbitrary epoch dwell in samples.
    constexpr size_t NUM_SAMPLES = 8192;

    ReiserRT::Signal::ChirpingPhasorToneGenerator chirpGen{ M_PI / NUM_SAMPLES };


    // Write to standard out. It can be redirected.
//    std::cout << std::scientific;
    std::cout.precision(6);
    for ( size_t n = 0; 10 != n; ++n  )
    {
        auto omegaN = chirpGen.getOmegaN();
        auto sample = chirpGen.getSample();
        std::cout
                << "Sample: " << n
                << ", theta = " << std::arg( sample )
                << ", omegaN = " << omegaN
                << std::endl;
    }

    return 0;
}

