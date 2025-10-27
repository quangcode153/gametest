#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Platform.hpp"

class Player {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    
    sf::IntRect currentFrame;
    int frameCount;
    int currentFrameIndex;
    float animationTimer;
    float frameTime;
    bool facingRight;
    
    sf::Vector2f velocity;
    float moveSpeed;
    float jumpStrength;
    float gravity;
    float maxFallSpeed;
    bool isOnGround;
    
    int health;
    int maxHealth;
    float invulnerabilityTimer;
    bool isPoweredUp;
    float powerupTimer;
    float normalScale;
    float poweredScale;

public:
    Player();
    ~Player();
    
    bool loadTexture(const std::string& path);
    void setPosition(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    
    // Movement
    void moveLeft();
    void moveRight();
    void jump();
    void stopMoving();
    
    // Update
    void update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms);
    void updateAnimation(float deltaTime);
    void applyGravity(float deltaTime);
    
    // Va chạm chính xác
    void checkCollisionX(const std::vector<std::unique_ptr<Platform>>& platforms);
    void checkCollisionY(const std::vector<std::unique_ptr<Platform>>& platforms);
    
    // Combat & Power
    void takeDamage();
    bool canTakeDamage() const;
    void activatePowerup(float duration);
    bool isInvulnerable() const;
    
    // Getters
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool getIsOnGround() const { return isOnGround; }
    bool getIsPoweredUp() const { return isPoweredUp; }
    float getPowerupTimer() const { return powerupTimer; }
    sf::Vector2f getVelocity() const { return velocity; }
    
    // Rendering
    void draw(sf::RenderWindow& window);
};

#endif