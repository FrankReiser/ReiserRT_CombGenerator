/**
 * @file RandomPhaseDistributor.h
 * @brief The specification file for the Random Phase Distributor.
 * @authors Frank Reiser
 * @date Initiated September 23rd, 2022
 */

#ifndef TSG_NG_RANDOMPHASEDISTRIBUTOR_H
#define TSG_NG_RANDOMPHASEDISTRIBUTOR_H

#include "CombGeneratorExport.h"

#include <cstdint>

namespace TSG_NG
{
    class CombGenerator_EXPORT RandomPhaseDistributor
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
}

#endif //TSG_NG_RANDOMPHASEDISTRIBUTOR_H
