/**
 * @file CombGenerator.cpp
 * @brief The implementation file for the Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#include "CombGenerator.h"
#include "CombGeneratorPrivate.h"

#include "FlyingPhasorToneGenerator.h"

#include <vector>
#include <memory>
#include <stdexcept>

using namespace TSG_NG;
using namespace ReiserRT::Signal;

class CombGenerator::Imple
{
private:
    friend class CombGenerator;

    Imple() = delete;

    ///@todo Should I compute the random phases here or should that be input with the magnitudes in a 'pair'.
    Imple(size_t theMaxSpectralLines, size_t theEpochSize )
      : maxSpectralLines( theMaxSpectralLines )
      , epochSize( theEpochSize )
      , spectralLineGenerators{ maxSpectralLines }
      , normalMagnitudes( maxSpectralLines, 0.0 )            // Default normalMagnitudes of zero
      , scintillationParams( maxSpectralLines, { 0.0, 0.0 } )
      , scintillationBuffer{ new double[ epochSize ] }
      , epochSampleBuffer{ new FlyingPhasorElementType[ epochSize ] }
      , scintillationEngine{ scintillationBuffer.get(), epochSize }
    {
    }

    void reset( const CombGeneratorResetParameters & resetParameters )
    {
        // Ensure that the user has not specified more lines than they constructed us to handle.
        if ( maxSpectralLines < resetParameters.numLines )
            throw std::length_error{ "The number of lines exceeds the maximum allocated during construction!" };

        // Record number of lines and decorrelation samples
        numLines = resetParameters.numLines;
        decorrelationSamples = resetParameters.decorrelationSamples;

        // Seed our Random Number Generator Engine
        randomNumberGenerator.reset( resetParameters.randSeed );

        // For each Spectral Line
        auto pAmp = normalMagnitudes.begin();
        auto pResetMag = resetParameters.pMagnitudes;
        for ( size_t i = 0; i != numLines; ++i )
        {
            // Copy the Amplitude for Spectral Line
            auto normalMag = *pResetMag++;
            *pAmp++ = normalMag;

            // Reset Spectral Line Tone Generator
            auto radiansPerSample = resetParameters.spacingRadiansPerSample + resetParameters.spacingRadiansPerSample * i;
            auto phi = randomNumberGenerator.getRandomPhaseAngle();
            spectralLineGenerators[ i ].reset( radiansPerSample, phi );

            // If scintillating, record initial scintillated magnitude from rayleigh distributed desired mean magnitude.
            // And set slope to the next scintillation value initially to zero.
            // The slope will be adjusted immediately upon first getSamples invocation after reset.
            if ( 0 != resetParameters.decorrelationSamples )
            {
                scintillationParams[ i ].first = randomNumberGenerator.getRayleighValue( normalMag );
                scintillationParams[ i ].second = 0.0;
            }
        }
    }

    FlyingPhasorElementBufferTypePtr getSamples()
    {
        // If no Scintillation.
        if ( !decorrelationSamples )
        {
            // For, each spectral line accumulate its samples.
            for ( size_t i = 0; i != numLines; ++i ) {
                // First line optimization, just get the samples. Accumulation not necessary.
                if ( 0 == i )
                    spectralLineGenerators[ i ].getSamplesScaled(epochSampleBuffer.get(), epochSize,
                        normalMagnitudes[i] );
                else
                    spectralLineGenerators[ i ].accumSamplesScaled(epochSampleBuffer.get(), epochSize,
                        normalMagnitudes[ i ] );
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
                scintillationManagement( i, currentSampleCount );

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

    void scintillationManagement( size_t lineNum, size_t startingSampleCount )
    {
        // We need to provide a random value to our Scintillation Engine as it may require.
        // It does not know anything about what sort of distribution we are using.
        // We take care of that.
        auto randFunk = [ this, lineNum ]()
        {
            return randomNumberGenerator.getRayleighValue( normalMagnitudes[ lineNum ] );
        };

        // Our scintillation engine will manage (i.e., mute) the scintillation parameters we provide
        // to complete the scintillation state machine for our given 'line' number.
        auto & sParams = scintillationParams[ lineNum ];
        scintillationEngine.run( std::ref( randFunk ), sParams, startingSampleCount, decorrelationSamples );
    }

    const size_t maxSpectralLines;
    const size_t epochSize;
    std::vector< FlyingPhasorToneGenerator > spectralLineGenerators;
    std::vector< double > normalMagnitudes;
    std::vector< ScintillationParamsType > scintillationParams;

    std::unique_ptr< double[] > scintillationBuffer;
    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer;

    // Random Number Requirements met with RandomNumberGenerator class.
    RandomNumberGenerator randomNumberGenerator{};

    // Scintillation Engine
    ScintillationEngine scintillationEngine;

    size_t numLines{};
    size_t decorrelationSamples{};

};

CombGenerator::CombGenerator(size_t maxSpectralLines , size_t epochSize )
  : pImple{ new Imple{maxSpectralLines, epochSize } }
{
}

CombGenerator::~CombGenerator()
{
    delete pImple;
}

void CombGenerator::reset(const CombGeneratorResetParameters & resetParameters )
{
    pImple->reset( resetParameters );
}

FlyingPhasorElementBufferTypePtr CombGenerator::getSamples()
{
    return pImple->getSamples();
}
