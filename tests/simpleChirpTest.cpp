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

    void analyzePhaseStability(const FlyingPhasorElementBufferTypePtr & pBuf, size_t nSamples,
                               double radiansPerSamplePerSample )
    {
        // Reset stats in case an instance is re-run.
        statsStateMachine.reset();

        auto phaseAccumulator = 0.0;
        for ( size_t n=0; nSamples != n; ++n )
        {
            // We cheat the first sample because there is no previous one in order to compute
            // a delta. We assume zero error in that case.
            // Otherwise, (n not zero), we compute an error based on where we should have advanced to.
            auto phaseError = 0.0;
            if ( n )
            {
                const auto phase = std::arg(pBuf[ n ] );
                auto expectedAngle = fmod( phaseAccumulator, 2 * M_PI );
                if ( M_PI < expectedAngle )
                    expectedAngle -= 2 * M_PI;
                phaseError = deltaAngle(expectedAngle, phase );
            }

#if 0
            // If the phaseDelta is far too far off of radiansPerSamplePerSample. This is an obvious failure.
            // We are trying to catch the oddball here as a few oddballs may not create much variance.
            // Variance we check later.
            if ( !inTolerance( phaseDelta, radiansPerSamplePerSample, 1e-12 ) )
            {
                std::cout << "Phase error at sample: " << n << "! Expected: " << radiansPerSamplePerSample
                          << ", Detected: " << phaseDelta << std::endl;
                retCode = 2;
                break;
            }
#endif
            // Run stats state machine for overall mean and variance check.
            statsStateMachine.addSample( phaseError );

            // Update Phase Accumulator
            phaseAccumulator += radiansPerSamplePerSample*(n+1);
        }

#if 0
        // Now test the statistics look good.
        auto rateStats = statsStateMachine.getStats();
        if ( !inTolerance( rateStats.first, radiansPerSamplePerSample, 1e-15 ) )
        {
            std::cout << "Mean Angular Rate error! Expected: " << radiansPerSamplePerSample
                      << ", Detected: " << rateStats.first << std::endl;
            retCode = 3;
        }
        else if ( rateStats.second > 1.5e-32 )
        {
            std::cout << "Phase Noise error! Expected less than: " << 1.5e-32
                      << ", Detected: " << rateStats.second << std::endl;
            retCode = 4;
        }
        std::cout << "Mean Angular Rate: " << rateStats.first << ", Variance: " << rateStats.second << std::endl;
#endif
    }

    std::pair<double, double> getStats() const { return statsStateMachine.getStats(); }
    std::pair<double, double> getMinMaxDev() const { return statsStateMachine.getMinMaxDev(); }

private:
    StatsStateMachine statsStateMachine{};
};

class MagPurityAnalyzer
{
public:
    void analyzeSinusoidMagnitudeStability(const FlyingPhasorElementBufferTypePtr & pBuf, size_t nSamples )
    {
        // Reset stats in case an instance is re-run.
        statsStateMachine.reset();

        for ( size_t n=0; nSamples != n; ++n )
        {
            // If the magnitude is far too far off of 1.0. This is an obvious failure.
            // We are trying to catch the oddball here as a few oddballs may not create much variance.
            // Variance we check later.
            auto mag = std::abs( pBuf[ n ] );
#if 0
            if ( !inTolerance( mag, 1.0, 1e-15 ) )
            {
                std::cout << "Magnitude error at sample: " << n << "! Expected: " << 1.0
                          << ", Detected: " << mag << std::endl;
                retCode = 1;
                break;
            }
#endif

            statsStateMachine.addSample( mag );
        }

#if 0
        // Now test the statistics look good.
        auto magStats = statsStateMachine.getStats();
        if ( !inTolerance( magStats.first, 1.0, 1e-12 ) )
        {
            std::cout << "Mean Magnitude error! Expected: " << 1.0
                      << ", Detected: " << magStats.first << std::endl;
            retCode = 2;
        }
        else if ( magStats.second > 5e-33 )
        {
            std::cout << "Magnitude Noise error! Expected less than: " << 5e-33
                      << ", Detected: " << magStats.second << std::endl;
            retCode = 3;
        }

        double signalPower = magStats.first * magStats.first / 2;   // Should always be 0.5
        std::cout << "Mean Magnitude: " << magStats.first << ", Variance: " << magStats.second
                  << ", SNR: " << 10.0 * std::log10( signalPower / magStats.second ) << " dB" << std::endl;

        return retCode;
#endif
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
    const auto phaseStats = phasePurityAnalyzer.getStats();
    const auto phaseMinMax = phasePurityAnalyzer.getMinMaxDev();
    const auto phasePeakAbsDev = std::max(-phaseMinMax.first, phaseMinMax.second );
    std::cout << "Mean Phase Error: " << phaseStats.first << ", Variance: " << phaseStats.second << std::endl;
    std::cout << "Phase Error: maxNegDev: " << phaseMinMax.first << ", maxPosDev: "
              << phaseMinMax.second << ", maxAbsDev: " << phasePeakAbsDev << std::endl;


    return 0;
}
