/**
 * @file ChirpingPhasorToneGenerator.h
 * @brief The implementation file for the Chirping Phasor Tone Generator.
 * @authors Frank Reiser
 * @date Initiated October 27th, 2022
 */

#include "ChirpingPhasorToneGenerator.h"

using namespace ReiserRT::Signal;

ChirpingPhasorToneGenerator::ChirpingPhasorToneGenerator( double accel, double omegaZero, double phi )
  : accelOver2{ accel / 2.0 }
  , rate{ accel, omegaZero + accelOver2 }
  , phasor{ std::polar(1.0, phi ) }
  , sampleCounter{}
{
}

void ChirpingPhasorToneGenerator::reset( double accel, double omegaZero, double phi )
{
    accelOver2 = accel / 2.0;
    rate.reset( accel, omegaZero + accelOver2 );
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
        normalize();
    }
}

FlyingPhasorElementType ChirpingPhasorToneGenerator::getSample()
{
    // We always start with the current phasor to nail the very first sample (s0)
    // and advance (rotate) afterwards.
    const auto retValue = phasor;

    // Now advance (rotate) the phasor by our "dynamic" rate (complex multiply).
    // We cache a copy of this rate as omegaN. This may be needed for a modAccel invocation
    // or client query.
    phasor *= rate.getSample();

    // Perform normalization
    normalize();

    return retValue;
}

void ChirpingPhasorToneGenerator::modifyAccel( double newAccel )
{
    // Capture omega value of next sample in the pipeline at current acceleration.
    // This is essentially a new omega zero value.
    const auto omegaN = getOmegaBar() - accelOver2;

    // Update acceleration divided by 2 attribute, and reset the rate phasor.
    rate.reset( newAccel, omegaN + ( accelOver2 = newAccel / 2.0 ) );
}
