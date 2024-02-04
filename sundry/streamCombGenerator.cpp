// Created on 20230103

#include "CombGenerator.h"

#include "CommandLineParser.h"
#include "SubSeedGenerator.h"
#include "RandomPhaseDistributor.h"
#include "FlyingPhasorToneGeneratorDataTypes.h"
#include "CombScintillationEnvelopeFunctor.h"

#include <memory>
#include <iostream>

void printHelpScreen()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    streamCombGenerator [options]" << std::endl;
    std::cout << "Available Options:" << std::endl;
    std::cout << "    --help" << std::endl;
    std::cout << "        Displays this help screen and exits." << std::endl;
    std::cout << "    --spacingRadsPerSample=<double>" << std::endl;
    std::cout << "        TThe spacing in radians per sample to use." << std::endl;
    std::cout << "        Defaults to pi/256 radians per sample if unspecified." << std::endl;
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
    std::cout << "    --decorrelSamples=<ulong>: The number of samples for scintillation decorrelation." << std::endl;
    std::cout << "        Defaults to zero (no scintillation)." << std::endl;
    std::cout << "    --seed=<uint>: Random seed for random phases and scintillation effects." << std::endl;
    std::cout << "        Defaults to zero which results in phases of zero and usage of std::random_device for" << std::endl;
    std::cout << "        scintillation effects assuming decorrelationSamples is non-zero." << std::endl;
    std::cout << "    --profile=<uint>: Use 0 for equal magnitude comb, 1 for tapered at the reciprocal of harmonic number." << std::endl;
    std::cout << "        Defaults to 1." << std::endl;
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
        std::cerr << "streamCombGenerator Parse Error: Use command line argument --help for instructions" << std::endl;
        exit(parseRes);
    }

    if ( cmdLineParser.getHelpFlag() )
    {
        printHelpScreen();
        exit( 0 );
    }
#if 0
    else
    {
        std::cout << "Parsed: " << std::endl
                  << " --spacingRadsPerSample=" << cmdLineParser.getSpacingRadsPerSample() << std::endl
                  << " --numHarmonics=" << cmdLineParser.getNumHarmonics() << std::endl
                  << " --chunkSize=" << cmdLineParser.getChunkSize() << std::endl
                  << " --numChunks=" << cmdLineParser.getNumChunks() << std::endl
                  << " --skipChunks=" << cmdLineParser.getSkipChunks() << std::endl
                  << " --decorrelSamples=" << cmdLineParser.getDecorrelSamples() << std::endl
                  << " --profile=" << cmdLineParser.getProfile() << std::endl
                  << " --seed=" << cmdLineParser.getProfile() << std::endl
                  << " --streamFormat=" << (int)cmdLineParser.getStreamFormat() << std::endl
                  << " --includeX=" << (int)cmdLineParser.getIncludeX() << std::endl
                  << std::endl;
    }
#endif

    // It would be normal to instantiate a Comb Generator for some maximum use case and then
    // reset it and use less than the maximum, thereby supporting numerous reset cycles for a
    // variety of signals.
    constexpr size_t MAX_HARMONICS = 240;

    // If the user desired greater than our maximums. Error out.
    const auto numHarmonics = cmdLineParser.getNumHarmonics();
    if ( MAX_HARMONICS < numHarmonics )
    {
        std::cerr << "The Number of Harmonics requested exceeds maximum of " << MAX_HARMONICS << std::endl;
        exit( -2 );
    }

    // Instantiate Comb Generator for maximum number of harmonics
    ReiserRT::Signal::CombGenerator combGenerator{ MAX_HARMONICS };

    // Instantiate Magnitude and Phase Buffers. We will go with the MAX_HARMONICS
    // for our length here even though we will use less than that.
    auto pMagnitudes = std::unique_ptr< double[] >{ new double[ MAX_HARMONICS ] };
    auto pPhases = std::unique_ptr< double[] >{ new double[ MAX_HARMONICS ] };

    // Set up Magnitudes of harmonics to 3db down per tone, or flat magnitudes of 1.0.
    switch ( cmdLineParser.getProfile() ) {
        case 1: {
            const auto sqrt2over2 = std::sqrt( 2.0 ) / 2.0;
            for (size_t i = 0; i != numHarmonics; ++i )
                pMagnitudes[i] = 1.0 * std::pow( sqrt2over2, i );
            break;
        }
        default: {
            for (size_t i = 0; i != numHarmonics; ++i )
                pMagnitudes[i] = 1.0;
            break;
        }
    }

    // We will need some random numbers. We will use a Sub-seed generator seeded with a master
    // seed provided by the command line. If defaulted to zero, then we end up using a random seed
    // from std::random_device which is performed by the instantiation of the Sub-seed generator.
    SubSeedGenerator subSeedGenerator{};
    auto masterSeed = cmdLineParser.getSeed();
    if ( masterSeed ) subSeedGenerator.reset( masterSeed );

    // We will initialize all harmonics with random phases, so we will seed it
    // with a value obtained from our SubSeedGenerator.
    RandomPhaseDistributor randomPhaseDistributor{};
    randomPhaseDistributor.reset( subSeedGenerator.getSubSeed() );
    for (size_t i = 0; i != numHarmonics; ++i )
        pPhases[i] = randomPhaseDistributor.getValue();

    // We may, or may not use our Comb Scintillation Envelope Functor, but we will instantiate it
    // regardless.
    auto epochSize = cmdLineParser.getChunkSize();
    CombScintillationEnvelopeFunctor combScintillationEnvelopeFunctor{ MAX_HARMONICS, epochSize };

    // Reset the Comb Generator for the job at hand
    const auto harmonicSpacing = cmdLineParser.getSpacingRadsPerSample();
    const auto decorrelationSamples = cmdLineParser.getDecorrelSamples();
    if ( !decorrelationSamples )
    {
        combGenerator.reset( numHarmonics, harmonicSpacing,
             std::move( pMagnitudes ), std::move( pPhases ) );
    }
    else
    {
        // We are going to share magnitudes between our Comb Generator and our Comb Scintillation Envelope Functor
        ReiserRT::Signal::CombGeneratorScalarVectorType sharedMagnitudes{std::move(pMagnitudes ) };

       // Reset our Comb Scintillation Envelope Functor
       combScintillationEnvelopeFunctor.reset( numHarmonics, decorrelationSamples, sharedMagnitudes,
                                               subSeedGenerator.getSubSeed() );

       // Reset our Comb Generator
       combGenerator.reset( numHarmonics, harmonicSpacing, sharedMagnitudes,
                            std::move( pPhases ), std::ref( combScintillationEnvelopeFunctor ) );
    }

    // Now we need a buffer to acquire samples into.
    using FlyingPhasorElementType = ReiserRT::Signal::FlyingPhasorElementType;
    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer{ new FlyingPhasorElementType [ epochSize ] };

    // Get samples
    combGenerator.getSamples( epochSampleBuffer.get(), epochSize );

    // Write to standard out. It can be redirected.
    std::cout << std::scientific;
    std::cout.precision(17);
    for (size_t n = 0; cmdLineParser.getChunkSize() != n; ++n )
    {
        const auto & sample = epochSampleBuffer[n];
        std::cout << sample.real() << " " << sample.imag() << std::endl;
    }

    return 0;
}
