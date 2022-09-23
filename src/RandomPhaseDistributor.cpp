/**
 * @file RandomPhaseDistributor.cpp
 * @brief The implementation file for the Random Phase Distributor.
 * @authors Frank Reiser
 * @date Initiated September 23rd, 2022
 */

#include "RandomPhaseDistributor.h"

#include <random>

using namespace TSG_NG;

class RandomPhaseDistributor::Imple
{
private:
    friend class RandomPhaseDistributor;

    using RandomNumberEngineType = std::mt19937;
    using UniformDistribution = std::uniform_real_distribution<double>;

    Imple() = default;
    ~Imple() = default;

    void reset( uint32_t seed )
    {
        rndEngine.seed( seed );
    }

    double getValue()
    {
        return uniformDistribution( rndEngine );
    }

    RandomNumberEngineType rndEngine{ std::random_device{}() };
    UniformDistribution uniformDistribution{ -M_PI, M_PI };
};

RandomPhaseDistributor::RandomPhaseDistributor()
  : pImple{ new Imple{} }
{
}

RandomPhaseDistributor::~RandomPhaseDistributor()
{
    delete pImple;
}

void RandomPhaseDistributor::reset( uint32_t seed )
{
    pImple->reset( seed );
}

double RandomPhaseDistributor::getValue()
{
    return pImple->getValue();
}
