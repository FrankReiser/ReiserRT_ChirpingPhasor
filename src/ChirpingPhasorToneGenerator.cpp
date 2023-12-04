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
  , phasor{ std::polar( 1.0, phi ) }
  , sampleCounter{}
{
    ///@see ChirpingPhasorToneGenerator::reset operation for an discussion on our initialization steps.
}

void ChirpingPhasorToneGenerator::reset( double accel, double omegaZero, double phi )
{
    // Our `phasor` represents our output, the next sample to be retrieved. This
    // is simply initialized with a magnitude of 1.0 at starting phase `phi`.
    phasor = std::polar( 1.0, phi );

    // We store `accelOver2` as it is needed within the `modifyAccel` operation.
    // It is also used to initialize our `rate` attribute below.
    accelOver2 = accel / 2.0;

    // Now for our dynamic `rate` attribute. We start out by noting that our `rate` has no
    // effect on the initial sample retrieved. The initial sample to be retrieved is already
    // baked into our `phasor` attribute, initialized above. Our `rate` attribute effects
    // every subsequent sample retrieved after the initial sample.
    //
    // We initialize our dynamic `rate` attribute with a rate of `accel` and, a phase of
    // `omegaZero + accelOver2`. The `accel` represents how much omega changes each sample but,
    // just like our rate, its internal rate attribute does not affect its first sample delivered.
    // Its first sample delivered is set by its initial phase. This first sample must advance
    // our `phasor` by an omegaBar value equivalent to `omegaZero + accelOver2` for the second sample
    // retrieved from our `phasor`.
    //
    // Post initialization, our rate attribute functions as an angular velocity integrator,
    // effectively implementing, omega(s) = omega0 * s + 0.5 * accel * s^2, via accumulation.
    rate.reset( accel, omegaZero + accelOver2 );

    // Sample counter starts at zero.
    sampleCounter = 0;
}

void ChirpingPhasorToneGenerator::getSamples( FlyingPhasorElementBufferTypePtr pElementBuffer, size_t numSamples )
{
    for ( size_t i = 0; numSamples != i; ++i )
    {
        // We always start with the current phasor to nail the very first sample (s0)
        // and advance (rotate) afterward.
        *pElementBuffer++ = phasor;

        // Now advance (rotate) the phasor by our "dynamic" rate (complex multiply).
        phasor *= rate.getSample();

        // Perform normalization work. This only actually normalized ever other invocation.
        // We invoke it to maintain that part of the state machine.
        normalize();
    }
}

FlyingPhasorElementType ChirpingPhasorToneGenerator::getSample()
{
    // We always start with the current phasor to nail the very first sample (s0)
    // and advance (rotate) afterward.
    const auto retValue = phasor;

    // Now advance (rotate) the phasor by our "dynamic" rate (complex multiply).
    phasor *= rate.getSample();

    // Perform normalization work. This only actually normalized ever other invocation.
    // We invoke it to maintain that part of the state machine.
    normalize();

    return retValue;
}

void ChirpingPhasorToneGenerator::modifyAccel( double newAccel )
{
    // Capture omegaN value of next sample in the pipeline at current acceleration.
    // Value omegaN is our current omegaBar value less current `accelOver2`.
    const auto omegaN = getOmegaBar() - accelOver2;

    // Update `accelOver2` attribute, and reset the rate phasor.
    ///@see ChirpingPhasorToneGenerator::reset function for detailed explanation
    ///of the resetting of the `rate` attribute.
    rate.reset( newAccel, omegaN + ( accelOver2 = newAccel / 2.0 ) );
}
