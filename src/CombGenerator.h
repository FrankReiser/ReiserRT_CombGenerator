/**
 * @file CombGenerator.h
 * @brief The specification file for the Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#ifndef TSG_NG_COMB_GENERATOR_H
#define TSG_NG_COMB_GENERATOR_H

// Include Export Specification File
#include "CombGeneratorExport.h"

#include "FlyingPhasorToneGeneratorDataTypes.h"

#include <functional>

namespace TSG_NG
{
    struct CombGeneratorResetParameters;

    class CombGenerator_EXPORT CombGenerator
    {
    private:
        class Imple;

    public:
#if 0
        using MagPhaseType = std::pair< double, double >;

        ///@note Put onus on user to not exceed nyquist or control it here. Seems that bandwidth limitation
        ///is a user parameter for TSG_NG. However, we could allow an absolute nyquist limit here unless we desire wrapping.
        struct ResetParameters
        {
            ResetParameters() = default;

            size_t numLines{};                  // Cannot exceed maxSpectralLines
            ///@note The MagPhaseType buffer must persist between resets as it may be referenced during getSamples if scintillating.
            const MagPhaseType * pMagPhase{};   // A series of Magnitudes and Initial Phases for each line.
            double spacingRadiansPerSample{};   // First tone and spacing between tones. No Zero tone.
            size_t decorrelationSamples{};      // Zero means no scintillation, otherwise specifies scintillation rate.
        };
#endif
        // The Line number hint is primarily intended for but, not limited to, testing purposes.
        using ScintillateFunkType = std::function< double( double desiredMean, size_t lineNumberHint ) >;

        CombGenerator() = delete;
        explicit  CombGenerator( size_t maxSpectralLines, size_t epochSize );

        ~CombGenerator();

        void reset( const CombGeneratorResetParameters & resetParameters, const ScintillateFunkType & scintillateFunk );

        ReiserRT::Signal::FlyingPhasorElementBufferTypePtr getSamples( const ScintillateFunkType & scintillateFunk );

    private:
        Imple * pImple;
    };
}

#endif // #ifndef TSG_NG_COMB_GENERATOR_H

