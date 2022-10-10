/**
 * @file CombGenerator.cpp
 * @brief The implementation file for the Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#include "CombGenerator.h"
#include "CombGeneratorResetParameters.h"
#include "FlyingPhasorToneGenerator.h"
#include "ScintillationEngine.h"

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

    Imple(size_t theMaxHarmonics, size_t theEpochSize )
      : maxHarmonics( theMaxHarmonics )
      , epochSize( theEpochSize )
      , harmonicGenerators{ maxHarmonics }
#if 0
      , scintillationStateVector{ maxHarmonics, {0.0, 0.0 } }
      , scintillationBuffer{ new double[ epochSize ] }
#endif
      , epochSampleBuffer{ new FlyingPhasorElementType[ epochSize ] }
    {
        std::memset( epochSampleBuffer.get(), 0, sizeof( FlyingPhasorElementType ) * epochSize );
    }

#if 0
    void  reset( const CombGeneratorResetParameters & resetParameters,
                const double * pMagVector, const double * pPhaseVector,
                const ScintillateFunkType & scintillateFunk )
    {
        // Ensure that the user has not specified more lines than they constructed us to handle.
        if (maxHarmonics < resetParameters.numLines )
            throw std::length_error{ "The number of lines exceeds the maximum allocated during construction!" };

        // Record number of lines and decorrelation samples
        numLines = resetParameters.numLines;
#if 0
        decorrelationSamples = resetParameters.decorrelationSamples;
#endif
        pMagnitude = pMagVector;

        // For each Spectral Line
        for ( size_t i = 0; i != numLines; ++i )
        {
            // Reset Spectral Line Tone Generator
            auto radiansPerSample = (i+1) * resetParameters.spacingRadiansPerSample;
            harmonicGenerators[ i ].reset(radiansPerSample, pPhaseVector ? *pPhaseVector++ : 0.0 );
#if 0
            // If scintillating, record initial scintillated magnitude from rayleigh distributed desired mean magnitude.
            // And set slope to the next scintillation value initially to zero.
            // The slope will be adjusted immediately upon first getEpoch invocation after reset.
            if ( 0 != resetParameters.decorrelationSamples )
            {
                scintillationStateVector[ i ].first = scintillateFunk(pMagVector ? *pMagVector++ : 1.0, i );
                scintillationStateVector[ i ].second = 0.0;
            }
#endif
        }
    }
#endif

    void reset ( size_t theNumLines, double fundamentalRadiansPerSample,
                 const double * pMagVector, const double * pPhaseVector,
                 const EnvelopeFunkType & theEnvelopeFunk )
    {
        // Ensure that the user has not specified more lines than they constructed us to handle.
        if ( maxHarmonics < theNumLines )
            throw std::length_error{ "The number of lines exceeds the maximum allocated during construction!" };

        // Record number of lines and decorrelation samples
        numLines = theNumLines;

        // Record the Envelope Function which could be NULL.
        envelopeFunkType = theEnvelopeFunk;

        pMagnitude = pMagVector;

        // For each Spectral Line
        ///@todo Consider start at 1 and <=
        for ( size_t i = 0; i != numLines; ++i )
        {
            // Reset Spectral Line Tone Generator
            auto radiansPerSample = (i+1) * fundamentalRadiansPerSample;
            harmonicGenerators[ i ].reset(radiansPerSample, pPhaseVector ? *pPhaseVector++ : 0.0 );
        }
    }

#if 0
    const FlyingPhasorElementBufferTypePtr getEpoch( const ScintillateFunkType & scintillateFunk )
    {
#if 0
        // If no Scintillation.
        if ( !decorrelationSamples )
        {
#endif
            // For, each spectral line accumulate its samples.
            auto pMag = pMagnitude;
            for ( size_t i = 0; i != numLines; ++i, ++pMag ) {
                // First line optimization, just get the samples. Accumulation not necessary.
                if ( 0 == i )
                    harmonicGenerators[ i ].getSamplesScaled(epochSampleBuffer.get(), epochSize, *pMag );
                else
                    harmonicGenerators[ i ].accumSamplesScaled(epochSampleBuffer.get(), epochSize, *pMag );
            }
#if 0
        }

        // Else, we are to scintillate
        else
        {
            // For, each spectral line accumulate its scintillated samples.
            for ( size_t i = 0; i != numLines; ++i )
            {
                // Scintillation Management for spectral line I
                auto currentSampleCount = harmonicGenerators[i].getSampleCount();
                scintillationManagement(i, currentSampleCount, scintillateFunk );

                // First line optimization, just get the scintillated samples. Accumulation not necessary.
                if ( 0 == i )
                    harmonicGenerators[ i ].getSamplesScaled(epochSampleBuffer.get(), epochSize,
                        scintillationBuffer.get() );
                else
                    harmonicGenerators[ i ].accumSamplesScaled(epochSampleBuffer.get(), epochSize,
                        scintillationBuffer.get() );
            }
        }
#endif

        return epochSampleBuffer.get();
    }
#endif

    const ReiserRT::Signal::FlyingPhasorElementBufferTypePtr getEpoch()
    {
        if ( !envelopeFunkType )
        {
            // For, each spectral line accumulate its samples.
            auto pMag = pMagnitude;
            for ( size_t i = 0; i != numLines; ++i )
            {
                auto mag = ( pMag ) ? *pMag++ : 1.0;
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
            for ( size_t i = 0; i != numLines; ++i )
            {
                auto mag = (pMag) ? *pMag++ : 1.0;
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

#if 0
    void scintillationManagement( size_t lineNum, size_t startingSampleCount, const ScintillateFunkType & scintillateFunk )
    {
        // We need to provide a random value to our Scintillation Engine as it may require.
        // It does not know anything about what sort of distribution we are using.
        // We take care of that.
        auto sFunk = [ this, scintillateFunk, lineNum ]()
        {
            return scintillateFunk( pMagnitude ? pMagnitude[ lineNum ] : 1.0, lineNum );
        };

        // Our scintillation engine will manage (i.e., mute) the scintillation parameters we provide
        // to complete the scintillation state machine for our given 'line' number.
        auto & sParams = scintillationStateVector[ lineNum ];
        ScintillationEngine::run( scintillationBuffer.get(), epochSize,
              std::ref( sFunk ), sParams, startingSampleCount, decorrelationSamples );
    }
#endif

    const size_t maxHarmonics;
    const size_t epochSize;
    std::vector< FlyingPhasorToneGenerator > harmonicGenerators;
    const double * pMagnitude{};

    EnvelopeFunkType envelopeFunkType{};

#if 0
    // Scintillation Engine Needs: A vector of states for each spectral line and
    // a reusable buffer where varying scintillation magnitudes are cached per line
    std::vector< ScintillationEngine::StateType > scintillationStateVector;
    std::unique_ptr< double[] > scintillationBuffer;
#endif

    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer;


    size_t numLines{};
#if 0
    size_t decorrelationSamples{};
#endif
};

CombGenerator::CombGenerator(size_t maxHarmonics , size_t epochSize )
  : pImple{ new Imple{maxHarmonics, epochSize } }
{
}

CombGenerator::~CombGenerator()
{
    delete pImple;
}

//void CombGenerator::reset( const CombGeneratorResetParameters & resetParameters,
//                           const double * pMagVector, const double * pPhaseVector,
//                           const ScintillateFunkType & scintillateFunk )
//{
//    pImple->reset( resetParameters, pMagVector, pPhaseVector, scintillateFunk );
//}

void CombGenerator::reset (size_t numHarmonics, double fundamentalRadiansPerSample,
                           const double * pMagVector, const double * pPhaseVector,
                           const EnvelopeFunkType & envelopeFunk )
{
    pImple->reset(numHarmonics, fundamentalRadiansPerSample,
                  pMagVector, pPhaseVector, envelopeFunk );
}


//const FlyingPhasorElementBufferTypePtr CombGenerator::getEpoch( const ScintillateFunkType & scintillateFunk )
//{
//    return pImple->getEpoch( scintillateFunk );
//}

const ReiserRT::Signal::FlyingPhasorElementBufferTypePtr CombGenerator::getEpoch()
{
    return pImple->getEpoch();
}