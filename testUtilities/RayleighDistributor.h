// Created on 20230103

#ifndef REISER_RT_COMBGENERATOR_RAYLEIGHDISTRIBUTOR_H
#define REISER_RT_COMBGENERATOR_RAYLEIGHDISTRIBUTOR_H

#include <cstdint>

class RayleighDistributor
{
private:
    class Imple;

public:
    RayleighDistributor();

    ~RayleighDistributor();

    void reset( uint32_t seed );

    double getValue( double desiredMean );

private:
    Imple * pImple;    //!< Pointer to hidden implementation.
};

#endif //REISER_RT_COMBGENERATOR_RAYLEIGHDISTRIBUTOR_H
