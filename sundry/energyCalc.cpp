/**
 * @file energyCalc.cpp
 * @brief A Simple Proof that RMS Voltage Can Be Used Over Numerical Integration
 * @authors Frank Reiser
 * @date Initiated October 21st, 2022
 */

#include "CombGenerator.h"

#include <memory>
#include <iostream>

using namespace ReiserRT::Signal;

int main()
{
    // This will be our both our maximum and also the number we will use.
    constexpr size_t numHarmonics = 4;

    // This will be the number of samples we will fetch.
    constexpr size_t epochSize = 4096;

    std::unique_ptr< FlyingPhasorElementType[] > epochSampleBuffer{new FlyingPhasorElementType [ epochSize ] };
    FlyingPhasorElementBufferTypePtr pEpochSampleBuffer = epochSampleBuffer.get();

    // We want a decreasing magnitude for each harmonic.
    // Each harmonic has the reciprocal amplitude of its position (classic sawtooth).
    std::unique_ptr< double[] > magnitudes{ new double[ numHarmonics ] };
    auto pMag = magnitudes.get();
    for (size_t i = 0; i != numHarmonics; ++i )
    {
        double mag = 1.0 / double( i+1 );
        *pMag++ = mag;
    }

    // Transfer managed memory in shared pointer type for CombGenerator reset operation.
    SharedScalarVectorType sharedMagnitudes{ std::move( magnitudes ) };

    // Now for the fundamental frequency (first harmonic), we want it to fill the epoch period
    // with one complete cycle. The harmonics will naturally have more than one cycle.
    // That is the purpose of this experiment, to determine the energy of one fundamental period and prove an
    // algebraic calculation is all that is required (we know the integral).
    const auto fundamental = M_PI * 2 / epochSize;

    // Instantiate Comb Generator for number of harmonics and epoch size.
    // Note: Max and Number of Harmonics same for this experiment.
    CombGenerator combGenerator{ numHarmonics };

    // Reset Comb Generator and fetch an epochs worth of data
    combGenerator.reset( numHarmonics, fundamental, sharedMagnitudes );
    combGenerator.getSamples( pEpochSampleBuffer, epochSize );

    // Calculate the energy as the magnitude squared by the number of samples.
    double realEnergy = 0;
    for ( size_t i = 0; i != epochSize; ++i)
    {
        auto sample = *pEpochSampleBuffer++;
        realEnergy += sample.real() * sample.real();
    }
    std::cout << "Real Energy: " << realEnergy << " (mag^2*samples)" << std::endl;

    // Now calculate the same algebraically using RMS value of sinusoids and epoch size.
    const auto sqrt2over2 = std::sqrt( 2.0 ) / 2.0;
    double calcEnergy = 0;
    for ( size_t i = 0; i != numHarmonics; ++i )
    {
        auto rmsMag = sharedMagnitudes[std::ptrdiff_t(i)] * sqrt2over2;
        calcEnergy += rmsMag * rmsMag;
    }
    calcEnergy *= epochSize;
    std::cout << "Calc Energy: " << calcEnergy << " (rmsMag^2*samples)" << std::endl;

    // Some Noise Calculations
    // We will start by specifying a desired SNR for our signal over a "Band Of Interest" arbitrarily set.
    const auto snr = 25.0;
    const auto noiseVRatio = std::pow( 10.0, snr/20.0);
    std::cout << "Noise Voltage Ratio: " << noiseVRatio << std::endl;

    // SNR is defined over a Band of Interest (BOI). This BOI over the Sample Rate sets up
    // what we will refer to as Band Of Interest to Sample Rate (f sub s) Ratio.
    // For this test, we will arbitrarily set this to ratio.
    const double bandOfInterestFsRatio = 0.1;

    // Total Noise Energy is the number of samples for a period of the fundamental which for us is the same
    // as epoch size here but, this may not always the case.
    const double periodSamples = epochSize;

    // Sigma Calculation
    // The times 2 in the formula is because we will incorporate sigma into both I and Q at the end of the day.
    const auto sigma = std::sqrt( calcEnergy / ( 2 * periodSamples * bandOfInterestFsRatio ) ) / noiseVRatio;
    std::cout << "Sigma: " << sigma << std::endl;

    return 0;
}
