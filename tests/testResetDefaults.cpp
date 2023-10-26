/**
 * @file testResetDefaults.cpp
 * @brief Test Harness for Comb Generator `reset` operation with default parameters.
 *
 * Default parameters for the reset function are nullptr for both magnitudes and phases and,
 * an empty envelope functor. This results in a magnitude of 1.0 and
 * a starting phase angle of 0.0 radians for all harmonic tones, and no envelope functor.
 * An empty envelope functor means no amplitude modulation.
 * We also test that exceeding max harmonics throws an exception and both `getSamples` and
 * `accumSamples` operations under these default conditions.
 *
 * Latest Addition is a default constructed CombGenerator which is relatively useless.
 * However, it better supports vector usage. This also brought about move semantics
 * which are tested here.
 *
 * @authors Frank Reiser
 * @date Initiated October 24th, 2023
 */

#include "CombGenerator.h"
#include "FlyingPhasorToneGenerator.h"

#include <stdexcept>
#include <iostream>

using namespace ReiserRT::Signal;

int testThrowOnExceedingMaxHarmonics()
{
    // Construct a CombGenerator specifying a maximum number of harmonics tones
    constexpr size_t maxHarmonics = 4;
    CombGenerator combGenerator{ maxHarmonics };

    // Attempt to reset the generator with a requested number of harmonics which exceeds the maximum
    // specified during construction. This should throw `std::length_error`.
    int retCode = 0;
    try
    {
        combGenerator.reset( maxHarmonics+1, M_PI / 8.0 );
        std::cout << "Failed to detect exception for exceeding maxHarmonics during reset invocation!" << std::endl;
        retCode = 1;
    }
    catch ( const std::length_error & )
    {
    }

    return retCode;
}

int testNoThrowOnMaxHarmonics()
{
    // Construct a CombGenerator specifying a maximum number of harmonics tones
    constexpr size_t maxHarmonics = 4;
    CombGenerator combGenerator{ maxHarmonics };

    // Reset the generator with a requested number of harmonics which exceeds the maximum
    // specified during construction. This should throw `std::length_error`.
    int retCode = 0;
    try
    {
        combGenerator.reset( maxHarmonics, M_PI / 8.0 );
    }
    catch ( const std::length_error & )
    {
        std::cout << "Detect exception for utilizing maxHarmonics during reset invocation!" << std::endl;
        retCode = 11;
    }

    return retCode;
}

int testGetSamples()
{
    // Construct a CombGenerator specifying a maximum number of harmonics tones
    constexpr size_t maxHarmonics = 4;
    CombGenerator combGenerator{ maxHarmonics };

    // Reset the Comb Generator specifying less than the maximum number of harmonic tones.
    constexpr size_t numHarmonics = 2;
    constexpr double fundamentalRadiansPerSample = M_PI / 8.0;
    combGenerator.reset( numHarmonics, fundamentalRadiansPerSample );

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
    combGenerator.reset( numHarmonics, fundamentalRadiansPerSample );

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

int testDefaultConstructAndMove()
{
    // Create a default constructed object.
    CombGenerator combGenerator{};

    // We need a buffer to store signal data provided by the CombGenerator getSamples.
    // This buffer needs to be large enough for the maximum number of samples we will be retrieving.
    constexpr size_t maxEpochSize = 4096;
    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer{ new FlyingPhasorElementType [ maxEpochSize ] };
    FlyingPhasorElementBufferTypePtr pEpochSampleBuffer = epochSampleBuffer.get();

    // Stuff something into the epochSampleBuffer, so we can ensure it gets overwritten.
    pEpochSampleBuffer[0] = FlyingPhasorElementType{ 1.0, 1.0 };
    pEpochSampleBuffer[maxEpochSize-1] = FlyingPhasorElementType{ 1.0, 1.0 };

    // Get samples for harmonic series.
    combGenerator.getSamples( pEpochSampleBuffer, maxEpochSize );

    // The buffer should contain all zeros
    for ( size_t i = 0; maxEpochSize != i; ++i )
    {
        const auto sample = pEpochSampleBuffer[i];
        if ( 0.0 != sample )
        {
            std::cout << "Failed Pure Reset Test at epoch sample index " << i << "." << std::endl;
            return 41;
        }
    }

    // Attempt to reset the generator with a requested number of harmonics which exceeds the maximum
    // specified during construction. This should throw `std::length_error`.
    bool fail = true;
    try
    {
        combGenerator.reset( 1, M_PI / 8.0 );
        std::cout << "Failed to detect exception for exceeding maxHarmonics during reset invocation!" << std::endl;
    }
    catch ( const std::length_error & )
    {
        fail = false;
    }
    if ( fail ) return 42;

    // Now we will try to move a useful CombGenerator into the useless one via "move" assignment.
    // And if we can reset it without it throwing. It worked.
    constexpr size_t maxHarmonics = 4;
    combGenerator = CombGenerator{ maxHarmonics };

    // Attempt to reset the generator with a requested number of harmonics now that we've reassigned it.
    fail = false;
    try
    {
        combGenerator.reset( 1, M_PI / 8.0 );
    }
    catch ( const std::length_error & )
    {
        fail = true;
        std::cout << "Failed with exception thrown after move reassignment!" << std::endl;
    }
    if ( fail ) return 43;

    // Now move construct another CombGenerator from the one we've been working with.
    CombGenerator combGenerator2{ std::move( combGenerator ) };

    // Attempt to get samples from it
    // Get samples for harmonic series.
    combGenerator2.getSamples( pEpochSampleBuffer, maxEpochSize );

    // Buffer should contain non-zero data. That's all we need to verify as we've extensively tested
    // already elsewhere.
    bool nonZeroData = false;
    for ( size_t i = 0; maxEpochSize != i; ++i )
    {
        const auto sample = pEpochSampleBuffer[i];
        if ( 0.0 != sample )
            nonZeroData = true;
    }
    if ( !nonZeroData )
    {
        std::cout << "Failed move construct test. Zero signal data detected." << std::endl;
        return 44;
    }

    return 0;
}

int main()
{
    // Test 1 - Verify we throw if we exceed maxHarmonics during `reset` operation
    int testResult = testThrowOnExceedingMaxHarmonics();
    if ( 0 != testResult ) return testResult;

    // Test 2 - Verify we DO NOT throw if we use all maxHarmonics during `reset` operation
    testResult = testNoThrowOnMaxHarmonics();
    if ( 0 != testResult ) return testResult;

    // Test 3 - Test the `getSamples` operation after `reset` with default parameters.
    testResult = testGetSamples();
    if ( 0 != testResult ) return testResult;

    // Test 4 - Test the `accumSamples` operation after `reset` with default parameters.
    testResult = testAccumSamples();
    if ( 0 != testResult ) return testResult;

    // Test 5 - Test the default construct and move features. This was added after the fact,
    // so we will test it after the above.
    testResult = testDefaultConstructAndMove();
    if ( 0 != testResult ) return testResult;

    return 0;
}
