// Created on 20230103

#include "CommandLineParser.h"

#include <iostream>

#include <getopt.h>

int CommandLineParser::parseCommandLine( int argc, char * argv[] )
{
    int c;
//    int digitOptIndex = 0;
    int retCode = 0;

    enum eOptions { SpacingRadsPerSample=1, NumHarmonics, Profile, EpochSize, DecorrelSamples, Seed, };

    while (true) {
//        int thisOptionOptIndex = optind ? optind : 1;
        int optionIndex = 0;
        static struct option longOptions[] = {
                {"spacingRadsPerSample", required_argument, nullptr, SpacingRadsPerSample },
                {"numHarmonics", required_argument, nullptr, NumHarmonics },
                {"profile", required_argument, nullptr, Profile },
                {"maxEpochSize", required_argument, nullptr, EpochSize },
                {"decorrelSamples", required_argument, nullptr, DecorrelSamples },
                {"seed", required_argument, nullptr, Seed },
                {nullptr, 0, nullptr, 0 }
        };

        c = getopt_long(argc, argv, "",
                        longOptions, &optionIndex);
        if (c == -1) {
            break;
        }

        switch (c) {
            case SpacingRadsPerSample:
                spacingRadsPerSampleIn = std::stod(optarg );
#if 0
                std::cout << "The getopt_long call detected the --spacingRadsPerSample=" << optarg
                          << ". Value extracted = " << spacingRadsPerSampleIn << "." << std::endl;
#endif
                break;
            case NumHarmonics:
                numHarmonicsIn = std::stoul(optarg );
#if 0
                std::cout << "The getopt_long call detected the --numHarmonics=" << optarg
                          << ". Value extracted = " << numHarmonicsIn << "." << std::endl;
#endif
                break;
            case Profile:
                profileIn = std::stoul(optarg );
#if 0
                std::cout << "The getopt_long call detected the --profile=" << optarg
                          << ". Value extracted = " << profileIn << "." << std::endl;
#endif
                break;
            case EpochSize:
                epochSizeIn = std::stoul(optarg );
#if 0
                std::cout << "The getopt_long call detected the --epochSize=" << optarg
                          << ". Value extracted = " << epochSizeIn << "." << std::endl;
#endif
                break;
            case DecorrelSamples:
                decorrelSamplesIn = std::stoul(optarg );
#if 0
                std::cout << "The getopt_long call detected the --decorrelSamples=" << optarg
                          << ". Value extracted = " << decorrelSamplesIn << "." << std::endl;
#endif
                break;
            case Seed:
                seedIn = std::stoul(optarg );
#if 0
                std::cout << "The getopt_long call detected the --seed=" << optarg
                          << ". Value extracted = " << seedIn << "." << std::endl;
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
