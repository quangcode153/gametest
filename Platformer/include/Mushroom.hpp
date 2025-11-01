#ifndef MUSHROOM_HPP
#define MUSHROOM_HPP

#include <SFML/Graphics.hpp>
#include <vector>

class Mushroom {
private:
    sf::CircleShape capShape;  
    sf::RectangleShape stemShape; 
    std::vector<sf::CircleShape> spots; 
    
    sf::Vector2f position;
    bool collected;
    
    float animationTimer;
    float scaleOffset;

public:
    Mushroom(const sf::Vector2f& pos);
    ~Mushroom();
    
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const { return position; }
    bool isCollected() const { return collected; }
    
    void collect();
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
};

#endif