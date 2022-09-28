/**
 * @file CombGenerator.cpp
 * @brief The implementation file for the Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#include "CombGenerator.h"

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

    Imple(size_t theMaxSpectralLines, size_t theEpochSize )
      : maxSpectralLines( theMaxSpectralLines )
      , epochSize( theEpochSize )
      , spectralLineGenerators{ maxSpectralLines }
      , scintillationStates{maxSpectralLines, {0.0, 0.0 } }
      , scintillationBuffer{ new double[ epochSize ] }
      , scintillationEngine{ scintillationBuffer.get(), epochSize }
      , epochSampleBuffer{ new FlyingPhasorElementType[ epochSize ] }
    {
        std::memset( epochSampleBuffer.get(), 0, sizeof( FlyingPhasorElementType ) * epochSize );
    }

    void reset( const ResetParameters & resetParameters, const ScintillateFunkType & scintillateFunk )
    {
        // Ensure that the user has not specified more lines than they constructed us to handle.
        if ( maxSpectralLines < resetParameters.numLines )
            throw std::length_error{ "The number of lines exceeds the maximum allocated during construction!" };

        // Record number of lines and decorrelation samples
        numLines = resetParameters.numLines;
        decorrelationSamples = resetParameters.decorrelationSamples;
        pMagPhase = resetParameters.pMagPhase;

        // For each Spectral Line
        auto pMP = pMagPhase;
        for ( size_t i = 0; i != numLines; ++i, ++pMP )
        {
            // Reset Spectral Line Tone Generator
//            auto radiansPerSample = resetParameters.spacingRadiansPerSample + resetParameters.spacingRadiansPerSample * i;
            auto radiansPerSample = (i+1) * resetParameters.spacingRadiansPerSample;
            spectralLineGenerators[ i ].reset( radiansPerSample, pMP->second );

            // If scintillating, record initial scintillated magnitude from rayleigh distributed desired mean magnitude.
            // And set slope to the next scintillation value initially to zero.
            // The slope will be adjusted immediately upon first getSamples invocation after reset.
            if ( 0 != resetParameters.decorrelationSamples )
            {
                scintillationStates[ i ].first = scintillateFunk(pMP->first, i );
                scintillationStates[ i ].second = 0.0;
            }
        }
    }

    FlyingPhasorElementBufferTypePtr getSamples( const ScintillateFunkType & scintillateFunk )
    {
        // If no Scintillation.
        if ( !decorrelationSamples )
        {
            // For, each spectral line accumulate its samples.
            for ( size_t i = 0; i != numLines; ++i ) {
                // First line optimization, just get the samples. Accumulation not necessary.
                if ( 0 == i )
                    spectralLineGenerators[ i ].getSamplesScaled(epochSampleBuffer.get(), epochSize,
                        pMagPhase[i].first );
                else
                    spectralLineGenerators[ i ].accumSamplesScaled(epochSampleBuffer.get(), epochSize,
                        pMagPhase[i].first );
            }
        }
        // Else, we are to scintillate
        else
        {
            // For, each spectral line accumulate its scintillated samples.
            for ( size_t i = 0; i != numLines; ++i )
            {
                // Scintillation Management for spectral line I
                auto currentSampleCount = spectralLineGenerators[i].getSampleCount();
                scintillationManagement(i, currentSampleCount, scintillateFunk );

                // First line optimization, just get the scintillated samples. Accumulation not necessary.
                if ( 0 == i )
                    spectralLineGenerators[ i ].getSamplesScaled(epochSampleBuffer.get(), epochSize,
                        scintillationBuffer.get() );
                else
                    spectralLineGenerators[ i ].accumSamplesScaled(epochSampleBuffer.get(), epochSize,
                        scintillationBuffer.get() );
            }
        }

        return epochSampleBuffer.get();
    }

    void scintillationManagement( size_t lineNum, size_t startingSampleCount, const ScintillateFunkType & scintillateFunk )
    {
        // We need to provide a random value to our Scintillation Engine as it may require.
        // It does not know anything about what sort of distribution we are using.
        // We take care of that.
        auto sFunk = [ this, scintillateFunk, lineNum ]()
        {
            return scintillateFunk( pMagPhase[ lineNum ].first, lineNum );
        };

        // Our scintillation engine will manage (i.e., mute) the scintillation parameters we provide
        // to complete the scintillation state machine for our given 'line' number.
        auto & sParams = scintillationStates[ lineNum ];
        scintillationEngine.run( std::ref( sFunk ), sParams, startingSampleCount, decorrelationSamples );
    }

    const size_t maxSpectralLines;
    const size_t epochSize;
    std::vector< FlyingPhasorToneGenerator > spectralLineGenerators;
    const MagPhaseType * pMagPhase{};

    // Scintillation Engine
    std::vector< ScintillationEngine::StateType > scintillationStates;
    std::unique_ptr< double[] > scintillationBuffer;
    ScintillationEngine scintillationEngine;

    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer;


    size_t numLines{};
    size_t decorrelationSamples{};

};

CombGenerator::CombGenerator( size_t maxSpectralLines , size_t epochSize )
  : pImple{ new Imple{maxSpectralLines, epochSize } }
{
}

CombGenerator::~CombGenerator()
{
    delete pImple;
}

void CombGenerator::reset( const ResetParameters & resetParameters, const ScintillateFunkType & scintillateFunk )
{
    pImple->reset(resetParameters, scintillateFunk );
}

FlyingPhasorElementBufferTypePtr CombGenerator::getSamples( const ScintillateFunkType & scintillateFunk )
{
    return pImple->getSamples(scintillateFunk );
}
