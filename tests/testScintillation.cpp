//
// Created by frank on 9/17/22.
//

#include "ScintillationEngine.h"

#include <memory>
#include <iostream>
#include <cmath>

bool inTolerance( double value, double desiredValue, double toleranceRatio )
{
    auto limitA = desiredValue * ( 1 + toleranceRatio );
    auto limitB = desiredValue * ( 1 - toleranceRatio );
    auto minValue = std::signbit( desiredValue ) ? limitA : limitB;
    auto maxValue = std::signbit( desiredValue ) ? limitB : limitA;

    return ( minValue <= value && value <= maxValue );
}

int main()
{
    // We're going to need a scintillation buffer to store gradually changing magnitude values over
    // some number of samples.
    constexpr size_t EPOCH_SIZE = 8192;
    std::unique_ptr< double[] > scintillationBuffer{ new double[ EPOCH_SIZE ] };

    // And with that, we can now instantiate our scintillation engine.
    TSG_NG::ScintillationEngine scintillationEngine{ scintillationBuffer.get(), EPOCH_SIZE };

    // Before we can use the ScintillationEngine, we need a have function that will give it a differing random number
    // when required. This would normally come from a specialized distribution. We are going to fake this so
    // we can focus on the test. We're going to set our decorrelation samples up so that we get some number of requests
    constexpr size_t DECORREL_SAMPLES = 4000;
    constexpr size_t DECORREL_PERIODS = EPOCH_SIZE / DECORREL_SAMPLES + 1;
    double scintillationValues[ DECORREL_PERIODS ] = { 2.0, 0.9,  1.5 }; // Kludge
    size_t scintillationIndex = 0;
    auto randFunk = [& scintillationIndex, &scintillationValues ]()
    {
        return scintillationValues[ scintillationIndex++ ];
    };

    // Invoke the ScintillationEngine's run function. It requires mutable scintillation parameters.
    // We will start will default values of 1 for magnitude and zero for slope. The initial slope will be
    // computed immediately.
    TSG_NG::ScintillationEngine::StateType scintillationState{1.0, 0.0 };
    scintillationEngine.run(std::ref( randFunk), scintillationState, 0, DECORREL_SAMPLES );

    // For maximum view of significant digits for diagnostic purposes.
    std::cout << std::scientific;
    std::cout.precision(17);

    // Now we want to look at what happened.
    // The initial sample should be a value of 1.0.
    auto pValue = scintillationBuffer.get();
    if ( 1.0 != pValue[0] )
    {
        std::cout << "Failed first decorrelation period: Initial Value expected to be 1.0 and is " << pValue[0] << std::endl;
        return 1;
    }
    ++pValue;

    // We should now see a trend of values towards the first scintillation point.
    // The scintillation engine uses simple accumulation to achieve its goals. However,
    // this will accumulate some error. We do not expect it to be very large though.
    // What will we tolerate over the course of a decorrelation period?
    // We will have to use multiplication of a slope to detect error.
    auto slopeFirstDecorrelationPeriod = ( scintillationValues[0] - 1.0 ) / DECORREL_SAMPLES;
    for ( size_t i = 1; DECORREL_SAMPLES != i; ++i )
    {
        const auto expectedValue = 1.0 + i * slopeFirstDecorrelationPeriod;
        if ( !inTolerance( *pValue, expectedValue, 1E-12 ) )
        {
            std::cout << "Failed first decorrelation period: Value[ " << i << " ] expected to be "
                << expectedValue << " and is " << *pValue << " - Out of Tolerance" << std::endl;
            return 2;
        }
        ++pValue;
    }

    // The value at the beginning of the second decorrelation period should be nearly equivalent to
    // the value of our first scintillation point. It may be off somewhat due to minor error accumulation.
    // Note though that this does not affect the accuracy of the next scintillation target.
    if ( !inTolerance( *pValue, scintillationValues[ 0 ], 1E-12 ) )
    {
        std::cout << "Failed second decorrelation period: Initial Value expected to be " << scintillationValues[ 0 ]
            << " and is " << *pValue << std::endl;
        return 3;
    }
    ++pValue;

    // We should now see a trend of values towards the second scintillation point.
    auto slopeSecondDecorrelationPeriod = ( scintillationValues[1] - scintillationValues[0] ) / DECORREL_SAMPLES;
    for ( size_t i = 1; DECORREL_SAMPLES != i; ++i )
    {
        const auto expectedValue = scintillationValues[0] + i * slopeSecondDecorrelationPeriod;
        if ( !inTolerance( *pValue, expectedValue, 1E-12 ) )
        {
            std::cout << "Failed second decorrelation period: Value[ " << i+DECORREL_SAMPLES << " ] expected to be "
                      << expectedValue << " and is " << *pValue << " - Out of Tolerance" << std::endl;
            return 4;
        }
        ++pValue;
    }

    // The value at the beginning of the third decorrelation period should be nearly equivalent to
    // the value of our second scintillation point. It may be off somewhat due to minor error accumulation.
    // Note again though, that this does not affect the accuracy of the next scintillation target.
    if ( !inTolerance( *pValue, scintillationValues[ 1 ], 1E-12 ) )
    {
        std::cout << "Failed third decorrelation period: Initial Value expected to be " << scintillationValues[ 1 ]
                  << " and is " << *pValue << std::endl;
        return 5;
    }
    ++pValue;

    // We will test what is only a partial, third decorrelation period to the end of the epoch buffer.
    auto slopeThirdDecorrelationPeriod = ( scintillationValues[2] - scintillationValues[1] ) / DECORREL_SAMPLES;
    for ( int i = 1; ( EPOCH_SIZE % DECORREL_SAMPLES ) != i; ++i )
    {
        const auto expectedValue = scintillationValues[1] + i * slopeThirdDecorrelationPeriod;
        if ( !inTolerance( *pValue, expectedValue, 1E-12 ) )
        {
            std::cout << "Failed third decorrelation period: Value[ " << i+DECORREL_SAMPLES*2 << " ] expected to be "
                      << expectedValue << " and is " << *pValue << " - Out of Tolerance" << std::endl;
            return 6;
        }
        ++pValue;
    }

    return 0;
}