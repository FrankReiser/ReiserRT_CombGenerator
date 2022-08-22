/**
 * @file SpectrumGenerator.h
 * @brief The specification file for the ReiserRT Spectrum Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#ifndef REISER_RT_SPECTRUM_GENERATOR_H
#define REISER_RT_SPECTRUM_GENERATOR_H

// Include Export Specification File
#include "ReiserRT_SpectrumExport.h"

#include <cstdlib>

namespace ReiserRT {
    namespace Signal {

        class ReiserRT_Spectrum_EXPORT SpectrumGenerator
        {
        private:
            class Imple;

        public:
            SpectrumGenerator() = delete;
            explicit  SpectrumGenerator( size_t numSpectralLines, size_t epochSize );

            ~SpectrumGenerator();

        private:
            Imple * pImple;
        };

    }
}

#endif // #ifndef REISER_RT_SPECTRUM_GENERATOR_H
