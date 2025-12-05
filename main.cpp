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

// Компаратор для сортировки по убыванию
struct Compare {
    bool operator()(const int& a, const int& b) const {
        return a > b; // сортировка по убыванию длины строк
    }
};

class TextFileProcessor {
private:
    // Контейнер: ключ - длина строки, значение - позиция в файле
    multimap<int, int, Compare> linesData;
    
public:
    // Чтение файла и заполнение контейнера
    void readAndStoreFile(const string& inputFilename) {
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
            
            // Сохраняем длину строки и её позицию в файле
            linesData.insert(make_pair(lineLength, position));
            
            // Получаем позицию для следующей строки
            linePosition = inputFile.tellg();
        }
        
        inputFile.close();
        
        if (linesData.empty()) {
            throw runtime_error("Файл не содержит данных или произошла ошибка чтения");
        }
    }
    
    // Запись отфильтрованных строк в выходной файл
    void writeFilteredOutput(const string& inputFilename, const string& outputFilename) {
        ifstream inputFile(inputFilename, ios::binary);
        if (!inputFile.is_open()) {
            throw runtime_error("Не удалось повторно открыть файл '" + inputFilename + "' для чтения");
        }
        
        ofstream outputFile(outputFilename);
        if (!outputFile.is_open()) {
            throw runtime_error("Не удалось создать файл '" + outputFilename + "' для записи");
        }
        
        // Используем lower_bound и upper_bound для поиска диапазона строк
        // lower_bound(20) - первая строка с длиной <= 20 (из-за сортировки по убыванию)
        auto rangeBegin = linesData.lower_bound(20);
        
        // upper_bound(10) - первая строка с длиной < 10
        auto rangeEnd = linesData.upper_bound(10);
        
        // Перебираем строки в найденном диапазоне
        for (auto it = rangeBegin; it != rangeEnd; ++it) {
            int lineLength = it->first;      // длина строки
            int filePosition = it->second;   // позиция в файле
            
            // Дополнительная проверка длины (10-20 символов)
            if (lineLength >= 10 && lineLength <= 20) {
                // Переходим к позиции строки в исходном файле
                inputFile.seekg(filePosition);
                
                string lineContent;
                if (getline(inputFile, lineContent)) {
                    // Записываем строку в выходной файл
                    outputFile << lineContent << '\n';
                }
            }
        }
        
        inputFile.close();
        outputFile.close();
    }
};

// Функция для вывода инструкции по использованию
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
        processor.readAndStoreFile(inputFile);       // читаем и обрабатываем файл
        processor.writeFilteredOutput(inputFile, outputFile); // записываем результат
        
    } catch (const exception& e) {
        // Обработка исключений
        cerr << "\nОшибка: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
