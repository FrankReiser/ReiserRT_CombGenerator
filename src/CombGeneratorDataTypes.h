/**
 * @file CombGeneratorDataTypes.h
 * @brief The specification file for the Comb Generator Data Types
 * @authors Frank Reiser
 * @date Initiated October 13th, 2022
 */

#ifndef TSG_NG_COMBGENERATORDATATYPES_H
#define TSG_NG_COMBGENERATORDATATYPES_H

#include <functional>

namespace TSG_NG {
    /**
     * @brief The Comb Generatore Envelope Functor Type
     *
     * The CombGenerator does not prescribe any particular form of envelope other than constant 1.0.
     * The client may require a particular envelope and, on a per tone basis.
     * This functor type allows the client to provide specific envelopes to each tone.
     * The parameters are all hints that the client might make use of in generating an envelope.
     *
     * @param currentSampleCount The current running sample counter for the Nth harmonic tone.
     * @param nHarmonic The zeroth based harmonic (0 being the fundamental).
     * @param nominalMag The default magnitude for the Nth harmonic, specified at reset time.
     *
     * @return Returns a pointer to a buffer of length epochSize (specified during CombGenerator construction),
     * populated the envelope to apply for the Nth harmonic tone.
     */
    using CombGeneratorEnvelopeFunkType =
    std::function< const double *( size_t currentSampleCount, size_t nHarmonic, double nominalMag ) >;

}
#endif //TSG_NG_COMBGENERATORDATATYPES_H
