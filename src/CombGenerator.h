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

#include <functional>

namespace TSG_NG
{
    class CombGenerator_EXPORT CombGenerator
    {
    private:
        class Imple;

    public:
        // The Line number hint is primarily intended for but, not limited to, testing purposes.
        using ScintillateFunkType = std::function< double(double desiredMean, size_t lineNumberHint ) >;

        CombGenerator() = delete;
        explicit  CombGenerator( size_t maxSpectralLines, size_t epochSize );

        ~CombGenerator();

        void reset( const CombGeneratorResetParameters & resetParameters, const ScintillateFunkType & scintillateFunk );

        ///@todo I want an observer here to get scintillated magnitudes based off a 'desiredMean'.
        ///I'll need a functor that takes a double and returns a double and use it in the signature.
        ReiserRT::Signal::FlyingPhasorElementBufferTypePtr getSamples( const ScintillateFunkType & scintillateFunk );


    private:
        Imple * pImple;
    };
}

#endif // #ifndef TSG_NG_COMB_GENERATOR_H

