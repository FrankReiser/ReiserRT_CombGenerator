/**
 * @file RayleighDistributor.cpp
 * @brief The implementation file for the Rayleigh Random Value Distributor.
 * @authors Frank Reiser
 * @date Initiated September 22nd, 2022
 */

#include "RayleighDistributor.h"

#include <random>

using namespace TSG_NG;

class RayleighDistributor::Imple
{
private:
    friend class RayleighDistributor;

    using RandomNumberEngineType = std::mt19937;
    using GaussianDistribution = std::normal_distribution< double >;

    Imple() = default;
    ~Imple() = default;

    void reset( uint32_t seed )
    {
        rndEngine.seed( seed );
    }

    double getValue( double desiredMean )
    {
        if ( desiredMean <= 0.0 ) return 0.0;

        const auto sigma = desiredMean / sqrtQtyPiOver2;

        // We use sigma and not sigma^2 (variance) in the arguments below because the C++ standard
        // for the normal (gaussian) distribution function requires sigma as input, not variance.
        using ParamType = GaussianDistribution::param_type;
        normalDistribution.param( ParamType{0.0, sigma } );
        const auto X = normalDistribution(rndEngine );
        const auto Y = normalDistribution(rndEngine );

        return std::sqrt( X * X + Y * Y );
    }

    const double sqrtQtyPiOver2{ std::sqrt( M_PI / 2.0 ) }; // Deliberately not static.
    RandomNumberEngineType rndEngine{std::random_device{}() };
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

double RayleighDistributor::getValue( double desiredMean )
{
    return pImple->getValue( desiredMean);
}
