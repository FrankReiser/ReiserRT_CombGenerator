// Created on 20220108

#ifndef COMB_GENERATOR_MISCTESTUTILITIES_H
#define COMB_GENERATOR_MISCTESTUTILITIES_H

#include <memory>
#include <functional>

#if 0
bool inTolerance( double value, double desiredValue, double toleranceRatio );

double deltaAngle( double angleA, double angleB );
#endif


// This is the buffer type we will use for observed bin counts from various distributions.
// Observed values within a bin can only be integer, therefore we use an integer type.
// However, it may be used for other purposes.
using BinBufferIntType = std::unique_ptr< size_t[] >;

// Distribution Function Type. Function call operator returns a real value from a particular distribution.
using DistributionFunkType = std::function< double() >;

BinBufferIntType categorizeIntoBins( size_t nBins, size_t sampleSize, const DistributionFunkType & funk,
                                    double binZeroOffset, double fullScale );


// This is the buffer type we will use for expected bin counts from various distributions.
// Expected counts within a bin may can be fractional, therefore we use a floating point type.
// However, it may be used for other purposes where a buffer of real values are needed.
using BinBufferRealType = std::unique_ptr< double[] >;

// This function returns an aggregate "power" level of the errors between observed and expected values.
double chiSquared(size_t nBins, const BinBufferIntType & observed, const BinBufferRealType & expected );


// This function uses ASCII text to plot the distribution.
void plotDistribution( const BinBufferIntType & binBuffer, size_t nBins,
                       double chiSquaredRes, double binZeroOffset, double fullScale );


#endif //COMB_GENERATOR_MISCTESTUTILITIES_H
