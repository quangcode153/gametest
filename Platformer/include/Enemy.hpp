#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Platform.hpp"
#include "AnimationManager.hpp"

class Player;

enum class EnemyState { IDLE, WALK, ATTACK, TAKE_HIT, DEATH };

class Enemy {
public:
    Enemy();
    virtual ~Enemy();

    virtual void init(const std::string& assetFolder) = 0;
    virtual void updateAI(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms, Player* player) = 0;

    void update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms, Player* player);
    virtual void draw(sf::RenderWindow& window);
    virtual void takeDamage(int damage);
    
    void setPosition(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;
    
    virtual sf::FloatRect getBounds() const;
    
    bool isAlive() const { return health > 0; }
    bool canBeHit() const;
    
    bool isAttacking() const { return currentState == EnemyState::ATTACK; }

protected:
    std::unique_ptr<AnimationManager> animManager;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    bool movingRight;
    int health;
    int maxHealth;
    float invulnerabilityTimer;
    EnemyState currentState;
    sf::Vector2i frameSize;

    float hitboxOffsetLeft;
    float hitboxOffsetTop;
    float hitboxReduceWidth;
    float hitboxReduceHeight;

    void applyGravity(float deltaTime);
    void checkPlatformCollisions(const std::vector<std::unique_ptr<Platform>>& platforms);
    void setAnimation(EnemyState newState);

    bool gravityEnabled;
};