/**
 * @file RayleighDistributor.h
 * @brief The specification file for the Rayleigh Random Value Distributor.
 * @authors Frank Reiser
 * @date Initiated September 21st, 2022
 */

#ifndef TSG_NG_RAYLEIGHDISTRIBUTOR_H
#define TSG_NG_RAYLEIGHDISTRIBUTOR_H

#include "CombGeneratorExport.h"

#include <cstdint>

namespace TSG_NG
{
    class CombGenerator_EXPORT RayleighDistributor
    {
    private:
        class Imple;

    public:
        RayleighDistributor();
        ~RayleighDistributor();

        void reset( uint32_t seed );
        double getDistributedValue( double desiredMean );

    private:
        Imple * pImple;
    };
}

#endif //TSG_NG_RAYLEIGHDISTRIBUTOR_H
