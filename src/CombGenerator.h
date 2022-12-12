/**
 * @file CombGenerator.h
 * @brief The specification file for the Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#ifndef REISER_RT_COMB_GENERATOR_H
#define REISER_RT_COMB_GENERATOR_H

// Include Export Specification File
#include "ReiserRT_CombGeneratorExport.h"

#include "CombGeneratorDataTypes.h"
#include "FlyingPhasorToneGeneratorDataTypes.h"

namespace ReiserRT
{
    namespace Signal
    {
        /**
         * @brief Comb Generator
         *
         * The CombGenerator generates a harmonic spectrum in the form of a complex time series
         * of specified length. Internally it utilizes a batch of ReiserRT_FlyingPhasor instances
         * set up at a prescribed harmonic spacing. The initial magnitudes and phases of each tone, along with
         * their harmonic spacing are specified at 'reset' time.
         *
         * The CombGenerator also provides support for individually modulating the tones produced through
         * an envelope functor interface. @see CombGeneratorEnvelopeFunkType.
         */
        class ReiserRT_CombGenerator_EXPORT CombGenerator
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
             * batch of ReiserRT_FlyingPhasor instances for a maximum number of harmonics, use case.
             *
             * @param maxHarmonics The maximum number of harmonics that an instance will support (fundamental included).
             */
            CombGenerator( size_t maxHarmonics );

            /**
             * @brief Destructor
             *
             * Deletes the Implementation.
             */
            ~CombGenerator();

            /**
             * @brief The Reset Operation
             *
             * This operation prepares the CombGenerator for a subsequent series of `getSamples` invocations.
             * It sets N FlyingPhasor instances for the appropriate harmonic spacing based on the fundamental
             * frequency in radians per sample at the specified starting phases. It will copy the user specified
             * (or defaulted) envelope functor instance for subsequent use.
             *
             * @todo Have this use Shared Pointers from Block Pool like MultiThreaded Version Does.
             *
             * @param numHarmonics The number of harmonics to generate. Must be less than or equal to
             * the maximum specified during construction.
             * @param pMagVector This argument provides a series of magnitude values, of length N harmonics.
             * Passing a null pointer results in default magnitude of 1.0 for all harmonics.
             * Otherwise, the data pointed to is expected to persist between CombGenerator reset cycles.
             * CombGenerator does not copy this data and it will be accessed during each subsequent `getEpoch`
             * invocation.
             * @param pPhaseVector This argument provides a series of starting phase values,
             * in radians, of length N harmonics. Passing a null pointer results in default phase of 0.0
             * for all harmonics. This data need not persist between CombGenerator reset cycles.
             * CombGenerator only uses this data within `reset` and has no further use for it.
             * @param envelopeFunk Functor interface for obtaining a magnitude envelop from a client.
             * The default for this parameter is to utilize an empty (null) function object.
             * In these cases, no envelope is applied.
             *
             * @throw Throws `std::length_error` if numHarmonics exceeds the maximum specified during construction.
             */
            void reset ( size_t numHarmonics, double fundamentalRadiansPerSample,
                         const double * pMagVector, const double * pPhaseVector,
                         const CombGeneratorEnvelopeFunkType & envelopeFunk = CombGeneratorEnvelopeFunkType{} );

            /**
             * @brief Get Samples Operation
             *
             * This operation delivers 'N' number samples from the Comb Generator into the user provided buffer.
             * If the user specified a non-empty envelope function during the `reset` operation. The function
             * will be invoked once per harmonic tone being accumulated, to obtain envelopes to modulate the tones.
             *
             * @param pElementBuffer User provided buffer large enough to hold the requested number of samples.
             * @param numSamples The number of samples to be delivered.
             */
            void getSamples( FlyingPhasorElementBufferTypePtr pElementBuffer, size_t numSamples );

        private:
            Imple * pImple;    //!< Pointer to hidden implementation.
        };
    }
}

#endif // #ifndef REISER_RT_COMB_GENERATOR_H

