/**
 * @file ChirpingPhasorToneGenerator.h
 * @brief The implementation file for the Chirp Phasor Tone Generator.
 * @authors Frank Reiser
 * @date Initiated October 27th, 2022
 */

#include "ChirpingPhasorToneGenerator.h"

using namespace ReiserRT::Signal;

ChirpingPhasorToneGenerator::ChirpingPhasorToneGenerator(
        double accelRadiansPerSamplePerSample, double startingRadiansPerSample, double startingPhase )
  : rate{ accelRadiansPerSamplePerSample, accelRadiansPerSamplePerSample }
  , phasor{ FlyingPhasorElementType{1.0, 0.0 } * std::polar(1.0, startingPhase ) }
  , sampleCounter{}
{
}

void ChirpingPhasorToneGenerator::reset(  double accelRadiansPerSamplePerSample, double startingRadiansPerSample, double startingPhase )
{
    rate = FlyingPhasorToneGenerator{ accelRadiansPerSamplePerSample, accelRadiansPerSamplePerSample };
    phasor = FlyingPhasorElementType{ 1.0, 0.0 } * std::polar( 1.0, startingPhase );
    sampleCounter = 0;
}

void ChirpingPhasorToneGenerator::getSamples( FlyingPhasorElementBufferTypePtr pElementBuffer, size_t numSamples )
{
    for ( size_t i = 0; numSamples != i; ++i )
    {
        // We always start with the current phasor to nail the very first sample (s0)
        // and advance (rotate) afterwards.
        *pElementBuffer++ = phasor;

        // Now advance (rotate) the phasor by our "dynamic" rate (complex multiply)
        phasor *= rate.getSample();

        // Perform normalization
        // Super-fast modulo 2 (for 4, 8, 16..., use 0x3, 0x7, 0xF...)
        ///@todo Considering "== 0x0" so it's alternating with the "rate" FlyingPhasor normalization cycle.
        if ( ( sampleCounter++ & 0x1 ) == 0x0 )
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