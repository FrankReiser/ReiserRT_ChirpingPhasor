/**
 * @file ChirpingPhasorToneGenerator.h
 * @brief The implementation file for the Chirp Phasor Tone Generator.
 * @authors Frank Reiser
 * @date Initiated October 27th, 2022
 */

#include "ChirpingPhasorToneGenerator.h"

using namespace TSG_NG;

ChirpingPhasorToneGenerator::ChirpingPhasorToneGenerator( double startingRadiansPerSample,
    double startingPhase, double thePhaseAccel )
  : rate{ std::polar( 1.0, startingRadiansPerSample ) }
  , phasor{ FlyingPhasorElementType{1.0, 0.0 } * std::polar(1.0, startingPhase ) }
  , phaseAccel{ thePhaseAccel }
  , sampleCounter{}
{
}

void ChirpingPhasorToneGenerator::reset( double startingRadiansPerSample, double startingPhase, double thePhaseAccel )
{
    rate = std::polar( 1.0, startingRadiansPerSample );
    phasor = FlyingPhasorElementType{ 1.0, 0.0 } * std::polar( 1.0, startingPhase );
    phaseAccel = thePhaseAccel;
    sampleCounter = 0;
}
