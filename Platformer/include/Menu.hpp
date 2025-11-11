#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class MenuOption {
    PLAY,
    SETTINGS,
    EXIT,
    COUNT
};


class Menu {
public:
    Menu();
    ~Menu();

    bool loadFont(const std::string& path);
    void initialize();
    void draw(sf::RenderWindow& window);
    void update(float deltaTime);

    void moveUp();
    void moveDown();
    MenuOption getSelectedOption() const;

    bool isPlayButtonClicked(const sf::Vector2f& mousePos) const;
    bool isSettingsButtonClicked(const sf::Vector2f& mousePos) const;
    bool isExitButtonClicked(const sf::Vector2f& mousePos) const;

private:
    sf::Font font;
    sf::Text titleText;
    sf::Text instructionsText;

    sf::Texture texPlayNormal, texPlayHighlight;
    sf::Texture texSettingsNormal, texSettingsHighlight;
    sf::Texture texExitNormal, texExitHighlight;
    
    sf::Sprite spritePlay;
    sf::Sprite spriteSettings;
    sf::Sprite spriteExit;

    sf::RectangleShape background;

    sf::Texture frameTexture;
    sf::Sprite frameSprite;

    int selectedIndex;
    bool fontLoaded;
    float animationTimer;

    void setSpriteToSize(sf::Sprite& sprite, const sf::Vector2f& size);
};

#endif