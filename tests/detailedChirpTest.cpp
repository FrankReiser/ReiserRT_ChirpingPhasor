
#include "CommandLineParser.h"
#include "MiscTestUtilities.h"
#include "ChirpingPhasorToneGenerator.h"

#include <iostream>
#include <memory>

int main( int argc, char * argv[] )
{
    // An arbitrary epoch dwell in samples.
    constexpr size_t NUM_SAMPLES = 4;

    int retCode = 0;

    // Parse potential command line. Defaults provided otherwise.
    CommandLineParser cmdLineParser{};
    if ( 0 != cmdLineParser.parseCommandLine(argc, argv) )
    {
        std::cout << "Failed parsing command line" << std::endl;
        std::cout << "Optional Arguments are:" << std::endl;
        std::cout << "\t--accel=<double>: The acceleration to use in radians per sample^2." << std::endl;
        std::cout << "\t--omegaZero=<double>: The initial angular velocity to use in radians per sample." << std::endl;
        std::cout << "\t--phi=<double>: The initial phase angle in radians." << std::endl;

        exit( -1 );
    }
#if 0
    else
    {
        std::cout << "Parsed: --accel=" << cmdLineParser.getAccel()
                  << " --omegaZero=" << cmdLineParser.getOmegaZero()
                  << " --phi=" << cmdLineParser.getPhi() << std::endl << std::endl;
    }
#endif

    const auto accelIn = cmdLineParser.getAccel();
    const auto omegaZeroIn = cmdLineParser.getOmegaZero();
    const auto phiIn = cmdLineParser.getPhi();

    ReiserRT::Signal::ChirpingPhasorToneGenerator chirpGen{ accelIn, omegaZeroIn, phiIn };

    // Buffers for an epoch's worth of data for the chirp.
    using SampleType = ReiserRT::Signal::FlyingPhasorElementType;
    std::unique_ptr< SampleType[] > chirpBuf{ new SampleType[ NUM_SAMPLES ] };

    // Get data from each of the tone generators.
    chirpGen.getSamples( chirpBuf.get(), NUM_SAMPLES );

    // Testing
    do {
        // The First Sample shall have a phase of phi
        auto thetaZero = std::arg( chirpBuf[0] );
        if ( !inTolerance( thetaZero, phiIn, 1e-40 ) )
        {
            std::cout << "Chirping Phasor FAILS initial phase test! Expected: " << phiIn
                      << ", Detected: " << thetaZero << std::endl;
            retCode = 1;
            break;
        }

        // The Second Sample shall have rotated by omegaZero plus half of the acceleration quantity.
        auto thetaOne = std::arg( chirpBuf[1] );
        auto deltaTheta = deltaAngle( thetaZero, thetaOne );
        auto expectedDeltaTheta = omegaZeroIn + 0.5 * accelIn;
        if ( !inTolerance( deltaTheta, expectedDeltaTheta, 1e-12 ) )
        {
            std::cout << "Chirping Phasor FAILS omegaZero test at second sample! Expected: " << expectedDeltaTheta
                      << ", Detected: " << deltaTheta << std::endl;
            retCode = 2;
            break;
        }

        // The Third Sample shall have rotated by omegaZero * 2 plus half of the acceleration quantity * 2^2.
        auto thetaTwo = std::arg( chirpBuf[2] );
        deltaTheta = deltaAngle( thetaZero, thetaTwo );
        expectedDeltaTheta = omegaZeroIn * 2 + 0.5 * accelIn * 2*2;
        if ( !inTolerance( deltaTheta, expectedDeltaTheta, 1e-12 ) )
        {
            std::cout << "Chirping Phasor FAILS omegaZero test at third sample! Expected: " << expectedDeltaTheta
                      << ", Detected: " << deltaTheta << std::endl;
            retCode = 3;
            break;
        }

        // The Fourth Sample shall have rotated by omegaZero * 3 plush half the acceleration quantity * 3^3
        auto thetaThree = std::arg( chirpBuf[3] );
        deltaTheta = deltaAngle( thetaZero, thetaThree );
        expectedDeltaTheta = omegaZeroIn * 3 + 0.5 * accelIn * 3*3;
        if ( !inTolerance( deltaTheta, expectedDeltaTheta, 1e-12 ) )
        {
            std::cout << "Chirping Phasor FAILS omegaZero test at forth sample! Expected: " << expectedDeltaTheta
                      << ", Detected: " << deltaTheta << std::endl;
            retCode = 4;
            break;
        }

    } while (false);


    return retCode;
}