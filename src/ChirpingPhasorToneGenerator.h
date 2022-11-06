/**
 * @file ChirpingPhasorToneGenerator.h
 * @brief The specification file for the Chirping Phasor Tone Generator.
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
         * This was developed to replace multiple invocations of, cos( f(s) + phi ) + j*sin( f(s) + phi ),
         * where f(s) is a second order function of sample number in the form f(s) = omega0 * s + 0.5 * accel * s^2.
         * It provides the classic 'chirp' of a linearly increasing frequency starting from omega0, and
         * accelerating (or decelerating) from omega0 at a constant acceleration. It makes usage of an internal
         * ReiserRT FlyingPhasorToneGenerator instance to provide a dynamic rate. Otherwise, it's implementation
         * looks almost identical to that of ReiserRT FlyingPhasorToneGenerator which has a fixed rate.
         *
         * Please see documentation for ReiserRT FlyingPhasorToneGenerator for more information.
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

            FlyingPhasorElementType getSample();

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
            inline size_t getSampleCount() const { return sampleCounter; }

            /**
             * @brief Obtain Next Omega Value (Angular Velocity)
             *
             * This operation returns the next value of omega, the angular velocity value that will be delivered.
             * If this value is allowed to exceed +/-pi radians per sample (nyquist rate), output will 'roll'.
             * Positive frequency becomes negative, and negative frequency becomes positive.
             * If it is expected that, in long running scenarios, that this might occur.
             * This operation can be used to monitor angular velocity and, through
             * the modifyAccel operation, acceleration may be halted or modified.
             *
             * @return Returns the next value of omega (angular velocity) of the tone generator to be delivered.
             */
            inline FlyingPhasorPrecisionType getOmegaN() const {
                auto & omegaBarNext = rate.peakNextSample();
                return std::arg( omegaBarNext ) - accelOver2;
            }

            /**
             * @brief Modify Acceleration
             *
             * This operation modifies the current acceleration profile. This may be useful in long
             * running scenarios where they nyquist point would be reached. The getOmegaN operation
             * may be used to monitor the angular velocity.
             *
             * @param newAccel New cceleration in radians per sample, per sample. Defaults to zero.
             */
            void modifyAccel( double newAccel=0 );

        private:
            /**
             * @brief The Normalize Operation.
             *
             * Normalize every N iterations to ensure we maintain a unit vector
             * as rounding errors accumulate. Doing this too often reduces computational performance
             * and not doing it often enough increases noise (phase and amplitude).
             * We are being pretty aggressive as it is at every 2 iterations.
             * By normalizing every two iterations, we push any slight adjustments to the nyquist rate.
             * This means that any spectral spurs created are at the nyquist and hopefully of less
             * consequence. Declared inline here for efficient reuse within the implementation.
             */
            inline void normalize( )
            {
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


        private:
            FlyingPhasorPrecisionType accelOver2;   //!< A useful internal quantity.
            FlyingPhasorToneGenerator rate;         //!< Dynamic angular rate provider
            FlyingPhasorElementType phasor;         //!< Current phase angle
            size_t sampleCounter;                   //!< Tracks sample count used or renormalization purposes.
        };
    }
}


#endif //REISER_RT_CHIRPINGPHASORTONEGENERATOR_H

