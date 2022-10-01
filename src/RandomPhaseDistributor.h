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
    /**
     * @brief A Random Phase Distributor
     *
     * This class produces random phase values, uniformly distributed between positive and negative
     * pi. It is intended to be utilized as a source of random phase values for the CombGenerator.
     */
    class CombGenerator_EXPORT RandomPhaseDistributor
    {
    private:
        /**
         * @brief Forward Reference to Hidden Implementation
         */
        class Imple;

    public:
        /**
         * @brief Default Constructor
         *
         * Instantiates the Implementation.
         */
        RandomPhaseDistributor();

        /**
         * @brief Destructor
         *
         * Deletes the Implementation.
         */
        ~RandomPhaseDistributor();

        /**
         * @brief Reset the Seed Used
         *
         * This operation resets the seed used by the internal std::mt19937 random number engine.
         *
         * @param seed The seed to use for reseeding.
         */
        void reset( uint32_t seed );

        /**
         * @brief Get Random Phase Value
         *
         * This operation returns a random phase value, uniformly distributed between positive and negative
         * pi.
         *
         * @return Returns a random phase value, uniformly distributed between positive and negative
         * pi.
         */
        double getValue();

    private:
        Imple * pImple;    //!< Pointer to hidden implementation.
    };
}

#endif //TSG_NG_RANDOMPHASEDISTRIBUTOR_H
