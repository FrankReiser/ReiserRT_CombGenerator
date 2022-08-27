// Created on 20220108

#ifndef TSG_COMPLEXTONEGEN_COMMANDLINEPARSER_H
#define TSG_COMPLEXTONEGEN_COMMANDLINEPARSER_H

class CommandLineParser
{
public:
    CommandLineParser() = default;
    ~CommandLineParser() = default;

    int parseCommandLine( int argc, char * argv[] );

    inline double getSpacingRadsPerSample() const { return spacingRadsPerSampleIn; }
    inline unsigned long getNumLines() const { return numLinesIn; }
    inline unsigned long getProfile() const { return profileIn; }

private:
    double spacingRadsPerSampleIn{ 1.0 };
    unsigned long numLinesIn{ 0 };
    unsigned long profileIn{ 0 };
    unsigned short seedIn{ 0 };
};


#endif //TSG_COMPLEXTONEGEN_COMMANDLINEPARSER_H
