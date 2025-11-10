#include "Item.hpp"
#include <iostream>

Item::Item(float x, float y, const std::string& texturePath, float duration)
    : collected(false),
      effectDuration(duration),
      currentEffectTime(0.0f)
{
    std::cout << "[DEBUG Item] Creating Item at (" << x << ", " << y << ") with texture: " << texturePath << std::endl;

    sf::Image img;
    if (!img.loadFromFile(texturePath)) {
        std::cerr << "ERROR: Could not load item IMAGE: " << texturePath << std::endl;
        img.create(32, 32, sf::Color::Magenta);
        std::cerr << "Using magenta fallback texture." << std::endl;
    } else {
        std::cout << "SUCCESS: Loaded item IMAGE: " << texturePath << std::endl;
        img.createMaskFromColor(sf::Color::White);
    }

    if (!texture.loadFromImage(img)) {
        std::cerr << "ERROR: Could not load item TEXTURE from image: " << texturePath << std::endl;
    } else {
        std::cout << "SUCCESS: Loaded item TEXTURE from image: " << texturePath << std::endl;
    }

    sprite.setTexture(texture);
    sprite.setPosition(x, y);

    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
}

void Item::update(float deltaTime) {
    if (collected && effectDuration > 0) {
        currentEffectTime -= deltaTime;
        if (currentEffectTime < 0) {
            currentEffectTime = 0;
        }
    }
}

void Item::draw(sf::RenderWindow& window) {
    if (!collected) {
        window.draw(sprite);
    }
}

sf::FloatRect Item::getBounds() const {
    return sprite.getGlobalBounds();
}

bool Item::isCollected() const {
    return collected;
}

void Item::setCollected(bool val) {
    collected = val;
}