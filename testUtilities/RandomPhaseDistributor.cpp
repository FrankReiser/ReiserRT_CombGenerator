// Created on 20230103

#include "RandomPhaseDistributor.h"

#include <random>

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
