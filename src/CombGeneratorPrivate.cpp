/**
 * @file CombGeneratorPrivate.cpp
 * @brief The implementation file for the Comb Generator Private Purposes.
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#include "CombGeneratorPrivate.h"

void ScintillationHarness::run( const RandomValueFunkType & randomFunk,
        ScintillationParamsType & scintillationParams,
        size_t startingSampleCount,
        size_t decorrelationSamples )
{
    // Compute Scintillated Magnitude over the Buffer Length.
    for ( int i = 0; i != bufLen; ++i )
    {
        auto pScintillationMag = pScintillationBuffer;

        // Set scintillation buffer magnitude value for sample i.
        // This is the current magnitude value (first) plus the change in magnitude per sample (second).
        // We conveniently update the magnitude while we are at it for the next iteration.
        // On the initial sample of zero, the change in magnitude per sample is zero.
        *pScintillationMag++ = scintillationParams.first += scintillationParams.second;

        // If time to calculate a new scintillation slope
        if ( 0 == ( startingSampleCount++ % decorrelationSamples ) )
        {
            // Get a new scintillation target magnitude based off of normal magnitude.
            auto scintillationTargetMag = randomFunk();

            // Calculate the change in magnitude per sample and store as the second parameter for the line.
            scintillationParams.second = ( scintillationTargetMag - scintillationParams.first ) / decorrelationSamples;
        }
    }
}
