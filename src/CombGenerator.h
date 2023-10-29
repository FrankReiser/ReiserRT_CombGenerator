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
         * an envelope functor interface, optionally specified at `reset` time.
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
             * @brief Qualified Constructor
             *
             * This constructor instantiates the implementation. This results in the creation of a
             * collection of ReiserRT_FlyingPhasor instances for a maximum number of harmonics required
             * of the instance during its lifetime. This `maxHarmonics` is inclusive of any fundamental frequency.
             *
             * @note A newly constructed instance will produce a series of zeros should the `getSamples`
             * operation be invoked prior to a `reset` invocation with specific harmonic series generation parameters.
             *
             * @param maxHarmonics The maximum number of harmonics that an instance will support (fundamental included)
             * during its lifetime.
             * @note Accepting the default value of zero instantiates a relatively useless CombGenerator instance.
             * It cannot be `reset` to generate any tones without throwing an exception. The reasons we allow this
             * is so that another instance can be "moved" into such a defaulted instance. It also helps get past
             * some issues should you try to reserve vector space for CombGenerator instances.
             */
            explicit CombGenerator( size_t maxHarmonics = 0 );

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
             * @brief Move Constructor
             *
             * This Constructor moves the "Implementation Pointer" out of another instance
             * resulting in a nullptr for said instance.
             * @note Moving raw pointer `pImple` requires a non-default move constructor implementation
             * as the compiler default treats it the same as a copy.
             *
             * @param another Another instance being moved from.
             */
            CombGenerator( CombGenerator && another ) noexcept;

            /**
             * @brief Move Assignment Operator
             *
             * This Assignment operator deletes its current "Implementation Pointer" and
             * moves the the one out of the other instance resulting in a nullptr for said instance.
             * @note Moving raw pointer `pImple` requires a non-default assignment operator implementation
             * as the compiler default treats it the same as a copy.
             *
             * @param another Another instance being moved from.
             */
            CombGenerator & operator =( CombGenerator && another ) noexcept;

            /**
             * @brief The Reset Operation with Specific Generation Parameters
             *
             * This operation prepares the CombGenerator for a subsequent series of `getSamples` invocations.
             * It sets up 'N' ReiserRT_FlyingPhasor instances for the appropriate harmonic spacing based on
             * a fundamental frequency. It also establishes the magnitudes and initial phase angle of each
             * harmonic tone. Lastly, if magnitude envelope control is required, a callback functor may be registered.
             *
             * @note We specify a shared pointer interface for the passing of scalar vectors
             * for this operation although we allow these to be empty pointers (containing nullptr).
             * This shared pointer interface was a design decision. It is anticipated that
             * CombGenerator instances are seldom `reset` and that it would be the `getSamples`
             * operation that is primarily leveraged. It is the `getSamples` operation
             * that actually makes use of any magnitude vector we register here.
             * By specifying a shared pointer type, we are ensuring a reference count on it.
             * Anticipated use cases of CombGenerator called for reuse of magnitude vectors.
             * Other design choices were considered such as copying the const data at `reset` time but,
             * this seemed wasteful. Also considered was just storing the data address and trusting the client
             * to maintain the storage but, this seemed unsafe. Reference counting seemed the best choice.
             * The phase vector does not have the same requirements but, we don't want
             * to use different semantics for it. That would be confusing.
             * @see SharedScalarVectorType
             *
             * @param numHarmonics The number of harmonics to generate. Must be less than or equal to
             * the maximum specified during construction.
             * @throw std::length_error If numHarmonics exceeds the maximum specified during construction.
             * @note It is not recommended that you use this operation on default constructed CombGenerator instance
             * as a `numHarmonics` value of just 1 will throw.
             * @param pMagVector This argument provides a series of magnitude values, of minimum length `numHarmonics`.
             * The argument type is that of a constant SharedScalarVectorType reference, which may be empty.
             * Passing an empty  pointer results in a magnitude of 1.0 for all harmonic tones.
             * @warning Not providing the minimum length of `numHarmonics`, for a non-null vector,
             * results in undefined behavior.
             * @param pPhaseVector This argument provides a series of starting phase values,
             * quantified in radians, of minimum length `numHarmonics`.
             * The argument type is that of a constant SharedScalarVectorType reference, which may be empty.
             * Passing an empty pointer results in an initial phase of 0.0 for all harmonic tones.
             * @warning Not providing the minimum length of `numHarmonics` harmonics, for a non-null vector
             * results in undefined behavior.
             * @param envelopeFunk Callback functor interface for hooking magnitude envelopes applied during
             * harmonic tone generation. The default for this parameter (empty function object) results
             * in constant magnitudes specified in the pMagVector for each harmonic tone.
             * A non-empty `envelopeFunk` will be invoked for each harmonic tone accumulated during a `getSamples`
             * invocation. Adequate information is provided through the CombGeneratorEnvelopeFunkType interface.
             * @see CombGeneratorEnvelopeFunkType for callback interface details.
             * @warning Functor `envelopeFunk` will be copied for subsequent usage by `getSamples`
             * This copy must remain viable until subsequent `reset` of the CombGenerator.
             */
            void reset( size_t numHarmonics, double fundamentalRadiansPerSample,
                         const SharedScalarVectorType & magVector,
                         const SharedScalarVectorType & phaseVector,
                         const CombGeneratorEnvelopeFunkType & envelopeFunk = CombGeneratorEnvelopeFunkType{} );

            /**
             * @brief Get Samples Operation
             *
             * This operation delivers 'N' number of samples from the CombGenerator into the user provided buffer
             * overwriting the buffers content.
             * If the user specified a non-empty envelope functor during the `reset` operation. That functor
             * will be invoked once per harmonic tone being accumulated over the `numSamples` epoch.
             *
             * @param pElementBuffer User provided buffer large enough to hold the requested number of samples.
             * @param numSamples The number of samples to be delivered.
             */
            void getSamples( FlyingPhasorElementBufferTypePtr pElementBuffer, size_t numSamples );

            /**
             * @brief Accumulate Samples Operation.
             *
             * This operation accumulates 'N' number of samples from the CombGenerator onto the user provided buffer.
             * This affords the ability to collect the output of multiple CombGenerator instances.
             * If the user specified a non-empty envelope functor during the `reset` operation. That functor
             * will be invoked once per harmonic tone being accumulated over the `numSamples` epoch.
             *
             * @param pElementBuffer User provided buffer large enough to hold the requested number of samples.
             * @param numSamples The number of samples to be delivered.
             */
            void accumSamples( FlyingPhasorElementBufferTypePtr pElementBuffer, size_t numSamples );

            /**
             * @brief The Reset Operation No Generation Parameters (Pure Reset)
             *
             * This "pure reset" operation exists to return an instance to its freshly constructed state.
             * A CombGenerator in this state will produce a "zero signal". This is useful for a larger use case
             * where a bank of CombGenerator instances may exist and contain contaminated state from a previous
             * generation run. If only some of a bank will be required on a subsequent generation run, a "pure reset"
             * on reserved instances ensures that so you don't leave trash laying around.
             */
            void reset();

            /**
             * @brief Query the Current Number of Harmonics
             *
             * This operation returns the current number of harmonic tones that will be produced
             * (fundamental included), within the signal data  delivered via a `getSamples` invocation.
             * This is primarily useful for verification purposes as the return value shall be zero right
             * after construction and potentially non-zero after a `reset` invocation.
             *
             * @return The number of harmonics
             */
            [[nodiscard]] size_t getNumHarmonics() const;

        private:
            Imple * pImple{};    //!< Pointer to hidden implementation.
        };
    }
}

#endif // #ifndef REISER_RT_COMBGENERATOR_H

