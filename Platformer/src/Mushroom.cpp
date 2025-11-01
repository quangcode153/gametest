#include "Mushroom.hpp"
#include <cmath>
#include <iostream>

Mushroom::Mushroom(const sf::Vector2f& pos) :
    position(pos),
    collected(false),
    animationTimer(0.f),
    scaleOffset(0.f)
{
    capShape.setRadius(20.f);
    capShape.setFillColor(sf::Color(255, 50, 50));
    capShape.setOutlineThickness(4.f);
    capShape.setOutlineColor(sf::Color::White);
    capShape.setOrigin(20.f, 20.f);
    capShape.setPosition(position);
    
    stemShape.setSize(sf::Vector2f(10.f, 15.f));
    stemShape.setFillColor(sf::Color::White);
    stemShape.setOrigin(5.f, 0.f);
    stemShape.setPosition(position);
    
    sf::CircleShape spot1(4.f);
    spot1.setFillColor(sf::Color::White);
    spot1.setOrigin(4.f, 4.f);
    spot1.setPosition(position.x - 8.f, position.y - 5.f);
    spots.push_back(spot1);
    
    sf::CircleShape spot2(3.f);
    spot2.setFillColor(sf::Color::White);
    spot2.setOrigin(3.f, 3.f);
    spot2.setPosition(position.x + 8.f, position.y - 3.f);
    spots.push_back(spot2);
    
    sf::CircleShape spot3(3.5f);
    spot3.setFillColor(sf::Color::White);
    spot3.setOrigin(3.5f, 3.5f);
    spot3.setPosition(position.x + 2.f, position.y - 10.f);
    spots.push_back(spot3);
    
    std::cout << "Mushroom created at (" << position.x << ", " << position.y << ")\n";
}

Mushroom::~Mushroom() {
}

sf::FloatRect Mushroom::getBounds() const {
    return capShape.getGlobalBounds();
}

void Mushroom::collect() {
    collected = true;
    std::cout << "Mushroom collected at (" << position.x << ", " << position.y << ")\n";
}

void Mushroom::update(float deltaTime) {
    if (collected) return;
    
    animationTimer += deltaTime * 3.f;
    scaleOffset = 0.1f * std::sin(animationTimer);
    
    float scale = 1.f + scaleOffset;
    capShape.setScale(scale, scale);
    
    for (size_t i = 0; i < spots.size(); ++i) {
        spots[i].setScale(scale, scale);
    }
}

void Mushroom::draw(sf::RenderWindow& window) {
    if (!collected) {
        window.draw(stemShape);
        window.draw(capShape);
        
        for (const auto& spot : spots) {
            window.draw(spot);
        }
    }
}