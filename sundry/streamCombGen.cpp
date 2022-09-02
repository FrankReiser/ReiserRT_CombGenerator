//
// Created by frank on 8/27/22.
//

#include "CombGenerator.h"
#include "CommandLineParser.h"

#include <iostream>
#include <memory>
using namespace TSG_NG;

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
        std::cout << "Parsed: " << std::endl
                  << " --spacingRadsPerSample=" << cmdLineParser.getSpacingRadsPerSample() << std::endl
                  << " --numLines=" << cmdLineParser.getNumLines() << std::endl
                  << " --epochSize=" << cmdLineParser.getEpochSize() << std::endl
                  << " --decorrelSamples=" << cmdLineParser.getDecorrelSamples() << std::endl
                  << " --profile=" << cmdLineParser.getProfile() << std::endl
                  << " --seed=" << cmdLineParser.getProfile() << std::endl
                  << std::endl;
    }
#endif

    // Instantiate CombGenerator for NLines and Epoch Size
    CombGenerator combGenerator{ cmdLineParser.getNumLines(), cmdLineParser.getEpochSize() };

    // What Profile did we ask for.
    std::unique_ptr< double[] > magnitudes{ new double[ cmdLineParser.getNumLines() ] };
    switch ( cmdLineParser.getProfile() ) {
        case 1: {
            const auto sqrt2over2 = std::sqrt( 2.0 ) / 2.0;
            for ( size_t i = 0; i != cmdLineParser.getNumLines(); ++i )
                magnitudes[i] = 1.0 * std::pow( sqrt2over2, i );
            break;
        }
        default: {
            for ( size_t i = 0; i != cmdLineParser.getNumLines(); ++i )
                magnitudes[i] = 1.0;
            break;
        }
    }

    // Reset the Comb Generator
    CombGeneratorResetParameters resetParams;
    resetParams.numLines = cmdLineParser.getNumLines();
    resetParams.spacingRadiansPerSample = cmdLineParser.getSpacingRadsPerSample();
    resetParams.pMagnitudes = magnitudes.get();
    resetParams.decorrelationSamples = cmdLineParser.getDecorrelSamples();
    resetParams.randSeed = cmdLineParser.getSeed();
    combGenerator.reset( resetParams );

    auto pSamples = combGenerator.getSamples();

    // Write to standard out. It can be redirected.
    std::cout << std::scientific;
    std::cout.precision(17);
    for ( size_t n = 0; cmdLineParser.getEpochSize() != n; ++n )
    {
        std::cout << pSamples[n].real() << " " << pSamples[n].imag() << std::endl;
    }

    return 0;
}
