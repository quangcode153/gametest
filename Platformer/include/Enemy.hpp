#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Platform.hpp"

class Enemy {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    
    sf::Vector2f velocity;
    float speed;
    bool movingRight;
    bool alive;
    
public:
    Enemy();
    ~Enemy();
    
    bool loadTexture(const std::string& path);
    void setPosition(const sf::Vector2f& pos);
    
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    bool isAlive() const { return alive; }
    
    void update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms);
    void patrol();
    
    void checkBounds(const std::vector<std::unique_ptr<Platform>>& platforms);
    
    void kill();
    
    void draw(sf::RenderWindow& window);
};

#endif