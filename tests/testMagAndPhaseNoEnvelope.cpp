/**
 * @file testMagAndPhaseNoEnvelope.cpp
 * @brief Test Harness for Comb Generator `reset` operation with some non-default (specific) parameters.
 *
 * Here, we use specific buffers for both magnitude and phase but, no envelope functor.
 * Having an empty envelope functor means no amplitude modulation.
 * We also test both `getSamples` and `accumSamples` operations under these conditions.
 *
 * @authors Frank Reiser
 * @date Initiated October 24th, 2023
 */

#include "CombGenerator.h"
#include "FlyingPhasorToneGenerator.h"

#include <iostream>

using namespace ReiserRT::Signal;

int testMagPhaseNoEnvelopeGetSamples()
{
    // Construct a CombGenerator specifying a maximum number of harmonics tones
    constexpr size_t maxHarmonics = 4;
    CombGenerator combGenerator{ maxHarmonics };

    // Initialize Mags and Phase. We will use incrementally changing magnitudes and phase.
    constexpr size_t numHarmonics = 3;
    constexpr double fundamentalRadiansPerSample = M_PI / 8.0;
    std::unique_ptr< double[] > magnitudes{ new double[ numHarmonics ] };
    std::unique_ptr< double[] > phases{ new double[ numHarmonics ] };
    for (size_t i = 0; numHarmonics != i; ++i )
    {
        magnitudes[i] = 4.0 - double(i);
        phases[i] = double(i) * M_PI / 32;
    }
    SharedScalarVectorType sharedMagnitudes{ std::move( magnitudes ) };
    SharedScalarVectorType sharedPhases{ std::move( phases ) };
    combGenerator.reset( numHarmonics, fundamentalRadiansPerSample,
                         sharedMagnitudes, sharedPhases );

    // We need a buffer to store signal data provided by the CombGenerator getSamples.
    // This buffer needs to be large enough for the maximum number of samples we will be retrieving.
    constexpr size_t maxEpochSize = 4096;
    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer{ new FlyingPhasorElementType [ maxEpochSize ] };
    FlyingPhasorElementBufferTypePtr pEpochSampleBuffer = epochSampleBuffer.get();

    // Get samples for harmonic series.
    combGenerator.getSamples( pEpochSampleBuffer, maxEpochSize );

    // To Verify the samples produced. We will use FlyingPhasors and attempt to remove the tones
    // generated. Since both use FlyingPhasor instances in the same order, we expect the delta to be exactly zero.
    std::vector< FlyingPhasorToneGenerator > spectralLineGenerators{ maxEpochSize };
    std::unique_ptr< FlyingPhasorElementType[] > compareSampleBuffer{ new FlyingPhasorElementType[ maxEpochSize ] };
    for ( size_t i = 0; numHarmonics != i; ++i )
    {
        spectralLineGenerators[i].reset( double(i+1) * fundamentalRadiansPerSample, sharedPhases[ std::ptrdiff_t(i) ] );
        if ( i )
            spectralLineGenerators[i].accumSamplesScaled( compareSampleBuffer.get(), maxEpochSize,
                sharedMagnitudes[ std::ptrdiff_t(i) ] );
        else
            spectralLineGenerators[i].getSamplesScaled( compareSampleBuffer.get(), maxEpochSize,
                sharedMagnitudes[ std::ptrdiff_t(i) ] );
    }
    std::unique_ptr< FlyingPhasorElementType[] > deltaSampleBuffer{new FlyingPhasorElementType[ maxEpochSize ] };
    for (size_t i = 0; maxEpochSize != i; ++i )
    {
        deltaSampleBuffer[i] = pEpochSampleBuffer[i] - compareSampleBuffer[i];
        if ( 0.0 != deltaSampleBuffer[i] )
        {
            std::cout << "Failed Get Samples Test at epoch sample index " << i << "." << std::endl;
            return 1;
        }
    }

    return 0;
}

int testMagPhaseNoEnvelopeAccumSamples()
{
    // Construct a CombGenerator specifying a maximum number of harmonics tones
    constexpr size_t maxHarmonics = 4;
    CombGenerator combGenerator{ maxHarmonics };

    // Initialize Mags and Phase. We will use a magnitude of 2.0 and an incrementally changing phase.
    constexpr size_t numHarmonics = 3;
    constexpr double fundamentalRadiansPerSample = M_PI / 8.0;
    std::unique_ptr< double[] > magnitudes{ new double[ numHarmonics ] };
    std::unique_ptr< double[] > phases{ new double[ numHarmonics ] };
    for (size_t i = 0; numHarmonics != i; ++i )
    {
        magnitudes[i] = 4.0 - double(i);
        phases[i] = double(i) * M_PI / 32;
    }
    SharedScalarVectorType sharedMagnitudes{ std::move( magnitudes ) };
    SharedScalarVectorType sharedPhases{ std::move( phases ) };
    combGenerator.reset( numHarmonics, fundamentalRadiansPerSample,
                         sharedMagnitudes, sharedPhases );

    // We need a buffer to store signal data provided by the CombGenerator accumSamples.
    // This buffer needs to be large enough for the maximum number of samples we will be retrieving.
    constexpr size_t maxEpochSize = 4096;
    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer{ new FlyingPhasorElementType [ maxEpochSize ] };
    FlyingPhasorElementBufferTypePtr pEpochSampleBuffer = epochSampleBuffer.get();

    // Since we are going to test the accumulate feature, we need something other than a bunch of zeros to
    // accumulate onto. We will use a DC component of (1.0 + 0j). Initialize the epoch sample buffer to
    // contain this value across all elements.
    for ( size_t i = 0; maxEpochSize != i; ++i )
        epochSampleBuffer[i] = FlyingPhasorElementType{ 1.0, 0.0 };

    // Accumulate samples for harmonic series onto the epoch sample buffer.
    combGenerator.accumSamples( pEpochSampleBuffer, maxEpochSize );

    // To Verify the samples produced. We will use FlyingPhasors and attempt to remove the tones
    // generated. We also have to consider the DC bias we started with.
    // Since both use FlyingPhasor instances in the same order, we expect the delta to be exactly zero.
    std::vector< FlyingPhasorToneGenerator > spectralLineGenerators{ maxEpochSize };
    std::unique_ptr< FlyingPhasorElementType[] > compareSampleBuffer{ new FlyingPhasorElementType[ maxEpochSize ] };
    for ( size_t i = 0; maxEpochSize != i; ++i )
        compareSampleBuffer[i] = FlyingPhasorElementType{ 1.0, 0.0 };
    for (size_t i = 0; numHarmonics != i; ++i )
    {
        spectralLineGenerators[i].reset( double(i+1) * fundamentalRadiansPerSample,
            sharedPhases[ std::ptrdiff_t(i) ] );
        spectralLineGenerators[i].accumSamplesScaled( compareSampleBuffer.get(), maxEpochSize,
            sharedMagnitudes[ std::ptrdiff_t(i) ] );
    }
    std::unique_ptr< FlyingPhasorElementType[] > deltaSampleBuffer{ new FlyingPhasorElementType[ maxEpochSize ] };
    for ( size_t i = 0; maxEpochSize != i; ++i )
    {
        deltaSampleBuffer[i] = pEpochSampleBuffer[i] - compareSampleBuffer[i];
        if ( 0.0 != deltaSampleBuffer[i] )
        {
            std::cout << "Failed Accumulate Samples Test at epoch sample index " << i << "." << std::endl;
            return 11;
        }
    }

    return 0;
}

int main()
{
    // Test 1 - Test the `getSamples` operation using after `reset` with specific parameters.
    int testResult = testMagPhaseNoEnvelopeGetSamples();
    if ( 0 != testResult ) return testResult;

    // Test 2 - Test the `accumSamples` operation using after `reset` with specific parameters.
    testResult = testMagPhaseNoEnvelopeAccumSamples();
    if ( 0 != testResult ) return testResult;

    return 0;
}
