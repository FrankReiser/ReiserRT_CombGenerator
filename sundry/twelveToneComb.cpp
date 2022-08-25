//
// Created by frank on 8/25/22.
//

#include "CombGenerator.h"

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

    // Setup some initial magnitudes. Each tone half the power of the previous.
    std::unique_ptr< double[] > magnitudes{ new double[maxSpectralLines] };
    for ( size_t i = 0; i != maxSpectralLines; ++i )
    {
        magnitudes[i] = 1.0 * std::pow( 0.707, i );
//        std::cout << "Mag[" << i << "] = " << magnitudes[i] << std::endl;
    }

    CombGenerator::ResetParameters resetParams{
            .numLines = maxSpectralLines,
            .spacingRadiansPerSample = M_PI / maxSpectralLines / 2.0,
            .pMagnitudes = magnitudes.get(),
            .decorrelationSamples = epochSize * 3 / 2,
            .randSeed = 1113,
    };
    combGenerator.reset( resetParams );

    double t0, t1;
    t0 = getClockMonotonic();
    combGenerator.getSamples();
    t1 = getClockMonotonic();

    std::cout
        << "Performance for maxSpectralLines=" << maxSpectralLines
        << ", epochSize=" << epochSize
        << " is " << t1-t0 << " seconds." << std::endl;

    return 0;
}