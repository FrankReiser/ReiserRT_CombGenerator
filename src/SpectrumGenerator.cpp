/**
 * @file SpectrumGenerator.cpp
 * @brief The implementation file for the ReiserRT Spectrum Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#include "SpectrumGenerator.h"

#include "FlyingPhasorToneGenerator.h"

#include <vector>
#include <memory>

using namespace ReiserRT::Signal;

class SpectrumGenerator::Imple
{
private:
    friend class SpectrumGenerator;

    Imple() = delete;
    Imple( size_t theNumSpectralLines, size_t theEpochSize )
      : numSpectralLines( theNumSpectralLines )
      , epochSize( theEpochSize )
      , spectralLineGenerators{ numSpectralLines }
      , epochSampleBuffer{ new FlyingPhasorElementType[ epochSize ] }
    {
    }

    size_t numSpectralLines;
    size_t epochSize;
    std::vector< FlyingPhasorToneGenerator > spectralLineGenerators;
    std::unique_ptr< FlyingPhasorElementType > epochSampleBuffer;
};

SpectrumGenerator::SpectrumGenerator( size_t numSpectralLines, size_t epochSize )
  : pImple{ new Imple{ numSpectralLines, epochSize } }
{
}

SpectrumGenerator::~SpectrumGenerator()
{
    delete pImple;
}
