/**
 * @file testResetDefaults.cpp
 * @brief Test Harness for Comb Generator `reset` operation with default parameters.
 *
 * Default parameters for the reset function are nullptr for both magnitudes and phases and,
 * an empty envelope functor. This results in a magnitude of 1.0 and
 * a starting phase angle of 0.0 radians for all harmonic tones, and no envelope functor.
 * An empty envelope functor means no amplitude modulation.
 *
 * @authors Frank Reiser
 * @date Initiated October 24th, 2023
 */

#include "CombGenerator.h"
#include "FlyingPhasorToneGenerator.h"

#include <memory>
#include <stdexcept>
#include <iostream>

using namespace ReiserRT::Signal;

int testGetSamples()
{
    // Construct a CombGenerator specifying a maximum number of harmonics tones
    constexpr size_t maxHarmonics = 4;
    CombGenerator combGenerator{ maxHarmonics };

    // Reset the Comb Generator specifying less than the maximum number of harmonic tones.
    constexpr size_t numHarmonics = 2;
    constexpr double fundamentalRadiansPerSample = M_PI / 8.0;
    combGenerator.reset( numHarmonics, fundamentalRadiansPerSample, nullptr, nullptr );

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
        spectralLineGenerators[i].reset( double(i+1) * fundamentalRadiansPerSample, 0.0 );
        if ( i )
            spectralLineGenerators[i].accumSamplesScaled( compareSampleBuffer.get(), maxEpochSize, 1.0 );
        else
            spectralLineGenerators[i].getSamplesScaled( compareSampleBuffer.get(), maxEpochSize, 1.0 );
    }
    std::unique_ptr< FlyingPhasorElementType[] > deltaSampleBuffer{ new FlyingPhasorElementType[ maxEpochSize ] };
    for ( size_t i = 0; maxEpochSize != i; ++i )
    {
        deltaSampleBuffer[i] = pEpochSampleBuffer[i] - compareSampleBuffer[i];
        if ( 0.0 != deltaSampleBuffer[i] )
        {
            std::cout << "Failed Get Samples Test at epoch sample index " << i << "." << std::endl;
            return 21;
        }
    }

    return 0;
}

int testAccumSamples()
{
    // Construct a CombGenerator specifying a maximum number of harmonics tones
    constexpr size_t maxHarmonics = 4;
    CombGenerator combGenerator{ maxHarmonics };

    // Reset the Comb Generator specifying less than the maximum number of harmonic tones.
    constexpr size_t numHarmonics = 2;
    constexpr double fundamentalRadiansPerSample = M_PI / 8.0;
    combGenerator.reset( numHarmonics, fundamentalRadiansPerSample, nullptr, nullptr );

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
    for ( size_t i = 0; numHarmonics != i; ++i )
    {
        spectralLineGenerators[i].reset( double(i+1) * fundamentalRadiansPerSample, 0.0 );
        spectralLineGenerators[i].accumSamplesScaled( compareSampleBuffer.get(), maxEpochSize, 1.0 );
    }
    std::unique_ptr< FlyingPhasorElementType[] > deltaSampleBuffer{ new FlyingPhasorElementType[ maxEpochSize ] };
    for ( size_t i = 0; maxEpochSize != i; ++i )
    {
        deltaSampleBuffer[i] = pEpochSampleBuffer[i] - compareSampleBuffer[i];
        if ( 0.0 != deltaSampleBuffer[i] )
        {
            std::cout << "Failed Accumulate Samples Test at epoch sample index " << i << "." << std::endl;
            return 31;
        }
    }

    return 0;
}

int main()
{
    // Test 1 - Test the `getSamples` operation after `reset` with default parameters.
    auto testResult = testGetSamples();
    if ( 0 != testResult ) return testResult;

    // Test 2 - Test the `accumSamples` operation after `reset` with default parameters.
    testResult = testAccumSamples();
    if ( 0 != testResult ) return testResult;

    return 0;
}
