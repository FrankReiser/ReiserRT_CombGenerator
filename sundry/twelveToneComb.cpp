//
// Created by frank on 8/25/22.
//

#include "CombGenerator.h"
#include "CombGeneratorDataTypes.h"
#include "SubSeedGenerator.h"
#include "RandomPhaseDistributor.h"
#include "RayleighDistributor.h"

#include <memory>
#include <cmath>
#include <iostream>
#include <cstring>

using namespace TSG_NG;

void setupScheduling()
{
    ///@note Assumptions: Assuming PTHREAD_SCOPE_SYSTEM is scheduler scope and PTHREAD_INHERIT_SCHED is set
    ///We will simply attempt to enable SCHED_FIFO and potentially set a minor level priority.
    ///Other threads may need prioritization of there own.

    sched_param schedParam;
    int policy;
    if ( pthread_getschedparam( pthread_self(), &policy, &schedParam ) )
    {
        std::cout << "Failed to get scheduling parameters. "
                  << "Unable to setup Realtime scheduling" << std::endl;
        return;
    }

    int minPriority = sched_get_priority_min( SCHED_FIFO );
    int maxPriority = sched_get_priority_max( SCHED_FIFO );
    schedParam.sched_priority = minPriority + ( maxPriority - minPriority ) * 5 / 100;

    int retCode = pthread_setschedparam( pthread_self(), SCHED_FIFO, &schedParam );
    if ( 0 != retCode )
    {
        std::cout << "Failed to set scheduling parameters. " << strerror( retCode ) << ". "
                  << "Unable to setup Realtime scheduling" << std::endl;
        return;
    }

    std::cout << "Enabled Real Time Scheduling!" << std::endl;
}

double getClockMonotonic()
{
    timespec tNow = { 0, 0 };
    clock_gettime( CLOCK_MONOTONIC, &tNow );

    return double( tNow.tv_sec ) + double( tNow.tv_nsec ) / 1e9;
}

int main()
{
    // Setup Realtime Scheduling
    setupScheduling();

    // This will be our both our maximum and also the number we will use.
    constexpr size_t maxSpectralLines = 12;

    // This will be the number of samples we will fetch at a time.
    constexpr size_t epochSize = 2048;

    // Instantiate our Comb Generator
    CombGenerator combGenerator{ maxSpectralLines, epochSize };

    // We are going to generate seeds from a master seed for eventual use with multiple comb generator instances.
    // We do not want to use the same engine used by the CombGenerator itself as that would be problematic.
    // Doing so would lead to multiple CombGenerators producing overlapping random sequences. That would be bad.
    const uint32_t masterSeed = 1113;
    SubSeedGenerator subSeedGenerator{};
    subSeedGenerator.reset( masterSeed );

    // Instantiate a Random Phase Distributor and seed it with a value from our sub-seed generator.
    RandomPhaseDistributor randomPhaseDistributor{};
    randomPhaseDistributor.reset( subSeedGenerator.getSubSeed() );

    // Setup some initial magnitudes. Each tone half the power of the previous.
    std::unique_ptr< double[] > magnitudes{ new double[ maxSpectralLines ] };
    std::unique_ptr< double[] > phases{ new double[ maxSpectralLines ] };
    const auto sqrt2over2 = std::sqrt( 2.0 ) / 2.0;
    for ( size_t i = 0; i != maxSpectralLines; ++i )
    {
        magnitudes[i] = 1.0 * std::pow( sqrt2over2, i );
        phases[i] = randomPhaseDistributor.getValue();
    }

    // We are going to need a scintillation random number distributor
    RayleighDistributor rayleighDistributor{};
    rayleighDistributor.reset( subSeedGenerator.getSubSeed() );
    auto scintillateFunk = [ &rayleighDistributor ]( double desiredMean, size_t lineNumberHint )
    {
        return rayleighDistributor.getValue( desiredMean );
    };

    // Reset the Comb Generator with some parameters.
    CombGeneratorResetParameters resetParams;
    resetParams.numLines = maxSpectralLines;
    resetParams.spacingRadiansPerSample = M_PI / maxSpectralLines / 2.0;
    resetParams.decorrelationSamples = epochSize * 2;   // Scintillation is costlier, so we're sort of getting a worse case.
    combGenerator.reset( resetParams, magnitudes.get(), phases.get(), std::ref( scintillateFunk ) );

    double t0, t1;
    t0 = getClockMonotonic();
    combGenerator.getEpoch(std::ref(scintillateFunk));
    t1 = getClockMonotonic();

    std::cout
        << "Performance for maxSpectralLines=" << maxSpectralLines
        << ", epochSize=" << epochSize
        << " is " << t1-t0 << " seconds." << std::endl;

    return 0;
}
