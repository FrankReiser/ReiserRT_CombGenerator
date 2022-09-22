//
// Created by frank on 9/21/22.
//

#include "SubSeedGenerator.h"
#include "CommandLineParser.h"
#include "MiscTestUtilities.h"

#include <iostream>
#include <vector>
#include <limits>
#include <cstring>
#include <random>

// Returns expected values for each bin of our sub-seed distribution. Basically this just divides
// the number of samples by the number of bins, the result of which is how many we expect in each bin.
// Because the sub-seed distribution is 'uniform', we expect the same division in each bin.
// We need not know the bin zero offset nor full range. We just need the number of bins
// and the sample size.
static BinBufferRealType getUniformExpectedValues( size_t theNumBins, size_t theSampleSize )
{
    auto expectedValuesBuffer = BinBufferRealType{new double[theNumBins] };
    const double expectedValue = static_cast< double >( theSampleSize ) / theNumBins;
    for (size_t zbBin=0; theNumBins != zbBin; ++zbBin )
    {
        expectedValuesBuffer[zbBin] = expectedValue;
    }

    return std::move( expectedValuesBuffer );
}

// During development, we may need to diagnose distribution anomalies.
// The first argument is the distribution observed occurrences buffer, the second
// argument is the number of bins and the third argument is its chiSquared "fit test" result.
using DistributionDiagnosticFunkType = std::function< void( const BinBufferIntType &, size_t, double ) >;

// Returns the Chi-squared Result of a sub-seed generator distribution test run. We will likely run
// many to obtain an average level of confidence that the distribution is good.
// This function basically invokes the sub-seed generator sample size times and then
// categorizes the results into N bins. The bins are then compared against the expected values for
// a Chi-squared result.
// The Diagnostic function allows the developer to find problems within the implementation
// if testing indicates problems.
static double runSubSeedGeneratorDistributionTest( TSG_NG::SubSeedGenerator & subSeedGenerator,
                                             const BinBufferRealType & expectedValues,
                                             size_t theNumBins, size_t theSampleSize,
                                             const DistributionDiagnosticFunkType & diagnosticFunk )
{
    auto uniformRndFunc = [ & subSeedGenerator ](){ return subSeedGenerator.getSubSeed(); };
    auto observedValues = categorizeIntoBins(theNumBins, theSampleSize,
    std::ref( uniformRndFunc ), std::numeric_limits< uint32_t>::min(), std::numeric_limits< uint32_t>::max() );
    auto chiSquaredRes = chiSquared(theNumBins, observedValues, expectedValues );

    diagnosticFunk(observedValues, theNumBins, chiSquaredRes );

    return chiSquaredRes;
}

int testSubSeedGeneratorDistribution( TSG_NG::SubSeedGenerator & subSeedGenerator )
{
    const size_t sampleSize = 10000;
    const size_t numBins = 21;

    // Expected values do not change run from run, so fetch them once.
    auto expectedValues = getUniformExpectedValues(numBins, sampleSize );

    unsigned diagnose = 4;
    auto chiSquaredFunk = [&]()
    {
        auto distributionDiagnosticFunk = [&diagnose]( const BinBufferIntType & observedValues,
                                                       size_t theNumBins, double chiSquaredRes )
        {
            if ( 0 < diagnose )
            {
                plotDistribution(observedValues, numBins, chiSquaredRes, std::numeric_limits< uint32_t>::min(),
                                 std::numeric_limits< uint32_t>::max() );
                --diagnose;
            }
        };

        return runSubSeedGeneratorDistributionTest(subSeedGenerator, expectedValues,
                       numBins, sampleSize, std::ref( distributionDiagnosticFunk ) );
    };

    // Running the above test only once may lead to occasional failure indications due to the nature of random number
    // generators. Perfect fits are bad and occasionally poor fits are acceptable. What we will do is run
    // many Chi-squared tests, sort error indications into bins of their own and then look at the Chi-squared error
    // cumulative distribution. It's almost like a Chi-squared of a Chi-Squared but, not exactly. Also, the thresholds we
    // use are typically derived from a table that requires the number of bins and number of degrees of freedom.
    // The minimum expected error energy is 0.0 (unlikely)
    // and the maximum expected error energy is 42.0. Larger values will be discarded which may
    // lead to failure.
    auto chiSquaredObservations = categorizeIntoBins( numBins, sampleSize,
                                           std::ref( chiSquaredFunk ), 0.0, 42.0 );

    // Now determine the average cumulative distribution bin over bin. This is a Cumulative Distribution Analysis.
    // After we do an integration pass, our last bin should indicate near 100% of the observations have
    // occurred.
    auto cumulativeBinBuf = BinBufferRealType{ new double[numBins] };
    std::memset( cumulativeBinBuf.get(), 0, sizeof( double ) * numBins );
    double cumulative = 0.0;
    for ( size_t i = 0; numBins != i; ++i )
    {
        cumulative += double( chiSquaredObservations[i] ) / double( sampleSize );
#if 0
        std::cout << "\tBin: " << i << ", Observed: " << observedBinBuf[i]
                  << ", Cumulative: " << cumulative
                  << std::endl;
#endif
        cumulativeBinBuf[ i ] = cumulative;
    }

    // *****************************************************************
    // **** From here, we bail on first failure returning error code ****
    // *****************************************************************

    // Chi-Squared results that are too low in value indicate non-random as we do not expect perfection
    // from a random number generator distribution. Therefore, we do not expect many "observed" low values.
    // We ranged our "observed" bin buffers from 0.0 to 42.0 over 21 bins (2.0 per bin).
    // Our cumulative bin buffer, bin 3, contains the proportion of "observed" Chi-Squared
    // results that were less than 8.0. Some occurrences are to be expected, but many occurrences
    // would appear too good to be true.
    double expected = 0.015;
    if ( expected < cumulativeBinBuf[ 3 ] )
    {
        std::cout << "Failed low Chi-Squared Results. Detected cumulativeBinBuf[ 3 ] of "
                  << cumulativeBinBuf[ 3 ] << ", expected less than " << expected << std::endl;
        return 11;
    }

    // We expect at least 63% of our Chi-Squared observations to be less than 22.0.
    // Our cumulative bin buffer, bin 10, contains the proportion of "observed" Chi-Squared
    // results that were less than 22.0
    expected = 0.63;
    if ( expected >= cumulativeBinBuf[ 10 ] )
    {
        std::cout << "Failed mid Chi-Squared Results. Detected cumulativeBinBuf[ 10 ] of "
                  << cumulativeBinBuf[ 10 ] << ", expected at least " << expected << std::endl;
        return 12;
    }

    // We expect at least 91% of our Chi-Squared observations to be less than 30.0.
    // Our cumulative bin buffer, bin 14, contains the proportion of "observed" Chi-Squared
    // results that were less than 30.0
    expected = 0.91;
    if ( expected >= cumulativeBinBuf[ 14 ] )
    {
        std::cout << "Failed upper Chi-Squared Results. Detected cumulativeBinBuf[ 14 ] of "
                  << cumulativeBinBuf[ 14 ] << ", expected at least " << expected << std::endl;
        return 13;
    }

    // We expect 99% of our Chi-Squared observations to be less than 42.0.
    // Our cumulative bin buffer, bin 20, contains the proportion of "observed"  Chi-Squared
    // results that are less than 42.0
    expected = 0.99;
    if ( expected >= cumulativeBinBuf[ 20 ] )
    {
        std::cout << "Failed max Chi-Squared Results. Detected cumulativeBinBuf[ 20 ] of "
                  << cumulativeBinBuf[ 20 ] << ", expected at least " << expected << std::endl;
        return 14;
    }

    return 0;
}

int main( int argc, char * argv[] )
{
    // Parse potential command line. Defaults provided otherwise.
    CommandLineParser cmdLineParser{};

    if ( 0 != cmdLineParser.parseCommandLine(argc, argv) )
    {
        std::cout << "Failed parsing command line" << std::endl;
        std::cout << "Optional Arguments are:" << std::endl;
        std::cout << "\t--seed=<uint32_t>: The seed to be used." << std::endl;

        exit( -1 );
    }
#if 1
    else
    {
        std::cout << "Parsed: --seed=" << cmdLineParser.getSeed() << std::endl << std::endl;
    }
#endif

    // Instantiate SubSeedGenerator
    TSG_NG::SubSeedGenerator subSeedGenerator{};

    // Seed the generator and obtain a set of values. Then reseed the generator with original seed and verify
    // that it produces the same sequence (Predictability Test)
    std::vector< uint32_t > randValues{};
    randValues.reserve( 5 );
    subSeedGenerator.reset( cmdLineParser.getSeed() );
    for ( int i = 0; 12 != i; ++i )
    {
        randValues.push_back( subSeedGenerator.getSubSeed() );
    }
    subSeedGenerator.reset( cmdLineParser.getSeed() );
    for ( const auto s : randValues )
    {
        if ( s != subSeedGenerator.getSubSeed() )
        {
            std::cout << "SubSeedGenerator FAILED Predictability Test!" << std::endl;
            return 1;
        }
    }

    // Verify that the sequence of sub-seeds produced is completely different than that of
    // the mt19937 engine utilized with an identical distribution. We use mt19937 as the engine
    // type for all of our other random number facilities. We will use the sub-seed generator
    // under test to provide seeds for all mt19937 instances.
    std::mt19937 mtEngine{ randValues[0] };     // Seed with the first sub-seed we obtained initially.
    std::uniform_int_distribution< uint32_t> mtEngineUniformDistribution{}; // Same as used internally by sub-seed generator.


    ///@todo Finish this.



    return testSubSeedGeneratorDistribution( subSeedGenerator );
}
