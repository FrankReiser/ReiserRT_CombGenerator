/**
 * @file ScintillationEngine.cpp
 * @brief The implementation file for the Scintillation Engine.
 * @authors Frank Reiser
 * @date Initiated September 23rd, 2022
 */

#include "ScintillationEngine.h"

void ScintillationEngine::run( double * pBuffer,
                               size_t runLen,
                               const ScintillateFunkType & scintillateFunk,
                               StateType & scintillationState,
                               size_t sampleCounter,
                               size_t decorrelationSamples )
{
    // Compute Scintillated Magnitude over the Buffer Length.
    for ( size_t i = 0; i != runLen; ++i )
    {
        // Set scintillation buffer magnitude value for sample i.
        // This is the current magnitude value (first) plus the change in magnitude per sample (second).
        // We conveniently update the magnitude while we are at it for the next iteration.
        // On the initial sample of zero, the change in magnitude per sample is zero.
        *pBuffer++ = scintillationState.first += scintillationState.second;

        // If time to calculate a new scintillation slope
        if ( 0 == (sampleCounter++ % decorrelationSamples ) )
        {
            // Get a new scintillation target magnitude by invoking scintillateFunk observer.
            auto scintillationTargetMag = scintillateFunk();

            // Calculate the change in magnitude per sample and store as the second parameter for the line.
            scintillationState.second = (scintillationTargetMag - scintillationState.first ) /
                    double( decorrelationSamples );
        }
    }
}

