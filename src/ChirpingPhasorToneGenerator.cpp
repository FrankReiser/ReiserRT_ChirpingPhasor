/**
 * @file ChirpingPhasorToneGenerator.h
 * @brief The implementation file for the Chirping Phasor Tone Generator.
 * @authors Frank Reiser
 * @date Initiated October 27th, 2022
 */

#include "ChirpingPhasorToneGenerator.h"

using namespace ReiserRT::Signal;

ChirpingPhasorToneGenerator::ChirpingPhasorToneGenerator( double accel, double omegaZero, double phi )
  : rate{ accel, initialDeltaTheta( omegaZero, accel ) }
  , phasor{ std::polar(1.0, phi ) }
  , sampleCounter{}
{
}

void ChirpingPhasorToneGenerator::reset( double accel, double omegaZero, double phi )
{
    rate.reset( accel, initialDeltaTheta( omegaZero, accel ) );
    phasor = std::polar(1.0, phi );
    sampleCounter = 0;
}

void ChirpingPhasorToneGenerator::getSamples( FlyingPhasorElementBufferTypePtr pElementBuffer, size_t numSamples )
{
    for ( size_t i = 0; numSamples != i; ++i )
    {
        // We always start with the current phasor to nail the very first sample (s0)
        // and advance (rotate) afterwards.
        *pElementBuffer++ = phasor;

        // Now advance (rotate) the phasor by our "dynamic" rate (complex multiply).
        // We cache a copy of this rate as omegaN. This may be needed for a modAccel invocation
        // or client query.
        phasor *= rate.getSample();

        // Perform normalization
        // Super-fast modulo 2 (for 4, 8, 16..., use 0x3, 0x7, 0xF...)
        if ( ( sampleCounter++ & 0x1 ) == 0x1 )
        {
            // Normally, this would require a sqrt invocation. However, when the sum of squares
            // is near a value of 1, the square root would also be near 1.
            // This is a first order Taylor Series approximation around 1 for the sqrt function.
            // The re-normalization adjustment is a scalar multiply (not complex multiply).
            const double d = 1.0 - ( phasor.real()*phasor.real() + phasor.imag()*phasor.imag() - 1.0 ) / 2.0;
            phasor *= d;
        }
    }
}

void ChirpingPhasorToneGenerator::modifyAccel( double newAccel )
{
    rate.reset( newAccel, initialDeltaTheta( getOmegaN(), newAccel ) );
}
