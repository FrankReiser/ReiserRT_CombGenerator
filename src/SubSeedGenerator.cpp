/**
 * @file SubSeedGenerator.cpp
 * @brief The specification file for the Sub Seed Generator.
 * @authors Frank Reiser
 * @date Initiated September 21st, 2022
 */

#include "SubSeedGenerator.h"

#include <random>

using namespace TSG_NG;

class SubSeedGenerator::Imple
{
private:
    friend class SubSeedGenerator;

    Imple() = default;
    ~Imple() = default;

    void reset( uint32_t seed )
    {
        subSeedEngine.seed( seed );
    }

    uint32_t getSubSeed()
    {
        return subSeedDistribution( subSeedEngine );
    }

    std::knuth_b subSeedEngine{ std::random_device{}() };
    std::uniform_int_distribution< uint32_t> subSeedDistribution{};
};

SubSeedGenerator::SubSeedGenerator()
  : pImple{ new Imple{} }
{
}

SubSeedGenerator::~SubSeedGenerator()
{
    delete pImple;
}

void SubSeedGenerator::reset( uint32_t seed )
{
    pImple->reset( seed );
}

uint32_t SubSeedGenerator::getSubSeed()
{
    return pImple->getSubSeed();
}
