/**
 * @file testCombGenerator.cpp
 * @brief Test Harness for Comb Generator Testing
 * @authors Frank Reiser
 * @date Initiated September 28th, 2022
 */

#include "CombGenerator.h"
#include "FlyingPhasorToneGenerator.h"

#include <iostream>
#include <memory>
#include <cmath>
#include <vector>

using namespace ReiserRT::Signal;

constexpr size_t numHarmonics = 2;
constexpr size_t maxEpochSize = 4096;
constexpr double fundamentalRadiansPerSample = M_PI / 8.0;

int defaultMagPhaseNoEnvelope()
{
    // Instantiate CombGenerator for a max of NLines and Epoch Size
    CombGenerator combGenerator{ numHarmonics };

    // Reset the Comb Generator
    combGenerator.reset( numHarmonics, fundamentalRadiansPerSample );

    // We need a buffer to store signal data provided by the CombGenerator getSamples.
    // This buffer needs to be large enough for the maximum number of samples we will be retrieving.
    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer{new FlyingPhasorElementType [ maxEpochSize ] };
    FlyingPhasorElementBufferTypePtr pEpochSampleBuffer = epochSampleBuffer.get();

    // Get samples for harmonic series.
    combGenerator.getSamples(pEpochSampleBuffer, maxEpochSize );

    // To Verify the non-scintillated samples produced. We will use a FlyingPhasor and attempt to remove the tones
    // generated. Since both use FlyingPhasor instances in the same order, we expect the delta to be exactly zero.
    std::vector< FlyingPhasorToneGenerator > spectralLineGenerators{maxEpochSize };
    std::unique_ptr< FlyingPhasorElementType[] > compareSampleBuffer{new FlyingPhasorElementType[ maxEpochSize ] };
    for (size_t i = 0; numHarmonics != i; ++i )
    {
        spectralLineGenerators[i].reset( double(i+1) * fundamentalRadiansPerSample, 0.0 );
        if ( 0 == i )
            spectralLineGenerators[i].getSamplesScaled(compareSampleBuffer.get(), maxEpochSize, 1.0 );
        else
            spectralLineGenerators[i].accumSamplesScaled(compareSampleBuffer.get(), maxEpochSize, 1.0 );
    }
    std::unique_ptr< FlyingPhasorElementType[] > deltaSampleBuffer{new FlyingPhasorElementType[ maxEpochSize ] };
    for (size_t i = 0; maxEpochSize != i; ++i )
    {
        deltaSampleBuffer[i] = pEpochSampleBuffer[i] - compareSampleBuffer[i];
        if ( 0.0 != deltaSampleBuffer[i] )
        {
            std::cout << "Failed Default Mag and Phase, No Envelope Test at epoch sample index " << i << "." << std::endl;
            return 1;
        }
    }

    return 0;
}

int specificMagPhaseNoEnvelope()
{
    // Instantiate CombGenerator for a max of NLines and Epoch Size
    CombGenerator combGenerator{ numHarmonics };

    // Initialize Mags and Phase. We will use a magnitude of 2.0 and an incrementally changing phase.
    std::unique_ptr< double[] > magnitudes{ new double[ numHarmonics ] };
    std::unique_ptr< double[] > phases{ new double[ numHarmonics ] };
    for (size_t i = 0; numHarmonics != i; ++i )
    {
        magnitudes[i] = 2.0;
        phases[i] = double(i) * M_PI / 32;
    }
    SharedScalarVectorType sharedMagnitudes{ std::move( magnitudes ) };
    SharedScalarVectorType sharedPhases{ std::move( phases ) };
    combGenerator.reset( numHarmonics, fundamentalRadiansPerSample,
                         sharedMagnitudes, sharedPhases );

    // We need a buffer to store signal data provided by the CombGenerator getSamples.
    // This buffer needs to be large enough for the maximum number of samples we will be retrieving.
    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer{new FlyingPhasorElementType [ maxEpochSize ] };
    FlyingPhasorElementBufferTypePtr pEpochSampleBuffer = epochSampleBuffer.get();

    // Get samples for harmonic series.
    combGenerator.getSamples(pEpochSampleBuffer, maxEpochSize );

    // To Verify the non-scintillated samples produced. We will use a FlyingPhasor and attempt to remove the tones
    // generated. Since both use FlyingPhasor instances in the same order, we expect the delta to be exactly zero.
    std::vector< FlyingPhasorToneGenerator > spectralLineGenerators{ maxEpochSize };
    std::unique_ptr< FlyingPhasorElementType[] > compareSampleBuffer{new FlyingPhasorElementType[ maxEpochSize ] };
    for (size_t i = 0; numHarmonics != i; ++i )
    {
        spectralLineGenerators[i].reset( double(i+1) * fundamentalRadiansPerSample,
                                         sharedPhases[ std::ptrdiff_t(i) ] );
        if ( 0 == i )
            spectralLineGenerators[i].getSamplesScaled(compareSampleBuffer.get(), maxEpochSize,
                                                       sharedMagnitudes[ std::ptrdiff_t(i) ] );
        else
            spectralLineGenerators[i].accumSamplesScaled(compareSampleBuffer.get(), maxEpochSize,
                                                         sharedMagnitudes[ std::ptrdiff_t(i) ] );
    }
    std::unique_ptr< FlyingPhasorElementType[] > deltaSampleBuffer{new FlyingPhasorElementType[ maxEpochSize ] };
    for (size_t i = 0; maxEpochSize != i; ++i )
    {
        deltaSampleBuffer[i] = pEpochSampleBuffer[i] - compareSampleBuffer[i];
        if ( 0.0 != deltaSampleBuffer[i] )
        {
            std::cout << "Failed Specific Mag and Phase, No Envelope Test at epoch sample index " << i << "." << std::endl;
            return 11;
        }
    }

    return 0;
}

int defaultMagWithEnvelope()
{
    // Instantiate CombGenerator for a max of NLines and Epoch Size
    CombGenerator combGenerator{ numHarmonics };

    // We're going to use an exponential decay for this test.
    std::unique_ptr< double[] > envelopeBuffer{new double[ maxEpochSize ] };
    auto envelopeFunk = [ &envelopeBuffer ]( size_t nSample,
            size_t numSamples, size_t nHarmonic, double nominalMag )
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

    // Reset the Comb Generator
    combGenerator.reset( numHarmonics, fundamentalRadiansPerSample,
                         nullptr, nullptr, envelopeFunk );

    // We need a buffer to store signal data provided by the CombGenerator getSamples.
    // This buffer needs to be large enough for the maximum number of samples we will be retrieving.
    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer{new FlyingPhasorElementType [ maxEpochSize ] };
    FlyingPhasorElementBufferTypePtr pEpochSampleBuffer = epochSampleBuffer.get();

    // Get samples for harmonic series.
    combGenerator.getSamples( pEpochSampleBuffer, maxEpochSize );

    // To Verify the non-scintillated samples produced. We will use a FlyingPhasor and attempt to remove the tones
    // generated. Since both use FlyingPhasor instances in the same order, we expect the delta to be exactly zero.
    std::vector< FlyingPhasorToneGenerator > spectralLineGenerators{ maxEpochSize };
    std::unique_ptr< FlyingPhasorElementType[] > compareSampleBuffer{new FlyingPhasorElementType[ maxEpochSize ] };
    for (size_t i = 0; numHarmonics != i; ++i )
    {
        auto pEnvelope = envelopeFunk( 0, maxEpochSize, i, 1.0 );

        spectralLineGenerators[i].reset( double(i+1) * fundamentalRadiansPerSample, 0.0 );
        if ( 0 == i )
            spectralLineGenerators[i].getSamplesScaled(compareSampleBuffer.get(), maxEpochSize, pEnvelope );
        else
            spectralLineGenerators[i].accumSamplesScaled(compareSampleBuffer.get(), maxEpochSize, pEnvelope );
    }
    std::unique_ptr< FlyingPhasorElementType[] > deltaSampleBuffer{new FlyingPhasorElementType[ maxEpochSize ] };
    for ( size_t i = 0; maxEpochSize != i; ++i )
    {
        deltaSampleBuffer[i] = pEpochSampleBuffer[i] - compareSampleBuffer[i];
        if ( 0.0 != deltaSampleBuffer[i] )
        {
            std::cout << "Failed Default Mag and Phase, With Envelope Test at epoch sample index " << i << "." << std::endl;
            return 21;
        }
    }

    return 0;
}

int specificMagWithEnvelope()
{
    // Instantiate CombGenerator for a max of NLines and Epoch Size
    CombGenerator combGenerator{ numHarmonics };

    // Initialize Mags. We will use a magnitude of 2.0.
    std::unique_ptr< double[] > magnitudes{ new double[ numHarmonics ] };
    for (size_t i = 0; numHarmonics != i; ++i )
        magnitudes[i] = 2.0;
    SharedScalarVectorType sharedMagnitudes{ std::move( magnitudes ) };

    // We're going to use an exponential decay for this test.
    std::unique_ptr< double[] > envelopeBuffer{new double[ maxEpochSize ] };
    auto envelopeFunk = [ &envelopeBuffer ](size_t nSample,
            size_t numSamples, size_t nHarmonic, double nominalMag )
    {
        auto tau = maxEpochSize / 2.0;
        auto pMag = envelopeBuffer.get();
        for ( size_t i = 0; numSamples != i; ++i )
        {
            auto env = nominalMag * std::exp( double( nSample++ ) / -tau );
            *pMag++ = env;
        }

        return envelopeBuffer.get();
    };

    // Reset the Comb Generator
    combGenerator.reset( numHarmonics, fundamentalRadiansPerSample,
                         sharedMagnitudes, nullptr, envelopeFunk );

    // We need a buffer to store signal data provided by the CombGenerator getSamples.
    // This buffer needs to be large enough for the maximum number of samples we will be retrieving.
    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer{new FlyingPhasorElementType [ maxEpochSize ] };
    FlyingPhasorElementBufferTypePtr pEpochSampleBuffer = epochSampleBuffer.get();

    // Get samples for harmonic series.
    combGenerator.getSamples( pEpochSampleBuffer, maxEpochSize );

    // To Verify the non-scintillated samples produced. We will use a FlyingPhasor and attempt to remove the tones
    // generated. Since both use FlyingPhasor instances in the same order, we expect the delta to be exactly zero.
    std::vector< FlyingPhasorToneGenerator > spectralLineGenerators{ maxEpochSize };
    std::unique_ptr< FlyingPhasorElementType[] > compareSampleBuffer{new FlyingPhasorElementType[ maxEpochSize ] };
    for (size_t i = 0; numHarmonics != i; ++i )
    {
        auto pEnvelope = envelopeFunk( 0, maxEpochSize, i, 2.0 );

        spectralLineGenerators[i].reset( double(i+1) * fundamentalRadiansPerSample, 0.0 );
        if ( 0 == i )
            spectralLineGenerators[i].getSamplesScaled(compareSampleBuffer.get(), maxEpochSize, pEnvelope );
        else
            spectralLineGenerators[i].accumSamplesScaled(compareSampleBuffer.get(), maxEpochSize, pEnvelope );
    }

    std::unique_ptr< FlyingPhasorElementType[] > deltaSampleBuffer{new FlyingPhasorElementType[ maxEpochSize ] };
    for ( size_t i = 0; maxEpochSize != i; ++i )
    {
        deltaSampleBuffer[i] = pEpochSampleBuffer[i] - compareSampleBuffer[i];
        if ( 0.0 != deltaSampleBuffer[i] )
        {
            std::cout << "Failed Specific Mag, With Envelope Test at epoch sample index " << i << "." << std::endl;
            return 31;
        }
    }

    return 0;
}

int main()
{
    // Test Number 1.
    int testResult = defaultMagPhaseNoEnvelope();
    if ( 0 != testResult ) return testResult;

    // Test Number 2.
    testResult = specificMagPhaseNoEnvelope();
    if ( 0 != testResult ) return testResult;

    // Test Number 3.
    testResult = defaultMagWithEnvelope();
    if ( 0 != testResult ) return testResult;

    // Test Number 4.
    testResult = specificMagWithEnvelope();
    if ( 0 != testResult ) return testResult;

    return 0;
}
