// Created on 20220108

#ifndef TSG_COMPLEXTONEGEN_COMMANDLINEPARSER_H
#define TSG_COMPLEXTONEGEN_COMMANDLINEPARSER_H

#include <cmath>

class CommandLineParser
{
public:
    CommandLineParser() = default;
    ~CommandLineParser() = default;

    int parseCommandLine( int argc, char * argv[] );

    inline double getSpacingRadsPerSample() const { return spacingRadsPerSampleIn; }
    inline unsigned long getNumLines() const { return numLinesIn; }
    inline unsigned int getProfile() const { return profileIn; }
    inline unsigned long getEpochSize() const { return epochSizeIn; }
    inline unsigned long getDecorrelSamples() const { return decorrelSamplesIn; }
    inline unsigned int getSeed() const { return seedIn; }

private:
    double spacingRadsPerSampleIn{ M_PI / 16 };
    unsigned long numLinesIn{ 10 };
    unsigned long epochSizeIn{ 2048 };
    unsigned long decorrelSamplesIn{ 0 };
    unsigned int profileIn{ 0 };
    unsigned int seedIn{ 0 };
};


#endif //TSG_COMPLEXTONEGEN_COMMANDLINEPARSER_H
