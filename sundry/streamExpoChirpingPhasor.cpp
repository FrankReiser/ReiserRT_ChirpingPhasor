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

void testExistingChirpingPhasorAccelZero()
{
    std::cout << "Testing Existing Chirping Phasor accelZero only" << std::endl;
    ChirpingPhasorToneGenerator chirpingPhasorToneGenerator{M_PI / 16384};
    auto omegaBar = chirpingPhasorToneGenerator.getOmegaBar();
    auto sample = chirpingPhasorToneGenerator.getSample();
    std::cout << "Chirp omegaBar0 = " << omegaBar << std::endl;
    std::cout << "Chirp theta0 = " << std::arg( sample ) << std::endl;
    omegaBar = chirpingPhasorToneGenerator.getOmegaBar();
    sample = chirpingPhasorToneGenerator.getSample();
    std::cout << "Chirp omegaBar1 = " << omegaBar << std::endl;
    std::cout << "Chirp theta1 = " << std::arg( sample ) << std::endl;
    omegaBar = chirpingPhasorToneGenerator.getOmegaBar();
    sample = chirpingPhasorToneGenerator.getSample();
    std::cout << "Chirp omegaBar2 = " << omegaBar << std::endl;
    std::cout << "Chirp theta2 = " << std::arg( sample ) << std::endl;
    std::cout << std::endl;
}

void testFlyingPhasorAccelDot()
{
    double accelDot = M_PI / 16384;

    std::cout << "Developing Flying Phasor Accel Dot" << std::endl;
    ChirpingPhasorToneGenerator accelRate{ accelDot, 0.0, accelDot };
    FlyingPhasorElementType omegaBarRate{ std::polar( 1.0, accelDot / 4.0 ) };

    std::cout << "Accel Experiment omegaBar0 = " << std::arg( omegaBarRate ) << std::endl;
    omegaBarRate *= accelRate.getSample();
    std::cout << "Accel Experiment omegaBar1 = " << std::arg( omegaBarRate ) << std::endl;
    omegaBarRate *= accelRate.getSample();
    std::cout << "Accel Experiment omegaBar2 = " << std::arg( omegaBarRate ) << std::endl;
    omegaBarRate *= accelRate.getSample();
    std::cout << "Accel Experiment omegaBar3 = " << std::arg( omegaBarRate ) << std::endl;

    std::cout << std::endl;
}


int main( int argc, char * argv[] )
{
    std::cout << std::scientific;
    std::cout.precision(17);

    testTraditionalExpoPhasorDefaults();    // Checks
    testTraditionalExpoPhasorPhiOnly();     // Checks
    testTraditionalExpoPhasorOmegaZero();   // Checks
    testTraditionalExpoPhasorAccelZero();   // Checks
    testTraditionalExpoPhasorAccelDot();    // Agrees with formula but is the formula good? I think it is.

    testExistingChirpingPhasorAccelZero();  // This looks good but of course, it's been tested already.

    testFlyingPhasorAccelDot();
#if 1
#endif

    ///@note Notes to self:
    ///For an expo chirp, a ChirpingPhasor could serve as 'rate' supplier for accelDot. This should work.
    ///NO I DON'T THINK THAT IS ENOUGH.
    ///
    ///For the 'phasor', it's almost a reimplementation of the chirping phasor.
    ///Should I use inheritance? No, chirping phasor was not really intended to be used that way.
    ///Should I aggregate one? No, because it cannot be controlled efficiently. See above.
    ///None of this addresses the 'rate' supplier for accelZero. We were talking about 'phasor',
    ///which at the end of all this is just a FlyingPhasorElementType (complex<double>).
    ///Here is what I see:

    ///1. The 'phasor', FlyingPhasorElementType
    ///2. The 'rateAccelDot', ChirpingPhasor
    ///3. The 'rateAccelZero', FlyingPhasor
    ///How to initialize all of these? I do not know yet.
    /// The 'phasor' is std::polar(phi)
    /// The 'rateAccelZero', probably just like we did for ChirpingPhasor 'rate' variable.
    /// The 'rateAccelDot', This is the big unknown. Going to need to understand this thoroughly.

    return 0;
    exit( 0 );
}
