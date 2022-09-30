/**
 * @file CombGeneratorDataTypes.h
 * @brief The Specification File for Data Types used by the Comb Generator
 * @authors Frank Reiser
 * @date Initiated September 1st, 2022
 */


#ifndef TSG_NG_COMBGENERATOR_COMBGENERATORDATATYPES_H
#define TSG_NG_COMBGENERATOR_COMBGENERATORDATATYPES_H

#include <utility>
#include <cstdlib>

///@note Put onus on user to not exceed nyquist or control it here. Seems that bandwidth limitation
///is a user parameter for TSG_NG. However, we could allow an absolute nyquist limit here unless we desire wrapping.

namespace TSG_NG
{
    /**
     * @brief Reset Parameters for the CombGenerator
     *
     * This type is utilized by the CombGenerator::reset operation. It may also be utilized by other APIs that
     * indirectly reset a CombGenerator instance.
     */
    struct CombGeneratorResetParameters
    {
        /**
         * @brief Default Constructor
         *
         * Defaults are all zero or nullptr.
         */
        CombGeneratorResetParameters() = default;

        /**
         * @brief The Number of Spectral Lines
         *
         * This field specifies the number of spectral lines to produce. The value must not
         * exceed the maximum spectral lines specified for an instance of CombGenerator or
         * exceptions may be thrown.
         */
        size_t numLines{};

        ///@note The magnitude buffer must persist between resets as it may be referenced during getSamples if scintillating.
        ///The phase buffer is only used during the reset call and need not persist.
        const double * pMagnitude{};        // A series of Magnitudes for each line of length numLines.
        const double * pPhase{};            // A series of Phases for each line of length numLines.
        double spacingRadiansPerSample{};   // First tone and spacing between tones. No Zero tone.
        size_t decorrelationSamples{};      // Zero means no scintillation, otherwise specifies scintillation rate.
    };
}



#endif //TSG_NG_COMBGENERATOR_COMBGENERATORDATATYPES_H
