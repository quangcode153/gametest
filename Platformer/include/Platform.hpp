#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <SFML/Graphics.hpp>

class Platform {
private:
    sf::RectangleShape shape;
    bool isSolid;
    sf::Vector2f position;
    sf::Vector2f size;

public:
    Platform(const sf::Vector2f& pos, const sf::Vector2f& sz, const sf::Color& color);
    ~Platform();
    
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getSize() const { return size; }
    bool getIsSolid() const { return isSolid; }
    
    void draw(sf::RenderWindow& window);
};

#endif