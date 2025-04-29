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

int main() {
    std::string title = "Infera";
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), title, sf::Style::Fullscreen);
    tgui::GuiSFML gui(window);

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

    auto drawTable = [&]() {
        for (std::size_t r = 0; r < TableCore.test.size(); ++r) {
            for (std::size_t c = 0; c < TableCore.test[r].size(); ++c) {
                std::string cellText = TableCore.test[r][c];

                float x = tablePosX + c * cellWidth;
                float y = tablePosY + r * cellHeight;

                Table cell(x, y, cellHeight, cellWidth, cellText);
                cell.draw(window);
            }
        }
        };

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
        if (clock.getElapsedTime().asSeconds() >= 1.0f) {
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
        }

        window.clear(color.bg);

        // Draw table only if data is loaded
        if (!TableCore.test.empty()) {
            drawTable();
        }

        gui.draw();
        window.display();
    }

    return 0;
}



//Nanti

    // Function to load UI from form.txt
//auto loadUI = [&]() {
//    gui.removeAllWidgets();
//    try {
//        gui.loadWidgetsFromFile("form.txt");
//    }
//    catch (const tgui::Exception& e) {
//        std::cerr << "TGUI Error: " << e.what() << std::endl;
//        return;
//    }
//
//    // Access widgets AFTER loading
//    auto sideNav = gui.get<tgui::Panel>("SideNav");
//    if (sideNav)
//    {
//        sideNav->setHeight(desktop.height);
//        sideNav->setWidth("15%");
//    }
//
//    auto button = gui.get<tgui::Button>("Button1");
//    if (button)
//    {
//        button->setPosition("0%", "20%");
//        button->setWidth("100%");
//        button->onPress([] {
//            std::cout << "Button clicked\n";
//            const char* file = tinyfd_openFileDialog("Pick File", "", 0, NULL, NULL, 0);
//            if (file)
//                std::cout << "Picked: " << file << std::endl;
//            else
//                std::cout << "No file selected.\n";
//            });
//
//        // Optional: add button to panel if needed
//        if (sideNav)
//            sideNav->add(button);
//    }
//    };

/*loadUI(); */// Initial load
