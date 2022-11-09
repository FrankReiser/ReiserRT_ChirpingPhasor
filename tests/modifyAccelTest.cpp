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

        // Here we will look at the difference in the last two phase values and calculate an omega.
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

        // Now use the second acceleration value. This one should be negative and half the magnitude of the initial.
        chirpGen.modifyAccel( secondAccel );

        // Get next sample. This sample should reflect the zero (canceled) acceleration because it is 'baked' in.
        // We should not see the effect of modifying acceleration until the sample after the next sample.
        const auto sample4 = chirpGen.getSample();
        const auto thetaS4 = std::arg(sample4 );

        // Here we will look at the difference in the last two phase values and calculate an omega.
        const auto omegaBar4 = deltaAngle( thetaS3, thetaS4 );
        const auto omegaS4 = omegaBar4 - 0 / 2.0;        // We had formerly halted acceleration.
        std::cout << "omegaBar4 = " << omegaBar4 << std::endl;
        std::cout << "omegaS4 = " << omegaS4 << std::endl;
        std::cout << "thetaS4 = " << thetaS4 << std::endl;
        if ( !inTolerance( omegaS4, omegaS3, 1e-15 ) )
        {
            std::cout << "Chirping Phasor FAILS omegaS4 Test, Expected: " << omegaS3
                      << ", Detected: " << omegaS4 << std::endl;
            retCode = 5;
            break;
        }

        // The next sample we retrieve should now reflect the second acceleration value.
        const auto omegaBar5 = chirpGen.getOmegaBar();
        const auto omegaS5 = omegaBar5 - secondAccel / 2.0;
        const auto sample5 = chirpGen.getSample();
        const auto thetaS5 = std::arg( sample5 );
        std::cout << "omegaBar5 = " << omegaBar5 << std::endl;
        std::cout << "omegaS5 = " << omegaS5 << std::endl;
        std::cout << "thetaS5 = " << thetaS5 << std::endl;
        const double expectedDeltaTheta5 = omegaS4 + secondAccel / 2.0;
        std::cout << "expectedDeltaTheta5 = " << expectedDeltaTheta5 << std::endl;
        const auto dTheta5 = deltaAngle(thetaS4, thetaS5 );
        if ( !inTolerance(dTheta5, expectedDeltaTheta5, 1e-15 ) )
        {
            std::cout << "Chirping Phasor FAILS dTheta5 Test, Expected: " << expectedDeltaTheta5
                      << ", Detected: " << dTheta5 << std::endl;
            retCode = 6;
            break;
        }

    } while (false);

    return retCode;
}
