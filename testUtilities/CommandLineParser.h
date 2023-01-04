// Created on 20230103

#ifndef REISER_RT_COMBGENERATOR_COMMANDLINEPARSER_H
#define REISER_RT_COMBGENERATOR_COMMANDLINEPARSER_H

#include <cmath>

class CommandLineParser
{
public:
    CommandLineParser() = default;
    ~CommandLineParser() = default;

    int parseCommandLine( int argc, char * argv[] );

    inline double getSpacingRadsPerSample() const { return spacingRadsPerSampleIn; }
    inline unsigned long getNumHarmonics() const { return numHarmonicsIn; }
    inline unsigned int getProfile() const { return profileIn; }
    inline unsigned long getEpochSize() const { return epochSizeIn; }
    inline unsigned long getDecorrelSamples() const { return decorrelSamplesIn; }
    inline unsigned int getSeed() const { return seedIn; }

private:
    double spacingRadsPerSampleIn{ M_PI / 16 };
    unsigned long numHarmonicsIn{ 120 };
    unsigned long epochSizeIn{ 2048 };
    unsigned long decorrelSamplesIn{ 0 };       // Zero for no scintillation envelope on individual tones
    unsigned int profileIn{ 0 };
    unsigned int seedIn{ 1 };                   // Zero will use std::random_device to initialize.
};

#endif //REISER_RT_COMBGENERATOR_COMMANDLINEPARSER_H
