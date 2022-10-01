/**
 * @file CombGenerator.h
 * @brief The specification file for the Comb Generator
 * @authors Frank Reiser
 * @date Initiated August 22nd, 2022
 */

#ifndef TSG_NG_COMB_GENERATOR_H
#define TSG_NG_COMB_GENERATOR_H

// Include Export Specification File
#include "CombGeneratorExport.h"

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
     * set up at a specified harmonic spacing. The magnitudes and phases of each tone
     * are to be specified by the client.
     *
     * The Comb Generator also provides support for scintillating each tone produced through
     * a scintillation observer interface. @see reset operation.
     */
    class CombGenerator_EXPORT CombGenerator
    {
    private:
        /**
         * @brief Forward Reference to Hidden Implementation
         */
        class Imple;

    public:
        // The Line number hint is primarily intended for but, not limited to, testing purposes.
        /**
         * @brief The Scintillate Function Type
         *
         * The CombGenerator does not specify particular distribution for scintillating. It relies on the
         * client to provide those details at the beginning of decorrelation periods.
         * We specify a functor that returns a double precision value and accepts for arguments, what the
         * nominal value should be and a spectral line number hint which may be useful to the client.
         * This type is used in the reset and getEpoch operations.
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
         * This constructor allocates a batch of ReiserRT_FlyingPhasor instances for a maximum use case scenario.
         * It also sets up the necessary buffers and logic for potential scintillation use cases.
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
         * This operation prepares the CombGenerator for a new series of getEpoch invocations.
         * It will set the initial scintillated magnitudes if resetParameters specifies a
         * non-zero value for decorrelationSamples. In this case, it will invoke the
         * the client provided scintillateFunk for an initial magnitude based on a nominal value.
         *
         * @param resetParameters The reset parameters. @see CombGeneratorResetParameters for details.
         * @param scintillateFunk Observer interface for obtaining scintillated magnitude values.
         */
        void reset( const CombGeneratorResetParameters & resetParameters, const ScintillateFunkType & scintillateFunk );

        /**
         * @brief The Get Epoch Operation
         *
         * This operation returns a pointer to internal buffer space when an epoch's worth of harmonic
         * spectrum, complex time series data, resides. It will invoke the client provided scintillateFunk
         * for magnitude values at the beginning of decorrelation periods, specifying a desired mean
         * (expected value) and a spectral line number hint.
         *
         * @param scintillateFunk Observer interface for obtaining scintillated magnitude values.
         * @return Returns a pointer to internal buffer space when an epoch's worth of harmonic
         * spectrum, complex time series data, resides.
         */
        ReiserRT::Signal::FlyingPhasorElementBufferTypePtr getEpoch( const ScintillateFunkType & scintillateFunk );

    private:
        Imple * pImple;    //!< Pointer to hidden implementation.
    };
}

#endif // #ifndef TSG_NG_COMB_GENERATOR_H

