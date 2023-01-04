// Created on 20230103
#ifndef TSG_NG_SCINTILLATIONENGINE_H
#define TSG_NG_SCINTILLATIONENGINE_H

#include <cstdlib>
#include <functional>

class ScintillationEngine
{
public:
    using StateType = std::pair< double, double >;

    using ScintillateFunkType = std::function< double() >;

    ScintillationEngine() = default;

    ~ScintillationEngine() = default;

    static void run( double * pBuffer,
                     size_t runLen,
                     const ScintillateFunkType & scintillateFunk,
                     StateType & scintillationState,
                     size_t sampleCounter,
                     size_t decorrelationSamples );
};

#endif //TSG_NG_SCINTILLATIONENGINE_H
