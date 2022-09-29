//
// Created by frank on 9/28/22.
//

#include "CombGenerator.h"
#include "CombGeneratorDataTypes.h"
#include "SubSeedGenerator.h"
#include "RandomPhaseDistributor.h"
#include "FlyingPhasorToneGenerator.h"
#include "RayleighDistributor.h"
#include "ScintillationEngine.h"

#include <iostream>
#include <memory>
#include <cmath>
#include <vector>

using namespace TSG_NG;
using namespace ReiserRT::Signal;

int main( int argc, char * argv[] )
{
    ///@note We are going to use hard coded test values here as opposed to utilizing our CommandLineParser.
    ///The testing at this level is very focused and we have already beaten up the various pieces
    ///that may be used in conjunction with our CombGenerator. We just need to verify the
    ///implementation logic.  This is simply, the summing of a potentially scintillated harmonic series.
    constexpr size_t numLines = 2;
    constexpr size_t epochSize = 4096;
    constexpr uint32_t seed = 0x3210dead;

    // Instantiate CombGenerator for a max of NLines and Epoch Size
    CombGenerator combGenerator{ numLines, epochSize };

    // We are going to generate seeds from a master seed for eventual use with multiple comb generator instances.
    // We do not want to use the same engine used by the CombGenerator itself as that would be problematic.
    // Doing so would lead to multiple CombGenerators producing overlapping random sequences. That would be bad.
    SubSeedGenerator subSeedGenerator{};
    subSeedGenerator.reset( seed );

    // Instantiate a Random Phase Distributor and seed it with a value from our sub-seed generator.
    RandomPhaseDistributor randomPhaseDistributor{};
    randomPhaseDistributor.reset( subSeedGenerator.getSubSeed() );

    // What Profile did we ask for.
    std::unique_ptr< MagPhaseType[] > magPhase{ new MagPhaseType [ numLines ] };
    for ( size_t i = 0; numLines != i; ++i )
    {
        magPhase[i].first = 1.0;
        magPhase[i].second = randomPhaseDistributor.getValue();
    }

    // We will use a null Function (empty) when not scintillating. Invocations to it should throw.
    // However, we do not expect it to be invoked if we are not scintillating. I.e., decorrelation samples of zero.
    CombGenerator::ScintillateFunkType nullScintillateFunk{};

    // Reset the Comb Generator
    CombGeneratorResetParameters resetParams;
    resetParams.numLines = numLines;
    resetParams.spacingRadiansPerSample = M_PI / 8;
    resetParams.pMagPhase = magPhase.get();
    resetParams.decorrelationSamples = 0;           // No Scintillation for Initial Test.
    combGenerator.reset( resetParams, std::ref(nullScintillateFunk ) );

    // Get samples for non-scintillated harmonic series.
    auto pSamples = combGenerator.getSamples( std::ref(nullScintillateFunk ) );

    // To Verify the non-scintillated samples produced. We will use a FlyingPhasor and attempt to remove the tones
    // generated. Since both use FlyingPhasors in the same order, we expect the delta to be exactly zero.
    std::vector< FlyingPhasorToneGenerator > spectralLineGenerators{ epochSize };
    std::unique_ptr< FlyingPhasorElementType[] > compareSampleBuffer{new FlyingPhasorElementType[ epochSize ] };
    for ( size_t i = 0; numLines != i; ++i )
    {
        const auto phi = magPhase[i].second;
        spectralLineGenerators[i].reset( (i+1) * resetParams.spacingRadiansPerSample, phi );
        if ( 0 == i )
            spectralLineGenerators[i].getSamples(compareSampleBuffer.get(), epochSize );
        else
            spectralLineGenerators[i].accumSamples(compareSampleBuffer.get(), epochSize );
    }
    std::unique_ptr< FlyingPhasorElementType[] > deltaSampleBuffer{new FlyingPhasorElementType[ epochSize ] };
    for ( size_t i = 0; numLines != i; ++i )
    {
        deltaSampleBuffer[i] = pSamples[i] - compareSampleBuffer[i];
        if ( 0.0 != deltaSampleBuffer[i] )
        {
            std::cout << "Failed Non-Scintillated Test at epoch sample index " << i << "." << std::endl;
            return 1;
        }
    }

    // Now we are going to Scintillate at a fraction epoch and each time we are asked for a scintillation value
    // we will cache it in order to reproduce the data manually.
    RayleighDistributor rayleighDistributor{};
    std::vector< double > svc{};
    resetParams.decorrelationSamples = epochSize * 3 / 8;
    svc.reserve(numLines * epochSize / resetParams.decorrelationSamples );
    rayleighDistributor.reset( subSeedGenerator.getSubSeed() );
    auto scintillateFunk = [ &svc, &rayleighDistributor ]( double desiredMean, size_t lineNumberHint )
    {
        auto rv = rayleighDistributor.getValue( desiredMean );
        svc.push_back( rv );
        return rv;
    };

    // Reset the CombGenerator and get samples for scintillated harmonic series.
    combGenerator.reset( resetParams, std::ref(scintillateFunk ) );
    pSamples = combGenerator.getSamples( std::ref(scintillateFunk ) );


    // Now we need to manually create a scintillated 'compare' buffer.
    // We have to set the initial scintillated state for each line. This is only initial the scintillated magnitudes.
    // The slopes will be computed immediately on the first comparison sample generation for each line.
    std::vector< ScintillationEngine::StateType > scintillationStates{ numLines, {0.0, 0.0 } };
    size_t cacheIndex = 0;
    for ( size_t i = 0; numLines != i; ++i )
    {
        scintillationStates[ i ].first = svc[ cacheIndex++ ];
        scintillationStates[ i ].second = 0.0;
    }
    std::unique_ptr< double[] > scintillationBuffer{ new double[ epochSize ] };
    ScintillationEngine scintillationEngine{ scintillationBuffer.get(), epochSize };

    // Scintillation Management Function, Invoked once per line, per epoch retrieved.
    size_t sampleCounter = 0;
    auto scintillationManagement =
            [ &scintillationStates, &scintillationEngine, &svc, &cacheIndex, &resetParams ](
            size_t lineNum, size_t startingSampleCount )
    {
        auto sFunk = [ &svc, &cacheIndex ]() { return svc[ cacheIndex++ ]; };
        auto & sParams = scintillationStates[ lineNum ];
        scintillationEngine.run( std::ref( sFunk ), sParams,
           startingSampleCount, resetParams.decorrelationSamples );
    };

    // Generate the scintillated 'compare' buffer.
    for ( size_t i = 0; numLines != i; ++i )
    {
        // Setup Scintillation Magnitude Buffer that we will scale by.
        scintillationManagement( i, sampleCounter );

        // Setup Tone Generator
        const auto phi = magPhase[i].second;
        spectralLineGenerators[i].reset( (i+1) * resetParams.spacingRadiansPerSample, phi );

        // First line optimization, just get the scintillated samples. Accumulation not necessary.
        if ( 0 == i )
            spectralLineGenerators[i].getSamplesScaled(compareSampleBuffer.get(), epochSize,
                 scintillationBuffer.get() );
        else
            spectralLineGenerators[i].accumSamplesScaled(compareSampleBuffer.get(), epochSize,
                 scintillationBuffer.get() );
    }
    sampleCounter += epochSize;

    // The difference should be zero
    for ( size_t i = 0; numLines != i; ++i )
    {
        deltaSampleBuffer[i] = pSamples[i] - compareSampleBuffer[i];
        if ( 0.0 != deltaSampleBuffer[i] )
        {
            std::cout << "Failed Scintillated Test#1 at epoch sample index " << i << "." << std::endl;
            return 2;
        }
    }

    // Grab another Epoch of Scintillated Data from the CombGenerator
    pSamples = combGenerator.getSamples( std::ref(scintillateFunk ) );

    // Generate another Epoch of scintillated 'compare' buffer.
    for ( size_t i = 0; numLines != i; ++i )
    {
        // Setup Scintillation Magnitude Buffer that we will scale by.
        scintillationManagement( i, sampleCounter );

        // First line optimization, just get the scintillated samples. Accumulation not necessary.
        if ( 0 == i )
            spectralLineGenerators[i].getSamplesScaled(compareSampleBuffer.get(), epochSize,
                                                       scintillationBuffer.get() );
        else
            spectralLineGenerators[i].accumSamplesScaled(compareSampleBuffer.get(), epochSize,
                                                         scintillationBuffer.get() );
    }
    sampleCounter += epochSize;

    // The difference for second scintillated epoch should be zero
    for ( size_t i = 0; numLines != i; ++i )
    {
        deltaSampleBuffer[i] = pSamples[i] - compareSampleBuffer[i];
        if ( 0.0 != deltaSampleBuffer[i] )
        {
            std::cout << "Failed Scintillated Test#2 at epoch sample index " << i << "." << std::endl;
            return 3;
        }
    }

    return 0;
}
