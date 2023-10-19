/**
 * @file CombGenerator.h
 * @brief The specification file for the Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#ifndef REISER_RT_COMBGENERATOR_H
#define REISER_RT_COMBGENERATOR_H

// Include Export Specification File
#include "ReiserRT_CombGeneratorExport.h"

#include "SharedScalarVectorTypeFwd.h"
#include "CombGeneratorEnvelopeFunkType.h"
#include "FlyingPhasorToneGeneratorDataTypes.h"

#include <memory>

namespace ReiserRT
{
    namespace Signal
    {
        /**
         * @brief Comb Generator
         *
         * The CombGenerator generates a harmonic spectrum in the form of a complex time series.
         * Internally it utilizes a collection of ReiserRT_FlyingPhasor instances set up at a
         * prescribed harmonic spacing. The initial magnitudes and phases of each tone, along with
         * their harmonic spacing are specified at `reset` time.
         *
         * The CombGenerator also provides support for individually modulating the tones produced through
         * an envelope functor interface also specified at `reset` time. @see CombGeneratorEnvelopeFunkType
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
             * The CombGenerator requires a maximum number of harmonics to be specified.
             * Use the qualified constructor.
             */
            CombGenerator() = delete;

            /**
             * @brief Qualified Constructor
             *
             * This constructor instantiates the implementation. This results in the creation of a
             * collection of ReiserRT_FlyingPhasor instances for a maximum number of harmonics required
             * of the instance during its lifetime. This `maxHarmonics` is inclusive of any fundamental frequency.
             *
             * @note A newly constructed instance will produce a series of zeros should the `getSamples`
             * operation be invoked prior to a `reset` invocation.
             *
             * @param maxHarmonics The maximum number of harmonics that an instance will support (fundamental included).
             */
            explicit CombGenerator( size_t maxHarmonics );

            /**
             * @brief Destructor
             *
             * Deletes the Implementation.
             */
            ~CombGenerator();

            /**
             * @brief Copy Construction is Disallowed
             *
             * The CombGenerator cannot be instantiated as a copy of another.
             */
            CombGenerator( const CombGenerator & another ) = delete;

            /**
             * @brief Copy Assignment is Disallowed
             *
             * The CombGenerator cannot be assigned from a copy of another.
             */
            CombGenerator & operator =( const CombGenerator & another ) = delete;

            /**
             * @brief The Reset Operation
             *
             * This operation prepares the CombGenerator for a subsequent series of `getSamples` invocations.
             * It sets 'N' ReiserRT_FlyingPhasor instances for the appropriate harmonic spacing based on the fundamental
             * frequency, and sets initial phases. It will copy the user specified envelope functor instance
             * for subsequent use. This is by default, a 'do nothing' envelope.
             *
             * @note We specify a shared pointer interface for the passing of scalar vectors
             * for this operation although we allow for nullptr defaults.
             * This shared pointer interface was a design decision. It is anticipated that
             * CombGenerator instances are seldom `reset` and that it would be the `getSamples`
             * operation that is primarily leveraged. It is the `getSamples` operation
             * that actually makes use of any magnitude vector we register here.
             * By specifying a shared pointer type, we are ensuring a reference count on it.
             * This is not actually necessary for the phase vector we don't want different semantics
             * for it.
             *
             * @param numHarmonics The number of harmonics to generate. Must be less than or equal to
             * the maximum specified during construction.
             * @param pMagVector This argument provides a series of magnitude values, of length `numHarmonics`.
             * The argument type is that of a constant shared pointer reference, which may be empty (null pointer).
             * Passing a null pointer (the default) results in a magnitude of 1.0 for all harmonic tones.
             * @warning Not providing the appropriate length of `numHarmonics`, for a non-null vector,
             * results in undefined behavior.
             * @param pPhaseVector This argument provides a series of starting phase values,
             * quantified in radians, of length `numHarmonics`.
             * The argument type is that of a constant shared pointer reference, which may be empty (null pointer).
             * Passing a null pointer (the default) results in an initial phase of 0.0 for all harmonic tones.
             * @warning Not providing the appropriate length of 'N' harmonics, for a non-null vector
             * results in undefined behavior.
             * @param envelopeFunk Functor interface for hooking the magnitude envelope applied during
             * harmonic tone generator.
             * The default for this parameter is to utilize an empty (null) function object.
             * In these cases, the magnitude specified in the pMagVector alone will be used.
             * A non-null `envelopeFunk` will be invoked for each harmonic tone generated.
             * This single instance `envelopeFunk` will be provided adequate information
             * through the CombGeneratorEnvelopeFunkType interface.
             *
             * @throw std::length_error If numHarmonics exceeds the maximum specified during construction.
             */
            void reset( size_t numHarmonics, double fundamentalRadiansPerSample,
                         const SharedScalarVectorType & magVector = nullptr,
                         const SharedScalarVectorType & phaseVector = nullptr,
                         const CombGeneratorEnvelopeFunkType & envelopeFunk = CombGeneratorEnvelopeFunkType{} );

            /**
             * @brief Get Samples Operation
             *
             * This operation delivers 'N' number samples from the CombGenerator into the user provided buffer.
             * If the user specified a non-null envelope functor during the `reset` operation. That functor
             * will be invoked once per harmonic tone being accumulated over the `numSamples` epoch.
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

#endif // #ifndef REISER_RT_COMBGENERATOR_H

