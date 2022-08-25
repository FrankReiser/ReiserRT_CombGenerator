/**
 * @file CombGenerator.h
 * @brief The specification file for the ReiserRT Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#ifndef REISER_RT_SPECTRUM_GENERATOR_H
#define REISER_RT_SPECTRUM_GENERATOR_H

// Include Export Specification File
#include "CombGeneratorExport.h"

#include "FlyingPhasorToneGeneratorDataTypes.h"

#include <cstdlib>
#include <cstdint>

namespace TSG_NG
{
    class CombGenerator_EXPORT CombGenerator
    {
    private:
        class Imple;

    public:
        CombGenerator() = delete;
        explicit  CombGenerator(size_t maxSpectralLines, size_t epochSize );

        ~CombGenerator();

        ///@todo Put onus on user to not exceed nyquist or control it here. Seems that bandwidth limitation
        ///is a user parameter for TSG_NG. However, we could allow an absolute nyquist limit here unless we desire wrapping.
        struct ResetParameters
        {
            size_t numLines;                // Cannot exceed maxSpectralLines
            double spacingRadiansPerSample; // First tone and spacing between tones. No Zero tone.
            const double * pMagnitudes;     // A series of magnitudes of numLines length.
            size_t decorrelationSamples;    // Zero means no scintillation, otherwise specifies scintillation rate.

            uint32_t randSeed;              // Used for generating random phases and scintillation magnitudes.
        };


        void reset( const ResetParameters & resetParameters );

        ReiserRT::Signal::FlyingPhasorElementBufferTypePtr getSamples();



    private:
        Imple * pImple;
    };
}

#endif // #ifndef REISER_RT_SPECTRUM_GENERATOR_H
