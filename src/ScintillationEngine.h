/**
 * @file ScintillationEngine.h
 * @brief The specification file for the Scintillation Engine.
 * @authors Frank Reiser
 * @date Initiated September 23rd, 2022
 */

#ifndef TSG_NG_SCINTILLATIONENGINE_H
#define TSG_NG_SCINTILLATIONENGINE_H

#include "CombGeneratorExport.h"

#include <cstdlib>
#include <functional>

namespace TSG_NG
{
    class CombGenerator_EXPORT ScintillationEngine
    {
    public:
        // The first value of the pair represents the initial scintillated magnitude over the course of a decorrelation
        // period. The second value of the pair represents the change per sample in magnitude (i.e., a slope) from the initial
        // (first) value.
        using StateType = std::pair< double, double >;


        using RandomValueFunkType = std::function< double() >;

        ScintillationEngine() = delete;

        // Requires a buffer space for putting results. We do not take ownership of this buffer.
        ScintillationEngine( double * pTheScintillationBuffer, size_t theBufLen )
                : pScintillationBuffer( pTheScintillationBuffer )
                , bufLen( theBufLen )
        {
        }

        ///@note Invoking with decorrelationSamples of zero is a bad idea. Don't do it.
        void run(const RandomValueFunkType & randomFunk,
                 StateType & scintillationState,
                 size_t sampleCounter,
                 size_t decorrelationSamples );

    private:
        double * pScintillationBuffer;
        size_t bufLen;
    };


}

#endif //TSG_NG_SCINTILLATIONENGINE_H
