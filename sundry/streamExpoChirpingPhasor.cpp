/**
 * @file streamExpoChirpingPhasor.cpp
 * @brief Utility program for generating output from a ExpoChirpingPhasor for whatever analytical purposes.
 *
 * @authors Frank Reiser
 * @date Initiated on Nov 29, 2023
 */

#include <iostream>

#include "ChirpingPhasorToneGenerator.h"

using namespace ReiserRT::Signal;

class TraditionalExpoPhasor
{
public:
    explicit TraditionalExpoPhasor( double theAccelZero=0.0, double theAccelDot=0.0, double theOmegaZero=0.0,
                                    double thePhi=0.0 )
      : accelZero{ theAccelZero }
      , accelDot{ theAccelDot }
      , omegaZero{ theOmegaZero }
      , phi{ thePhi }
    {
    }

    FlyingPhasorElementType getSample()
    {
        FlyingPhasorElementType phasor{ std::polar( 1.0, getTheta() ) };

        return phasor;
    }

private:
    double getTheta()
    {
        double sampleCounterSquared = double( sampleCounter )  * double ( sampleCounter );

        auto theta =
            phi +
            omegaZero * double( sampleCounter ) +
            accelZero * sampleCounterSquared / 2.0 +
            accelDot * sampleCounterSquared * double( sampleCounter ) / 6.0;

        ++sampleCounter;
        return theta;
    }

private:
    double accelZero{};
    double accelDot{};
    double omegaZero{};
    double phi{};
    size_t sampleCounter{};
};

void testTraditionalExpoPhasorDefaults()
{
    TraditionalExpoPhasor traditionalExpoPhasor{};

    std::cout << "Testing All Arguments Zero (default)" << std::endl;
    auto sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta0 = " << std::arg( sample ) << std::endl;
    sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta1 = " << std::arg( sample ) << std::endl;
    sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta2 = " << std::arg( sample ) << std::endl;
    std::cout << std::endl;
}

void testTraditionalExpoPhasorPhiOnly()
{
    TraditionalExpoPhasor traditionalExpoPhasor{ 0.0, 0.0, 0.0, 1.0 };

    std::cout << "Testing phi only" << std::endl;
    auto sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta0 = " << std::arg( sample ) << std::endl;
    sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta1 = " << std::arg( sample ) << std::endl;
    sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta2 = " << std::arg( sample ) << std::endl;
    std::cout << std::endl;
}

void testTraditionalExpoPhasorOmegaZero()
{
    TraditionalExpoPhasor traditionalExpoPhasor{ 0.0, 0.0, M_PI / 256 };

    std::cout << "Testing omegaZero only" << std::endl;
    auto sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta0 = " << std::arg( sample ) << std::endl;
    sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta1 = " << std::arg( sample ) << std::endl;
    sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta2 = " << std::arg( sample ) << std::endl;
    std::cout << std::endl;
}

void testTraditionalExpoPhasorAccelZero()
{
    TraditionalExpoPhasor traditionalExpoPhasor{ M_PI / 16384 };

    std::cout << "Testing accelZero only" << std::endl;
    auto sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta0 = " << std::arg( sample ) << std::endl;
    sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta1 = " << std::arg( sample ) << std::endl;
    sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta2 = " << std::arg( sample ) << std::endl;
    std::cout << std::endl;
}

void testTraditionalExpoPhasorAccelDot()
{
    TraditionalExpoPhasor traditionalExpoPhasor{ 0.0, M_PI / 16384 };

    std::cout << "Testing accelDot only" << std::endl;
    auto sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta0 = " << std::arg( sample ) << std::endl;
    sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta1 = " << std::arg( sample ) << std::endl;
    sample = traditionalExpoPhasor.getSample();
    std::cout << "Default theta2 = " << std::arg( sample ) << std::endl;
    std::cout << std::endl;
}



int main( int argc, char * argv[] )
{
    testTraditionalExpoPhasorDefaults();    // Checks
    testTraditionalExpoPhasorPhiOnly();     // Checks
    testTraditionalExpoPhasorOmegaZero();   // Checks

#if 0
    ChirpingPhasorToneGenerator chirpingPhasorToneGenerator{M_PI / 16384};
    auto sample = chirpingPhasorToneGenerator.getSample();
    std::cout << "Chirp theta0 = " << std::arg( sample ) << std::endl;
    sample = chirpingPhasorToneGenerator.getSample();
    std::cout << "Chirp theta1 = " << std::arg( sample ) << std::endl;
    sample = chirpingPhasorToneGenerator.getSample();
    std::cout << "Chirp theta2 = " << std::arg( sample ) << std::endl;
#endif
    testTraditionalExpoPhasorAccelZero();   // Checks
    testTraditionalExpoPhasorAccelDot();    // Agrees with formula but is the formula good. I think it is.

    return 0;
    exit( 0 );
}
