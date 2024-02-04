// Created on 20230103

#include "CombGenerator.h"

#include "CommandLineParser.h"
#include "SubSeedGenerator.h"
#include "RandomPhaseDistributor.h"
#include "FlyingPhasorToneGeneratorDataTypes.h"
#include "CombScintillationEnvelopeFunctor.h"

#include <memory>
#include <iostream>

using namespace ReiserRT::Signal;

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
    std::cout << "        Defaults to 0 which results in phases of 0.0 for all harmonics and a seed of 0 for" << std::endl;
    std::cout << "        scintillation effects assuming a non-zero decorrelationSamples value." << std::endl;
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
        exit( parseRes );
    }

    if ( cmdLineParser.getHelpFlag() )
    {
        printHelpScreen();
        exit( 0 );
    }
#if 0
    std::cout << "Parsed: " << std::endl
              << " --spacingRadsPerSample=" << cmdLineParser.getSpacingRadsPerSample() << std::endl
              << " --numHarmonics=" << cmdLineParser.getNumHarmonics() << std::endl
              << " --chunkSize=" << cmdLineParser.getChunkSize() << std::endl
              << " --numChunks=" << cmdLineParser.getNumChunks() << std::endl
              << " --skipChunks=" << cmdLineParser.getSkipChunks() << std::endl
              << " --decorrelSamples=" << cmdLineParser.getDecorrelSamples() << std::endl
              << " --profile=" << cmdLineParser.getProfile() << std::endl
              << " --seed=" << cmdLineParser.getSeed() << std::endl
              << " --streamFormat=" << (int)cmdLineParser.getStreamFormat() << std::endl
              << " --includeX=" << (int)cmdLineParser.getIncludeX() << std::endl
              << std::endl;
#endif

    // It would be normal to instantiate a Comb Generator for some maximum use case and
    // then consistently use less than the maximum, thereby supporting numerous reset cycles for a
    // variety of signals.
    constexpr size_t MAX_HARMONICS = 240;

    // If the user desired greater than our maximums. Error out.
    const auto numHarmonics = cmdLineParser.getNumHarmonics();
    if ( MAX_HARMONICS < numHarmonics )
    {
        std::cerr << "The Number of Harmonics requested exceeds maximum of " << MAX_HARMONICS << std::endl;
        exit( 3 );
    }

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
    const auto streamFormat = cmdLineParser.getStreamFormat();
    if ( CommandLineParser::StreamFormat::Invalid == streamFormat )
    {
        std::cerr << "streamCombGenerator Error: Invalid Stream Format Specified. Use --help for instructions" << std::endl;
        exit( 4 );
    }

    // Do we have a valid profile to use?
    const auto profile = cmdLineParser.getProfile();
    if ( profile > 1 )
    {
        std::cerr << "streamCombGenerator Error: Invalid Profile Specified. Use --help for instructions" << std::endl;
        exit( 5 );
    }

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

    // Set the master seed from the command line. If this value is zero coming in
    auto masterSeed = cmdLineParser.getSeed();

    // We might need some random numbers. We will use a sub-seed generator seeded with a master
    // seed provided by the command line to provide seeds for other distributions should we use them.
    SubSeedGenerator subSeedGenerator{};
    subSeedGenerator.reset( masterSeed );
#if 0
    for ( int i = 0; i != 4; ++i )
        std::cout << "Sub-seed Generator out: " << subSeedGenerator.getSubSeed() << std::endl;
#endif

    // We may or may not need magnitude and phase buffers dependent on command line parameters.
    // A nullptr magnitude buffer will result in magnitudes of one and a nullptr phase buffer
    // will result in phases of zero. We will start out assuming this is the case.
    std::unique_ptr< double[] > pMagnitudes{};
    std::unique_ptr< double[] > pPhases{};

    // If profile is 1, then we use a magnitudes that are equal to the reciprocal of their harmonic number
    if ( 1 == profile )
    {
        pMagnitudes = std::unique_ptr< double[] >{ new double[ MAX_HARMONICS ] };
        for ( size_t i = 0; numHarmonics != i; ++i )
            pMagnitudes[ i ] = 1.0 / double( i + 1 );
    }

    // If master seed is non-zero, then we will create some random phases for each harmonic tone
    if ( 0 != masterSeed )
    {
        pPhases = std::unique_ptr< double[] >{ new double[ MAX_HARMONICS ] };

        // We will initialize all harmonics with random phases, so we will seed it
        // with a value obtained from our SubSeedGenerator.
        RandomPhaseDistributor randomPhaseDistributor{};
        randomPhaseDistributor.reset( subSeedGenerator.getSubSeed() );
        for ( size_t i = 0; numHarmonics != i; ++i )
            pPhases[ i ] = randomPhaseDistributor.getValue();
    }

    // We may, or may not, use our Comb Scintillation Envelope Functor, but we will instantiate it
    // regardless. It's just simpler this way because the code structure that follows. It cannot
    // go out of scope until we are potentially done using it.
    CombScintillationEnvelopeFunctor combScintillationEnvelopeFunctor{ MAX_HARMONICS, chunkSize };

    // Instantiate Comb Generator for maximum number of harmonics.
    ReiserRT::Signal::CombGenerator combGenerator{ MAX_HARMONICS };

    // Reset the Comb Generator for the job at hand
    const auto harmonicSpacing = cmdLineParser.getSpacingRadsPerSample();
    const auto decorrelationSamples = cmdLineParser.getDecorrelSamples();
    if ( !decorrelationSamples )
    {
        // The magnitudes and the phases are moved into shared pointer interfaces implicitly.
        combGenerator.reset( numHarmonics, harmonicSpacing,
                             std::move( pMagnitudes ), std::move( pPhases ) );
    }
    else
    {
        // We are going to share magnitudes between our Comb Generator and our Comb Scintillation Envelope Functor
        ReiserRT::Signal::CombGeneratorScalarVectorType sharedMagnitudes{ std::move( pMagnitudes ) };

        // Reset our Comb Scintillation Envelope Functor
        combScintillationEnvelopeFunctor.reset( numHarmonics, decorrelationSamples, sharedMagnitudes,
                                                subSeedGenerator.getSubSeed() );

        // Reset our Comb Generator
        combGenerator.reset( numHarmonics, harmonicSpacing, sharedMagnitudes,
                             std::move( pPhases ), std::ref( combScintillationEnvelopeFunctor ) );
    }

    // Allocate Memory for Comb Generator Output Samples
    std::unique_ptr< FlyingPhasorElementType[] > pCombSampleSeries{ new FlyingPhasorElementType [ chunkSize ] };

    // Are we including Sample count in the output?
    auto includeX = cmdLineParser.getIncludeX();

    FlyingPhasorElementBufferTypePtr p = pCombSampleSeries.get();
    size_t sampleCount = 0;
    size_t skippedChunks = 0;
    for ( size_t chunk = 0; numChunks != chunk; ++chunk )
    {
        // Get Samples. If we are skipping chunks, we may not output, but we must
        // maintain flying phasor state.
        combGenerator.getSamples( p, chunkSize );

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

    return 0;
}
