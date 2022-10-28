/**
 * @file ChirpingPhasorToneGenerator.h
 * @brief The specification file for the Chirp Phasor Tone Generator.
 * @authors Frank Reiser
 * @date Initiated October 27th, 2022
 */


#ifndef REISER_RT_CHIRPINGPHASORTONEGENERATOR_H
#define REISER_RT_CHIRPINGPHASORTONEGENERATOR_H

#include "ReiserRT_ChirpingPhasorExport.h"

#include <complex>

namespace TSG_NG
{
    class ReiserRT_ChirpingPhasor_EXPORT ChirpingPhasorToneGenerator
    {
    public:
        using PrecisionType = double;
        using FlyingPhasorElementType = std::complex< PrecisionType >;
        using FlyingPhasorElementBufferTypePtr = FlyingPhasorElementType *;

        explicit ChirpingPhasorToneGenerator(
                double startingRadiansPerSample=0.0,
                double startingPhase=0.0,
                double thePhaseAccel=0.0 );

        void reset(
                double startingRadiansPerSample=0.0,
                double startingPhase=0.0,
                double thePhaseAccel=0.0 );

    private:
        FlyingPhasorElementType rate;
        FlyingPhasorElementType phasor;
        double phaseAccel;
        size_t sampleCounter;
    };
}


#endif //REISER_RT_CHIRPINGPHASORTONEGENERATOR_H

