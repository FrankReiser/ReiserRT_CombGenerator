/**
 * @file CombGenerator.cpp
 * @brief The implementation file for the Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#include "CombGenerator.h"
#include "SharedScalarVectorTypeFwd.h"
#include "FlyingPhasorToneGenerator.h"

#include <vector>
#include <stdexcept>
#include <cstring>

using namespace ReiserRT::Signal;

class CombGenerator::Imple
{
// Deleted Operations Should be `public`
public:
    Imple() = delete;

// Everything else is private but accessible by our nested class.
private:
    friend class CombGenerator;

    explicit Imple( size_t theMaxHarmonics )
      : maxHarmonics( theMaxHarmonics )
      , harmonicGenerators{ maxHarmonics }
    {
    }

    void reset ( size_t theNumHarmonics, double fundamentalRadiansPerSample,
                 const SharedScalarVectorType & theMagVector, const SharedScalarVectorType & thePhaseVector,
                 const CombGeneratorEnvelopeFunkType & theEnvelopeFunk )
    {
        // Ensure that the user has not specified more lines than they constructed us to handle.
        if ( maxHarmonics < theNumHarmonics )
            throw std::length_error{ "The number of harmonics exceeds the maximum allocated during construction!" };

        // Record number of lines and decorrelation samples
        numHarmonics = theNumHarmonics;

        // Record the Magnitude vector for later use by getSamples.
        magVector = theMagVector;

        // Record the Envelope Function which could be empty.
        envelopeFunkType = theEnvelopeFunk;

        // Reset each Spectral Line Tone Generator
        auto pPhase = thePhaseVector.get();
        size_t i = 0;
        for ( auto & hg : harmonicGenerators )
        {
            auto radiansPerSample = double(++i) * fundamentalRadiansPerSample;
            hg.reset(radiansPerSample, pPhase ? *pPhase++ : 0.0 );
        }
    }

    void getSamples( FlyingPhasorElementBufferTypePtr pElementBuffer, size_t numSamples )
    {
        // Local flag/counter depending on presence of non-empty envelope functor.
        size_t i = 0;

        // If no envelope functor
        if ( !envelopeFunkType )
        {
            // For, each spectral line accumulate its samples.
            auto pMag = magVector.get();
            for ( auto & hg : harmonicGenerators )
            {
                auto mag = pMag ? *pMag++ : 1.0;

                // First line optimization, just get the samples. Accumulation not necessary.
                // Also note, we use local `i` as a flag here. We are not counting it as we
                // do not need to know.
                if ( 0 == i )
                {
                    ++i;
                    hg.getSamplesScaled( pElementBuffer, numSamples, mag );
                }
                else
                    hg.accumSamplesScaled( pElementBuffer, numSamples, mag );
            }
        }
        // Else, envelope functor
        else
        {
            // For, each spectral line accumulate its envelope modulated samples
            auto pMag = magVector.get();
            auto nSample = harmonicGenerators[0].getSampleCount();  // All the same
            for ( auto & hg : harmonicGenerators )
            {
                // Invoke the envelope functor for this harmonic to obtain its modulation envelop.
                auto mag = pMag ? *pMag++ : 1.0;
                auto pEnvelope = envelopeFunkType( nSample, numSamples, i, mag );

                // First line optimization, just get the modulated samples. Accumulation not necessary.
                if ( 1 == ++i )
                    hg.getSamplesScaled( pElementBuffer, numSamples, pEnvelope );
                else
                    hg.accumSamplesScaled( pElementBuffer, numSamples, pEnvelope );
            }
        }
    }

    const size_t maxHarmonics;
    std::vector< FlyingPhasorToneGenerator > harmonicGenerators;
    SharedScalarVectorType magVector{};
    CombGeneratorEnvelopeFunkType envelopeFunkType{};
    size_t numHarmonics{};
};

CombGenerator::CombGenerator(size_t maxHarmonics )
  : pImple{ new Imple{maxHarmonics} }
{
}

CombGenerator::~CombGenerator()
{
    delete pImple;
}

void CombGenerator::reset( size_t numHarmonics, double fundamentalRadiansPerSample,
                            const SharedScalarVectorType & magVector, const SharedScalarVectorType & phaseVector,
                            const CombGeneratorEnvelopeFunkType & envelopeFunk )
{
    pImple->reset( numHarmonics, fundamentalRadiansPerSample,
                   magVector, phaseVector, envelopeFunk );
}

void CombGenerator::getSamples( FlyingPhasorElementBufferTypePtr pElementBuffer, size_t numSamples )
{
    pImple->getSamples( pElementBuffer, numSamples );
}
