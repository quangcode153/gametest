#include "Menu.hpp"
#include <iostream>
#include <cmath>

Menu::Menu() :
    selectedIndex(0),
    fontLoaded(false),
    animationTimer(0.f)
{
    background.setSize(sf::Vector2f(1200.f, 800.f));
    background.setFillColor(sf::Color(20, 30, 50, 230));

    if (!frameTexture.loadFromFile("assets/khungMenu.png")) {
        std::cerr << "!!! LOI: Khong the tai assets/khungMenu.png\n";
    }

    if (!texPlayNormal.loadFromFile("assets/button_play_normal.png"))
        std::cerr << "Loi tai button_play_normal.png\n";
    if (!texPlayHighlight.loadFromFile("assets/button_play_highlight.png"))
        std::cerr << "Loi tai button_play_highlight.png\n";

    if (!texSettingsNormal.loadFromFile("assets/button_settings_normal.png"))
        std::cerr << "Loi tai button_settings_normal.png\n";
    if (!texSettingsHighlight.loadFromFile("assets/button_settings_highlight.png"))
        std::cerr << "Loi tai button_settings_highlight.png\n";

    if (!texExitNormal.loadFromFile("assets/button_exit_normal.png"))
        std::cerr << "Loi tai button_exit_normal.png\n";
    if (!texExitHighlight.loadFromFile("assets/button_exit_highlight.png"))
        std::cerr << "Loi tai button_exit_highlight.png\n";

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

void Menu::setSpriteToSize(sf::Sprite& sprite, const sf::Vector2f& targetSize) {
    const sf::Texture* texture = sprite.getTexture();
    if (!texture) return;
    sf::Vector2u texSize = texture->getSize();
    if (texSize.x == 0 || texSize.y == 0) return;

    sprite.setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));

    sprite.setOrigin(static_cast<float>(texSize.x) / 2.f, static_cast<float>(texSize.y) / 2.f);

    float scaleX = targetSize.x / static_cast<float>(texSize.x);
    float scaleY = targetSize.y / static_cast<float>(texSize.y);

    sprite.setScale(scaleX, scaleY);
}


void Menu::initialize() {
    if (!fontLoaded) {
        std::cout << "WARNING: Font not loaded, menu text will not display\n";
    }

    frameSprite.setTexture(frameTexture);
    sf::FloatRect frameBounds = frameSprite.getLocalBounds();
    frameSprite.setOrigin(frameBounds.width / 2.f, frameBounds.height / 2.f);
    frameSprite.setPosition(600.f, 400.f);

    float frameCenterY = frameSprite.getPosition().y;
    float spacing = 80.f;
    float startY = frameCenterY - spacing + 10;

    sf::Vector2f buttonSize(300.f, 60.f);

    spritePlay.setTexture(texPlayHighlight);
    spritePlay.setPosition(600.f, startY + 0 * spacing);
    setSpriteToSize(spritePlay, buttonSize);

    spriteSettings.setTexture(texSettingsNormal);
    spriteSettings.setPosition(600.f, startY + 1 * spacing);
    setSpriteToSize(spriteSettings, buttonSize);

    spriteExit.setTexture(texExitNormal);
    spriteExit.setPosition(600.f, startY + 2 * spacing);
    setSpriteToSize(spriteExit, buttonSize);

    instructionsText.setFont(font);
    instructionsText.setString("Press Enter to Select");
    instructionsText.setCharacterSize(24);
    instructionsText.setFillColor(sf::Color(255, 215, 0));
    sf::FloatRect instBounds = instructionsText.getLocalBounds();
    instructionsText.setOrigin(instBounds.width / 2.f, instBounds.height / 2.f);
    instructionsText.setPosition(600.f, frameSprite.getPosition().y + frameSprite.getGlobalBounds().height / 2.f + 40.f);

    selectedIndex = 0;

    std::cout << "Menu initialized with 3 sprites\n";
}

void Menu::moveUp() {
    selectedIndex--;
    if (selectedIndex < 0) {
        selectedIndex = static_cast<int>(MenuOption::COUNT) - 1;
    }

    spritePlay.setTexture(selectedIndex == 0 ? texPlayHighlight : texPlayNormal);
    spriteSettings.setTexture(selectedIndex == 1 ? texSettingsHighlight : texSettingsNormal);
    spriteExit.setTexture(selectedIndex == 2 ? texExitHighlight : texExitNormal);

    sf::Vector2u sizeP = spritePlay.getTexture()->getSize();
    spritePlay.setTextureRect(sf::IntRect(0, 0, sizeP.x, sizeP.y));
    sf::Vector2u sizeS = spriteSettings.getTexture()->getSize();
    spriteSettings.setTextureRect(sf::IntRect(0, 0, sizeS.x, sizeS.y));
    sf::Vector2u sizeE = spriteExit.getTexture()->getSize();
    spriteExit.setTextureRect(sf::IntRect(0, 0, sizeE.x, sizeE.y));

    std::cout << "Menu selection: " << selectedIndex << "\n";
}

void Menu::moveDown() {
    selectedIndex++;
    if (selectedIndex >= static_cast<int>(MenuOption::COUNT)) {
        selectedIndex = 0;
    }

    spritePlay.setTexture(selectedIndex == 0 ? texPlayHighlight : texPlayNormal);
    spriteSettings.setTexture(selectedIndex == 1 ? texSettingsHighlight : texSettingsNormal);
    spriteExit.setTexture(selectedIndex == 2 ? texExitHighlight : texExitNormal);

    sf::Vector2u sizeP = spritePlay.getTexture()->getSize();
    spritePlay.setTextureRect(sf::IntRect(0, 0, sizeP.x, sizeP.y));
    sf::Vector2u sizeS = spriteSettings.getTexture()->getSize();
    spriteSettings.setTextureRect(sf::IntRect(0, 0, sizeS.x, sizeS.y));
    sf::Vector2u sizeE = spriteExit.getTexture()->getSize();
    spriteExit.setTextureRect(sf::IntRect(0, 0, sizeE.x, sizeE.y));

    std::cout << "Menu selection: " << selectedIndex << "\n";
}

MenuOption Menu::getSelectedOption() const {
    return static_cast<MenuOption>(selectedIndex);
}

void Menu::update(float deltaTime) {
    animationTimer += deltaTime;

    float pulseScale = 1.0f + 0.05f * std::abs(std::sin(animationTimer * 5.f));
    sf::Vector2f buttonSize(300.f, 60.f);

    setSpriteToSize(spritePlay, (selectedIndex == 0) ? (buttonSize * pulseScale) : buttonSize);
    setSpriteToSize(spriteSettings, (selectedIndex == 1) ? (buttonSize * pulseScale) : buttonSize);
    setSpriteToSize(spriteExit, (selectedIndex == 2) ? (buttonSize * pulseScale) : buttonSize);
}

void Menu::draw(sf::RenderWindow& window) {

    window.draw(frameSprite);

    window.draw(spritePlay);
    window.draw(spriteSettings);
    window.draw(spriteExit);

    if (fontLoaded) {
        window.draw(instructionsText);
    }
}

bool Menu::isPlayButtonClicked(const sf::Vector2f& mousePos) const {
    return spritePlay.getGlobalBounds().contains(mousePos);
}

bool Menu::isExitButtonClicked(const sf::Vector2f& mousePos) const {
    return spriteExit.getGlobalBounds().contains(mousePos);
}
bool Menu::isSettingsButtonClicked(const sf::Vector2f& mousePos) const {
    return spriteSettings.getGlobalBounds().contains(mousePos);
}