#ifndef DUMPER_H
#define DUMPER_H

#include <fstream>

// enumeration type output format
// ALL for -a, BINARY for -1, OCTAL for -2, DECIMAL for -3, HEXADECIMAL for -4
enum OutputFormat {
    ALL,
    BINARY,
    OCTAL,
    DECIMAL,
    HEXADECIMAL
};

extern const std::string RESET;
extern const std::string BLACK;
extern const std::string RED;
extern const std::string GREEN;
extern const std::string YELLOW;
extern const std::string BLUE;
extern const std::string MAGENTA;
extern const std::string CYAN;
extern const std::string WHITE;

void printUsage(const char* programName);
void parseCommandLineArguments(int argc, char* argv[],
    bool& hasInputFile,
    bool& lineShow,
    std::string& inputFilename,
    std::istringstream& inputStringStream,
    bool& hasOutputFile,
    OutputFormat& format,
    bool& color,
    long& linesPerScreen,
    long& startLine,
    long& endLine,
    bool& onlyContent,
    bool& hasLineRange,
    bool& hasHelpFlag,
    bool& isRAW,
    std::ofstream& outputFile);

void processInputFile(std::istream& inputStream,
    bool hasOutputFile,
    bool lineShow,
    OutputFormat format,
    bool color,
    long linesPerScreen,
    long startLine,
    long endLine,
    bool onlyContent,
    bool hasLineRange,
    bool isRAW,
    std::ofstream& outputFile,
    char* argv[]);

#endif
