//
// Created by frank on 11/3/22.
//

#include "ChirpingPhasorToneGenerator.h"

#include <memory>
#include <iostream>

int main()
{
    // An arbitrary epoch dwell in samples.
    constexpr size_t NUM_SAMPLES = 4;
    constexpr auto initialAccel = M_PI / NUM_SAMPLES;
    constexpr auto secondAccel = M_PI / NUM_SAMPLES / 2;

    int retCode = 0;

    ReiserRT::Signal::ChirpingPhasorToneGenerator chirpGen{ initialAccel };

    // Buffers for an epoch's worth of data for the chirp.
    using SampleType = ReiserRT::Signal::FlyingPhasorElementType;
    std::unique_ptr< SampleType[] > chirpBuf{ new SampleType[ NUM_SAMPLES ] };

    // Get data from each of the tone generator.
    chirpGen.getSamples( chirpBuf.get(), NUM_SAMPLES );

    // Do once construct
    do {
        // Omega N should equal initialAccel * N to a high degree.
        auto expectedOmegaN = initialAccel * (NUM_SAMPLES-1);

        // Get the phase of the last sample and query the angular velocity
        auto endPhase = std::arg( chirpBuf[NUM_SAMPLES-1] );
//        auto omegaN = chirpGen.getOmegaN();

//        if ( expectedOmegaN != omegaN )
//            std::cout << "BLAH" << std::endl;


    } while (false);

    return retCode;
}