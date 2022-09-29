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
    using MagPhaseType = std::pair< double, double >;

    struct CombGeneratorResetParameters
    {
        CombGeneratorResetParameters() = default;

        size_t numLines{};                  // Cannot exceed maxSpectralLines
        ///@note The MagPhaseType buffer must persist between resets as it may be referenced during getSamples if scintillating.
        const MagPhaseType * pMagPhase{};   // A series of Magnitudes and Initial Phases for each line.
        double spacingRadiansPerSample{};   // First tone and spacing between tones. No Zero tone.
        size_t decorrelationSamples{};      // Zero means no scintillation, otherwise specifies scintillation rate.
    };
}



#endif //TSG_NG_COMBGENERATOR_COMBGENERATORDATATYPES_H
