// Created on 20230103

#include "CombGenerator.h"

#include "CommandLineParser.h"
#include "SubSeedGenerator.h"
#include "RandomPhaseDistributor.h"
#include "FlyingPhasorToneGeneratorDataTypes.h"
#include "CombScintillationEnvelopeFunctor.h"

#include <memory>
#include <iostream>

int main( int argc, char * argv[] )
{
    // Parse potential command line. Defaults provided otherwise.
    CommandLineParser cmdLineParser{};
    if ( 0 != cmdLineParser.parseCommandLine(argc, argv) )
    {
        std::cout << "Failed parsing command line" << std::endl;
        std::cout << "Optional Arguments are:" << std::endl;
        std::cout << "\t--spacingRadsPerSample=<double>: The spacing in radians per sample to use (default pi/16)." << std::endl;
        std::cout << "\t--numHarmonics=<ulong>: The number of harmonics to generate at given spacing (default 10)." << std::endl;
        std::cout << "\t--maxEpochSize=<ulong>: The number of samples to stream out (default 2048)." << std::endl;
        std::cout << "\t--decorrelSamples=<ulong>: The number of samples for scintillation decorrelation (default 0 no scint)." << std::endl;
        std::cout << "\t--profile=<uint>: 0 for equal mag comb, 1 for tapered 3db per tone (default 0)." << std::endl;
        std::cout << "\t--seed=<uint>: Random seed for phase and scintillation. Use zero for std::random_device." << std::endl;

        exit( -1 );
    }
#if 1
    else
    {
        std::cout << "Parsed: " << std::endl
                  << " --spacingRadsPerSample=" << cmdLineParser.getSpacingRadsPerSample() << std::endl
                  << " --numHarmonics=" << cmdLineParser.getNumHarmonics() << std::endl
                  << " --maxEpochSize=" << cmdLineParser.getEpochSize() << std::endl
                  << " --decorrelSamples=" << cmdLineParser.getDecorrelSamples() << std::endl
                  << " --profile=" << cmdLineParser.getProfile() << std::endl
                  << " --seed=" << cmdLineParser.getProfile() << std::endl
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
    auto epochSize = cmdLineParser.getEpochSize();
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
        ReiserRT::Signal::SharedScalarVectorType sharedMagnitudes{ std::move( pMagnitudes ) };

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
    for ( size_t n = 0; cmdLineParser.getEpochSize() != n; ++n )
    {
        const auto & sample = epochSampleBuffer[n];
        std::cout << sample.real() << " " << sample.imag() << std::endl;
    }

    return 0;
}
