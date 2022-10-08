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

#include "FlyingPhasorToneGeneratorDataTypes.h"

#include <functional>

namespace TSG_NG
{
    struct CombGeneratorResetParameters;

    /**
     * @brief Comb Generator
     *
     * The CombGenerator generates a harmonic spectrum in the form of a complex time series
     * of specified length. Internally it utilizes a batch of ReiserRT_FlyingPhasor instances
     * set up at a prescribed harmonic spacing. The magnitudes and phases of each tone, along with
     * the harmonic spacing are specified at 'reset' time.
     *
     * The CombGenerator also provides support for individually scintillating the tones produced through
     * a scintillation observer interface with specified decorrelation period. @see ScintillateFunkType.
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
         * @brief The Scintillate Functor Type
         *
         * The CombGenerator does not specify particular distribution for scintillating. It relies on the
         * client to provide those details at the beginning of decorrelation periods.
         * We specify a functor that returns a double precision value. This functor receives arguments for the
         * expected magnitude value and a spectral line number hint which may be useful to the client.
         * This functor type is required in the reset and getEpoch operations.
         */
        using ScintillateFunkType = std::function< double( double desiredMean, size_t lineNumberHint ) >;

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
         * It lso creates up the necessary buffer for the aggregations of signal data and state machine data
         * for potential scintillation use cases.
         *
         * @param maxSpectralLines The maximum number of spectral lines that an instance will support.
         * @param epochSize The number of samples that make up an epoch.
         */
        CombGenerator( size_t maxSpectralLines, size_t epochSize );

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
         * It will set the initial scintillated magnitudes if resetParameters specifies a
         * non-zero value for decorrelationSamples. If non-zero, it will invoke the
         * the client provided scintillateFunk for an initial magnitude based on an expected value.
         *
         * @param resetParameters The reset parameters. @see CombGeneratorResetParameters for details.
         * @param pMagVector This argument provides a series of magnitude values, of length resetParameters.numLines.
         * The data pointed to is expected to persist between CombGenerator reset cycles.
         * Passing a null pointer results in default magnitude of 1.0 for all lines.
         * CombGenerator does not make it's own copy and the data may be accessed during getEpoch invocations.
         * @param pPhaseVector This argument provides a series of radian phase values, of length resetParameters.numLines.
         * The data pointed to need not persist between CombGenerator reset cycles.
         * CombGenerator only uses it within the reset call and has no further use for it.
         * Passing a null pointer results in default phase of 0.0 for all lines.
         * @param scintillateFunk Observer interface for obtaining scintillated magnitude values from a client.
         * This may be an empty (null) function object if resetParameters.decorrelationSamples is zero.
         */
        void reset( const CombGeneratorResetParameters & resetParameters,
                    const double * pMagVector, const double * pPhaseVector,
                    const ScintillateFunkType & scintillateFunk );

        /**
         * @brief The Get Epoch Operation
         *
         * This operation returns a pointer to internal buffer space when an epoch's worth of harmonic
         * spectrum, complex time series data, resides. It will invoke the client provided scintillateFunk
         * for magnitude values at the beginning of decorrelation periods, specifying a desired mean
         * (expected value) and a spectral line number hint.
         *
         * @param scintillateFunk Observer interface for obtaining scintillated magnitude values from a client.
         * This may be an empty (null) function object if resetParameters.decorrelationSamples is zero.
         *
         * @return Returns a pointer to an internal buffer where an epoch's worth of harmonic
         * spectrum, complex time series data, resides.
         */
        const ReiserRT::Signal::FlyingPhasorElementBufferTypePtr
            getEpoch( const ScintillateFunkType & scintillateFunk );

    private:
        Imple * pImple;    //!< Pointer to hidden implementation.
    };
}

#endif // #ifndef TSG_NG_COMB_GENERATOR_H

