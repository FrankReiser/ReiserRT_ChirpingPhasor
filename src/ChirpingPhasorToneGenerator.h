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
         * This was developed to replace multiple invocations of cos( f(s) + phi ) + j*sin( f(s) + phi ),
         * where f(s) is a second order function of sample number 's' in the form f(s) = omega0*s + 0.5*accel*s^2).
         * It provides the classic 'chirp' of a linearly increasing frequency starting from omega0, and
         * accelerating (or decelerating) from omega0 at a constant rate.
         */
        class ReiserRT_ChirpingPhasor_EXPORT ChirpingPhasorToneGenerator
        {
        public:

            /**
             * @brief Constructor
             *
             * Constructs a Chirping Phasor Tone Generator instance.
             *
             * @param accel Acceleration in radians per sample, per sample.
             * @param omegaZero Starting angular velocity in radians per sample.
             * @param phi Starting phase angle in radians.
             */
            explicit ChirpingPhasorToneGenerator( double accel=0.0, double omegaZero=0.0, double phi=0.0 );

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

            /**
             * @brief Reset Operation
             *
             * This operation resets the chirping phasor as if it were just constructed with the parameters.
             * This allows reuse of an existing instance for a differing run including
             * setting the sample counter to zero.
             *
             * @param accel Acceleration in radians per sample, per sample.
             * @param omegaZero Starting angular velocity in radians per sample.
             * @param phi Starting phase angle in radians.
             */
            void reset( double accel=0.0, double omegaZero=0.0, double phi=0.0 );

            /**
             * @brief Get Sample Counter
             *
             * This operation returns the current value of the sample counter.
             *
             * @return Returns the current value of the sample counter.
             */
            inline size_t getSampleCount() { return sampleCounter; }

            ///@todo Implement a setAccel function that can be called at any time between `getSamples`
            ///operations to provide a new acceleration profile or to cancel any previous acceleration.
            ///Can this be done? I believe it can but would need to be verified.

        private:
            FlyingPhasorToneGenerator rate;     //!< Dynamic angular rate provider is a FlyingPhasorToneGenerator
            FlyingPhasorElementType phasor;     //!< Current phase angle
            size_t sampleCounter;               //!< Tracks sample count.
        };
    }
}


#endif //REISER_RT_CHIRPINGPHASORTONEGENERATOR_H

