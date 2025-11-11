#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Scheduler.cpp"

using namespace std;

void parseAndExecute(const string& line, Scheduler& scheduler) {
    if (line.empty() || line[0] == '#') return;
    
    size_t openParen = line.find('(');
    if (openParen == string::npos) return;
    
    string command = line.substr(0, openParen);
    size_t closeParen = line.find(')');
    string paramStr = line.substr(openParen + 1, closeParen - openParen - 1);
    
    vector<int> params;
    stringstream ss(paramStr);
    string param;
    
    while (getline(ss, param, ',')) {
        param.erase(0, param.find_first_not_of(" \t"));
        param.erase(param.find_last_not_of(" \t") + 1);
        if (!param.empty()) {
            params.push_back(stoi(param));
        }
    }
    
    if (command == "Initialize" && params.size() >= 1) {
        scheduler.Initialize(params[0]);
    }
    else if (command == "SubmitFlight" && params.size() >= 5) {
        scheduler.SubmitFlight(params[0], params[1], params[2], params[3], params[4]);
    }
    else if (command == "CancelFlight" && params.size() >= 2) {
        scheduler.CancelFlight(params[0], params[1]);
    }
    else if (command == "Reprioritize" && params.size() >= 3) {
        scheduler.Reprioritize(params[0], params[1], params[2]);
    }
    else if (command == "AddRunways" && params.size() >= 2) {
        scheduler.AddRunways(params[0], params[1]);
    }
    else if (command == "GroundHold" && params.size() >= 3) {
        scheduler.GroundHold(params[0], params[1], params[2]);
    }
    else if (command == "PrintActive") {
        scheduler.PrintActive();
    }
    else if (command == "PrintSchedule" && params.size() >= 2) {
        scheduler.PrintSchedule(params[0], params[1]);
    }
    else if (command == "Tick" && params.size() >= 1) {
        scheduler.Tick(params[0]);
    }
    else if (command == "Quit") {
        scheduler.Quit();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_filename>" << endl;
        return 1;
    }
    
    string inputFilename = argv[1];
    string outputFilename;
    size_t dotPos = inputFilename.find_last_of('.');
    if (dotPos != string::npos) {
        outputFilename = inputFilename.substr(0, dotPos) + "_output_file.txt";
    } else {
        outputFilename = inputFilename + "_output_file.txt";
    }
    
    ifstream inputFile(inputFilename);
    if (!inputFile.is_open()) {
        cerr << "Error: Cannot open input file " << inputFilename << endl;
        return 1;
    }
    
    ofstream outputFile(outputFilename);
    if (!outputFile.is_open()) {
        cerr << "Error: Cannot create output file " << outputFilename << endl;
        return 1;
    }
    
    Scheduler scheduler(outputFile);
    
    string line;
    while (getline(inputFile, line)) {
        parseAndExecute(line, scheduler);
        if (line.find("Quit") != string::npos) {
            break;
        }
    }
    
    inputFile.close();
    outputFile.close();
    
    return 0;
}