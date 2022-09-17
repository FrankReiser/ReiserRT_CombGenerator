/**
 * @file CombGeneratorPrivate.h
 * @brief The specification file for the Comb Generator Private Purposes.
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#ifndef COMBGENERATOR_COMBGENERATORPRIVATE_H
#define COMBGENERATOR_COMBGENERATORPRIVATE_H

#include <functional>
#include <cstdlib>

using ScintillationParamsType = std::pair< double, double >;

// Provides unit testability of scintillation logic without the entire baggage of the CombGenerator.
// Also, reduces the need for redundant and painful testing of the logic.
class ScintillationHarness
{
public:
    using RandomValueFunkType = std::function< double() >;

    ScintillationHarness() = delete;

    // Requires a buffer space for putting results.
    ScintillationHarness( double * pTheScintillationBuffer, size_t theBufLen )
      : pScintillationBuffer( pTheScintillationBuffer )
      , bufLen( theBufLen )
    {
    }

    ///@note Invoking with decorrelationSamples of zero is a bad idea. Don't do it.
    void run( const RandomValueFunkType & randomFunk,
             ScintillationParamsType & scintillationParams,
             size_t startingSampleCount,
             size_t decorrelationSamples );

private:
    double * pScintillationBuffer;
    size_t bufLen;
};

#endif //COMBGENERATOR_COMBGENERATORPRIVATE_H
