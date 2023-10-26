/**
 * @file testPureReset.cpp
 * @brief Test Harness for Comb Generator `reset` operation with no parameters.
 *
 * The `reset` operation with no parameters resets as if just constructed.
 * We want to verify we get no signal output when we use it.
 *
 * @authors Frank Reiser
 * @date Initiated October 25th, 2023
 */
#include "CombGenerator.h"
#include "FlyingPhasorToneGenerator.h"

#include <stdexcept>
#include <iostream>

using namespace ReiserRT::Signal;


int testPureReset()
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

    // Now perform a "pure" reset which should set us up as if we have just been constructed.
    combGenerator.reset();

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
        std::cout << "Failed Pure Reset Test. Zero signal data detected before pure reset." << std::endl;
        return 1;
    }

    // Get samples after reset
    combGenerator.getSamples( pEpochSampleBuffer, maxEpochSize );

    // The buffer should contain all zeros
    for ( size_t i = 0; maxEpochSize != i; ++i )
    {
        const auto sample = pEpochSampleBuffer[i];
        if ( 0.0 != sample )
        {
            std::cout << "Failed Pure Reset Test at epoch sample index " << i << "." << std::endl;
            return 2;
        }
    }

    return 0;
}

int main()
{

    return 0;
}
