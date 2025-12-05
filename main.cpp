#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <functional>
#include <exception>
#include <algorithm>
#include <cstring>
#include <windows.h>

using namespace std;

struct Compare {
    bool operator()(const int& a, const int& b) const {
        return a > b;
    }
};

class TextFileProcessor {
private:
    multimap<int, int, Compare> linesData;
    
public:
    void readAndStoreFile(const string& inputFilename) {
        ifstream inputFile(inputFilename, ios::binary);
        
        if (!inputFile.is_open()) {
            string errorMsg = "Ошибка открытия файла '" + inputFilename + "': ";
            errorMsg += strerror(errno);
            throw runtime_error(errorMsg);
        }
        
        string currentLine;
        streampos linePosition = inputFile.tellg();
        int lineCounter = 0;
        
        while (getline(inputFile, currentLine)) {
            int lineLength = currentLine.length();
            int position = static_cast<int>(linePosition);
            
            linesData.insert(make_pair(lineLength, position));
            
            linePosition = inputFile.tellg();
            lineCounter++;
        }
        
        inputFile.close();
        
        if (linesData.empty()) {
            throw runtime_error("Файл не содержит данных или произошла ошибка чтения");
        }
    }
    
    void writeFilteredOutput(const string& inputFilename, const string& outputFilename) {
        ifstream inputFile(inputFilename, ios::binary);
        if (!inputFile.is_open()) {
            throw runtime_error("Не удалось повторно открыть файл '" + inputFilename + "' для чтения");
        }
        
        ofstream outputFile(outputFilename);
        if (!outputFile.is_open()) {
            throw runtime_error("Не удалось создать файл '" + outputFilename + "' для записи");
        }
        
        auto rangeBegin = linesData.lower_bound(20);
        auto rangeEnd = linesData.upper_bound(10);
        
        for (auto it = rangeBegin; it != rangeEnd; ++it) {
            int lineLength = it->first;
            int filePosition = it->second;
            
            if (lineLength >= 10 && lineLength <= 20) {
                inputFile.seekg(filePosition);
                
                string lineContent;
                if (getline(inputFile, lineContent)) {
                    outputFile << lineContent << '\n';
                }
            }
        }
        
        inputFile.close();
        outputFile.close();
    }
};

void printUsage(const char* programName) {
    cout << endl;
    cout << "Необходимо ввести: " << programName << " \"входной_файл.txt\" \"выходной_файл.txt\"" << endl;
    }

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    if (argc != 3) {
        cerr << "Ошибка: неверное количество аргументов" << endl;
        printUsage(argv[0]);
        return 1;
    }
    
    string inputFile = argv[1];
    string outputFile = argv[2];
    
    try {
        TextFileProcessor processor;
        processor.readAndStoreFile(inputFile);
        processor.writeFilteredOutput(inputFile, outputFile);
        
    } catch (const exception& e) {
        cerr << "\nОшибка: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
