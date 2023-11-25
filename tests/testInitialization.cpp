/**
 * @file testInitialization.cpp
 * @brief Initializes ChirpingPhasor Generator and Performs Basic Tests
 *
 * @authors Frank Reiser
 * @date Initiated on Nov 25, 2023
 */

#include "ChirpingPhasorToneGenerator.h"

#include "MiscTestUtilities.h"

#include <iostream>
#include <memory>

using namespace ReiserRT::Signal;

int main()
{
    int retCode = 0;

    std::cout << "Initialization Testing of TSG Flying Phasor Tone Generator" << std::endl;

    // For maximum view of significant digits for diagnostic purposes.
    std::cout << std::scientific;
    std::cout.precision(17);

    do
    {
        // A small buffer for storing two elements
        std::unique_ptr< FlyingPhasorElementType[] > pElementBuf{new FlyingPhasorElementType[2] };

        // Test Default Construction
        {
            // Default construction does not lead to a very useful chirp generator as it is stuck at zero.
            ChirpingPhasorToneGenerator chirpingPhasorToneGenerator{};

            // It's sampleCounter should be zero
            auto sampleCount = chirpingPhasorToneGenerator.getSampleCount();
            if ( 0 != sampleCount )
            {
                std::cout << "Sample Count should be zero and is " <<  sampleCount << std::endl;
                retCode = 1;
                break;
            }

            // Fetch 2 samples.
            chirpingPhasorToneGenerator.getSamples( pElementBuf.get(), 2 );

            // It's sampleCounter should be two
            sampleCount = chirpingPhasorToneGenerator.getSampleCount();
            if ( 2 != sampleCount )
            {
                std::cout << "Sample Count should be two and is " <<  sampleCount << std::endl;
                retCode = 2;
                break;
            }

            // The first sample should have a mag of one and a phase of zero.
            auto phase = std::arg( pElementBuf[0] );
            if ( !inTolerance( phase, 0.0, 1e-12 ) )
            {
                std::cout << "First Sample Phase: " <<  phase << " out of Tolerance! Should be: "
                          << 0.0 << std::endl;
                retCode = 3;
                break;
            }
            auto mag = std::abs( pElementBuf[0] );
            if ( !inTolerance( mag, 1.0, 1e-12 ) )
            {
                std::cout << "First Sample Magnitude: " <<  mag << " out of Tolerance! Should be: "
                          << 1.0 << std::endl;
                retCode = 4;
                break;
            }

            // The second sample should also have a mag of one and a phase of zero (pure DC).
            phase = std::arg( pElementBuf[1] );
            if ( !inTolerance( phase, 0.0, 1e-12 ) )
            {
                std::cout << "Second Sample Phase: " <<  phase << " out of Tolerance! Should be: "
                          << 0.0 << std::endl;
                retCode = 5;
                break;
            }
            mag = std::abs( pElementBuf[1] );
            if ( !inTolerance( mag, 1.0, 1e-12 ) )
            {
                std::cout << "Second Sample Magnitude: " <<  mag << " out of Tolerance! Should be: "
                          << 1.0 << std::endl;
                retCode = 6;
                break;
            }

            // Attempt a reset to something else and test again.
            const auto accel = M_PI / 1024;
            const auto omegaZero = M_PI / 512;
            const auto phi = M_PI / 256;
            chirpingPhasorToneGenerator.reset( accel, omegaZero, phi );

            // It's sampleCounter should be zero
            sampleCount = chirpingPhasorToneGenerator.getSampleCount();
            if ( 0 != sampleCount )
            {
                std::cout << "Sample Count should be zero and is " <<  sampleCount << std::endl;
                retCode = 7;
                break;
            }

            // Peek at the next sample
            const auto peek = chirpingPhasorToneGenerator.peekNextSample();

            // The first sample should have a mag of one and a phase of 1.0.
            phase = std::arg( peek );
            if ( !inTolerance( phase, phi, 1e-12 ) )
            {
                std::cout << "Peek First Sample Phase: " <<  phase << " out of Tolerance! Should be: "
                          << 1.0 << std::endl;
                retCode = 8;
                break;
            }
            mag = std::abs( peek );
            if ( !inTolerance( mag, 1.0, 1e-12 ) )
            {
                std::cout << "Peek First Sample Magnitude: " <<  mag << " out of Tolerance! Should be: "
                          << 1.0 << std::endl;
                retCode = 9;
                break;
            }

            // Fetch two samples
            chirpingPhasorToneGenerator.getSamples( pElementBuf.get(), 2 );

            // The first sample should equal the peeked sample
            if ( peek != pElementBuf[0] )
            {
                std::cout << "First Sample not equal to peeked at sample." << std::endl;
                retCode = 10;
                break;
            }

            // The second sample should be advanced by phi plus omegaZero plus accel over two.
            phase = std::arg( pElementBuf[1] );
            auto expectedPhase = phi + omegaZero + accel / 2.0;
            if ( !inTolerance( phase, expectedPhase, 1e-12 ) )
            {
                std::cout << "Second Sample Phase: " <<  phase << " out of Tolerance! Should be: "
                          << expectedPhase << std::endl;
                retCode = 11;
                break;
            }

            // Test Getting Single Samples. We will reset again with the same parameters
            chirpingPhasorToneGenerator.reset( accel, omegaZero, phi );
            for ( size_t i = 0; 2 != i; ++i )
            {
                const auto sample = chirpingPhasorToneGenerator.getSample();
                if ( sample != pElementBuf[i] )
                {
                    std::cout << "Get Single Sample failed at index " << i << ". Expected " << pElementBuf[i]
                              << ", obtained " << sample << std::endl;
                    retCode = 12;
                    break;
                }
            }
            if ( retCode ) break;
        }
    } while (false);

    exit( retCode );
    return retCode;
}