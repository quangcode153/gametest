#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include "AnimationManager.hpp" 

class Platform;

enum class State {
    IDLE,
    RUNNING,
    JUMPING,
    FALLING,
    ATTACK1,
    ATTACK2,
    CASTING,
    TAKE_HIT,
    DEATH
};

class Player {
public:
    Player();
    ~Player();

    bool loadTexture(const std::string& folderPath); 

    void update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms);
    void draw(sf::RenderWindow& window);

    void moveLeft();
    void moveRight();
    void jump();
    void attack();
    void stopMoving();

    void setPosition(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const { return velocity; }

    sf::FloatRect getBounds() const;
    sf::FloatRect getHitbox() const;
    float getScaledFootOffset() const;

    void takeDamage();
    bool canTakeDamage() const;
    bool isInvulnerable() const;
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    void heal(int amount);
    State getCurrentState() const;
    bool hasDeathAnimationFinished() const;

    void addCoin(int amount);
    int getTotalCoins() const;
    void setTotalCoins(int amount);
    void activatePowerup(float duration);
    bool getIsPoweredUp() const { return isPoweredUp; }
    float getPowerupTimer() const { return powerupTimer; }
    
    void activateSpeedBoost(float duration);
    bool getIsBoosted() const { return isBoosted; }
    float getBoostTimer() const { return boostTimer; }

    void activateShield(float duration);
    bool getIsShielded() const { return isShielded; }
    float getShieldTimer() const { return shieldTimer; }
    bool isAttacking() const; // Kiểm tra xem Player có đang trong animation tấn công không
    sf::FloatRect getAttackHitbox() const;

private:
    std::unique_ptr<AnimationManager> animManager;
    sf::Sprite sprite;
    State currentState;
    sf::Vector2i frameSize; 
    bool facingRight;

    sf::FloatRect localHitbox; 

    sf::Vector2f velocity;
    float moveSpeed;
    float normalMoveSpeed;
    float jumpStrength;
    float gravity;
    float maxFallSpeed;
    bool isOnGround;

    int health;
    int maxHealth;
    float invulnerabilityTimer;
    
    bool isPoweredUp;
    float powerupTimer;
    bool isBoosted;
    float boostTimer;
    bool isShielded;
    float shieldTimer;
    float normalScale;
    float poweredScale;
    int totalCoins;

    std::string currentAssetFolder; 

    void loadAnimations(const std::string& folderPath);
    void setAnimation(State newState);
    void applyGravity(float deltaTime);
    void checkCollisionX(const std::vector<std::unique_ptr<Platform>>& platforms);
    void checkCollisionY(const std::vector<std::unique_ptr<Platform>>& platforms);
};