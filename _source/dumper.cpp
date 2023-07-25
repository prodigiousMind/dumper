/*

# Author: prodigiousMind
# youtube: https://www.youtube.com/c/prodigiousMind
# github: https://github.com/prodigiousMind/

Dumper is a CLI tool written in CPP to take a quick look into any type of file, it allows to view the binary, decimal, octal, hexadecimal representations of files.
It also serves the purpose for reading a file at once, or at a specific line or range of lines.
Moreover, it allows to write the processed data into an output file for later analysis. 
In short, it serves the main features of commands such as cat, xxd, hexdump, more etcetera.

*/

#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../_headers/headerDUMP.h"

/*

// for standard input
void readCinString(std::string& cinStr) {
    std::getline(std::cin, cinStr, '\0');
}

*/

// print usage by invoking printUsage in dumperFunc with the name of the program
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1; 
    }

    /*
    std::string cinStr;
    std::thread t(readCinString, std::ref(cinStr));
    if (argc < 2) {
        t.join();
        if (cinStr.size()<1){
            printUsage(argv[0]);
            return 1; 
        }
    }
    */

    // if -h flag is passed along with any flag, invoke the help function to print the usage
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-h") {
            printUsage(argv[0]);
            return 0;
        }
    }


    // declaring variable for handling situations later
    bool isRAW;
    bool hasInputFile = false;
    std::string inputFilename;
    std::istringstream inputStringStream;
    std::istream* inputStream = nullptr;

    bool hasOutputFile = false;
    bool lineShow = false;
    OutputFormat format = ALL;
    bool color = false;
    long linesPerScreen = 10;
    long  startLine = 0, endLine = 0;
    bool onlyContent = false, hasLineRange = false, hasHelpFlag = false;
    std::ofstream outputFile;

    // calling parse command line argument function with appropriate function arguments
    parseCommandLineArguments(argc, argv, hasInputFile, lineShow, inputFilename, inputStringStream, hasOutputFile, format, color, linesPerScreen, startLine, endLine, onlyContent, hasLineRange, hasHelpFlag, isRAW, outputFile);

    if (hasHelpFlag) {
        printUsage(argv[0]);
        return 0;
    }

    // opening input file
    std::ifstream inputFile;
    if (hasInputFile) {
        inputFile.open(inputFilename);
        if (!inputFile.is_open()) {
            std::cerr << "Error opening input file\n";
            return 1;
        }
        inputStream = &inputFile;
    } else if (argc > 1 && argv[1][0] != '-') {
        inputStringStream.str(argv[1]);
        inputStream = &inputStringStream;
    } 
    /*
    else if (cinStr.size()>0){
        inputStringStream.str(cinStr);
        inputStream = &inputStringStream;
    }
    */
    else {
        std::cerr << "No input provided\n";
        return 1;
    }

    // function call
    processInputFile(*inputStream, hasOutputFile, lineShow, format, color, linesPerScreen, startLine, endLine, onlyContent, hasLineRange, isRAW, outputFile, argv);

    // if input file is passed
    if (hasInputFile) {
        static_cast<std::ifstream*>(inputStream)->close();
    }
    if(outputFile.is_open()){
        outputFile.close();
    }
    return 0; 
}
