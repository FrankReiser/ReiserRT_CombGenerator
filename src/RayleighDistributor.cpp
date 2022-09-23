/**
 * @file RayleighDistributor.cpp
 * @brief The implementation file for the Rayleigh Random Value Distributor.
 * @authors Frank Reiser
 * @date Initiated September 21st, 2022
 */

#include "RayleighDistributor.h"

#include <random>

class RayleighDistributor::Imple
{
private:
    friend class RayleighDistributor;

    using CombGeneratorRandomNumberEngineType = std::mt19937;
    using GaussianDistribution = std::normal_distribution< double >;

    Imple() = default;
    ~Imple() = default;

    void reset( uint32_t seed )
    {
        rndEngine.seed( seed );
    }

    uint32_t getDistributedValue( double desiredMean )
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

    const double sqrtQtyPiOver2{ std::sqrt( M_PI / 2.0 ) }; // Deliberately not static.
    CombGeneratorRandomNumberEngineType rndEngine{ std::random_device{}() };
    GaussianDistribution normalDistribution{};  // Parameterized when utilized.
};

RayleighDistributor::RayleighDistributor()
  : pImple{ new Imple{} }
{
}

RayleighDistributor::~RayleighDistributor()
{
    delete pImple;
}

void RayleighDistributor::reset( uint32_t seed )
{
    pImple->reset( seed );
}

uint32_t RayleighDistributor::getDistributedValue( double desiredMean )
{
    return pImple->getDistributedValue( desiredMean );
}
