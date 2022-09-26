/**
 * @file CombGeneratorDataTypes.h
 * @brief The Specification File for Data Types used by the Comb Generator
 * @authors Frank Reiser
 * @date Initiated September 1st, 2022
 */


#ifndef TSG_NG_COMBGENERATOR_COMBGENERATORDATATYPES_H
#define TSG_NG_COMBGENERATOR_COMBGENERATORDATATYPES_H

// Include Export Specification File
#include "CombGeneratorExport.h"

#include "FlyingPhasorToneGeneratorDataTypes.h"

///@todo Put onus on user to not exceed nyquist or control it here. Seems that bandwidth limitation
///is a user parameter for TSG_NG. However, we could allow an absolute nyquist limit here unless we desire wrapping.
struct CombGenerator_EXPORT CombGeneratorResetParameters
{
    using MagPhaseType = std::pair< double, double >;

    CombGeneratorResetParameters() = default;

    size_t numLines{};                  // Cannot exceed maxSpectralLines
    double spacingRadiansPerSample{};   // First tone and spacing between tones. No Zero tone.
//    const double * pMagnitudes{};       // A series of magnitudes of numLines length.
    const MagPhaseType * pMagPhase{};   // A series of Magnitudes and Initial Phases for each line.
    size_t decorrelationSamples{};      // Zero means no scintillation, otherwise specifies scintillation rate.

//    std::pair< uint32_t, u_int32_t >seeds{};  // Used for generating random phases and scintillation magnitudes.
    uint32_t seed{};                    // Used for generating scintillation magnitudes.
};



#endif //TSG_NG_COMBGENERATOR_COMBGENERATORDATATYPES_H
