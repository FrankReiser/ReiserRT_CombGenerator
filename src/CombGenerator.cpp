/**
 * @file CombGenerator.cpp
 * @brief The implementation file for the Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#include "CombGenerator.h"
#include "FlyingPhasorToneGenerator.h"

#include <vector>
#include <memory>
#include <stdexcept>
#include <cstring>

using namespace TSG_NG;
using namespace ReiserRT::Signal;

class CombGenerator::Imple
{
private:
    friend class CombGenerator;

    Imple() = delete;

    Imple( size_t theMaxHarmonics, size_t theEpochSize )
      : maxHarmonics( theMaxHarmonics )
      , epochSize( theEpochSize )
      , harmonicGenerators{ maxHarmonics }
      , epochSampleBuffer{ new FlyingPhasorElementType[ epochSize ] }
    {
        std::memset( epochSampleBuffer.get(), 0, sizeof( FlyingPhasorElementType ) * epochSize );
    }

    void reset ( size_t theNumHarmonics, double fundamentalRadiansPerSample,
                 const double * pMagVector, const double * pPhaseVector,
                 const EnvelopeFunkType & theEnvelopeFunk )
    {
        // Ensure that the user has not specified more lines than they constructed us to handle.
        if ( maxHarmonics < theNumHarmonics )
            throw std::length_error{ "The number of harmonics exceeds the maximum allocated during construction!" };

        // Record number of lines and decorrelation samples
        numHarmonics = theNumHarmonics;

        // Record the Envelope Function which could be NULL.
        envelopeFunkType = theEnvelopeFunk;

        pMagnitude = pMagVector;

        // For each Spectral Line
        for (size_t i = 0; i != numHarmonics; ++i )
        {
            // Reset Spectral Line Tone Generator
            auto radiansPerSample = (i+1) * fundamentalRadiansPerSample;
            harmonicGenerators[ i ].reset(radiansPerSample, pPhaseVector ? *pPhaseVector++ : 0.0 );
        }

        std::memset( epochSampleBuffer.get(), 0, sizeof( FlyingPhasorElementType ) * epochSize );
    }

    const ReiserRT::Signal::FlyingPhasorElementBufferTypePtr getEpoch()
    {
        if ( !envelopeFunkType )
        {
            // For, each spectral line accumulate its samples.
            auto pMag = pMagnitude;
            for (size_t i = 0; i != numHarmonics; ++i )
            {
                auto mag = pMag ? *pMag++ : 1.0;
                // First line optimization, just get the samples. Accumulation not necessary.
                if ( 0 == i )
                    harmonicGenerators[ i ].getSamplesScaled(epochSampleBuffer.get(), epochSize, mag );
                else
                    harmonicGenerators[ i ].accumSamplesScaled(epochSampleBuffer.get(), epochSize, mag );
            }
        }
        else
        {
            auto pMag = pMagnitude;
            auto nSample = harmonicGenerators[0].getSampleCount();  // All the same
            for (size_t i = 0; i != numHarmonics; ++i )
            {
                auto mag = pMag ? *pMag++ : 1.0;
                auto pEnvelope = envelopeFunkType( nSample, i, mag );

                // First line optimization, just get the scintillated samples. Accumulation not necessary.
                if ( 0 == i )
                    harmonicGenerators[ i ].getSamplesScaled(epochSampleBuffer.get(), epochSize,
                                                             pEnvelope );
                else
                    harmonicGenerators[ i ].accumSamplesScaled(epochSampleBuffer.get(), epochSize,
                                                               pEnvelope );
            }
        }

        return epochSampleBuffer.get();
    }

    const size_t maxHarmonics;
    const size_t epochSize;
    std::vector< FlyingPhasorToneGenerator > harmonicGenerators;
    const double * pMagnitude{};

    EnvelopeFunkType envelopeFunkType{};

    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer;

    size_t numHarmonics{};
};

CombGenerator::CombGenerator(size_t maxHarmonics , size_t epochSize )
  : pImple{ new Imple{maxHarmonics, epochSize } }
{
}

CombGenerator::~CombGenerator()
{
    delete pImple;
}

void CombGenerator::reset ( size_t numHarmonics, double fundamentalRadiansPerSample,
                            const double * pMagVector, const double * pPhaseVector,
                            const EnvelopeFunkType & envelopeFunk )
{
    pImple->reset(numHarmonics, fundamentalRadiansPerSample,
                  pMagVector, pPhaseVector, envelopeFunk );
}

const ReiserRT::Signal::FlyingPhasorElementBufferTypePtr CombGenerator::getEpoch()
{
    return pImple->getEpoch();
}