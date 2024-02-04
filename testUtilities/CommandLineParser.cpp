// Created on 20230103

#include "CommandLineParser.h"

#include <iostream>

#include <getopt.h>

int CommandLineParser::parseCommandLine( int argc, char * argv[] )
{
    int c;
//    int digitOptIndex = 0;
    int retCode = 0;

    enum eOptions { SpacingRadsPerSample=1, NumHarmonics, Profile, ChunkSize, NumChunks, SkipChunks, DecorrelSamples,
            Seed, StreamFormat, Help, IncludeX };

    while (true) {
//        int thisOptionOptIndex = optind ? optind : 1;
        int optionIndex = 0;
        static struct option longOptions[] = {
                {"spacingRadsPerSample", required_argument, nullptr, SpacingRadsPerSample },
                {"numHarmonics", required_argument, nullptr, NumHarmonics },
                {"profile", required_argument, nullptr, Profile },
                {"chunkSize", required_argument, nullptr, ChunkSize },
                {"numChunks", required_argument, nullptr, NumChunks },
                {"skipChunks", required_argument, nullptr, SkipChunks },
                {"decorrelSamples", required_argument, nullptr, DecorrelSamples },
                {"seed", required_argument, nullptr, Seed },
                {"streamFormat", required_argument, nullptr, StreamFormat },
                {"help", no_argument, nullptr, Help },
                {"includeX", no_argument, nullptr, IncludeX },
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
                break;

            case NumHarmonics:
                numHarmonicsIn = std::stoul(optarg );
                break;

            case Profile:
                profileIn = std::stoul(optarg );
                break;

            case ChunkSize:
                chunkSizeIn = std::stoul(optarg );
                break;

            case NumChunks:
                numChunksIn = std::stoul( optarg );
                break;

            case SkipChunks:
                skipChunksIn = std::stoul( optarg );
                break;

            case DecorrelSamples:
                decorrelSamplesIn = std::stoul(optarg );
                break;

            case Seed:
                seedIn = std::stoul(optarg );
                break;

            case StreamFormat:
            {
                // This one is more complicated. We either detect a valid string here, or we don't.
                const std::string streamFormatStr{ optarg };
                if ( streamFormatStr == "t32" )
                    streamFormatIn = StreamFormat::Text32;
                else if ( streamFormatStr == "t64" )
                    streamFormatIn = StreamFormat::Text64;
                else if ( streamFormatStr == "b32" )
                    streamFormatIn = StreamFormat::Bin32;
                else if ( streamFormatStr == "b64" )
                    streamFormatIn = StreamFormat::Bin64;
                else
                    streamFormatIn = StreamFormat::Invalid;
                break;
            }

            case Help:
                helpFlagIn = true;
                break;

            case IncludeX:
                includeX_In = true;
                break;

            case '?':
                std::cout << "The getopt_long call returned '?'" << std::endl;
                retCode = 1;
                break;

            default:
                std::cout << "The getopt_long call returned character code" << c << std::endl;
                retCode = 2;
                break;
        }
    }

    return retCode;
}
