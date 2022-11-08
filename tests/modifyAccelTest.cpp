//
// Created by frank on 11/3/22.
//

#include "ChirpingPhasorToneGenerator.h"
#include "MiscTestUtilities.h"

#include <iostream>

int main()
{
    // An arbitrary epoch dwell in samples.
    constexpr auto initialAccel = M_PI / 8192;
    constexpr auto secondAccel = -initialAccel / 2;

    int retCode = 0;

    ReiserRT::Signal::ChirpingPhasorToneGenerator chirpGen{ initialAccel };
    std::cout << "initialAccel = " << initialAccel << std::endl;

    // Do once construct
    do {
        std::cout << std::fixed;
        std::cout.precision(7);

#if 1
        // Get two samples with the initial acceleration value. We only need the second one.
        // we want to capture the omegaBar value before we actually get the second value.
        // The 'getOmegaBar' function obtains the average angular velocity between what we will call sample1
        // and sample2 which we will fetch later.
        chirpGen.getSample();
        const auto omegaBar1 = chirpGen.getOmegaBar();
        const auto sample1 = chirpGen.getSample();
        const auto omegaS1 = omegaBar1 - initialAccel / 2.0;
        const auto thetaS1 = std::arg(sample1 );
        std::cout << "omegaBar1 = " << omegaBar1 << std::endl;
        std::cout << "omegaS1 = " << omegaS1 << std::endl;
        std::cout << "thetaS1 = " << thetaS1 << std::endl;
        if ( !inTolerance( omegaS1, initialAccel, 1e-15 ) )
        {
            std::cout << "Chirping Phasor FAILS OmegaS1 Test, Expected: " << initialAccel * 2
                      << ", Detected: " << omegaS1 << std::endl;
            retCode = 1;
            break;
        }

        // Cancel acceleration. This should not affect the next sample, just those that follow.
        // The ones that follow should have stopped accelerating.
        chirpGen.modifyAccel();

        // Get next sample. This sample should reflect the initial acceleration because it is 'baked' in.
        // We should not see the effect of modifying acceleration until the sample after the next sample.
        const auto sample2 = chirpGen.getSample();
        const auto thetaS2 = std::arg(sample2 );

        // Here we will look at the difference in the last two phase values and calculate an omegaS2.
        const auto omegaBar2 = deltaAngle(thetaS1, thetaS2 );
        const auto omegaS2 = omegaBar2 + initialAccel / 2.0;
        std::cout << "omegaBar2 = " << omegaBar2 << std::endl;
        std::cout << "omegaS2 = " << omegaS2 << std::endl;
        std::cout << "thetaS2 = " << thetaS2 << std::endl;
        if ( !inTolerance( omegaS2, initialAccel * 2.0, 1e-15 ) )
        {
            std::cout << "Chirping Phasor FAILS omegaS2 Test, Expected: " << initialAccel * 2.0
                      << ", Detected: " << omegaS2 << std::endl;
            retCode = 2;
            break;
        }

        // The next sample we retrieve should now reflect the cancelling of acceleration.
        // We should remain at a fixed velocity now.
        const auto omegaBar3 = chirpGen.getOmegaBar();
        const auto omegaS3 = omegaBar3 - 0.0 / 2.0;     // We halted acceleration.
        const auto sample3 = chirpGen.getSample();
        const auto thetaS3 = std::arg( sample3 );
        std::cout << "omegaBar3 = " << omegaBar3 << std::endl;
        std::cout << "omegaS3 = " << omegaS3 << std::endl;
        std::cout << "thetaS3 = " << thetaS3 << std::endl;
        if ( !inTolerance( omegaS3, omegaS2, 1e-15 ) )
        {
            std::cout << "Chirping Phasor FAILS omegaS3 Test, Expected: " << omegaS2
                      << ", Detected: " << omegaS3 << std::endl;
            retCode = 3;
            break;
        }

        // Here we will look at the difference in the last two phase values.
        // It should be equivalent to omegaBar3
        const auto dTheta3 = deltaAngle(thetaS2, thetaS3 );
        std::cout << "dTheta3 = " << dTheta3 << std::endl;
        if ( !inTolerance(dTheta3, omegaBar3, 1e-15 ) )
        {
            std::cout << "Chirping Phasor FAILS dTheta3 Test, Expected: " << omegaBar3
                      << ", Detected: " << dTheta3 << std::endl;
            retCode = 4;
            break;
        }

        // Now slow the thing down.

#else
        // Get data from each of the tone generator.
        chirpGen.getSamples( chirpBuf.get(), NUM_SAMPLES );

        // Get the Angular Velocity of the Next Sample to be retrieved.
        auto omegaS1 = chirpGen.getOmegaBar() - initialAccel / 2;
        std::cout << "First Epoch Retrieved. OmegaN = " << omegaS1 << std::endl;
        if ( !inTolerance( omegaS1, initialAccel * NUM_SAMPLES, 1e-10 ) )
        {
            std::cout << "Chirping Phasor FAILS First Epoch Retrieval Omega Test, Expected: " << initialAccel * NUM_SAMPLES
                      << ", Detected: " << omegaS1 << std::endl;
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
        if ( !inTolerance( omegaN2, omegaS1, 1e-4 ) )
        {
            std::cout << "Chirping Phasor FAILS Second Epoch Retrieval Omega Test, Expected: " << omegaS1
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
        if ( !inTolerance( omegaN3, omegaS1 / 2.0, 1e-3 ) )
        {
            std::cout << "Chirping Phasor FAILS Third Epoch Retrieval Omega Test, Expected: " << omegaS1 / 2.0
                      << ", Detected: " << omegaN3 << std::endl;
            retCode = 3;
            break;
        }
#endif
    } while (false);

    return retCode;
}
