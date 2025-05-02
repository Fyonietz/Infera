#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <json.hpp>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include "tinyfiledialogs.h"

using json = nlohmann::json;
json j;

// Global color struct
struct Color {
    sf::Color bg = sf::Color(255, 255, 255);
    sf::Color Colour = sf::Color(48, 62, 155);
    sf::Color Colour2 = sf::Color(123, 47, 126);
    sf::Color Accent = sf::Color(222, 192, 250);
};

// Global Variables
unsigned int fontSize = static_cast<unsigned int>(10 * 1.618f);
Color color;
sf::Font fonts;

// Class Table
class Table {
public:
    float posX, posY, height, width;
    sf::RectangleShape table;
    sf::Text content;

    Table(float x, float y, float h, float w, const std::string& text)
        : posX(x), posY(y), height(h), width(w), table(sf::Vector2f(w, h)) {
        table.setFillColor(sf::Color::White);
        table.setPosition(x, y);
        table.setOutlineThickness(1.f);
        table.setOutlineColor(sf::Color::Black);

        content.setFont(fonts);
        content.setCharacterSize(fontSize);
        content.setString(text);
        content.setPosition(x + 10, y + 10);
        content.setFillColor(sf::Color::Black);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(table);
        window.draw(content);
    }
  
};

// Class TableData
class TableData {
public:
    std::vector<std::vector<std::string>> test;
    std::string line;
    std::string jsonPathAccesser; // Store the CSV file path

    int streamFile() {
        test.clear();
        std::ifstream file("paths.json");
        if (!file.is_open()) {
            std::cerr << "Error: Failed to open paths.json" << std::endl;
            return -1;
        }

        try {
            file >> j;
        }
        catch (const json::parse_error& e) {
            std::cerr << "Error: JSON parsing failed - " << e.what() << std::endl;
            return -1;
        }

        if (!j.contains("paths") || !j["paths"].contains("csv")) {
            std::cerr << "Error: Missing 'paths' or 'csv' key in JSON file" << std::endl;
            return -1;
        }

        jsonPathAccesser = j["paths"]["csv"]; // Assign to the class member
        std::ifstream inputFile(jsonPathAccesser);
        if (!inputFile.is_open()) {
            std::cerr << "Error: Failed to open CSV file at " << jsonPathAccesser << std::endl;
            return -1;
        }
        
        // Read CSV file
        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
            std::vector<std::string> row;
            std::string lineColumn;
            while (std::getline(iss, lineColumn, ',')) { // Use ',' as delimiter for CSV
                row.push_back(lineColumn);
            }
            test.push_back(row);
        }

        return 0;
    }
    
};

TableData TableCore;

class barChartContainer {
public:
    float posX, posY, Height, Width;
    sf::RectangleShape chartContainer;
    sf::RectangleShape leftBorder;
    sf::RectangleShape bottomBorder;
    barChartContainer(float x, float y, float h, float w) : posX(x),posY(y),Height(h),Width(w) {
        chartContainer.setPosition(x, y);
        chartContainer.setSize(sf::Vector2f(h, w));
        chartContainer.setFillColor(sf::Color::White);
        const float borderSize = 2.5f;
        sf::Color borderColor = sf::Color::Black;
        //outline
        leftBorder.setSize(sf::Vector2f(borderSize,chartContainer.getSize().y));
        leftBorder.setPosition(chartContainer.getPosition().x, chartContainer.getPosition().y);
        leftBorder.setFillColor(borderColor);
        bottomBorder.setSize(sf::Vector2f(chartContainer.getSize().x, borderSize));
        bottomBorder.setPosition(chartContainer.getPosition().x, chartContainer.getPosition().y+chartContainer.getSize().y);
        bottomBorder.setFillColor(borderColor);
     
    }
    void draw(sf::RenderWindow& window) {
        window.draw(chartContainer);
        window.draw(leftBorder);
        window.draw(bottomBorder);
    }

};

int main() {
    std::string title = "Infera";
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), title, sf::Style::Fullscreen);
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f windowSizeF(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y));
    tgui::GuiSFML gui(window);
    barChartContainer chartContainer(100.f,100.f,300.f,300.f);
    // Load font
    if (!fonts.loadFromFile("Quicksand-Medium.ttf")) {
        std::cerr << "Error: Failed to load font 'Quicksand-Medium.ttf'" << std::endl;
        return -1;
    }

    // Load table data
    if (TableCore.streamFile() != 0) {
        std::cerr << "Error: Failed to load table data" << std::endl;
        return -1;
    }

    // Grid Table
    float tablePosX = 40.f, tablePosY = 20.f;
    float cellHeight = 50.f, cellWidth = 100.f * 1.618f + fontSize;

    /*auto drawTable = [&]() {
        for (std::size_t r = 0; r < TableCore.test.size(); ++r) {
            for (std::size_t c = 0; c < TableCore.test[r].size(); ++c) {
                std::string cellText = TableCore.test[r][c];

                float x = tablePosX + c * cellWidth;
                float y = tablePosY + r * cellHeight;

                Table cell(x, y, cellHeight, cellWidth, cellText);
                cell.draw(window);
            }
        }
        };*/

    // Check if the file is saved
    std::filesystem::file_time_type lastModified;
    try {
        lastModified = std::filesystem::last_write_time(TableCore.jsonPathAccesser);
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: Unable to access file modification time - " << e.what() << std::endl;
        return -1;
    }

    sf::Clock clock; // Timer for periodic file checks

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            gui.handleEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                std::cout << "Reloading table data...\n";
                if (TableCore.streamFile() == 0) {
                    std::cout << "Table data reloaded successfully.\n";
                }
                else {
                    std::cerr << "Failed to reload table data.\n";
                }
            }
        }
        
        // Periodically check for file changes
        if (clock.getElapsedTime().asMilliseconds() >= 50.0f) {
            clock.restart();
            try {
                auto currentModified = std::filesystem::last_write_time(TableCore.jsonPathAccesser);
                if (currentModified != lastModified) {
                    if (TableCore.streamFile() == 0) {
                        std::cout << "Table data reloaded successfully.\n";
                    }
                    lastModified = currentModified;
                }
            }
            catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "Error: Unable to access file modification time - " << e.what() << std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        window.clear(color.bg);

        // Draw table only if data is loaded
      /*  if (!TableCore.test.empty()) {
            drawTable();
        }*/
        chartContainer.draw(window);
        gui.draw();
        window.display();
    }

    return 0;
}



