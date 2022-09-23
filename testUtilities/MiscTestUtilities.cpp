// Created on 20220108

#include "MiscTestUtilities.h"

#include <cstring>
#include <iostream>
#include <iomanip>

#if 0
#include <cmath>

bool inTolerance( double value, double desiredValue, double toleranceRatio )
{
    auto limitA = desiredValue * ( 1 + toleranceRatio );
    auto limitB = desiredValue * ( 1 - toleranceRatio );
    auto minValue = std::signbit( desiredValue ) ? limitA : limitB;
    auto maxValue = std::signbit( desiredValue ) ? limitB : limitA;

    return ( minValue <= value && value <= maxValue );
}

double deltaAngle( double angleA, double angleB )
{
    auto delta = angleB - angleA;
    if ( delta > M_PI ) delta -= 2*M_PI;
    else if ( delta < -M_PI ) delta += 2*M_PI;
    return delta;
}
#endif

BinBufferIntType categorizeIntoBins( size_t nBins, size_t sampleSize,
                                     const DistributionFunkType & funk, double binZeroOffset, double fullScale )
{
    auto binBuffer = BinBufferIntType{new size_t[nBins] };
    std::memset( binBuffer.get(), 0, sizeof( size_t ) * nBins );

    auto fullSpan = (fullScale - binZeroOffset);
    for ( size_t j=0; sampleSize!=j; ++j )
    {
        // Get a value from the supplied random number function.
        auto val = funk();

        // If the value is inside the bound, we will accumulate it
        // into the appropriate bin.
        if ( (binZeroOffset <= val) && (fullScale > val) )
        {
            // Calculate the zero based bin from the number of bins and
            // the proportion of the span.
            auto zbBin = static_cast< size_t >( double(nBins) * ( val - binZeroOffset ) / fullSpan );
            ++binBuffer[ zbBin ];
        }
    }

    return std::move( binBuffer );
}

double chiSquared( size_t nBins, const BinBufferIntType & observed, const BinBufferRealType & expected )
{
    double P = 0;
    for ( size_t i=0; i!=nBins; ++i )
    {
        const auto expect = expected[i];
        const auto delta = static_cast< double >( observed[i] ) - expect;
        P += delta * delta / expect;
    }
    return P;
}

void plotDistribution( const BinBufferIntType & binBuffer, size_t nBins,
                       double chiSquaredRes, double binZeroOffset, double fullScale )
{
    std::cout << "Diagnostic (1-10 observations per star) - Chi-Squared Result: "
              << chiSquaredRes << std::endl;
    auto fullRange = fullScale - binZeroOffset;
    for ( size_t i = 0; nBins != i; ++i )
    {
        std::cout << "Bin: " << std::fixed << std::setw( 2 ) << i << " ("
                  << std::setprecision( 3 ) << std::setw(15)
                  << binZeroOffset + fullRange*double(i) / double(nBins)  << "+)\t";
        auto n = binBuffer[ i ] / 10;
        if ( 0 != ( binBuffer[ i ] % 10 )  ) ++n;
        for ( size_t j = 0; n != j; ++j )
        {
            std::cout << '*';
        }
        std::cout << std::endl;
    }
}