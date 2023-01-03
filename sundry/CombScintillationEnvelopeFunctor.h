// Created on 20230103

#ifndef REISER_RT_COMBGENERATOR_COMBSCINTILLATIONENVELOPEFUNCTOR_H
#define REISER_RT_COMBGENERATOR_COMBSCINTILLATIONENVELOPEFUNCTOR_H

#include "SharedScalarVectorTypeFwd.h"

#include <cstddef>
#include <cstdint>

class CombScintillationEnvelopeFunctor
{
private:
    class Imple;

public:
    CombScintillationEnvelopeFunctor( size_t maxHarmonics, size_t maxEpochSize );
    ~CombScintillationEnvelopeFunctor();

    CombScintillationEnvelopeFunctor( const CombScintillationEnvelopeFunctor & another ) = delete;
    CombScintillationEnvelopeFunctor & operator =( const CombScintillationEnvelopeFunctor & another ) = delete;

    void reset( size_t numHarmonics, size_t decorrelationSamples,
                const ReiserRT::Signal::SharedScalarVectorType & pNominalMagnitudes, uint32_t seed );

    const double * operator()( size_t currentSampleCount, size_t numSamples, size_t nHarmonic, double nominalMag );

private:
    Imple * pImple;
};

#endif //REISER_RT_COMBGENERATOR_COMBSCINTILLATIONENVELOPEFUNCTOR_H
