#include "ChirpingPhasorToneGenerator.h"

#include <memory>
#include <iostream>

int main()
{
    // An arbitrary epoch dwell in samples.
    constexpr size_t NUM_SAMPLES = 2048;

    ReiserRT::Signal::ChirpingPhasorToneGenerator chirpGen{ M_PI / NUM_SAMPLES };

    // Buffers for an epoch's worth of data for the chirp.
    using SampleType = ReiserRT::Signal::FlyingPhasorElementType;
    std::unique_ptr< SampleType[] > chirpBuf{ new SampleType[ NUM_SAMPLES ] };

    // Get data from each of the tone generators.
    chirpGen.getSamples( chirpBuf.get(), NUM_SAMPLES );

    // Write to standard out. It can be redirected.
//    std::cout << std::scientific;
    std::cout.precision(6);
    auto p = chirpBuf.get();
    for ( size_t n = 0; NUM_SAMPLES != n; ++n, ++p )
    {
        std::cout << p->real() << " " << p->imag() << std::endl;
    }

    return 0;
}

