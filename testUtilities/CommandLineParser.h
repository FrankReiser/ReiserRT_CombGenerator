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
    inline unsigned long getChunkSize() const { return chunkSizeIn; }
    inline unsigned long getNumChunks() const { return numChunksIn; }
    inline unsigned long getSkipChunks() const { return skipChunksIn; }
    inline unsigned long getDecorrelSamples() const { return decorrelSamplesIn; }
    inline unsigned int getSeed() const { return seedIn; }

    enum class StreamFormat : short { Invalid=0, Text32, Text64, Bin32, Bin64 };
    [[nodiscard]] StreamFormat getStreamFormat() const { return streamFormatIn; }

    inline bool getHelpFlag() const { return helpFlagIn; }
    inline bool getIncludeX() const { return includeX_In; }

private:
    double spacingRadsPerSampleIn{ M_PI / 256 };
    unsigned long numHarmonicsIn{ 12 };
    unsigned long chunkSizeIn{ 4096 };
    unsigned long decorrelSamplesIn{ 0 };       // Zero for no scintillation envelope on individual tones
    unsigned long numChunksIn{ 1 };
    unsigned long skipChunksIn{ 0 };
    unsigned int profileIn{ 1 };
    unsigned int seedIn{ 1 };                   // Zero will use std::random_device to initialize.

    bool helpFlagIn{ false };
    bool includeX_In{ false };

    StreamFormat streamFormatIn{ StreamFormat::Text64 };
};

#endif //REISER_RT_COMBGENERATOR_COMMANDLINEPARSER_H
