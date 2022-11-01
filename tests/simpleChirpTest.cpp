//
// Created by frank on 10/29/22.
//

#include "ChirpingPhasorToneGenerator.h"
#include "MiscTestUtilities.h"

#include <memory>
#include <iostream>
#include <limits>

using namespace ReiserRT::Signal;

// Performs "Running/Online" statistics accumulation.
// Implements the Welford's "Online" in a state machine plus additional statistics.
// This algorithm is much less prone to loss of precision due to catastrophic cancellation.
// Additionally, it uses "long double" format for mathematics and state to better compute
// variance from small deviations in the input train.
class StatsStateMachine
{
public:
    void addSample( double value )
    {
        long double delta = value - mean;
        ++nSamples;
        mean += delta / (long double)( nSamples );
        M2 += delta * ( value - mean );

        delta = value - mean;
        if (delta < maxNegDev ) maxNegDev = delta;
        if (delta > maxPosDev ) maxPosDev = delta;
    }

    // Currently, returns mean and variance
    std::pair<double, double> getStats() const
    {
        switch ( nSamples )
        {
            case 0 : return { std::numeric_limits<long double>::quiet_NaN(), std::numeric_limits<long double>::quiet_NaN() };
            case 1 : return { mean, std::numeric_limits<long double>::quiet_NaN() };
            default : return { mean, M2 / (long double)(nSamples-1) };
        }
    }
    std::pair<double, double> getMinMaxDev() const
    {
        switch ( nSamples )
        {
            case 0 : return { std::numeric_limits<long double>::quiet_NaN(), std::numeric_limits<long double>::quiet_NaN() };
            default : return {maxNegDev, maxPosDev };
        }
    }

    void reset()
    {
        mean = 0.0;
        M2 = 0.0;
        maxNegDev = std::numeric_limits< double >::max();
        maxPosDev = std::numeric_limits< double >::lowest();
        nSamples = 0;
    }

private:
    long double mean{ 0.0 };
    long double M2{ 0.0 };
    long double maxNegDev{std::numeric_limits< long double >::max() };
    long double maxPosDev{std::numeric_limits< long double >::lowest() };
    size_t nSamples{};
};

class PhasePurityAnalyzer
{
public:

    void analyzePhaseStability( const FlyingPhasorElementBufferTypePtr & pBuf, size_t nSamples,
                                double radiansPerSamplePerSample )
    {
        // Reset stats in case an instance is re-run.
        statsStateMachine.reset();

        double prevOmega = 0.0;
        for ( size_t n=0; nSamples != n; ++n )
        {

            // We cheat the first sample because there is no previous one in order to compute
            // a delta.
            if ( 0 == n )
            {
                // Add sample for n = 0 based on expected acceleration value.
                statsStateMachine.addSample( radiansPerSamplePerSample );
            }
            else
            {
                const auto testSamplePhase = std::arg( pBuf[ n ] );
                const auto prevTestSamplePhase = std::arg(pBuf[ n-1 ] );

                // Calculate omega mean (omegaBar) over the course of 1 sample.
                // This is simple the delta angle, in radians per sample.
                const auto omegaBar = deltaAngle(prevTestSamplePhase, testSamplePhase );

                // Calculate omega(n) for this sample based knowledge of previous omega and
                // equation,  omegaBar = ( omega(n) + omega(n-1) ) / 2, solved for omega(n).
                const auto omega = 2 * omegaBar - prevOmega;

                // Acceleration is the value of omega(n) divided by n.
                const auto accel = omega / double( n );

                // Track previous omega for next iteration.
                prevOmega = omega;

                // Add sample to statistics state machine.
                statsStateMachine.addSample( accel );
            }
        }
    }

    std::pair<double, double> getStats() const { return statsStateMachine.getStats(); }
    std::pair<double, double> getMinMaxDev() const { return statsStateMachine.getMinMaxDev(); }

private:
    StatsStateMachine statsStateMachine{};
};

class MagPurityAnalyzer
{
public:
    void analyzeMagnitudeStability( const FlyingPhasorElementBufferTypePtr & pBuf, size_t nSamples )
    {
        // Reset stats in case an instance is re-run.
        statsStateMachine.reset();

        for ( size_t n=0; nSamples != n; ++n )
        {
            auto mag = std::abs( pBuf[ n ] );

            statsStateMachine.addSample( mag );
        }
    }

    std::pair<double, double> getStats() const { return statsStateMachine.getStats(); }
    std::pair<double, double> getMinMaxDev() const { return statsStateMachine.getMinMaxDev(); }

private:
    StatsStateMachine statsStateMachine{};
};

int main()
{
    // An arbitrary epoch dwell in samples.
    constexpr size_t NUM_SAMPLES = 8192;

    const auto accelRadiansPerSamplePerSample = M_PI / NUM_SAMPLES;

    ReiserRT::Signal::ChirpingPhasorToneGenerator chirpGen{ accelRadiansPerSamplePerSample };

    // Buffers for an epoch's worth of data for the chirp.
    using SampleType = ReiserRT::Signal::FlyingPhasorElementType;
    std::unique_ptr< SampleType[] > chirpBuf{ new SampleType[ NUM_SAMPLES ] };

    // Get data from each of the tone generators.
    chirpGen.getSamples( chirpBuf.get(), NUM_SAMPLES );

    // Phase Purity Test
    PhasePurityAnalyzer phasePurityAnalyzer{};
    phasePurityAnalyzer.analyzePhaseStability( chirpBuf.get(), NUM_SAMPLES, accelRadiansPerSamplePerSample );
    const auto phaseAccelStats = phasePurityAnalyzer.getStats();
    const auto phaseAccelMinMax = phasePurityAnalyzer.getMinMaxDev();
    const auto phaseAccelPeakAbsDev = std::max(-phaseAccelMinMax.first, phaseAccelMinMax.second );
    std::cout << "Mean Acceleration (radsPerSample^2): " << phaseAccelStats.first << ", Variance: " << phaseAccelStats.second << std::endl;
    std::cout << "Acceleration Noise: maxNegDev: " << phaseAccelMinMax.first << ", maxPosDev: "
              << phaseAccelMinMax.second << ", maxAbsDev: " << phaseAccelPeakAbsDev << std::endl;

    MagPurityAnalyzer magPurityAnalyzer{};
    magPurityAnalyzer.analyzeMagnitudeStability( chirpBuf.get(), NUM_SAMPLES );
    const auto magStats = magPurityAnalyzer.getStats();
    const auto magMinMax = magPurityAnalyzer.getMinMaxDev();
    const auto magPeakAbsDev = std::max(-magMinMax.first, magMinMax.second );
    std::cout << "Mean Magnitude: " << magStats.first << ", Variance: " << magStats.second
              << ", SNR: " << 10.0 * std::log10( 0.5 / magStats.second ) << " dB" << std::endl;
    std::cout << "Magnitude Noise: maxNegDev: " << magMinMax.first << ", maxPosDev: "
              << magMinMax.second << ", maxAbsDev: " << magPeakAbsDev << std::endl;

    int retCode = 0;
    do {
        std::cout.precision(17);
        // ***** Chirping Phasor Phase Acceleration Purity - Mean, Variance and Peak Absolute Deviation *****
        // We are not comparing against legacy here.
        // We are simply going to verify that the phase acceleration noise is minuscule.
        if ( !inTolerance( phaseAccelStats.first, accelRadiansPerSamplePerSample, 1e-10 ) )
        {
            std::cout << "Chirping Phasor FAILS Mean Angular Acceleration Test! Expected: " << accelRadiansPerSamplePerSample
                      << ", Detected: " << phaseAccelStats.first << std::endl;
            retCode = 1;
            break;
        }
        if ( phaseAccelStats.second > 2e-26 )
        {
            std::cout << "Chirping Phasor FAILS Angular Acceleration Variance Test! Expected: less than " << 2e-26
                      << ", Detected: " << phaseAccelStats.second << std::endl;
            retCode = 2;
            break;
        }
        if ( phaseAccelPeakAbsDev > 4e-13 )
        {
            std::cout << "Chirping Phasor FAILS Angular Acceleration Peak Absolute Deviation! Expected less than: " << 4e-13
                      << ", Detected: " << phaseAccelPeakAbsDev << std::endl;
            retCode = 3;
            break;
        }

        // ***** Flying Phasor Magnitude Purity -  Mean, Variance and Peak Absolute Deviation *****
        // We are not comparing against the legacy here. Both are very good "mean" wise
        // and have extremely low variance.
        // We are simply going to verify that the difference is minuscule.
        if ( !inTolerance( magStats.first, 1.0, 1e-15 ) )
        {
            std::cout << "Chirping Phasor FAILS Mean Magnitude Test! Expected: " << 1.0
                      << ", Detected: " << magStats.first << std::endl;
            retCode = 4;
            break;
        }
        if ( magStats.second > 6.0e-33 )
        {
            std::cout << "Chirping Phasor FAILS Magnitude Variance Test! Expected: less than " << 6.0e-33
                      << ", Detected: " << magStats.second << std::endl;
            retCode = 5;
            break;
        }
        if ( magPeakAbsDev > 3.0e-16 )
        {
            std::cout << "Chirping Phasor FAILS Magnitude Peak Absolute Deviation! Expected less than: " << 3.0e-16
                      << ", Detected: " << magPeakAbsDev << std::endl;
            retCode = 6;
            break;
        }

    } while (false);

    return retCode;
}
