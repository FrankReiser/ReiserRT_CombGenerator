/**
 * @file ScintillationEngine.h
 * @brief The specification file for the Scintillation Engine.
 * @authors Frank Reiser
 * @date Initiated September 23rd, 2022
 */

#ifndef TSG_NG_SCINTILLATIONENGINE_H
#define TSG_NG_SCINTILLATIONENGINE_H

#include "CombGeneratorExport.h"

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
    class CombGenerator_EXPORT ScintillationEngine
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
         * initial magnitude over the course of an epoch. The 'second' value represents
         * the change per sample in magnitude over the course of a decorrelation period.
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
         * @brief Default Construction is Disallowed
         *
         * The ScintillationEngine requires a buffer space and its length. Use the qualified constructor.
         */
        ScintillationEngine() = delete;

        /**
         * @brief Qualified Constructor
         *
         * The ScintillationEngine requires buffer space and its length. It will operate
         * on this buffer when run so it is expected to persist. It does not take ownership
         * of this buffer. The ScintillationEngine is primarily a logic facility and maintains little
         * state information of its own.
         *
         * @param pTheScintillationBuffer A buffer where scintillated magnitude values will be written when run.
         * @param theBufLen The buffer length.
         */
        ScintillationEngine( double * pTheScintillationBuffer, size_t theBufLen )
                : pScintillationBuffer( pTheScintillationBuffer )
                , bufLen( theBufLen )
        {
        }

        /**
         * @brief Default Destructor
         *
         * We have no special requirements.
         */
        ~ScintillationEngine() = default;

        /**
         * @brief The Run Operation
         *
         * Invoking this operation will generate scintillated magnitude values into the scintillation
         * buffer provided during construction. It will invoke the client provided scintillateFunk
         * a the beginning of decorrelation periods.
         *
         * @param scintillateFunk Client provided functor interface that returns a scintillated magnitude value
         * when invoked. The distribution utilized is up to the client.
         * @param scintillationState The StateType to be used and potentially managed on start of decorrelation period.
         * @param sampleCounter The value of the sample counter at the time of invocation. This would typically be the
         * start value for an Nth epoch.
         * @param decorrelationSamples The number of samples representing a decorrelation period.
         * @warning Invoking with decorrelationSamples of zero is a bad idea. Don't do it.
         */
        void run( const ScintillateFunkType & scintillateFunk,
                  StateType & scintillationState,
                  size_t sampleCounter,
                  size_t decorrelationSamples );

    private:
        double * const pScintillationBuffer;    //!< The Scintillation Buffer
        const size_t bufLen;                    //!< The Scintillation Buffer Length
    };


}

#endif //TSG_NG_SCINTILLATIONENGINE_H
