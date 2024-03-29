/**
 * @file streamChirpingPhasor.cpp
 * @brief Utility program for generating output from a ChirpingPhasor for whatever analytical purposes.
 *
 * @authors Frank Reiser
 * @date Initiated on Nov 27, 2023
 */

#include "ChirpingPhasorToneGenerator.h"

#include "CommandLineParser.h"

#include <iostream>
#include <memory>
#include <limits>

using namespace ReiserRT::Signal;

void printHelpScreen()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    streamChirpingPhasor [options]" << std::endl;
    std::cout << "Available Options:" << std::endl;
    std::cout << "    --help" << std::endl;
    std::cout << "        Displays this help screen and exits." << std::endl;
    std::cout << "    --accel=<double>" << std::endl;
    std::cout << "        The acceleration in radians per sample per sample." << std::endl;
    std::cout << "        Defaults to pi/16384 radians per sample per sample if unspecified." << std::endl;
    std::cout << "    --omegaZero=<double>" << std::endl;
    std::cout << "        The initial angular velocity in radians per sample (equal 2*pi*f)" << std::endl;
    std::cout << "        Defaults to zero radians per sample if unspecified." << std::endl;
    std::cout << "    --phi=<double>" << std::endl;
    std::cout << "        The initial phase of the starting sample in radians." << std::endl;
    std::cout << "        Defaults to 0.0 radians if unspecified." << std::endl;
    std::cout << "    --chunkSize=<uint>" << std::endl;
    std::cout << "        The number of samples to produce per chunk. If zero, no samples are produced." << std::endl;
    std::cout << "        Defaults to 4096 samples if unspecified." << std::endl;
    std::cout << "    --numChunks=<uint>" << std::endl;
    std::cout << "        The number of chunks to generate. If zero, runs continually up to max uint64 chunks." << std::endl;
    std::cout << "        This maximum value is inclusive of any skipped chunks." << std::endl;
    std::cout << "        Defaults to 1 chunk if unspecified." << std::endl;
    std::cout << "    --skipChunks=<uint>" << std::endl;
    std::cout << "        The number of chunks to skip before any chunks are output. Does not effect the numChunks output." << std::endl;
    std::cout << "        In essence if numChunks is 1 and skip chunks is 4, chunk number 5 is the only chunk output." << std::endl;
    std::cout << "        Defaults to 0 chunks skipped if unspecified." << std::endl;
    std::cout << "    --streamFormat=<string>" << std::endl;
    std::cout << "        t32 - Outputs samples in text format with floating point precision of (9 decimal places)." << std::endl;
    std::cout << "        t64 - Outputs samples in text format with floating point precision (17 decimal places)." << std::endl;
    std::cout << "        b32 - Outputs data in raw binary with 32bit precision (uint32 and float), native endian-ness." << std::endl;
    std::cout << "        b64 - Outputs data in raw binary 64bit precision (uint64 and double), native endian-ness." << std::endl;
    std::cout << "        Defaults to t64 if unspecified." << std::endl;
    std::cout << "    --includeX" << std::endl;
    std::cout << "        Include sample count in the output stream. This is useful for gnuplot using any format." << std::endl;
    std::cout << "        Defaults to no inclusion if unspecified." << std::endl;
    std::cout << std::endl;
    std::cout << "Error Returns:" << std::endl;
    std::cout << "    1 - Command Line Parsing Error - Unrecognized Long Option." << std::endl;
    std::cout << "    2 - Command Line Parsing Error - Unrecognized Short Option (none supported)." << std::endl;
    std::cout << "    3 - Invalid streamFormat specified." << std::endl;
}

int main( int argc, char * argv[] )
{
    // Parse potential command line. Defaults provided otherwise.
    CommandLineParser cmdLineParser{};

    auto parseRes = cmdLineParser.parseCommandLine(argc, argv);
    if ( 0 != parseRes )
    {
        std::cerr << "streamChirpingPhasor Parse Error: Use command line argument --help for instructions" << std::endl;
        exit(parseRes);
    }

    if ( cmdLineParser.getHelpFlag() )
    {
        printHelpScreen();
        exit( 0 );
    }

    // Get Frequency and Starting Phase
    auto accel = cmdLineParser.getAccel();
    auto omegaZero = cmdLineParser.getOmegaZero();
    auto phi = cmdLineParser.getPhi();

    // Get the Skip Chunk Count
    const auto skipChunks = cmdLineParser.getSkipChunks();

    // Get Chunk Size.
    const auto chunkSize = cmdLineParser.getChunkSize();

    // Condition Number of Chunks. If it's zero, we set to maximum less skipChunks
    // because we will incorporate skipChunks into numChunks to simplify logic.
    auto numChunks = cmdLineParser.getNumChunks();
    if ( 0 == numChunks )
        numChunks = std::numeric_limits<decltype( numChunks )>::max() - skipChunks;
    numChunks += skipChunks;

    // Do we have a valid stream output format to use?
    auto streamFormat = cmdLineParser.getStreamFormat();
    if ( CommandLineParser::StreamFormat::Invalid == streamFormat )
    {
        std::cerr << "streamChirpingPhasor Error: Invalid Stream Format Specified. Use --help for instructions" << std::endl;
        exit( 3 );
    }

    // Instantiate a Chirping Phasor
    ChirpingPhasorToneGenerator chirpingPhasorToneGenerator{ accel, omegaZero, phi };

    // Allocate Memory for Chunk Size
    std::unique_ptr< FlyingPhasorElementType[] > pSampleSeries{ new FlyingPhasorElementType [ chunkSize ] };

    // If we are using a text stream format, set the output precision
    if ( CommandLineParser::StreamFormat::Text32 == streamFormat)
    {
        std::cout << std::scientific;
        std::cout.precision(9);
    }
    else if ( CommandLineParser::StreamFormat::Text64 == streamFormat)
    {
        std::cout << std::scientific;
        std::cout.precision(17);
    }

    // Are we including Sample count in the output?
    auto includeX = cmdLineParser.getIncludeX();

    FlyingPhasorElementBufferTypePtr p = pSampleSeries.get();
    size_t sampleCount = 0;
    size_t skippedChunks = 0;
    for ( size_t chunk = 0; numChunks != chunk; ++chunk )
    {
        // Get Samples. If we are skipping chunks, we may not output, but we must
        // maintain chirping phasor state.
        chirpingPhasorToneGenerator.getSamples( p, chunkSize );

        // Skip this Chunk?
        if ( skipChunks != skippedChunks )
        {
            ++skippedChunks;
            sampleCount += chunkSize;
            continue;
        }

        if ( CommandLineParser::StreamFormat::Text32 == streamFormat ||
             CommandLineParser::StreamFormat::Text64 == streamFormat )
        {
            for ( size_t n = 0; chunkSize != n; ++n )
            {
                if ( includeX ) std::cout << sampleCount++ << " ";
                std::cout << p[n].real() << " " << p[n].imag() << std::endl;
            }
        }
        else if ( CommandLineParser::StreamFormat::Bin32 == streamFormat )
        {
            for ( size_t n = 0; chunkSize != n; ++n )
            {
                if ( includeX )
                {
                    auto sVal = uint32_t( sampleCount++);
                    std::cout.write( reinterpret_cast< const char * >(&sVal), sizeof( sVal ) );
                }
                auto fVal = float( p[n].real() );
                std::cout.write( reinterpret_cast< const char * >(&fVal), sizeof( fVal ) );
                fVal = float( p[n].imag() );
                std::cout.write( reinterpret_cast< const char * >(&fVal), sizeof( fVal ) );
            }
        }
        else if ( CommandLineParser::StreamFormat::Bin64 == streamFormat )
        {
            for ( size_t n = 0; chunkSize != n; ++n )
            {
                if ( includeX )
                {
                    auto sVal = sampleCount++;
                    std::cout.write( reinterpret_cast< const char * >(&sVal), sizeof( sVal ) );
                }
                auto fVal = p[n].real();
                std::cout.write( reinterpret_cast< const char * >(&fVal), sizeof( fVal ) );
                fVal = p[n].imag();
                std::cout.write( reinterpret_cast< const char * >(&fVal), sizeof( fVal ) );
            }
        }
        std::cout.flush();

    }

    exit( 0 );
    return 0;
}

