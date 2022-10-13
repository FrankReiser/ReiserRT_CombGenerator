/**
 * @file CombGenerator.h
 * @brief The specification file for the Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#ifndef TSG_NG_COMB_GENERATOR_H
#define TSG_NG_COMB_GENERATOR_H

// Include Export Specification File
#include "TsgNgCombGeneratorExport.h"

#include "CombGeneratorDataTypes.h"
#include "FlyingPhasorToneGeneratorDataTypes.h"

namespace TSG_NG
{
     /**
     * @brief Comb Generator
     *
     * The CombGenerator generates a harmonic spectrum in the form of a complex time series
     * of specified length. Internally it utilizes a batch of ReiserRT_FlyingPhasor instances
     * set up at a prescribed harmonic spacing. The magnitudes and phases of each tone, along with
     * the harmonic spacing are specified at 'reset' time.
     *
     * The CombGenerator also provides support for individually modulating the tones produced through
     * an envelope functor interface. @see CombGeneratorEnvelopeFunkType.
     */
    class TsgNgCombGenerator_EXPORT CombGenerator
    {
    private:
        /**
         * @brief Forward Reference to Hidden Implementation
         */
        class Imple;

    public:
        /**
         * @brief Default Construction is Disallowed
         *
         * The CombGenerator requires a maximum number of lines and an epoch size (number of samples per epoch).
         * Use the qualified constructor.
         */
        CombGenerator() = delete;

        /**
         * @brief Qualified Constructor
         *
         * This constructor instantiates the implementation. This results in the creation of a
         * batch of ReiserRT_FlyingPhasor instances for a the given, maximum use case scenario.
         * It also creates up the necessary buffer for the aggregations of signal data and state machine data
         * for potential scintillation use cases.
         *
         * @param maxHarmonics The maximum number of harmonics that an instance will support (fundamental included).
         * @param epochSize The number of samples that make up an epoch.
         */
        CombGenerator( size_t maxHarmonics, size_t epochSize );

        /**
         * @brief Destructor
         *
         * Deletes the Implementation.
         */
        ~CombGenerator();

        /**
         * @brief The Reset Operation
         *
         * This operation prepares the CombGenerator for a subsequent series of getEpoch invocations.
         * It sets N FlyingPhasors instances for the appropriate harmonic spacing based on the fundamental
         * frequency in radians per sample at the specified starting phases. It will copy the user specified
         * (or defaulted) envelope functor instance for subsequent use.
         *
         * @param numHarmonics The number of harmonics to generate. Must be no greater than the maximum specified
         * during construction.
         * @param pMagVector This argument provides a series of magnitude values, of length N harmonics.
         * Passing a null pointer results in default magnitude of 1.0 for all harmonics.
         * Otherwise, the data pointed to is expected to persist between CombGenerator reset cycles.
         * CombGenerator does not cache the data and is accessed during getEpoch invocations.
         * @param pPhaseVector This argument provides a series of radian phase values, of length N harmonics.
         * Passing a null pointer results in default phase of 0.0 for all harmonics.
         * The data pointed to need not persist between CombGenerator reset cycles.
         * CombGenerator only this data within the reset invocation and has no further use for it.
         * @param envelopeFunk Functor interface for obtaining a magnitude envelop from a client.
         * The default for this parameter is to utilize an empty (null) function object.
         * In these cases, no envelope is applied.
         */
        void reset ( size_t numHarmonics, double fundamentalRadiansPerSample,
                     const double * pMagVector, const double * pPhaseVector,
                     const CombGeneratorEnvelopeFunkType & envelopeFunk = CombGeneratorEnvelopeFunkType{} );

        /**
         * @brief The Get Epoch Operation
         *
         * This operation returns a pointer to internal buffer space where an epoch's worth of harmonic
         * spectrum, complex time series data, resides. It will invoke the client provided envelopeFunk (if non-null)
         * for each N harmonics specified during reset, for an envelope to use.
         *
         * @return Returns a pointer to an internal buffer where an epoch's worth of harmonic
         * spectrum, complex time series data, resides.
         */
        const ReiserRT::Signal::FlyingPhasorElementBufferTypePtr getEpoch();

    private:
        Imple * pImple;    //!< Pointer to hidden implementation.
    };
}

#endif // #ifndef TSG_NG_COMB_GENERATOR_H

