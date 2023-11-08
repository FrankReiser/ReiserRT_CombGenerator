/**
 * @file testDefaultMagWithEnvelope.cpp
 * @brief Test Harness for Comb Generator `reset` operation default magnitude and phase with envelope functor.
 *
 * Here, we are primarily concerned with verifying the envelope functor callback interface using default
 * magnitudes of 1.0. Phase is somewhat irrelevant at this point and a default phase of zero is of no
 * consequence. We also test both `getSamples` and `accumSamples` operations under these conditions.
 *
 * @authors Frank Reiser
 * @date Initiated October 24th, 2023
 */

#include "CombGenerator.h"
#include "FlyingPhasorToneGenerator.h"

#include <memory>
#include <iostream>

using namespace ReiserRT::Signal;

int testDefaultMagWithEnvelopGetSamples()
{
    // Construct a CombGenerator specifying a maximum number of harmonics tones
    constexpr size_t maxHarmonics = 4;
    CombGenerator combGenerator{ maxHarmonics };

    // We're going to use an exponential decay for this test.
    ///@note We do not utilize the `nHarmonic` parameter.
    constexpr size_t maxEpochSize = 4096;
    std::unique_ptr< double[] > envelopeBuffer{ new double[ maxEpochSize ] };
    auto envelopeFunk = [ &envelopeBuffer ]( size_t nSample,
                                             size_t numSamples, size_t /*nHarmonic*/, double nominalMag )
    {
        auto tau = maxEpochSize / 2.0;
        auto pMag = envelopeBuffer.get();
        for ( size_t i = 0; numSamples != i; ++i )
        {
            auto env = nominalMag * std::exp( double( nSample++) / -tau );
            *pMag++ = env;
        }

        return envelopeBuffer.get();
    };

    // Reset the Comb Generator with default magnitudes and phases, and our envelope functor.
    // We wrap the envelope functor in a std::ref so that that gets copied as opposed to our entire lambda.
    constexpr size_t numHarmonics = 3;
    constexpr double fundamentalRadiansPerSample = M_PI / 8.0;
    combGenerator.reset( numHarmonics, fundamentalRadiansPerSample, nullptr, nullptr, std::ref( envelopeFunk ) );

    // We need a buffer to store signal data provided by the CombGenerator getSamples.
    // This buffer needs to be large enough for the maximum number of samples we will be retrieving.
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
        spectralLineGenerators[i].reset( double(i+1) * fundamentalRadiansPerSample, 0.0 );  // Default phase
        auto pEnvelope = envelopeFunk( 0, maxEpochSize, i, 1.0 );                   // Default magnitude
        if ( i )
            spectralLineGenerators[i].accumSamplesScaled( compareSampleBuffer.get(), maxEpochSize, pEnvelope );
        else
            spectralLineGenerators[i].getSamplesScaled( compareSampleBuffer.get(), maxEpochSize, pEnvelope );
    }
    std::unique_ptr< FlyingPhasorElementType[] > deltaSampleBuffer{new FlyingPhasorElementType[ maxEpochSize ] };
    for ( size_t i = 0; maxEpochSize != i; ++i )
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

int testDefaultMagWithEnvelopAccumSamples()
{
    // Construct a CombGenerator specifying a maximum number of harmonics tones
    constexpr size_t maxHarmonics = 4;
    CombGenerator combGenerator{ maxHarmonics };

    // We're going to use an exponential decay for this test.
    ///@note We do not utilize the `nHarmonic` parameter.
    constexpr size_t maxEpochSize = 4096;
    std::unique_ptr< double[] > envelopeBuffer{ new double[ maxEpochSize ] };
    auto envelopeFunk = [ &envelopeBuffer ]( size_t nSample,
                                             size_t numSamples, size_t /*nHarmonic*/, double nominalMag )
    {
        auto tau = maxEpochSize / 2.0;
        auto pMag = envelopeBuffer.get();
        for ( size_t i = 0; numSamples != i; ++i )
        {
            auto env = nominalMag * std::exp( double( nSample++) / -tau );
            *pMag++ = env;
        }

        return envelopeBuffer.get();
    };

    // Reset the Comb Generator with default magnitudes and phases, and our envelope functor.
    // We wrap the envelope functor in a std::ref so that that gets copied as opposed to our entire lambda.
    constexpr size_t numHarmonics = 3;
    constexpr double fundamentalRadiansPerSample = M_PI / 8.0;
    combGenerator.reset( numHarmonics, fundamentalRadiansPerSample, nullptr, nullptr, std::ref( envelopeFunk ) );

    // We need a buffer to store signal data provided by the CombGenerator accumSamples.
    // This buffer needs to be large enough for the maximum number of samples we will be retrieving.
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
        spectralLineGenerators[i].reset( double(i+1) * fundamentalRadiansPerSample, 0.0 );  // Default phase
        auto pEnvelope = envelopeFunk( 0, maxEpochSize, i, 1.0 );                   // Default magnitude
        spectralLineGenerators[i].accumSamplesScaled( compareSampleBuffer.get(), maxEpochSize, pEnvelope );
    }
    std::unique_ptr< FlyingPhasorElementType[] > deltaSampleBuffer{new FlyingPhasorElementType[ maxEpochSize ] };
    for ( size_t i = 0; maxEpochSize != i; ++i )
    {
        deltaSampleBuffer[i] = pEpochSampleBuffer[i] - compareSampleBuffer[i];
        if ( 0.0 != deltaSampleBuffer[i] )
        {
            std::cout << "Failed Accum Samples Test at epoch sample index " << i << "." << std::endl;
            return 11;
        }
    }

    return 0;
}

int main()
{
    // Test 1 - Test the `getSamples` operation using after `reset` with specific parameters.
    int testResult = testDefaultMagWithEnvelopGetSamples();
    if ( 0 != testResult ) return testResult;

    // Test 1 - Test the `getSamples` operation using after `reset` with specific parameters.
    testResult = testDefaultMagWithEnvelopAccumSamples();
    if ( 0 != testResult ) return testResult;

    return 0;
}
