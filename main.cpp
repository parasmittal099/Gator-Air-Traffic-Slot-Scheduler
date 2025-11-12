#include "Scheduler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Parse command line and execute scheduler operation
void parseAndExecute(const std::string& line, Scheduler& scheduler, 
                     std::vector<std::string>& output) {
    size_t openParen = line.find('(');
    if (openParen == std::string::npos) return;
    
    std::string command = line.substr(0, openParen);
    size_t closeParen = line.find(')');
    std::string argsStr = line.substr(openParen + 1, closeParen - openParen - 1);
    
    std::vector<int> args;
    std::stringstream ss(argsStr);
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        if (!token.empty()) {
            args.push_back(std::stoi(token));
        }
    }
    
    if (command == "Initialize") {
        if (args.size() >= 1) {
            scheduler.initialize(args[0], output);
        }
    } else if (command == "SubmitFlight") {
        if (args.size() >= 5) {
            scheduler.submitFlight(args[0], args[1], args[2], args[3], args[4], output);
        }
    } else if (command == "CancelFlight") {
        if (args.size() >= 2) {
            scheduler.cancelFlight(args[0], args[1], output);
        }
    } else if (command == "Reprioritize") {
        if (args.size() >= 3) {
            scheduler.reprioritize(args[0], args[1], args[2], output);
        }
    } else if (command == "AddRunways") {
        if (args.size() >= 2) {
            scheduler.addRunways(args[0], args[1], output);
        }
    } else if (command == "GroundHold") {
        if (args.size() >= 3) {
            scheduler.groundHold(args[0], args[1], args[2], output);
        }
    } else if (command == "PrintActive") {
        scheduler.printActive(output);
    } else if (command == "PrintSchedule") {
        if (args.size() >= 2) {
            scheduler.printSchedule(args[0], args[1], output);
        }
    } else if (command == "Tick") {
        if (args.size() >= 1) {
            scheduler.tick(args[0], output);
        }
    } else if (command == "Quit") {
        output.push_back("Program Terminated!!");
    }
}

// Main entry: read input, execute commands, write output
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    
    std::string inputFilename = argv[1];
    std::string outputFilename = inputFilename;
    
    size_t dotPos = outputFilename.find_last_of('.');
    if (dotPos != std::string::npos) {
        outputFilename = outputFilename.substr(0, dotPos);
    }
    outputFilename += "_output_file.txt";
    
    std::ifstream inputFile(inputFilename);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file: " << inputFilename << std::endl;
        return 1;
    }
    
    Scheduler scheduler;
    std::vector<std::string> output;
    std::string line;
    
    while (std::getline(inputFile, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) continue;
        
        parseAndExecute(line, scheduler, output);
        
        if (line.find("Quit") != std::string::npos) {
            break;
        }
    }
    
    inputFile.close();
    
    std::ofstream outputFile(outputFilename);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening output file: " << outputFilename << std::endl;
        return 1;
    }
    
    for (const auto& str : output) {
        outputFile << str << std::endl;
    }
    
    outputFile.close();
    
    return 0;
}