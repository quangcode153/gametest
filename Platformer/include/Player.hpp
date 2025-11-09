#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Platform.hpp"
#include "AnimationManager.hpp" 

class Player {
private:
    sf::Sprite sprite;
    
    std::unique_ptr<AnimationManager> animManager; 
    
    enum class State { IDLE, RUNNING, JUMPING, FALLING, ATTACK1, ATTACK2, TAKE_HIT, DEATH };
    State currentState;

    sf::Vector2i frameSize; 
    
    // === THÊM BIẾN NÀY ===
    sf::FloatRect localHitbox; // Khung va chạm local (so với Origin)

    bool facingRight;
    
    sf::Vector2f velocity;
    float moveSpeed;
    float normalMoveSpeed; 
    bool isBoosted;
    float boostTimer;      

    bool isShielded;
    float shieldTimer;

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

    int totalCoins; 

    // Hàm helper
    void setAnimation(State newState);
    void loadAnimations(); 

public:
    void drawHitbox(sf::RenderWindow& window);
    Player();
    ~Player();
    Player::State getCurrentState() const;
    bool hasDeathAnimationFinished() const;
    bool loadTexture(const std::string& path); 
    void setPosition(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;

    // === SỬA/THÊM CÁC HÀM NÀY ===
    sf::FloatRect getBounds() const;     // Giữ lại (trả về khung hình ảnh)
    sf::FloatRect getHitbox() const;     // Hàm va chạm MỚI
    float getScaledFootOffset() const; // Hàm MỚI để sửa lỗi spawn "bay"
    // ============================
    
    void moveLeft();
    void moveRight();
    void jump();
    void stopMoving();
    
    void update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms);
    void applyGravity(float deltaTime);
    
    void checkCollisionX(const std::vector<std::unique_ptr<Platform>>& platforms);
    void checkCollisionY(const std::vector<std::unique_ptr<Platform>>& platforms);
    
    void takeDamage();
    bool canTakeDamage() const;
    void activatePowerup(float duration);
    bool isInvulnerable() const;
    
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    bool getIsOnGround() const { return isOnGround; }
    bool getIsPoweredUp() const { return isPoweredUp; }
    float getPowerupTimer() const { return powerupTimer; }
    sf::Vector2f getVelocity() const { return velocity; }
    
    void draw(sf::RenderWindow& window);

    // Coin/Heart
    void addCoin(int amount);
    int getTotalCoins() const;
    void setTotalCoins(int amount);
    void heal(int amount);

    // Powerups
    void activateSpeedBoost(float duration);
    bool getIsBoosted() const { return isBoosted; }
    float getBoostTimer() const { return boostTimer; }
    void activateShield(float duration);
    bool getIsShielded() const { return isShielded; }
    float getShieldTimer() const { return shieldTimer; }
};

#endif