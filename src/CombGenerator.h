/**
 * @file CombGenerator.h
 * @brief The specification file for the Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#ifndef TSG_NG_COMB_GENERATOR_H
#define TSG_NG_COMB_GENERATOR_H

// Include Export Specification File
#include "CombGeneratorDataTypes.h"

namespace TSG_NG
{
    class CombGenerator_EXPORT CombGenerator
    {
    private:
        class Imple;

    public:
        CombGenerator() = delete;
        explicit  CombGenerator( size_t maxSpectralLines, size_t epochSize );

        ~CombGenerator();

        void reset( const CombGeneratorResetParameters & resetParameters );

        ReiserRT::Signal::FlyingPhasorElementBufferTypePtr getSamples();



    private:
        Imple * pImple;
    };
}

#endif // #ifndef TSG_NG_COMB_GENERATOR_H

