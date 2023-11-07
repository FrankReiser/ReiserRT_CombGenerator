// Created on 20230103

#include "CombScintillationEnvelopeFunctor.h"
#include "RayleighDistributor.h"
#include "ScintillationEngine.h"

#include <memory>
#include <cstring>
#include <vector>

class CombScintillationEnvelopeFunctor::Imple
{
public:
    Imple() = delete;

private:
    friend class CombScintillationEnvelopeFunctor;

    Imple( size_t theMaxHarmonics, size_t theEpochSize )
      : maxHarmonics{ theMaxHarmonics }
      , maxEpochSize{ theEpochSize }
      , envelopeBuffer{ new double[ maxEpochSize ] }
      , scintillationStates{ maxHarmonics }
    {
        std::memset( envelopeBuffer.get(), 0, sizeof ( double ) * maxEpochSize );
    }

    ~Imple() = default;

    void reset(size_t numHarmonics, size_t theDecorrelationSamples,
               const ReiserRT::Signal::CombGeneratorScalarVectorType & pNominalMagnitudes, uint32_t seed )
    {
        decorrelationSamples = theDecorrelationSamples;
        rayleighDistributor.reset( seed );

        // Initialize Scintillation State Vector for each harmonic tone.
        auto pNominalMag = pNominalMagnitudes.get();
        for ( size_t i = 0; i != numHarmonics; ++i )
        {
            // The 'first' field is initial scintillated magnitude value and the 'second' field is
            // reserved for the delta magnitude per sample value. We initialize this to zero as it
            // will be set on the very first sample obtained per harmonic tone.
            auto & ss = scintillationStates[i];
            ss.first = rayleighDistributor.getValue( pNominalMag ? *pNominalMag++ : 1.0 );
            ss.second = 0.0;    // The delta (slope) gets set by scintillation logic
        }
        std::memset( envelopeBuffer.get(), 0, sizeof ( double ) * maxEpochSize );
    }

    const double * operator()( size_t currentSampleCount, size_t numSamples, size_t nHarmonic, double nominalMag )
    {
        ///@todo Throw if nHarmonic is greater than or equal to max harmonics?
        ///What about numSamples and maxEpochSize.

        auto scintillateFunk = [ this, nominalMag ]()
        {
            return rayleighDistributor.getValue( nominalMag );
        };

        auto & ss = scintillationStates[ nHarmonic ];

        ScintillationEngine::run(envelopeBuffer.get(), numSamples,
             std::ref( scintillateFunk ), ss, currentSampleCount, decorrelationSamples );

        return envelopeBuffer.get();
    }

    const size_t maxHarmonics;
    const size_t maxEpochSize;
    std::unique_ptr< double[] > envelopeBuffer;
    size_t decorrelationSamples{};
    RayleighDistributor rayleighDistributor{};
    std::vector< ScintillationEngine::StateType > scintillationStates;
};

CombScintillationEnvelopeFunctor::CombScintillationEnvelopeFunctor( size_t maxHarmonics, size_t maxEpochSize )
  : pImple{ new Imple{ maxHarmonics, maxEpochSize } }
{
}

CombScintillationEnvelopeFunctor::~CombScintillationEnvelopeFunctor()
{
    delete pImple;
}

void CombScintillationEnvelopeFunctor::reset(size_t numHarmonics, size_t decorrelationSamples,
                                             const ReiserRT::Signal::CombGeneratorScalarVectorType & pNominalMagnitudes, uint32_t seed )
{
    pImple->reset( numHarmonics, decorrelationSamples, pNominalMagnitudes, seed );
}

const double * CombScintillationEnvelopeFunctor::operator ()( size_t currentSampleCount,
        size_t numSamples, size_t nHarmonic, double nominalMag )
{
    return (*pImple)( currentSampleCount, numSamples, nHarmonic, nominalMag );
}
