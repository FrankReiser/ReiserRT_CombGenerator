/**
 * @file CombGeneratorEnvelopFunkType.h
 * @brief The specification file for the Comb Generator Envelope Functor Type
 * @authors Frank Reiser
 * @date Initiated October 13th, 2022
 */

#ifndef REISER_RT_COMBGENERATORDATATYPES_H
#define REISER_RT_COMBGENERATORDATATYPES_H

#include <functional>

namespace ReiserRT
{
    namespace Signal
    {
        /**
         * @brief The Comb Generator Envelope Functor Type
         *
         * The CombGenerator does not prescribe any particular form of envelope other than that
         * of a constant magnitude for each tone.
         * However, a client may require a particular envelope and, on a per tone basis.
         * This functor type provides a mechanism for clients to provide envelopes
         * for each tone.
         * The parameters are all hints that the client may make use of in generation of
         * envelopes.
         *
         * @param currentSample The current running sample counter for the Nth harmonic tone.
         * @param numSamples The number of samples of envelope to generate. This number is passed
         * down directly from a `CombGenerator::getSamples` invocation.
         * @note The client is expected to provide the necessary buffering for the generation of envelopes.
         * Envelope data will be incorporated immediately after functor invocation and the client
         * may safely reuse the buffer for subsequent functor invocations of any additional harmonics.
         * @param nHarmonic The zeroth based harmonic (0 being the fundamental).
         * @param nominalMag The default magnitude for the Nth harmonic, specified at reset time.
         *
         * @return Returns a pointer to a buffer of length numSamples, populated with the envelope to apply
         * for the Nth harmonic tone.
         * @warning Failure to return the length of numSamples may result in undefined behaviour.
         * The data should linger until next invocation but will be consumed immediately.
         */
        using CombGeneratorEnvelopeFunkType =
                std::function< const double *( size_t currentSample, size_t numSamples,
                                               size_t nHarmonic, double nominalMag ) >;
    }
}
#endif //REISER_RT_COMBGENERATORDATATYPES_H

