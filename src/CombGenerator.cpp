/**
 * @file CombGenerator.cpp
 * @brief The implementation file for the ReiserRT Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#include "CombGenerator.h"

#include "FlyingPhasorToneGenerator.h"

#include <vector>
#include <memory>
#include <random>
#include <stdexcept>

using namespace TSG_NG;
using namespace ReiserRT::Signal;

class CombGenerator::Imple
{
private:
    friend class CombGenerator;

    using RandomNumberEngine = std::mt19937;
    using UniformDistribution = std::uniform_real_distribution<double>;
    using GaussianDistribution = std::normal_distribution<double>;

    Imple() = delete;
    Imple(size_t theMaxSpectralLines, size_t theEpochSize )
      : maxSpectralLines( theMaxSpectralLines )
      , epochSize( theEpochSize )
      , spectralLineGenerators{ maxSpectralLines }
      , normalMagnitudes( maxSpectralLines, 0.0 )            // Default normalMagnitudes of zero
      , scintillationParams( maxSpectralLines, { 0.0, 0.0 } )
      , scintillationBuffer{ new double[ epochSize ] }
      , epochSampleBuffer{ new FlyingPhasorElementType[ epochSize ] }
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
        rndEngine.seed( resetParameters.randSeed );

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
            auto phi = uniformDistribution( rndEngine );
            spectralLineGenerators[ i ].reset( radiansPerSample, phi );

            // If scintillating, record initial scintillated magnitude from rayleigh distributed desired mean magnitude.
            // And set slope to the next scintillation value initially to zero.
            // The slope will be adjusted immediately upon first getSamples invocation after reset.
            if ( 0 != resetParameters.decorrelationSamples )
            {
                scintillationParams[ i ].first = getRayleighValue( normalMag );
                scintillationParams[ i ].second = 0.0;
            }
        }
    }

    FlyingPhasorElementBufferTypePtr getSamples()
    {
        // If no Scintillation.
        if ( !decorrelationSamples )
        {
            for ( size_t i = 0; i != numLines; ++i ) {
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
            // Get the current sample count for the first spectral line generator. They are all the same value.
            auto currentSampleCount = spectralLineGenerators[0].getSampleCount();
            for ( size_t i = 0; i != numLines; ++i )
            {
                // Scintillation Management for spectral line I
                scintillationManagement( i, currentSampleCount );

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

    void scintillationManagement( size_t lineNum, size_t currentSampleCount )
    {
        auto & scintillationParam = scintillationParams[ lineNum ];
        auto pScintillationMag = scintillationBuffer.get();
        for ( int i = 0; i != epochSize; ++i )
        {
            // If time to calculate a new scintillation slope
            if ( 0 == ( currentSampleCount++ % decorrelationSamples ) )
            {
                // Get a new scintillation target magnitude based off of normal magnitude.
                auto scintillationTargetMag = getRayleighValue( normalMagnitudes[ lineNum ] );

                // Calculate the change in magnitude per sample and store as the second parameter for the line.
                scintillationParam.second = ( scintillationTargetMag - scintillationParam.first ) / decorrelationSamples;
            }

            // Set scintillation buffer magnitude value for sample i.
            // This is the current magnitude value plus the change in magnitude per sample.
            // We conveniently update the magnitude while we are at it.
            *pScintillationMag++ = scintillationParam.first += scintillationParam.second;
        }
    }

    double getRayleighValue( double desiredMean )
    {
        if ( desiredMean <= 0.0 ) return 0.0;

        const auto sigma = desiredMean / sqrtQtyPiOver2;

        // We use sigma and not sigma^2 (variance) in the arguments below because the C++ standard
        // for the normal (gaussian) distribution function requires sigma as input, not variance.
        using ParamType = GaussianDistribution::param_type;
        const auto X = normalDistribution(rndEngine, ParamType{0.0, sigma } );
        const auto Y = normalDistribution(rndEngine, ParamType{0.0, sigma } );

        return std::sqrt( X * X + Y * Y );
    }

    const double sqrtQtyPiOver2{ std::sqrt( M_PI / 2.0 ) };
    const size_t maxSpectralLines;
    const size_t epochSize;
    std::vector< FlyingPhasorToneGenerator > spectralLineGenerators;
    std::vector< double > normalMagnitudes;
    std::vector< std::pair< double, double > > scintillationParams;

    std::unique_ptr< double > scintillationBuffer;
    std::unique_ptr< FlyingPhasorElementType > epochSampleBuffer;

    // Construct Temporary std::random_device and invoke it for a default seed.
    RandomNumberEngine rndEngine{ std::random_device{}() };
    UniformDistribution uniformDistribution{ -M_PI, M_PI };     // For Random Phase.
    GaussianDistribution normalDistribution{};                      // Parameterized before each use.

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
