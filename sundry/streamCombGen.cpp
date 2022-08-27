//
// Created by frank on 8/27/22.
//

#include "CommandLineParser.h"

#include <iostream>

int main( int argc, char * argv[] ) {

    // Parse potential command line. Defaults provided otherwise.
    CommandLineParser cmdLineParser{};
    if ( 0 != cmdLineParser.parseCommandLine(argc, argv) )
    {
        std::cout << "Failed parsing command line" << std::endl;
        std::cout << "Optional Arguments are:" << std::endl;
        std::cout << "\t--spacingRadsPerSample=<double>: The spacing in radians per sample to used (default pi/16)." << std::endl;
        std::cout << "\t--numLines=<ulong>: The number of spectral lines to generate at given spacing (default 10)." << std::endl;
        std::cout << "\t--epochSize=<ulong>: The number of samples to stream out (default 2048)." << std::endl;
        std::cout << "\t--decorrelSamples=<ulong>: The number of samples for scintillation decorrelation (default 0 no scint)." << std::endl;
        std::cout << "\t--profile=<uint>: 0 for equal mag comb, 1 for tapered 3db per tone (default 0)." << std::endl;
        std::cout << "\t--seed=<uint>: Random seed for phase and scintillation." << std::endl;

        exit( -1 );
    }
#if 0
    else
    {
        std::cout << "Parsed: --radiansPerSample=" << cmdLineParser.getRadsPerSample()
                  << " --phase=" << cmdLineParser.getPhase() << std::endl << std::endl;
    }
#endif


}
