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

private:
    double accelIn{ M_PI / 8192 };
    double omegaZeroIn{ 0.0 };
    double phiIn{ 0.0 };
};


#endif //REISER_RT_CHIRPINGPHASORCOMMANDLINEPARSER_H
