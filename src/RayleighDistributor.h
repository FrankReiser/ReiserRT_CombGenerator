/**
 * @file RayleighDistributor.h
 * @brief The specification file for the Rayleigh Random Value Distributor.
 * @authors Frank Reiser
 * @date Initiated September 22nd, 2022
 */

#ifndef TSG_NG_RAYLEIGHDISTRIBUTOR_H
#define TSG_NG_RAYLEIGHDISTRIBUTOR_H

#include "TsgNgCombGeneratorExport.h"

#include <cstdint>

namespace TSG_NG
{
    /**
     * @brief A Rayleigh Random Value Distributor
     *
     * This class produces Rayleigh distributed values around an desired mean value or
     * as sometime referred to as an 'expected value'. It is a robust implementation that
     * is not sensitive to a low value of the expected. It is intended to be utilized as
     * a source of scintillated magnitude values for the CombGenerator.
     */
    class TsgNgCombGenerator_EXPORT RayleighDistributor
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
        RayleighDistributor();

        /**
         * @brief Destructor
         *
         * Deletes the Implementation.
         */
        ~RayleighDistributor();

        /**
         * @brief Reset the Seed Used
         *
         * This operation resets the seed used by the internal std::mt19937 random number engine.
         *
         * @param seed The seed to use for reseeding.
         */
        void reset( uint32_t seed );

        /**
         * @brief Get Rayleigh Distrubuted Value
         *
         * This operation returns a Rayleigh distributed value around a desired mean or expected value.
         * @note It does not provide any clamping of potential high output values. Those are details
         * we have no knowledge of at this level. This is a job for the client.
         *
         * @param desiredMean The desired mean or expected value use.
         *
         * @return Returns a Rayleigh distributed value around the desiredMean.
         */
        double getValue( double desiredMean );

    private:
        Imple * pImple;    //!< Pointer to hidden implementation.
    };
}

#endif //TSG_NG_RAYLEIGHDISTRIBUTOR_H
