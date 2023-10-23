/**
 * @file CombGenerator.cpp
 * @brief The implementation file for the Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#include "CombGenerator.h"
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

        // Record number of lines and de-correlation samples
        numHarmonics = theNumHarmonics;

        // Record the Magnitude vector for later use by getSamples.
        magVector = theMagVector;

        // Record the Envelope Function which could be empty.
        envelopeFunkType = theEnvelopeFunk;

        // Reset each Harmonic Tone Generator specified.
        auto pPhase = thePhaseVector.get();
        for ( size_t i = 0; numHarmonics != i; ++i )
        {
            const auto radiansPerSample = double(i+1) * fundamentalRadiansPerSample;
            harmonicGenerators[i].reset( radiansPerSample, pPhase ? *pPhase++ : 0.0 );
        }

        // Reset the excess harmonic generators. We do not want them to contain garbage.
        for (size_t i = numHarmonics; maxHarmonics != i; ++i )
            harmonicGenerators[i].reset();
    }

    void getSamples( FlyingPhasorElementBufferTypePtr pElementBuffer, size_t numSamples )
    {
        // Get pointer to harmonic magnitudes. This is allowed to be nullptr.
        auto pMag = magVector.get();

        // If no envelope functor, we use a constant magnitude.
        if ( !envelopeFunkType )
        {
            // For each harmonic tone specified last reset, accumulate its samples.
            for ( size_t i = 0; numHarmonics != i; ++i )
            {
                // Get the nth harmonic magnitude or default to unity gain.
                auto mag = pMag ? *pMag++ : 1.0;

                // Fundamental tone optimization: We just store the samples. Otherwise, we accumulate.
                if ( 0 == i )
                    harmonicGenerators[i].getSamplesScaled( pElementBuffer, numSamples, mag );
                else
                    harmonicGenerators[i].accumSamplesScaled( pElementBuffer, numSamples, mag );
            }
        }
        // Else, we have an envelope functor. We will utilize it
        else
        {
            // For, each spectral line accumulate its envelope modulated samples
            auto nSample = harmonicGenerators[0].getSampleCount();  // All the same

            // For each harmonic tone specified last reset, accumulate its samples.
            for ( size_t i = 0; numHarmonics != i; ++i )
            {
                // Get the nth harmonic magnitude or default to unity gain.
                auto mag = pMag ? *pMag++ : 1.0;

                // Invoke the envelope functor for this harmonic to obtain its modulation envelope.
                auto pEnvelope = envelopeFunkType( nSample, numSamples, i, mag );

                // Fundamental tone optimization: We just store the samples. Otherwise, we accumulate.
                if ( 0 == i )
                    harmonicGenerators[i].getSamplesScaled( pElementBuffer, numSamples, pEnvelope );
                else
                    harmonicGenerators[i].accumSamplesScaled( pElementBuffer, numSamples, pEnvelope );
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
