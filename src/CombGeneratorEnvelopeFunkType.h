/**
 * @file CombGeneratorEnvelopFunkType.h
 * @brief The specification file for the Comb Generator Envelope Functor Type
 * @authors Frank Reiser
 * @date Initiated October 13th, 2022
 */

#ifndef REISER_RT_COMBGENERATORENVELOPEFUNKTYPE_H
#define REISER_RT_COMBGENERATORENVELOPEFUNKTYPE_H

#include <functional>

namespace ReiserRT
{
    namespace Signal
    {
        /**
         * @brief The Comb Generator Envelope Functor Type
         *
         * The CombGenerator does not prescribe any particular form of envelope other than that
         * of a constant magnitude for each harmonic generated.
         * If a client requires specific envelopes, potentially on a tone by tone basis,
         * this functor type may be used to register a callback operation which will deliver envelopes.
         * Instances are to be registered with `CombGenerator::reset` operation and will be
         * notified during subsequent `CombGenerator::getSamples` invocations.
         *
         * The parameters provided to the client during callback are all hints
         * that the client may use of in its generation of envelopes.
         *
         * @param currentSample The current running sample counter for the Nth harmonic tone.
         * @param numSamples The number of samples of envelope to generate. This number is passed
         * down directly from a `CombGenerator::getSamples` invocation.
         * @note The client is expected to provide the necessary buffering for the generation of envelopes
         * up to some predetermined maximum length.
         * @param nHarmonic The zeroth based harmonic (0 being the fundamental).
         * @param nominalMag The default magnitude for the Nth harmonic, specified at reset time.
         *
         * @return Returns a pointer to a buffer of minimum length, `numSamples`, populated with the
         * envelope to apply for the Nth harmonic tone.
         * Envelope data will be incorporated immediately after functor invocation
         * and this buffer may be safely reused for subsequent functor invocations.
         * @warning Failure to provide envelop data of minimum length `numSamples` results in undefined behaviour.
         */
        using CombGeneratorEnvelopeFunkType =
                std::function< const double *( size_t currentSample, size_t numSamples,
                                               size_t nHarmonic, double nominalMag ) >;
    }
}
#endif //REISER_RT_COMBGENERATORENVELOPEFUNKTYPE_H
