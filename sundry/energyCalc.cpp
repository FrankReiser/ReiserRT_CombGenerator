//
// Created by frank on 10/21/22.
//

#include "CombGenerator.h"

#include <memory>
#include <iostream>

int main()
{
    // This will be our both our maximum and also the number we will use.
    constexpr size_t numHarmonics = 4;

    // This will be the number of samples we will fetch.
    constexpr size_t epochSize = 4096;

    // Instantiate Comb Generator for number of harmonics and epoch size.
    // Note: Max and Number of Harmonics same for this experiment.
    TSG_NG::CombGenerator combGenerator{numHarmonics, epochSize };

    // We want a decreasing magnitude for each harmonic.
    // Each harmonic has the reciprocal amplitude of its position (classic sawtooth).
    std::unique_ptr< double[] > magnitudes{ new double[ numHarmonics ] };
    auto pMag = magnitudes.get();
    for (size_t i = 0; i != numHarmonics; ++i )
    {
        double mag = 1.0 / double( i+1 );
        *pMag++ = mag;
    }

    // Now for the fundamental frequency (first harmonic), we want it to fill the epoch period
    // with one complete cycle. The harmonics will naturally have more than one cycle.
    // That is the purpose of this experiment, to determine the energy and prove an
    // algebraic calculation is all that is required (we know the integral).
    const auto fundamental = M_PI * 2 / epochSize;

    // Reset Comb Generator and fetch an epochs worth of data
    combGenerator.reset( numHarmonics, fundamental, magnitudes.get(), nullptr );
    auto pSamples = combGenerator.getEpoch();

    // Calculate the energy as the magnitude squared by the number of samples.
    double realEnergy = 0;
    for ( size_t i = 0; i != epochSize; ++i)
    {
        auto sample = *pSamples++;
        realEnergy += sample.real() * sample.real();
    }
    std::cout << "Real Energy: " << realEnergy << std::endl;

    // Now calculate the same algebraically using RMS value of sinusoids and epoch size.
    const auto sqrt2over2 = std::sqrt( 2.0 ) / 2.0;
    double calcEnergy = 0;
    for ( size_t i = 0; i != numHarmonics; ++i )
    {
        auto rmsV = magnitudes[i] * sqrt2over2;
        calcEnergy += rmsV * rmsV;
    }
    calcEnergy *= epochSize;
    std::cout << "Calc Energy: " << calcEnergy << std::endl;

    return 0;
}
