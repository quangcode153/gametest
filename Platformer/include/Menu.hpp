#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class MenuOption {
    START_GAME,
    SETTINGS,
    EXIT,
    COUNT
};

class Menu {
private:
    sf::Font font;
    sf::Text titleText;
    std::vector<sf::Text> menuOptions;
    
    int selectedIndex;
    bool fontLoaded;
    
    // Background
    sf::RectangleShape background;
    // Animation
    float animationTimer;

public:
    Menu();
    ~Menu();
    bool isStartButtonClicked(const sf::Vector2f& mousePos) const;
    bool isExitButtonClicked(const sf::Vector2f& mousePos) const;

    bool loadFont(const std::string& path);
    void initialize();
    
    void moveUp();
    void moveDown();
    MenuOption getSelectedOption() const;
    
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
};

#endif