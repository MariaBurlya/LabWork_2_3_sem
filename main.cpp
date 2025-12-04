#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <functional>

using namespace std;

struct Compare {
    bool operator()(int a, int b) const {
        return a > b;
    }
};

int main() {
    string input_file = "111.txt";
    string output_file = "output.txt";
    
    multimap<int, long, Compare> lines_map;
    
    try {
        // 1. Чтение файла и заполнение контейнера
        ifstream in_file(input_file, ios::binary);
        if (!in_file) throw runtime_error("Cannot open input file");
        
        // Используем алгоритмы для чтения и обработки
        string line;
        long pos = in_file.tellg();
        
        while (getline(in_file, line)) {
            int len = line.length();
            // Используем алгоритм find_if для проверки диапазона длин
            if (find_if(&len, &len + 1, [](int l) { return l >= 10 && l <= 20; }) != &len + 1) {
                lines_map.insert(make_pair(len, pos));
            }
            pos = in_file.tellg();
        }
        in_file.close();
        
        if (lines_map.empty()) throw runtime_error("No suitable lines found");
        
        // 2. Использование lower_bound и upper_bound (требуется в задании)
        cout << "Using STL bounds functions:" << endl;
        
        // lower_bound - первая строка с длиной <= 15 (из-за обратного порядка)
        auto lower = lines_map.lower_bound(15);
        if (lower != lines_map.end()) {
            cout << "Lower bound (15): length=" << lower->first << endl;
        }
        
        // upper_bound - первая строка с длиной < 15 (из-за обратного порядка)
        auto upper = lines_map.upper_bound(15);
        if (upper != lines_map.end()) {
            cout << "Upper bound (15): length=" << upper->first << endl;
        }
        
        // 3. Использование алгоритмов для работы с диапазонами
        // Находим количество строк каждой длины с помощью алгоритмов
        auto unique_len = lines_map.begin()->first;
        auto count = count_if(lines_map.begin(), lines_map.end(),
            [&unique_len](const auto& p) { return p.first == unique_len; });
        cout << "Count of lines with length " << unique_len << ": " << count << endl;
        
        // 4. Запись в выходной файл с использованием алгоритмов STL
        ofstream out_file(output_file);
        if (!out_file) throw runtime_error("Cannot create output file");
        
        in_file.open(input_file, ios::binary);
        if (!in_file) throw runtime_error("Cannot reopen input file");
        
        // Используем алгоритм for_each для обхода контейнера
        for_each(lines_map.begin(), lines_map.end(), [&](const auto& pair) {
            in_file.seekg(pair.second);
            string current_line;
            if (getline(in_file, current_line)) {
                // Двойная проверка длины с использованием алгоритмов
                bool valid_length = any_of(&current_line.length(), &current_line.length() + 1,
                    [](size_t len) { return len >= 10 && len <= 20; });
                
                if (valid_length) {
                    out_file << current_line << endl;
                }
            }
        });
        
        in_file.close();
        out_file.close();
        
        // 5. Дополнительные алгоритмы STL для демонстрации
        // Находим минимальную и максимальную длину с помощью алгоритмов
        auto min_max = minmax_element(lines_map.begin(), lines_map.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });
        
        cout << "Length range: " << min_max.first->first << " - " << min_max.second->first << " characters" << endl;
        
        cout << "Processing completed successfully!" << endl;
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
