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

        /**
         * @brief Magnitude Vector Address
         *
         * This field provides for addressing of a series of magnitude values, of length numLines.
         * This data pointed to is expected to persist between CombGenerator reset cycles.
         * CombGenerator does not make it's own copy and the data may be accessed during getEpoch invocations.
         * Other API's using CombGeneratorResetParameters may relax this requirement, providing their own guarantee.
         * If a nullptr is provided, default magnitude values of 1.0 will be used for each spectral line.
         */
        const double * pMagnitude{};        // A series of Magnitudes for each line of length numLines.

        /**
         * @brief Phase Vector Address
         *
         * This field provides for addressing of a series of radian phase values, of length numLines.
         * This data pointed to need not persist between CombGenerator reset cycles.
         * CombGenerator only needs it for the reset call.
         * Other API's using CombGeneratorResetParameters may strengthen this requirement if necessary.
         * If a nullptr is provided, default phase values of 0.0 will be used for each spectral line.
         */
        const double * pPhase{};            // A series of Phases for each line of length numLines.

        /**
         * @brief Spectral Line Spacing
         *
         * The CombGenerator creates a harmonic series starting with a fundamental frequency specified
         * in radians per sample, and followed by numLines harmonics. The fundamental frequency and the
         * spacing, specify the same thing.
         * This field specifies the fundamental frequency and the spacing per tone.
         */
        double spacingRadiansPerSample{};

        /**
         * @brief Decorrelation Samples
         *
         * If non-zero, this field specifies that the generated spectral lines will scintillate over a period
         * of N samples. If zero, no scintillation will occur. The CombGenerator relies on client hooks
         * to provide scintillation values. See CombGenerator reset and getEpoch operation.
         */
        size_t decorrelationSamples{};      // Zero means no scintillation, otherwise specifies scintillation rate.
    };
}



#endif //TSG_NG_COMBGENERATOR_COMBGENERATORDATATYPES_H
