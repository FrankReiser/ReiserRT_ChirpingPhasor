
#include "CommandLineParser.h"

#include <iostream>

#include <getopt.h>

int CommandLineParser::parseCommandLine( int argc, char * argv[] )
{
    int c;
//    int digitOptIndex = 0;
    int retCode = 0;

    enum eOptions { Accel=1, OmegaZero=2, Phi=3 };

    while (true) {
//        int thisOptionOptIndex = optind ? optind : 1;
        int optionIndex = 0;
        static struct option longOptions[] = {
                {"accel", required_argument, nullptr, Accel },
                {"omegaZero", required_argument, nullptr, OmegaZero },
                {"phi", required_argument, nullptr, Phi },
                {nullptr, 0, nullptr, 0 }
        };

        c = getopt_long(argc, argv, "",
                        longOptions, &optionIndex);
        if (c == -1) {
            break;
        }

        switch (c) {
            case Accel:
                accelIn = std::stod( optarg );
#if 0
                std::cout << "The getopt_long call detected the --accel=" << optarg
                          << ". Value extracted = " << accelIn << "." << std::endl;
#endif
                break;
            case OmegaZero:
                omegaZeroIn = std::stod( optarg );
#if 0
                std::cout << "The getopt_long call detected the --omegaZero=" << optarg
                          << ". Value extracted = " << omegaZeroIn << "." << std::endl;
#endif
                break;
            case Phi:
                phiIn = std::stod( optarg );
#if 0
                std::cout << "The getopt_long call detected the --phi=" << optarg
                          << ". Value extracted = " << phiIn << "." << std::endl;
#endif
                break;
            case '?':
                std::cout << "The getopt_long call returned '?'" << std::endl;
                retCode = -1;
                break;
            default:
                std::cout << "The getopt_long call returned character code" << c << std::endl;
                retCode = -1;
                break;
        }
    }

    return retCode;
}
