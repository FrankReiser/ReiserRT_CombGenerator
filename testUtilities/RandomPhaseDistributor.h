// Created on 20230103

#ifndef REISER_RT_COMBGENERATOR_RANDOMPHASEDISTRIBUTOR_H
#define REISER_RT_COMBGENERATOR_RANDOMPHASEDISTRIBUTOR_H

#include <cstdint>

    class RandomPhaseDistributor
    {
    private:
        class Imple;

    public:
        RandomPhaseDistributor();

        ~RandomPhaseDistributor();

        void reset( uint32_t seed );

        double getValue();

    private:
        Imple * pImple;
    };

#endif //REISER_RT_COMBGENERATOR_RANDOMPHASEDISTRIBUTOR_H
