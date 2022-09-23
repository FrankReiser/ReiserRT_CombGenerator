/**
 * @file ScintillationEngine.cpp
 * @brief The implementation file for the Scintillation Engine.
 * @authors Frank Reiser
 * @date Initiated September 23rd, 2022
 */

#include "ScintillationEngine.h"

using namespace TSG_NG;

void ScintillationEngine::run(const RandomValueFunkType & randomFunk,
                              StateType & scintillationState,
                              size_t sampleCounter,
                              size_t decorrelationSamples )
{
    // Compute Scintillated Magnitude over the Buffer Length.
    auto pScintillationMag = pScintillationBuffer;
    for ( int i = 0; i != bufLen; ++i )
    {
        // Set scintillation buffer magnitude value for sample i.
        // This is the current magnitude value (first) plus the change in magnitude per sample (second).
        // We conveniently update the magnitude while we are at it for the next iteration.
        // On the initial sample of zero, the change in magnitude per sample is zero.
        *pScintillationMag++ = scintillationState.first += scintillationState.second;

        // If time to calculate a new scintillation slope
        if ( 0 == (sampleCounter++ % decorrelationSamples ) )
        {
            // Get a new scintillation target magnitude by invoking randomFunk observer.
            auto scintillationTargetMag = randomFunk();

            // Calculate the change in magnitude per sample and store as the second parameter for the line.
            scintillationState.second = (scintillationTargetMag - scintillationState.first ) / decorrelationSamples;
        }
    }
}

