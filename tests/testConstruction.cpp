/**
 * @file testConstruction.cpp
 * @brief Test Harness for Comb Generator Constructors and Basic Functionality
 *
 * This test checks default CombGenerator construction, qualified construction, move assignment,
 * and move construction. It also verifies some basic functionality such as throwing of
 * exceptions when exceeding the maximum number of internal FlyingPhasorToneGenerator instances specified
 * during construction, and not throwing when remaining in bounds.
 *
 * We also verify that a default constructed instance is incapable of producing non-zero signal data.
 * Lastly, the "pure" reset functionality is verified.
 *
 * This seems like a lot to do to verify "construction" which is the primary purpose of this test.
 * It's just too convenient to test basic features while we are at it. However, we do not verify
 * the quality of any non-zero signal data during this test. That will be accomplished in a more
 * specific test.
 *
 * @authors Frank Reiser
 * @date Initiated October 25th, 2023
 */
#include "CombGenerator.h"
#include "FlyingPhasorToneGenerator.h"

#include <memory>
#include <stdexcept>
#include <iostream>


using namespace ReiserRT::Signal;


int testDefaultConstructAndMove()
{
    // Create a default constructed object. It is relatively useless as such. It shall not
    // be capable of generating a signal because it does not allocate facilities for doing so.
    CombGenerator combGenerator{};

    // Query the number of combGenerators. This shall be zero. We have not attempted to add any yet.
    auto nH = combGenerator.getNumHarmonics();
    if ( 0 != nH )
    {
        std::cout << "Failed Default Construct Test. Query of nHs should be zero and is " << nH << "." << std::endl;
        return 1;
    }

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

    // The buffer should contain all zeros. A default constructed CombGenerator shall be incapable of
    // producing non-zero signal data. However, it shall overwrite the buffer with zero signal data.
    for ( size_t i = 0; maxEpochSize != i; ++i )
    {
        const auto sample = pEpochSampleBuffer[i];
        if ( 0.0 != sample )
        {
            std::cout << "Failed Pure Reset Test at epoch sample index " << i << "." << std::endl;
            return 2;
        }
    }

    // Attempt to reset the generator with a requested number of harmonics which exceeds the maximum
    // specified during construction. This should throw `std::length_error`.
    bool fail = true;
    try
    {
        combGenerator.reset( 1, M_PI / 8.0, nullptr, nullptr );
        std::cout << "Failed to detect exception for exceeding maxHarmonics during reset invocation!" << std::endl;
    }
    catch ( const std::length_error & )
    {
        fail = false;
    }
    if ( fail ) return 3;

    // Now we will try to move a useful CombGenerator into the useless one via "move" assignment.
    // And if we can reset it without it throwing. It worked.
    constexpr size_t maxHarmonics = 4;
    combGenerator = CombGenerator{ maxHarmonics };

    // Query the number of Harmonics established. This shall be zero. We have not specified any yet
    // via the reset operation.
    nH = combGenerator.getNumHarmonics();
    if ( 0 != nH )
    {
        std::cout << "Failed Move Assignment Test. Query of nHs should be zero and is " << nH << "." << std::endl;
        return 4;
    }

    // Attempt to reset the generator with a requested number of harmonics now that we've reassigned it.
    // This shall not throw.
    fail = false;
    try
    {
        combGenerator.reset( 1, M_PI / 8.0, nullptr, nullptr );
    }
    catch ( const std::length_error & )
    {
        fail = true;
        std::cout << "Failed with exception thrown after move reassignment!" << std::endl;
    }
    if ( fail ) return 5;

    // Query the number of harmonics. This shall be 1 now.
    nH = combGenerator.getNumHarmonics();
    if ( 1 != nH )
    {
        std::cout << "Failed Move Assignment Test. Query of nHs should be one and is " << nH << "." << std::endl;
        return 6;
    }

    // Now move construct another CombGenerator from the one we've been working with.
    CombGenerator combGenerator2{ std::move( combGenerator ) };

    // Query the number of harmonics. This shall be 1 as this is what was moved.
    nH = combGenerator2.getNumHarmonics();
    if ( 1 != nH )
    {
        std::cout << "Failed Move Construct Test. Query of nHs should be one and is " << nH << "." << std::endl;
        return 7;
    }

    // Attempt to get samples from it
    combGenerator2.getSamples( pEpochSampleBuffer, maxEpochSize );

    // Buffer should contain non-zero data. That's all we need to verify as we will test
    // signal quality elsewhere.
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
        return 8;
    }

    // Now perform a "pure" reset which should set us up as if we have just been constructed.
    combGenerator2.reset();

    // Test that a "pure" reset returns to the constructed state
    nH = combGenerator2.getNumHarmonics();
    if ( 0 != nH )
    {
        std::cout << "Failed Reset Test. Query of nHs should be zero and is " << nH << "." << std::endl;
        return 9;
    }

    // Get samples after "pure" reset
    combGenerator2.getSamples( pEpochSampleBuffer, maxEpochSize );

    // The buffer should contain all zeros
    for ( size_t i = 0; maxEpochSize != i; ++i )
    {
        const auto sample = pEpochSampleBuffer[i];
        if ( 0.0 != sample )
        {
            std::cout << "Failed Pure Reset Test at epoch sample index " << i << "." << std::endl;
            return 10;
        }
    }

    return 0;
}


int main()
{
    // Test 5 - Test the default construct and move features. This was added after the fact,
    // so we will test it after the above.
    auto testResult = testDefaultConstructAndMove();
    if ( 0 != testResult ) return testResult;


    return 0;
}
