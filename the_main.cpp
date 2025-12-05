#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <functional>
#include <exception>
#include <algorithm>
#include <cstring>

using namespace std;

struct Compare {
    bool operator()(const int& a, const int& b) const {
        return a > b; // сортировка по убыванию длины строк
    }
};

class TextFileProcessor {
private:
    // контейнер: ключ - длина строки, значение - позиция в файле
    multimap<int, int, Compare> linesData;
    
public:
    // читаем файл и заполняем контейнер
    void readFile(const string& inputFilename) {
        ifstream inputFile(inputFilename, ios::binary);
        
        if (!inputFile.is_open()) {
            string errorMsg = "Ошибка открытия файла '" + inputFilename + "': ";
            errorMsg += strerror(errno);
            throw runtime_error(errorMsg);
        }
        
        string currentLine;
        streampos linePosition = inputFile.tellg(); // текущая позиция в файле
        
        while (getline(inputFile, currentLine)) {
            int lineLength = currentLine.length();
            int position = static_cast<int>(linePosition);
            
            linesData.insert(make_pair(lineLength, position)); // сохраняем длину строки и её позицию
            
            linePosition = inputFile.tellg(); // получаем позицию для след.строки
        }
        
        inputFile.close();
        
        if (linesData.empty()) {
            throw runtime_error("Файл не содержит данных или произошла ошибка чтения");
        }
    }
    
    void writeFilter(const string& inputFilename, const string& outputFilename) {
        ifstream inputFile(inputFilename, ios::binary);
        if (!inputFile.is_open()) {
            throw runtime_error("Не удалось повторно открыть файл '" + inputFilename + "' для чтения");
        }
        
        ofstream outputFile(outputFilename);
        if (!outputFile.is_open()) {
            throw runtime_error("Не удалось создать файл '" + outputFilename + "' для записи");
        }
        
        // lower_bound и upper_bound для поиска диапазона строк
        // lower_bound(20) - первая строка с длиной <= 20 (из-за сортировки по убыванию)
        auto rangeBegin = linesData.lower_bound(20);
        
        // upper_bound(10) - первая строка с длиной < 10
        auto rangeEnd = linesData.upper_bound(10);
        
        // перебор строк в найденном диапазоне
        for (auto it = rangeBegin; it != rangeEnd; ++it) {
            int lineLength = it->first;      // длина строки
            int filePosition = it->second;   // позиция в файле
            
            // доп.проверка длины (10-20 символов)
            if (lineLength >= 10 && lineLength <= 20) {
                inputFile.seekg(filePosition);
                
                string lineContent;
                if (getline(inputFile, lineContent)) {
                    // записываем строку в выходной файл
                    outputFile << lineContent << '\n';
                }
            }
        }
        
        inputFile.close();
        outputFile.close();
    }
};

void printUsage(const char* program_name) {
    cout << endl;
    cout << "Необходимо ввести: " << program_name << " \"входной_файл.txt\" \"выходной_файл.txt\"" << endl;
}

int main(int argc, char* argv[]) {


    if (argc != 3) {
        cerr << "Ошибка: неверное количество аргументов" << endl;
        printUsage(argv[0]);
        return 1;
    }
    
    string inputFile = argv[1];
    string outputFile = argv[2];  
    
    try {
        TextFileProcessor processor;
        processor.readFile(inputFile);       // читаем и обрабатываем файл
        processor.writeFilter(inputFile, outputFile); // записываем результат
        
    } catch (const exception& e) {
        // обработка исключений
        cerr << "\nОшибка: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
