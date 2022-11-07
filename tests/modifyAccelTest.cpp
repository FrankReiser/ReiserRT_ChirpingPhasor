//
// Created by frank on 11/3/22.
//

#include "ChirpingPhasorToneGenerator.h"
#include "MiscTestUtilities.h"

#include <memory>
#include <iostream>

int main()
{
    // An arbitrary epoch dwell in samples.
    constexpr size_t NUM_SAMPLES = 8192;
    constexpr auto initialAccel = M_PI / NUM_SAMPLES / 2;
    constexpr auto secondAccel = -initialAccel / 2;

    int retCode = 0;

    ReiserRT::Signal::ChirpingPhasorToneGenerator chirpGen{ initialAccel };

    // Buffers for an epoch's worth of data for the chirp.
    using SampleType = ReiserRT::Signal::FlyingPhasorElementType;
    std::unique_ptr< SampleType[] > chirpBuf{ new SampleType[ NUM_SAMPLES ] };

    // Do once construct
    do {
        std::cout.precision(6);

        // Get data from each of the tone generator.
        chirpGen.getSamples( chirpBuf.get(), NUM_SAMPLES );

        // Get the Angular Velocity of the Next Sample to be retrieved.
        auto omegaN1 = chirpGen.getOmegaN();
        std::cout << "First Epoch Retrieved. OmegaN = " << omegaN1 << std::endl;
        if ( !inTolerance( omegaN1, initialAccel * NUM_SAMPLES, 1e-10 ) )
        {
            std::cout << "Chirping Phasor FAILS First Epoch Retrieval Omega Test, Expected: " << initialAccel * NUM_SAMPLES
                      << ", Detected: " << omegaN1 << std::endl;
            retCode = 1;
            break;
        }

        // Modify Acceleration to 0 acceleration (coast)
        chirpGen.modifyAccel();

        // Get data from each of the tone generator.
        chirpGen.getSamples( chirpBuf.get(), NUM_SAMPLES );

        // Get the Angular Velocity of the Next Sample to be retrieved.
        // It should be roughly the same as the original angular velocity.
        auto omegaN2 = chirpGen.getOmegaN();
        std::cout << "Second Epoch Retrieved. OmegaN = " << omegaN2 << std::endl;
        if ( !inTolerance( omegaN2, omegaN1, 1e-4 ) )
        {
            std::cout << "Chirping Phasor FAILS Second Epoch Retrieval Omega Test, Expected: " << omegaN1
                      << ", Detected: " << omegaN2 << std::endl;
            retCode = 2;
            break;
        }

        // Modify Acceleration to inverse half of original (decelerate more slowly).
        chirpGen.modifyAccel( secondAccel );

        // Get data from each of the tone generator.
        chirpGen.getSamples( chirpBuf.get(), NUM_SAMPLES );

        // Get the Angular Velocity of the Next Sample to be retrieved.
        // It should be roughly half as the original velocity.
        auto omegaN3 = chirpGen.getOmegaN();
        std::cout << "Third Epoch Retrieved. OmegaN = " << omegaN3 << std::endl;
        if ( !inTolerance( omegaN3, omegaN1 / 2.0, 1e-3 ) )
        {
            std::cout << "Chirping Phasor FAILS Third Epoch Retrieval Omega Test, Expected: " << omegaN1 / 2.0
                      << ", Detected: " << omegaN3 << std::endl;
            retCode = 2;
            break;
        }

    } while (false);

    return retCode;
}