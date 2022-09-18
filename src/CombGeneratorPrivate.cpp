/**
 * @file CombGeneratorPrivate.cpp
 * @brief The implementation file for the Comb Generator Private Purposes.
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#include "CombGeneratorPrivate.h"

#include <random>

void ScintillationEngine::run( const RandomValueFunkType & randomFunk,
      ScintillationParamsType & scintillationParams,
      size_t sampleCounter,
      size_t decorrelationSamples )
{
    // Compute Scintillated Magnitude over the Buffer Length.
    auto pScintillationMag = pScintillationBuffer;
    for ( int i = 0; i != bufLen; ++i )
    {
        // Set scintillation buffer magnitude value for sample i.
        // This is the current magnitude value (first) plus the change in magnitude per sample (second).
        // We conveniently update the magnitude while we are at it for the next iteration.
        // On the initial sample of zero, the change in magnitude per sample is zero.
        *pScintillationMag++ = scintillationParams.first += scintillationParams.second;

        // If time to calculate a new scintillation slope
        if ( 0 == (sampleCounter++ % decorrelationSamples ) )
        {
            // Get a new scintillation target magnitude by invoking randomFunk observer.
            auto scintillationTargetMag = randomFunk();

            // Calculate the change in magnitude per sample and store as the second parameter for the line.
            scintillationParams.second = ( scintillationTargetMag - scintillationParams.first ) / decorrelationSamples;
        }
    }
}

class RandomNumberGenerator::Imple
{
private:
    friend class RandomNumberGenerator;

    using CombGeneratorRandomNumberEngineType = std::mt19937;
    using UniformDistribution = std::uniform_real_distribution<double>;
    using GaussianDistribution = std::normal_distribution<double>;

    void reset( SeedType seed )
    {
        rndEngine.seed( seed );
    }

    double getRandomPhaseAngle()
    {
        return uniformDistribution( rndEngine );
    }

    double getRayleighValue( double desiredMean )
    {
        if ( desiredMean <= 0.0 ) return 0.0;

        const auto sigma = desiredMean / sqrtQtyPiOver2;

        // We use sigma and not sigma^2 (variance) in the arguments below because the C++ standard
        // for the normal (gaussian) distribution function requires sigma as input, not variance.
#if 1
        using ParamType = GaussianDistribution::param_type;
        normalDistribution.param( ParamType{0.0, sigma } );
        const auto X = normalDistribution(rndEngine );
        const auto Y = normalDistribution(rndEngine );
#else
        ///@todo Would it be more efficient to 'set' it once than parameterize it twice
        ///now that the normal distributions sole purpose is Rayleigh, not both Rayleigh and a fixed Gaussian.
        using ParamType = GaussianDistribution::param_type;
        const auto X = normalDistribution(rndEngine, ParamType{0.0, sigma } );
        const auto Y = normalDistribution(rndEngine, ParamType{0.0, sigma } );
#endif

        return std::sqrt( X * X + Y * Y );
    }

    const double sqrtQtyPiOver2{ std::sqrt( M_PI / 2.0 ) };
    CombGeneratorRandomNumberEngineType rndEngine{ std::random_device{}() };
    UniformDistribution uniformDistribution{ -M_PI, M_PI };     // For Random Phase.
    GaussianDistribution normalDistribution{};                      // Parameterized before each use.
};

RandomNumberGenerator::RandomNumberGenerator()
  : pImple{ new Imple{} }
{
}

RandomNumberGenerator::~RandomNumberGenerator()
{
    delete pImple;
}

void RandomNumberGenerator::reset( SeedType seed )
{
    pImple->reset( seed );
}

double RandomNumberGenerator::getRandomPhaseAngle()
{
    return pImple->getRandomPhaseAngle();
}

double RandomNumberGenerator::getRayleighValue( double desiredMean )
{
    return pImple->getRayleighValue( desiredMean );
}
