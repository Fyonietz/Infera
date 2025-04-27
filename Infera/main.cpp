#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "tinyfiledialogs.h"

// Global color struct
struct Color {
    sf::Color bg = sf::Color(255, 255, 255);
    sf::Color Colour = sf::Color(48, 62, 155);
    sf::Color Colour2 = sf::Color(123, 47, 126);
    sf::Color Accent = sf::Color(222, 192, 250);
};
//Global Variable;
unsigned int fontSize =10 * 1.618f ;
Color color;
//Font
sf::Font fonts;
//Class Table
class Table {
public:
    float posX, posY, height, width;
    sf::RectangleShape table;
    sf::Text content;
    
    Table(float x, float y, float h, float w,const std::string& text) :posX(x), posY(y), height(h), width(w), table(sf::Vector2f(w, h)) {
        table.setFillColor(sf::Color::White);
        table.setPosition(x, y);
        table.setOutlineThickness(1.f);
        table.setOutlineColor(sf::Color::Black);

        content.setFont(fonts);  
        content.setCharacterSize(fontSize);
        content.setString(text);
        content.setPosition(x+10,y+10);
        content.setFillColor(sf::Color::Black);
    }
    void draw(sf::RenderWindow& window) {
        window.draw(table);
        window.draw(content);
    }
    ~Table() {};
};

//Table TableCore(40.f, 20.f, 50.f, 200.f);
class TableData {
public:
    std::vector<std::vector<std::string>> test = {
        {"Nama","Umur","Jurusan","Skill","User"},
       {"Habib Herdiansyah","16","RPL","Desktop","C/C++ And ASM"},
       {"Raditya Alfarisi","16","RPL","Fullstack Web","TypeScript And React"},
       {"Fathir Adzan Satia","15","RPL","Android Dev","Flutter Or Dart"},
    };
    std::vector<float> columnWidth;
};
TableData TableCore;
int main()
{
    std::string title = "Infera";
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), title, sf::Style::Fullscreen);
    tgui::GuiSFML gui(window);

    //Font
    fonts.loadFromFile("Quicksand-Medium.ttf");
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    //Grid Table 
    float tablePosX = 40.f, tablePosY = 20.f;
    float cellHeight = 50.f, cellWidth=100.f * 1.618f + fontSize;
    bool showTable = false;
    // Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            gui.handleEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                std::cout << "Reloading UI...\n";
                
            /*    loadUI()*/; // Reconnects handlers too
            }
        }
       

        window.clear(color.bg);
        
        //Cell Loop
        for (int r = 0; r < TableCore.test.size(); ++r) {
            for (int c = 0; c < TableCore.test[r].size(); ++c) {
                std::string cellText = TableCore.test[r][c];

                float x = tablePosX + c * cellWidth;
                float y = tablePosY + r * cellHeight;

                Table cell(x, y, cellHeight, cellWidth, cellText);
                cell.draw(window);
            }
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
