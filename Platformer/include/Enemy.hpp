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
    
    // AI & Movement
    sf::Vector2f velocity; // <-- SỬA: Dùng (x, y) để có trọng lực
    float speed;
    bool movingRight;
    bool alive;
    
    // (Không cần patrol bounds nữa)

public:
    Enemy();
    ~Enemy();
    
    bool loadTexture(const std::string& path);
    void setPosition(const sf::Vector2f& pos);
    
    // (Không cần hàm này nữa)
    // void setPatrolBounds(float left, float right); 
    
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    bool isAlive() const { return alive; }
    
    // AI behavior
    void update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms);
    void patrol();
    
    // SỬA: checkBounds giờ cần biết platform
    void checkBounds(const std::vector<std::unique_ptr<Platform>>& platforms);
    
    // Combat
    void kill();
    
    // Rendering
    void draw(sf::RenderWindow& window);
};

#endif