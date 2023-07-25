/*

# Author: prodigiousMind
# youtube: https://www.youtube.com/c/prodigiousMind
# github: https://github.com/prodigiousMind/

Dumper is a CLI tool written in CPP to take a quick look into any type of file, it allows to view the binary, decimal, octal, hexadecimal representations of files.
It also serves the purpose for reading a file at once, or at a specific line or range of lines.
Moreover, it allows to write the processed data into an output file for later analysis. 
In short, it serves the main features of commands such as cat, xxd, hexdump, more etcetera.

*/

#include <sstream>

#include <cctype>

#include <iostream>

#include <iomanip>

#include <bitset>

#include <cstring>

#include "../_headers/headerDUMP.h"

// for colored output, stored in constant 
const std::string RESET = "\033[0m";
const std::string BLACK = "\033[30m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[36m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";

// declaration of the function
int checkLinePerScreen(long linesPerScreen,
    long lineCount,
    std::ofstream & outputFile, bool oncePassed);

// show help if less arguments are passed or there is an error in command or -h flag is called in combination with any flags
void printUsage(const char * programName) {
    std::cerr << "Usage: " << programName << " -[I<input filename>] [-O<output filename>] [-l <lines>] [-c] [-h] [-a] [-0] [-1] [-2] [-3] [-4] [-n X|X1,X2] [-oc]\n";
    std::cerr << "\n  -l<number>: Number of lines to output at once [Default 10]\n";
    std::cerr << "  -c: enable colored output\n";
    std::cerr << "  -h: display this help message\n";
    std::cerr << "  -O: specify output file name\n";
    std::cerr << "  -a: print all representations [default]\n";
    std::cerr << "  -0: print content, can be used with -1/-2/-3/-4 representation\n";
    std::cerr << "  -1: print binary representation\n";
    std::cerr << "  -2: print octal representation\n";
    std::cerr << "  -3: print decimal representation\n";
    std::cerr << "  -4: print hexadecimal representation\n";
    std::cerr << "  -n: print only line X or lines X1 to X2\n";
    std::cerr << "  -oc: print only content without representations\n";
}

// to check whether input file is passed (-O) flag or not, so we can make use of string passed as argument for processing
bool hasInp = false;

// this function parse command line arguments, flags, and extract the values (if any) they were passed with
void parseCommandLineArguments(int argc, char * argv[],
    bool & hasInputFile,
    bool & lineShow,
    std::string & inputFilename,
    std::istringstream & inputStringStream,
    bool & hasOutputFile,
    OutputFormat & format,
    bool & color,
    long & linesPerScreen,
    long & startLine,
    long & endLine, 
    bool & onlyContent,
    bool & hasLineRange,
    bool & hasHelpFlag,
    bool & isRAW,
    std::ofstream & outputFile) {
    if (argc > 1 && argv[1][0] != '-') {
        inputStringStream.str(argv[1]);
    }

    // checking what flag were passed and using switch cases extracting their values
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            
            switch (argv[i][1]) {
            
            case 'I':
                if (strlen(argv[i]) > 2) {
                    hasInputFile = true;
                    hasInp =true;
                    inputFilename = argv[i] + 2;
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    hasInputFile = true;
                    inputFilename = argv[++i];
                    hasInp =true;
                } else {
                    // if it does not exists
                    std::cerr << "Error: couldn't find input file\n";
                    exit(1);
                }
                break;
            
            case 'O':
                // for extracting values passed in -O flag with or without spaces next to the flag
                if (strlen(argv[i]) > 2) {
                    hasOutputFile = true;
                    outputFile.open(argv[i] + 2);
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    hasOutputFile = true;
                    outputFile.open(argv[++i]);
                } else {
                    // if it can't be written
                    std::cerr << "Error: couldn't find output file name\n";
                    exit(1);
                }
                if (!outputFile.is_open()) {
                    // if there is error such as permissions and stuff
                    std::cerr << "Error opening output file\n";
                    exit(1);
                }
                break;
            
            case 'c':
                // for enabling colored output
                if (hasOutputFile) {
                    std::cerr << "Error: -c flag cannot be used when writing to a file\n";
                    exit(1);
                }
                color = true;
                break;
            
            case 'l':
                // if line (line per screen is passed)
                if (strlen(argv[i]) > 2) {
                    linesPerScreen = std::stoi(argv[i] + 2);
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    linesPerScreen = std::stoi(argv[++i]);
                } else {
                    std::cerr << "Error: couldn't find value for -l flag\n";
                    exit(1);
                }
                break;

            // 'a' flag for output in all (binary, hex, decimal, octal)
            // '0' flag for content of the file, can be used with -1,-2,-3,-4
            // '1', '2', '3', '4' for binary, octal, decimal, hexadecimal representation only 
            case 'a':
                format = ALL;
                break;
            
            case '0':
                isRAW = true;
                break;
            
            case '1':
                format = BINARY;
                break;
            
            case '2':
                format = OCTAL;
                break;
            
            case '3':
                format = DECIMAL;
                break;
            
            case '4':
                format = HEXADECIMAL;
                break;

            // if -n flag is passed for showing specific line(s)
            case 'n':
                
                // if it is passed, boolean hasLineRange becomes true
                // the below code extract the value, it can be in the form of -n <number> or -n <n1, n2>.
                // for -n <number>, only "number" line will be output, otherwise from n1 to n2
                hasLineRange = true;
                char * value;
                
                if (strlen(argv[i]) > 2) {
                    value = argv[i] + 2;
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    value = argv[++i];
                } else {
                    std::cerr << "Error: couldn't find value for -n flag\n";
                    exit(1);
                }
                
                // extracting startLine and endLine
                if (strchr(value, ',')) {
                    startLine = std::stoi(value);
                    endLine = std::stoi(strchr(value, ',') + 1);
                } else {
                    startLine = endLine = std::stoi(value);
                }
                break;
            
            // if -o flag is passed and the second "-o" after this is 'c', it means, it is -oc flag, specifying that only content needs to be output
            case 'o':
                if (argv[i][2] == 'c') {
                    onlyContent = true;
                }
                break;
            
            // for showing line numbers while outputting the processed data
            case 's':
                lineShow = true;
                break;
            
            // invoke help menu
            case 'h':
                hasHelpFlag = true;
            }
       
        } else if (!hasInputFile) {
            inputStringStream.str(argv[i]);
        }
    }

}

// this function process stuff based on the inputFile, or string passed as argument
void processInputFile(std::istream & inputStream,
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
    std::ofstream & outputFile,
    char * argv[]) {
  
    // long long type for large files, even though it is quite a big value
    // lineCount holds the lines to count
    // lineNo holds the value of current processed line, at beginning both are 0
   
    long long lineCount = 0;
    long long lineNo = 0;
    long long lineNos = 0;
    int what = 1;
    bool oncePassed = false; // it becomes true once one Lines per screen are printed
    
    // if line has range (-n present with value) and startLine is equal to endLine (only one value or equal values passed)
    if (hasLineRange && startLine==endLine){lineNo = (long long) startLine - 1;startLine--;endLine--;}
    if (hasLineRange && startLine < endLine){lineNo = --startLine;endLine--;}
   
    // string type holds the content of the line
    std::string line;

    // read while there is a line
    while (std::getline(inputStream, line)) {
        ++lineNos; // keep tracks of line whether lineShow enabled or not

        // if (-n has value and lineCount < startLine or lineCount > endLine) then increase lineCount
        if (hasLineRange && !oncePassed && (lineCount < startLine || lineCount > endLine)) {
            lineCount++;
            continue;
        }

        // if output file  is passed then out will write to file, else out will work as cout
        std::ostream & out = outputFile.is_open() ? outputFile : std::cout;

        // if lineShow is enabled, then print the decimal value of line number
        if (lineShow) out << std::dec << ++lineNo << " ";

        // if onlyContent is present
        if (onlyContent) {
            out << line << '\n';
        } else {

            // iterating through each character of a line in pairs of 4 at one time
            for (int i = 0; i < line.size(); i += 6) {

                // holds the respective representations
                std::string decimalT, octalT, hexT, binT, cT;

                // if lineShow is enabled and i !=  0, (not a first character of the line) as there is already a space for it
                if (lineShow && i != 0) {
                    std::string longString = std::to_string(lineNo);
                    for (int x = 0; x < longString.size() + 1; x++) out << " ";
                }

                // iterating though each character in a group of 4 if there exists such
                for (int j = i; j < i + 6 && j < line.size(); j++) {

                    // char c for holding the character at a specific index 'j'
                    char c = line[j];

                    // tmp is stringstream object for holding decimal,octal,hex values of a group
                    std::ostringstream tmp;
                    int value = static_cast < int > (c);

                    // making it 8 bit and appending the last values of the group
                    auto binaryValue = std::bitset < 8 > (value).to_string();
                    binT += binaryValue;
                    
                    // same this but with binary, here we calculated decimal from binary using stoi() and base 2
                    int decimalValue = std::stoi(binaryValue, nullptr, 2);
                    tmp << std::dec << std::setw(3) << std::setfill('0') << decimalValue;
                   
                    // the value is appended to decimalT from tmp
                    decimalT += tmp.str();
                   
                    // clearing tmp to store octal value now
                    tmp.str("");
                    tmp.clear();
                    tmp << std::oct << std::setw(3) << std::setfill('0') << decimalValue;
                    octalT += tmp.str();
                    
                    // clearing tmp to store hex value now
                    tmp.str("");
                    tmp.clear();
                   
                    tmp << std::hex << std::setw(2) << std::setfill('0') << decimalValue;
                    hexT += tmp.str();
                    tmp.str("");
                    tmp.clear();
                   
                    // if the character is of non-printable, then show it using '.' such as '\n' to avoid insertion of new line in output
                    if (!isprint(c) || c == '\n' || c==' ') c = '.';
                   
                    tmp << std::setw(1) << std::setfill('.') << c;
                    cT += tmp.str();


                    // add color per octet
                    if ((j==(i) || j==(i+2) || j==(i+4) || j==(i+6)) && color){
                        binT += YELLOW;
                        decimalT += YELLOW;
                        octalT += YELLOW;
                        hexT += YELLOW;
                        cT += YELLOW;
                    }else if ((j==(i+1) || j==(i+3) || j==(i+5)) && color){
                        binT += BLUE;
                        decimalT += BLUE;
                        octalT += BLUE;
                        hexT += BLUE;
                        cT += BLUE;                  
                    }
                    
                }
                
                // if line show is not enabled, then 6 characters will make up one line (not of the file but of the terminal), its for files having large number
                // of characters per line
                if (!lineShow){
                    lineCount++;
                    what = checkLinePerScreen(linesPerScreen, lineCount, outputFile, oncePassed);
                    if (what == 0) return;
                    else what = 1;
                }

                // adding spaces in case binary is less than 32 such as binary representation only of 3,2,1 character
                // this will keep the format as it is and avoid distortion
                while (!color && binT.size() < 48) {
                    binT += "  ";
                }
                                
                // if hex is smaller than 8 in size, appending space at last just like the above
                while (!color && hexT.size() < 12) {
                    hexT += " ";
                }
               
                // if decimal is smaller than 9 (max 255 for 8 bit (3 digits), for 4 characters perline, it is 12 digits) in size, appending space at last just like the above
                while (!color && decimalT.size() < 18) {
                    decimalT += " ";
                }

                // same as decimal (3 digit max octal for 8 bit binary)
                while (!color && octalT.size() < 18) {
                    octalT += " ";
                }

                if (color){
                    //bcs of adding color codes
                    int sB=78;
                    if (binT.size() == 13) sB = 53;
                    if (binT.size() == 26) sB = 32+26;
                    if (binT.size() == 39) sB = 24+39;
                    if (binT.size() == 52) sB = 52+16;
                    if (binT.size() == 65) sB = 8+65;
                    while (color && binT.size() < sB) binT += " ";
                    int sH = 42;
                    if (hexT.size() == 7) sH = 17;
                    if (hexT.size() == 14) sH = 8+14;
                    if (hexT.size() == 21) sH = 6 + 21;
                    if (hexT.size() == 28) sH = 4 + 28;
                    if (hexT.size() == 35) sH = 2 + 35;
                    while (color && hexT.size() < sH) hexT += " ";
                    int sD = 48;
                    if (decimalT.size() == 8) sD = 8+15;
                    if (decimalT.size() == 16) sD = 16+12;
                    if (decimalT.size() == 24) sD = 24+9;
                    if (decimalT.size() == 32) sD = 32+6;
                    if (decimalT.size() == 40) sD = 43;
                    while (color && decimalT.size() < sD) decimalT += " ";
                    while (color && octalT.size() < sD) octalT += " ";

                }

                // this is nothing but added to show the output in different colors (same for each character and its respective binary, octal, decimal, hex representation)
                /*
                    if (j==i && color){
                        binT += GREEN;
                        decimalT += GREEN;
                        octalT += GREEN;
                        hexT += GREEN;
                        cT += GREEN;
                    }else if (j==(i+1) && color){
                        binT += YELLOW;
                        decimalT += YELLOW;
                        octalT += YELLOW;
                        hexT += YELLOW;  
                        cT += YELLOW;                      
                    }else if (j==(i+2) && color){
                        binT += MAGENTA;
                        decimalT += MAGENTA;
                        octalT += MAGENTA;
                        hexT += MAGENTA;
                        cT += MAGENTA;
                    }else if(j==(i+3) && color){
                        binT += BLUE;
                        decimalT += BLUE;
                        octalT += BLUE;
                        hexT += BLUE; 
                        cT += BLUE
                    }
                    else if(j==(i+4) && color){
                        binT += WHITE;
                        decimalT += WHITE;
                        octalT += WHITE;
                        hexT += WHITE; 
                        cT +=  WHITE;
                    }
                    */

                // if input and output file are specified, then write to the file
                if (hasOutputFile && hasInp){
                    if (onlyContent) out << cT;
                    else{

                        // use of switch case to check what to write
                        // here no colors are added as we are writing into output file
                        switch (format) {
                     
                        case ALL:
                            out << std::left << std::setw(48);
                            out << binT <<  " ";
                            out << std::right << std::setw(12);
                            out << std::hex << hexT << " ";
                            out << std::right << std::setw(18);
                            out << decimalT << " ";
                            out << std::right << std::setw(18);
                            out << octalT << " ";
                            out << std::right << std::setw(6);
                            out << cT << "\n";
                            break;
                      
                        case BINARY:
                            out << std::left << std::setw(48);
                            out << binT << RESET << " ";
                            out << std::right << std::setw(6);
                            if (isRAW) out << cT;
                            out << "\n";
                            break;
                      
                        case OCTAL:
                            out << std::left << std::setw(18);
                            out <<  octalT << RESET << " ";
                            out << std::right << std::setw(6);
                            if (isRAW) out << cT;
                            out << "\n";
                            break;
                        
                        case DECIMAL:
                            out << std::left << std::setw(18);
                            out <<  decimalT << RESET << " ";
                            out << std::right << std::setw(6);
                            if (isRAW) out << cT;
                            out << "\n";
                            break;
                        
                        case HEXADECIMAL:
                            out << std::left << std::setw(12);
                            out <<  octalT << RESET << " ";
                            out << std::right << std::setw(6);
                            if (isRAW) out << cT;
                            out << "\n";
                            break;
                        }
                    }

                    // could have been worked even without if-else, as we are doing nothing much if output file is present in the below code
                    // but done so, in case, we implement new thing in future.

                }else if (!hasOutputFile){
                   
                    // if no output file is passed, then output to the standard output with colors (if '-c' enabled)
                    switch (format) {
                   
                    // for '-a' flag
                    case ALL:
                        out << std::left << std::setw(48);

                        //the below commented lines are for coloring binary, hex, octal, decimal characters with different colors not per octet
                        //if (color) out << GREEN;
                        out << binT << RESET << " ";
                        out << std::right << std::setw(12);
                       // if (color) out << YELLOW;
                        //  out<<std::hex<<std::setw(2)<<std::setfill(' ')<<hexT<<RESET<<" "; 
                        out << std::hex << hexT << RESET << " ";
                        out << std::right << std::setw(18);
                       // if (color) out << MAGENTA;
                        out << decimalT << RESET << " ";
                        out << std::right << std::setw(18);
                       // if (color) out << BLUE;
                        out << octalT << RESET << " ";
                        out << std::right << std::setw(6);
                       // if (color) out << WHITE;
                        out << cT << RESET << "\n";
                        break;
                    
                    // for -1 flag
                    case BINARY:
                      //  if (color) out << GREEN;
                        out << std::left << std::setw(48);
                        out << binT << RESET << "    ";
                        out << std::right << std::setw(6);
                        if (isRAW) out << cT;
                        out << "\n";
                        break;

                    // for -2 flag
                    case OCTAL:
                       // if (color) out << BLUE;
                        out << std::left << std::setw(18);
                        out << octalT << RESET << "    ";
                        out << std::right << std::setw(6);
                        if (isRAW) out << cT;
                        out << "\n";
                        break;
                    
                    // for -3 flag
                    case DECIMAL:
                       // if (color) out << MAGENTA;
                        out << std::left << std::setw(18);
                        out << decimalT << RESET << "    ";
                        out << std::right << std::setw(6);
                        if (isRAW) out << cT;
                        out << "\n";
                        break;
                    
                    // for -4 flag
                    case HEXADECIMAL:
                       // if (color) out << YELLOW;
                        out << std::left << std::setw(12);
                        out << hexT << RESET << "    ";
                        out << std::right << std::setw(6);
                        if (isRAW) out << cT;
                        out << "\n";
                        break;
                    }
                }
            }
        }

        
        // to get the decimal of lineNos
        out << std::dec <<RESET;

        // if range of line has been printed then break out
        if (hasLineRange && (lineNos==endLine+1 || lineNos > endLine+1)) break;


        // line Count is increased after reading a line
        if (lineShow){
            lineCount++;
            what = checkLinePerScreen(linesPerScreen, lineCount, outputFile, oncePassed);
            if (what == 0) return;
            else what = 1; 
        }

    }
}

int checkLinePerScreen(long linesPerScreen,
    long lineCount,
    std::ofstream & outputFile, bool oncePassed){
    // if output file is not opened, the output is writing to the standard output then to avoid filling the terminal with data print only l number of lines if passed,
    // otherwise, only 10 lines per screen, after printing if user wants to print next 'l' number of lines, let them press enter, or else if 'q' is pressed exit the program.
    // here press enter does not specific to pressing only enter but any key
    if (!outputFile.is_open() && lineCount >= linesPerScreen) {
        
        lineCount = 0;
        oncePassed = true;
        std::cout << "Press any key to continue (q to exit)...";
        char ch = std::cin.get();
        //std::cout.flush();
        //std::cin.ignore();

        // this will put the curson in the above line at the first character and will erase whole (press continue...) line from the output
        std::cout << "\033[A\033[2K\r" << std::flush;
        if (ch == 'q') {
            return 0;
        }
    }
    return 1;
}
