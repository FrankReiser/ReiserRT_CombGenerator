/**
 * @file ScintillationEngine.h
 * @brief The specification file for the Scintillation Engine.
 * @authors Frank Reiser
 * @date Initiated September 23rd, 2022
 */

#ifndef TSG_NG_SCINTILLATIONENGINE_H
#define TSG_NG_SCINTILLATIONENGINE_H

#include "TsgNgCombGeneratorExport.h"

#include <cstdlib>
#include <functional>

namespace TSG_NG
{
    /**
     * @brief Scintillation Engine
     *
     * This class provides reusable logic to generate a sliding magnitude vector (scintillation)
     * over decorrelation periods, over the source of a number of samples (an epoch). It does
     * not maintain state. It relies on the client to provide state information to manage.
     * It also relies on the client to provide scintillated magnitudes upon request via an observer
     * interface when a new scintillation target magnitude is needed.
     *
     * It was specifically designed this way so that the logic of performing scintillation can be
     * tested with necessarily employing a random number distribution.
     */
    class TsgNgCombGenerator_EXPORT ScintillationEngine
    {
    public:
        // The first value of the pair represents the initial scintillated magnitude over the course of a decorrelation
        // period. The second value of the pair represents the change per sample in magnitude (i.e., a slope) from the initial
        // (first) value.

        /**
         * @brief The Scintillation State Type
         *
         * This type specifies the state information managed by the ScintillationEngine.
         * As already mentioned, the ScintillationEngine does not maintain state of its own.
         * It relies on the client to provide the state being managed when the ScintillationEngine
         * is run. This StateType is is simply a pair of doubles. The 'first' value represents the
         * initial magnitude over the course of scintillation. The 'second' value represents
         * the change per sample in magnitude over the course of a decorrelation period.
         * These values are managed (read and potentially written) during the course of a run.
         */
        using StateType = std::pair< double, double >;

        /**
         * @brief The Scintillate Function Type
         *
         * The ScintillationEngine does not specify particular distribution. It relies on the
         * client to provide those details at the beginning of decorrelation periods.
         * We specify a functor that returns a double precision value and requires no arguments.
         * This type is used in the run operation.
         */
        using ScintillateFunkType = std::function< double() >;

        /**
         * @brief Default Construction
         *
         * The ScintillationEngine requires no information to instantiate. If fact, it is designed to
         * be instance-less. It's only member function is declared static and operates strictly from
         * arguments provided.
         */
        ScintillationEngine() = default;

        /**
         * @brief Default Destructor
         *
         * We have no special requirements.
         */
        ~ScintillationEngine() = default;

        /**
         * @brief The Static Run Operation
         *
         * Invoking this operation will generate scintillated magnitude values into a user provided
         * buffer of the desired run length. It will invoke the client provided scintillateFunk
         * a the beginning of decorrelation periods.
         *
         * @param pBuffer A buffer where scintillated magnitude values will be written to.
         * @param runLen The number of scintillated magnitude values to write.
         * @param scintillateFunk Client provided functor interface that returns a scintillated magnitude value
         * when invoked. The distribution utilized is up to the client.
         * @param scintillationState The StateType to be used and potentially managed on start of decorrelation period.
         * @param sampleCounter The value of the sample counter at the time of invocation. This would typically be the
         * start value for an Nth epoch.
         * @param decorrelationSamples The number of samples representing a decorrelation period.
         * @warning Invoking with decorrelationSamples of zero is a bad idea. Don't do it.
         */
        static void run( double * pBuffer,
                         size_t runLen,
                         const ScintillateFunkType & scintillateFunk,
                         StateType & scintillationState,
                         size_t sampleCounter,
                         size_t decorrelationSamples );

    };


}

#endif //TSG_NG_SCINTILLATIONENGINE_H
