/**
 * @file SubSeedGenerator.h
 * @brief The specification file for the Sub Seed Generator.
 * @authors Frank Reiser
 * @date Initiated September 21st, 2022
 */

#ifndef TSG_NG_SUBSEEDGENERATOR_H
#define TSG_NG_SUBSEEDGENERATOR_H

#include "CombGeneratorExport.h"

#include <cstdint>

namespace TSG_NG
{
    /**
     * @brief Sub Seed Generator
     *
     * This class provides for the generation of "sub-seeds" from a master seed. We require sub-seeds so that
     * we can execute parallel tasks using these "sub-seeds" and obtaining repeatable results when rerun with
     * the same master seed. Internally, it utilizes the 'std::knuth_b' random number engine with a default
     * 'std::uniform_int_distribution< uint32_t>' distribution. This which provides for the uniform
     * distribution from the full range of 'uint32_t' values to use as 'sub-seeds'.
     *
     * The usage of 'std::knuth_b' for sub-seed generation was chosen because we will utilize the more robust
     * 'std::mt19937' engine in all other places as a source of randomness. We cannot use the same engine
     * instance, nor type, for both the purposes of generating 'sub-seeds' and generating other random
     * number sequences. They must be different to avoid overlapping bit stream sequences from each.
     */
    class CombGenerator_EXPORT SubSeedGenerator
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
        SubSeedGenerator();

        /**
         * @brief Destructor
         *
         * Deletes the Implementation.
         */
        ~SubSeedGenerator();

        /**
         * @brief Reset the Seed Used
         *
         * This operation resets the seed used by the internal std::knuth_b random number engine.
         *
         * @param seed The seed to use for reseeding.
         */
        void reset( uint32_t seed );

        /**
         * @brief Get Sub-seed
         *
         * This operation returns the next random sub-seed from the full set of uint32_t values.
         *
         * @return Returns the next sub-seed value.
         */
        uint32_t getSubSeed();

    private:
        Imple * pImple;    //!< Pointer to hidden implementation.
    };
}

#endif // TSG_NG_SUBSEEDGENERATOR_H
