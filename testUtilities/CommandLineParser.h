// Created on 20220108

#ifndef REISER_RT_CHIRPINGPHASORCOMMANDLINEPARSER_H
#define REISER_RT_CHIRPINGPHASORCOMMANDLINEPARSER_H

#include <cmath>

class CommandLineParser
{
public:
    CommandLineParser() = default;
    ~CommandLineParser() = default;

    int parseCommandLine( int argc, char * argv[] );

    inline double getAccel() const { return accelIn; }
    inline double getOmegaZero() const { return omegaZeroIn; }
    inline double getPhi() const { return phiIn; }

    inline unsigned long getChunkSize() const { return chunkSizeIn; }
    inline unsigned long getNumChunks() const { return numChunksIn; }
    inline unsigned long getSkipChunks() const { return skipChunksIn; }

    enum class StreamFormat : short { Invalid=0, Text32, Text64, Bin32, Bin64 };
    StreamFormat getStreamFormat() const { return streamFormatIn; }

    inline bool getHelpFlag() const { return helpFlagIn; }
    inline bool getIncludeX() const { return includeX_In; }

private:
    double accelIn{ M_PI / 16384 };
    double omegaZeroIn{ 0.0 };
    double phiIn{ 0.0 };
    unsigned long chunkSizeIn{ 4096 };
    unsigned long numChunksIn{ 1 };
    unsigned long skipChunksIn{ 0 };
    bool helpFlagIn{ false };
    bool includeX_In{ false };

    StreamFormat streamFormatIn{ StreamFormat::Text64 };
};


#endif //REISER_RT_CHIRPINGPHASORCOMMANDLINEPARSER_H
