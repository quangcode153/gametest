#include "Menu.hpp"
#include <iostream>
#include <cmath>

Menu::Menu() :
    selectedIndex(0),
    fontLoaded(false),
    animationTimer(0.f)
{
    // Setup background
    background.setSize(sf::Vector2f(1200.f, 800.f));
    background.setFillColor(sf::Color(20, 30, 50, 230));
    
    std::cout << "Menu initialized\n";
}

Menu::~Menu() {
}

bool Menu::loadFont(const std::string& path) {
    if (!font.loadFromFile(path)) {
        std::cout << "ERROR: Cannot load menu font: " << path << "\n";
        return false;
    }
    fontLoaded = true;
    std::cout << "Menu font loaded: " << path << "\n";
    return true;
}

void Menu::initialize() {
    if (!fontLoaded) {
        std::cout << "WARNING: Font not loaded, menu text will not display\n";
        return;
    }
    
    // Setup title
    titleText.setFont(font);
    titleText.setString("PLATFORMER ADVENTURE");
    titleText.setCharacterSize(72);
    titleText.setFillColor(sf::Color::Green);
    titleText.setStyle(sf::Text::Bold);
    
    // Center title
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2.f, titleBounds.height / 2.f);
    titleText.setPosition(600.f, 200.f);
    
    // Setup menu options
    std::vector<std::string> optionStrings = {
        "START GAME",
        "SETTINGS",
        "EXIT"
    };
    
    menuOptions.clear();
    float startY = 400.f;
    float spacing = 80.f;
    
    for (size_t i = 0; i < optionStrings.size(); ++i) {
        sf::Text option;
        option.setFont(font);
        option.setString(optionStrings[i]);
        option.setCharacterSize(40);
        option.setFillColor(sf::Color::White);
        
        // Center option
        sf::FloatRect bounds = option.getLocalBounds();
        option.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        option.setPosition(600.f, startY + i * spacing);
        
        menuOptions.push_back(option);
    }
    
    // Highlight first option
    menuOptions[selectedIndex].setFillColor(sf::Color::Yellow);
    menuOptions[selectedIndex].setScale(1.2f, 1.2f);
    
    std::cout << "Menu initialized with " << menuOptions.size() << " options\n";
}

void Menu::moveUp() {
    // Reset current selection
    menuOptions[selectedIndex].setFillColor(sf::Color::White);
    menuOptions[selectedIndex].setScale(1.f, 1.f);
    
    // Move up
    selectedIndex--;
    if (selectedIndex < 0) {
        selectedIndex = static_cast<int>(MenuOption::COUNT) - 1;
    }
    
    // Highlight new selection
    menuOptions[selectedIndex].setFillColor(sf::Color::Yellow);
    menuOptions[selectedIndex].setScale(1.2f, 1.2f);
    
    std::cout << "Menu selection: " << selectedIndex << "\n";
}

void Menu::moveDown() {
    // Reset current selection
    menuOptions[selectedIndex].setFillColor(sf::Color::White);
    menuOptions[selectedIndex].setScale(1.f, 1.f);
    
    // Move down
    selectedIndex++;
    if (selectedIndex >= static_cast<int>(MenuOption::COUNT)) {
        selectedIndex = 0;
    }
    
    // Highlight new selection
    menuOptions[selectedIndex].setFillColor(sf::Color::Yellow);
    menuOptions[selectedIndex].setScale(1.2f, 1.2f);
    
    std::cout << "Menu selection: " << selectedIndex << "\n";
}

MenuOption Menu::getSelectedOption() const {
    return static_cast<MenuOption>(selectedIndex);
}

void Menu::update(float deltaTime) {
    // Title animation (floating effect)
    animationTimer += deltaTime;
    float offset = 10.f * std::sin(animationTimer * 2.f);
    titleText.setPosition(600.f, 200.f + offset);
    
    // Selected option pulse animation
    float pulseScale = 1.2f + 0.1f * std::sin(animationTimer * 5.f);
    menuOptions[selectedIndex].setScale(pulseScale, pulseScale);
}

void Menu::draw(sf::RenderWindow& window) {
    // Draw background
    window.draw(background);
    
    // Draw title
    window.draw(titleText);
    
    // Draw menu options
    for (const auto& option : menuOptions) {
        window.draw(option);
    }
    
    // Draw instructions
    if (fontLoaded) {
        sf::Text instructions;
        instructions.setFont(font);
        instructions.setString("Use UP/DOWN arrows, press ENTER to select");
        instructions.setCharacterSize(20);
        instructions.setFillColor(sf::Color(150, 150, 150));
        
        sf::FloatRect bounds = instructions.getLocalBounds();
        instructions.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        instructions.setPosition(600.f, 700.f);
        
        window.draw(instructions);
    }
}
bool Menu::isStartButtonClicked(const sf::Vector2f& mousePos) const {
    // "START GAME" là menuOptions[0]
    if (menuOptions.size() > 0 && menuOptions[0].getGlobalBounds().contains(mousePos))
        return true;
    return false;
}

bool Menu::isExitButtonClicked(const sf::Vector2f& mousePos) const {
    // "EXIT" là menuOptions[2]
    if (menuOptions.size() > 2 && menuOptions[2].getGlobalBounds().contains(mousePos))
        return true;
    return false;
}
