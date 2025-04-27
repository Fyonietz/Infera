#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <variant>
#include <map>
#include <filesystem>
#include <thread>
#include <chrono>

using Cell = std::variant<std::string, int, float>;
using Row = std::map<std::string, Cell>;
using Table = std::vector<Row>;

std::vector<std::string> headers;

Cell detectType(const std::string& value) {
    try { return std::stoi(value); }
    catch (...) {}
    try { return std::stof(value); }
    catch (...) {}
    return value;
}

int readHeaders(std::ifstream& file) {
    headers.clear();
    std::string line;
    int colCount = 0;

    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string column;
        while (std::getline(ss, column, ',')) {
            headers.push_back(column);
            colCount++;
        }
    }
    return colCount;
}

Table readCSV(std::ifstream& file) {
    Table table;
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string value;
        Row row;
        int colIndex = 0;

        while (std::getline(ss, value, ',')) {
            if (colIndex < headers.size()) {
                row[headers[colIndex]] = detectType(value);
            }
            ++colIndex;
        }

        table.push_back(row);
    }

    return table;
}

void displayTable(const Table& table) {
    system("clear");

    std::cout << "=== Data Reloaded ===\n";
    for (const auto& h : headers) std::cout << h << "\t";
    std::cout << "\n----------------------------------------\n";

    for (const auto& row : table) {
        for (const auto& h : headers) {
            if (row.count(h)) {
                std::visit([](auto&& val) {
                    std::cout << val << "\t";
                    }, row.at(h));
            }
            else {
                std::cout << " \t";
            }
        }
        std::cout << "\n";
    }


}

int parse() {
    std::string filename = "data.txt";
    std::filesystem::file_time_type lastModified;

    if (!std::filesystem::exists(filename)) {
        std::cerr << "File does not exist.\n";
        return 1;
    }


    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file.\n";
            return 1;
        }
        readHeaders(file);
        Table data = readCSV(file);
        displayTable(data);
        file.close();
    }

    lastModified = std::filesystem::last_write_time(filename);

    while (true) {
        auto currentModified = std::filesystem::last_write_time(filename);
        if (currentModified != lastModified) {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Failed to open file.\n";
                return 1;
            }

            readHeaders(file);
            Table data = readCSV(file);
            displayTable(data);

            lastModified = currentModified;
            file.close();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Check every second
    }

    return 0;
}
