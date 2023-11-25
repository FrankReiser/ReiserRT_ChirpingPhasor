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
         * This class was developed to replace multiple invocations of, cos( omega(s) + phi ) + j*sin( omega(s) + phi ),
         * where omega(s) is a second order function of sample number in the form,
         * omega(s) = omega0 * s + 0.5 * accel * s^2, with an accelerating rotating phasor.
         * This provides the classic linear 'chirp' with increasing frequency starting from omega0, and
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

            /**
             * @brief Get Sample Operation
             *
             * This operation deliver a single samples from the tone generator. The sample is unscaled
             * (i.e., a magnitude of one).
             *
             * @return Returns next sample value.
             */
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
             * This operation returns the average angular velocity between the next two samples yet to be retrieved.
             * This value is mathematically referred to as 'omega bar'. The instantaneous angular velocities of
             * these next two samples can be deduced by adding or subtracting half the acceleration quantity last
             * programmed into the instance, from the returned value.
             *
             * If this value is allowed to exceed +/-pi radians per sample (nyquist rate), output frequency will 'rollover'.
             * In essence, positive frequency becomes negative, and negative frequency becomes positive.
             * This operation may be used to monitor angular velocity.
             * If it is expected in long running scenarios, that 'rollover' might occur,
             * then acceleration may be halted or modified to prevent 'rollover' by using the modifyAccel operation.
             *
             * @return Returns the average angular velocity between the next two, yet to be retrieved, samples.
             */
            inline FlyingPhasorPrecisionType getOmegaBar() const {
                return std::arg( rate.peakNextSample() );
            }

            /**
             * @brief Modify Acceleration
             *
             * This operation modifies the acceleration value. This may be useful in long
             * running scenarios where they nyquist rate would be reached.
             * This does not affect the value of the next sample, which is already 'baked' into
             * the pipeline, rather the velocity between it and subsequent samples.
             *
             * @note The getOmegaBar operation may be used to monitor the angular velocity for the next samples
             * to be delivered.
             *
             * @param newAccel New acceleration value in radians per sample, per sample. Defaults to zero which
             * halts all acceleration and maintains the last omegaN value from there on out.
             */
            void modifyAccel( double newAccel=0 );

            /**
             * @brief Peek Next Sample
             *
             * This operation exists for uses cases, where querying the current phase of an instance is necessary
             * without 'working' the machine. The phasor state remains unchanged.
             */
            inline const FlyingPhasorElementType & peekNextSample() const { return phasor; }

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
            FlyingPhasorToneGenerator rate;         //!< Dynamic angular rate provider (sample to sample, omegaBar)
            FlyingPhasorElementType phasor;         //!< Phase angle of next sample.
            size_t sampleCounter;                   //!< Tracks sample count used or renormalization purposes.
        };
    }
}


#endif //REISER_RT_CHIRPINGPHASORTONEGENERATOR_H

