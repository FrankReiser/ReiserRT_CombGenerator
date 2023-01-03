// Created on 20230103

#ifndef REISER_RT_COMBGENERATOR_SUBSEEDGENERATOR_H
#define REISER_RT_COMBGENERATOR_SUBSEEDGENERATOR_H

#include <cstdint>

    class SubSeedGenerator
    {
    private:
        class Imple;

    public:
        SubSeedGenerator();

        ~SubSeedGenerator();

        void reset( uint32_t seed );

        uint32_t getSubSeed();

    private:
        Imple * pImple;
    };

#endif //REISER_RT_COMBGENERATOR_SUBSEEDGENERATOR_H
