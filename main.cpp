#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <functional>
#include <exception>
#include <algorithm>
#include <cstring> // для strerror
#include <locale>

using namespace std;

// Структура для сравнения ключей - сортировка по убыванию
struct Compare {
    bool operator()(const int& a, const int& b) const {
        return a > b; // true если a должен идти перед b (убывающий порядок)
    }
};

class TextFileProcessor {
private:
    // Контейнер multimap: ключ - длина строки, значение - позиция в файле
    multimap<int, int, Compare> linesData;
    
public:
    // Метод для чтения файла и заполнения контейнера
    void readAndStoreFile(const string& inputFilename) {
        // Открываем файл в бинарном режиме для точного определения позиций
        ifstream inputFile(inputFilename, ios::binary);
        
        // Проверка успешности открытия файла
        if (!inputFile.is_open()) {
            string errorMsg = "Ошибка открытия файла '" + inputFilename + "': ";
            errorMsg += strerror(errno); // получаем текстовое описание ошибки
            throw runtime_error(errorMsg);
        }
        
        cout << "Открыт файл: " << inputFilename << endl;
        
        string currentLine;
        // Получаем начальную позицию (0 - начало файла)
        streampos linePosition = inputFile.tellg();
        int lineCounter = 0;
        
        // Построчное чтение всего файла
        while (getline(inputFile, currentLine)) {
            int lineLength = currentLine.length();
            int position = static_cast<int>(linePosition);
            
            // Вставляем пару (длина строки, позиция в файле) в контейнер
            // insert() автоматически размещает элемент согласно компаратору Compare
            linesData.insert(make_pair(lineLength, position));
            
            // Получаем позицию для следующей строки
            linePosition = inputFile.tellg();
            lineCounter++;
            
            // Вывод прогресса для больших файлов
            if (lineCounter % 10000 == 0) {
                cout << "Обработано строк: " << lineCounter << endl;
            }
        }
        
        inputFile.close();
        
        if (linesData.empty()) {
            throw runtime_error("Файл не содержит данных или произошла ошибка чтения");
        }
        
        cout << "Файл прочитан. Всего строк: " << lineCounter << endl;
        cout << "Записано в контейнер: " << linesData.size() << " записей" << endl;
    }
    
    // Метод для записи отфильтрованных строк в выходной файл
    void writeFilteredOutput(const string& inputFilename, const string& outputFilename) {
        // Открываем исходный файл для чтения конкретных строк
        ifstream inputFile(inputFilename, ios::binary);
        if (!inputFile.is_open()) {
            throw runtime_error("Не удалось повторно открыть файл '" + inputFilename + "' для чтения");
        }
        
        // Создаем/открываем выходной файл для записи
        ofstream outputFile(outputFilename);
        if (!outputFile.is_open()) {
            throw runtime_error("Не удалось создать/открыть файл '" + outputFilename + "' для записи");
        }
        
        cout << "\nФильтрация строк длиной от 10 до 20 символов..." << endl;
        
        // Используем lower_bound и upper_bound для эффективного поиска диапазона
        // lower_bound(20) - первая строка с длиной <= 20 (из-за сортировки по убыванию)
        auto rangeBegin = linesData.lower_bound(20);
        
        // upper_bound(10) - первая строка с длиной < 10
        auto rangeEnd = linesData.upper_bound(10);
        
        cout << "Диапазон поиска определен." << endl;
        cout << "Начинаем с длины: " << (rangeBegin != linesData.end() ? to_string(rangeBegin->first) : "не найдено") << endl;
        
        int writtenLines = 0;
        int checkedLines = 0;
        
        // Перебираем все строки в найденном диапазоне
        for (auto it = rangeBegin; it != rangeEnd; ++it) {
            checkedLines++;
            int lineLength = it->first;
            int filePosition = it->second;
            
            // Дополнительная проверка длины (на всякий случай)
            if (lineLength >= 10 && lineLength <= 20) {
                // Перемещаемся к нужной позиции в исходном файле
                inputFile.seekg(filePosition);
                
                string lineContent;
                if (getline(inputFile, lineContent)) {
                    outputFile << lineContent << '\n';
                    writtenLines++;
                    
                    // Вывод прогресса
                    if (writtenLines % 1000 == 0) {
                        cout << "Записано строк: " << writtenLines << endl;
                    }
                }
            }
        }
        
        inputFile.close();
        outputFile.close();
        
        cout << "\nРезультат фильтрации:" << endl;
        cout << "Проверено строк в диапазоне: " << checkedLines << endl;
        cout << "Записано подходящих строк: " << writtenLines << endl;
        cout << "Выходной файл создан: " << outputFilename << endl;
    }
    
    // Метод для вывода статистики
    void displayStatistics() const {
        if (linesData.empty()) {
            cout << "Контейнер пуст!" << endl;
            return;
        }
        
        cout << "\n=== СТАТИСТИКА КОНТЕЙНЕРА ===" << endl;
        cout << "Всего записей: " << linesData.size() << endl;
        
        // Первый элемент - самая длинная строка (из-за сортировки по убыванию)
        auto longestIt = linesData.begin();
        cout << "Максимальная длина: " << longestIt->first << " символов" << endl;
        
        // Последний элемент - самая короткая строка
        auto shortestIt = linesData.rbegin();
        cout << "Минимальная длина: " << shortestIt->first << " символов" << endl;
        
        // Подсчет строк нужной длины
        int count10to20 = 0;
        for (const auto& entry : linesData) {
            if (entry.first >= 10 && entry.first <= 20) {
                count10to20++;
            }
        }
        cout << "Строк длиной 10-20 символов: " << count10to20 << endl;
        
        // Пример использования lower_bound и upper_bound
        cout << "\nПример работы lower_bound и upper_bound:" << endl;
        
        auto lbExample = linesData.lower_bound(20);
        if (lbExample != linesData.end()) {
            cout << "lower_bound(20) указывает на строку длиной " << lbExample->first;
            cout << " (позиция в файле: " << lbExample->second << ")" << endl;
        }
        
        auto ubExample = linesData.upper_bound(10);
        if (ubExample != linesData.end()) {
            cout << "upper_bound(10) указывает на строку длиной " << ubExample->first;
            cout << " (позиция в файле: " << ubExample->second << ")" << endl;
        }
    }
};

// Функция для вывода инструкции по использованию
void printUsage(const char* programName) {
    cout << "Использование: " << programName << " \"входной_файл.txt\" \"выходной_файл.txt\"" << endl;
    cout << endl;
    cout << "Примеры:" << endl;
    cout << "  " << programName << " input.txt output.txt" << endl;
    cout << "  " << programName << " 111.txt result.txt" << endl;
    cout << "  " << programName << " \"C:\\My Files\\data.txt\" \"C:\\My Files\\filtered.txt\"" << endl;
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "rus");
    cout << "==========================================" << endl;
    cout << "  ОБРАБОТКА ТЕКСТОВЫХ ФАЙЛОВ (STL multimap)" << endl;
    cout << "==========================================" << endl;
    
    // Проверяем количество аргументов командной строки
    if (argc != 3) {
        cerr << "Ошибка: неверное количество аргументов!" << endl;
        printUsage(argv[0]);
        return 1;
    }
    
    string inputFile = argv[1];
    string outputFile = argv[2];
    
    cout << "Входной файл:  " << inputFile << endl;
    cout << "Выходной файл: " << outputFile << endl;
    cout << endl;
    
    try {
        TextFileProcessor processor;
        
        // Шаг 1: Чтение входного файла и заполнение контейнера
        cout << "ШАГ 1: Загрузка данных из файла в контейнер..." << endl;
        processor.readAndStoreFile(inputFile);
        
        // Шаг 2: Вывод статистики
        processor.displayStatistics();
        
        // Шаг 3: Запись отфильтрованных данных
        cout << "\nШАГ 2: Запись отфильтрованных строк в выходной файл..." << endl;
        processor.writeFilteredOutput(inputFile, outputFile);
        
        cout << "\n==========================================" << endl;
        cout << "         ОБРАБОТКА ЗАВЕРШЕНА!" << endl;
        cout << "==========================================" << endl;
        
    } catch (const exception& e) {
        cerr << "\n!!! ОШИБКА: " << e.what() << " !!!" << endl;
        return 1;
    }
    
    return 0;
}
