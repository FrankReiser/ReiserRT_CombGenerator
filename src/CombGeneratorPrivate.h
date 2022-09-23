/**
 * @file CombGeneratorPrivate.h
 * @brief The specification file for the Comb Generator Private Purposes.
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#ifndef COMBGENERATOR_COMBGENERATORPRIVATE_H
#define COMBGENERATOR_COMBGENERATORPRIVATE_H

#include "CombGeneratorExport.h"

#include <functional>
#include <cstdlib>

///@todo Deprecate as much of this as possible for new "Exportable" APIs


// The first value of the pair represents the initial scintillated magnitude over the course of a decorrelation
// period. The second value of the pair represents the change per sample in magnitude (i.e., a slope) from the initial
// (first) value.
using ScintillationParamsType = std::pair< double, double >;

// Provides unit testability of scintillation logic without the entire baggage of the CombGenerator.
// Also, reduces the need for redundant and painful testing of that logic once integrated.
///@note We EXPORT this so we can test it. It should to be invisible to the linker because it an
///implementation detail but then we cannot test it. Regardless, this header file doesn't get installed,
///so it would be difficult to write anything but a 'ctest' against it.
class CombGenerator_EXPORT ScintillationEngine
{
public:
    using RandomValueFunkType = std::function< double() >;

    ScintillationEngine() = delete;

    // Requires a buffer space for putting results. We do not take ownership of this buffer.
    ScintillationEngine( double * pTheScintillationBuffer, size_t theBufLen )
      : pScintillationBuffer( pTheScintillationBuffer )
      , bufLen( theBufLen )
    {
    }

    ///@note Invoking with decorrelationSamples of zero is a bad idea. Don't do it.
    void run( const RandomValueFunkType & randomFunk,
             ScintillationParamsType & scintillationParams,
             size_t sampleCounter,
             size_t decorrelationSamples );

private:
    double * pScintillationBuffer;
    size_t bufLen;
};

#endif //COMBGENERATOR_COMBGENERATORPRIVATE_H
