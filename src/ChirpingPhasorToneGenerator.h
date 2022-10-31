/**
 * @file ChirpingPhasorToneGenerator.h
 * @brief The specification file for the Chirp Phasor Tone Generator.
 * @authors Frank Reiser
 * @date Initiated October 27th, 2022
 */


#ifndef REISER_RT_CHIRPINGPHASORTONEGENERATOR_H
#define REISER_RT_CHIRPINGPHASORTONEGENERATOR_H

#include "ReiserRT_ChirpingPhasorExport.h"

#include "FlyingPhasorToneGenerator.h"

namespace ReiserRT
{
    namespace Signal
    {
        /**
         * @brief Chirping Phasor Tone Generator
         *
         * This was developed to replace multiple invocations of cos( n*s ) + j*sin( n*s ),
         * where omega(s) is a first order function of sample number 's' (e.g. omega = n*s, where n is an
         * acceleration scalar).
         *
         */
        class ReiserRT_ChirpingPhasor_EXPORT ChirpingPhasorToneGenerator
        {
        public:

            /**
             * @brief Constructor
             *
             * Constructs a Chirping Phasor Tone Generator instance.
             *
             * @param accelRadiansPerSamplePerSample
             * @param startingRadiansPerSample
             * @param startingPhase
             */
            explicit ChirpingPhasorToneGenerator(
                    double accelRadiansPerSamplePerSample=0.0,
                    double startingRadiansPerSample=0.0,
                    double startingPhase=0.0 );

            /**
             * @brief Get Samples Operation
             *
             * This operation delivers 'N' number samples from the tone generator into the user provided buffer.
             * The samples are unscaled (i.e., a magnitude of one).
             *
             * @param pElementBuffer User provided buffer large enough to hold the requested number of samples.
             * @param numSamples The number of samples to be delivered.
             */
            void getSamples( FlyingPhasorElementBufferTypePtr pElementBuffer, size_t numSamples );

            void reset(
                    double accelRadiansPerSamplePerSample=0.0,
                    double startingRadiansPerSample=0.0,
                    double startingPhase=0.0 );

            /**
             * @brief Get Sample Counter
             *
             * This operation returns the current value of the sample counter.
             *
             * @return Returns the current value of the sample counter.
             */
            inline size_t getSampleCount() { return sampleCounter; }

        private:
            FlyingPhasorToneGenerator rate;
            FlyingPhasorElementType phasor;
            size_t sampleCounter;
        };
    }
}


#endif //REISER_RT_CHIRPINGPHASORTONEGENERATOR_H

