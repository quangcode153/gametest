#include "Enemy.hpp"
#include <iostream>

Enemy::Enemy() : 
    velocity(0.f, 0.f), 
    movingRight(true), 
    health(100), 
    maxHealth(100), 
    invulnerabilityTimer(0.f), 
    currentState(EnemyState::IDLE),
    hitboxOffsetLeft(0.f), 
    hitboxOffsetTop(0.f), 
    hitboxReduceWidth(0.f), 
    hitboxReduceHeight(0.f) 
{
    animManager = std::make_unique<AnimationManager>(sprite);
}

Enemy::~Enemy() {}

sf::FloatRect Enemy::getBounds() const { 
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();
    
    float width = spriteBounds.width - hitboxReduceWidth;
    float height = spriteBounds.height - hitboxReduceHeight;

    float left = (spriteBounds.left + spriteBounds.width / 2.f) - (width / 2.f);

    float top = spriteBounds.top + hitboxOffsetTop;

    return sf::FloatRect(left, top, width, height);
}

void Enemy::update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms, Player* player) {
    if (health <= 0) {
        if (currentState != EnemyState::DEATH) setAnimation(EnemyState::DEATH);
        animManager->update(deltaTime);
        return;
    }

    if (invulnerabilityTimer > 0.f) invulnerabilityTimer -= deltaTime;

    if (currentState == EnemyState::TAKE_HIT && !animManager->isFinished()) {
        animManager->update(deltaTime);
        applyGravity(deltaTime);
        checkPlatformCollisions(platforms);
        return;
    }

    updateAI(deltaTime, platforms, player); 

    applyGravity(deltaTime);
    checkPlatformCollisions(platforms);
    animManager->update(deltaTime);
}

void Enemy::applyGravity(float deltaTime) {
    velocity.y += 1000.f * deltaTime;
    sprite.move(0, velocity.y * deltaTime);
}

void Enemy::checkPlatformCollisions(const std::vector<std::unique_ptr<Platform>>& platforms) {
    sf::FloatRect enemyBounds = getBounds();

    for (const auto& platform : platforms) {
        sf::FloatRect platformBounds = platform->getBounds();

        if (enemyBounds.intersects(platformBounds)) {
            
            if (velocity.y > 0 && (enemyBounds.top + enemyBounds.height) <= (platformBounds.top + 25.f)) {
                
                float penetration = (enemyBounds.top + enemyBounds.height) - platformBounds.top;
                
                sprite.move(0.f, -penetration);
                
                velocity.y = 0;
            }
        }
    }
}

void Enemy::draw(sf::RenderWindow& window) {
    if (health <= 0 && currentState == EnemyState::DEATH && animManager->isFinished()) return;
    window.draw(sprite);
}

void Enemy::takeDamage(int damage) {
    if (health <= 0 || !canBeHit()) return;
    health -= damage;
    invulnerabilityTimer = 0.4f;
    setAnimation(EnemyState::TAKE_HIT);
    if (health <= 0) health = 0;
}

bool Enemy::canBeHit() const { return invulnerabilityTimer <= 0.f; }

void Enemy::setPosition(const sf::Vector2f& pos) { sprite.setPosition(pos); }
sf::Vector2f Enemy::getPosition() const { return sprite.getPosition(); }

void Enemy::setAnimation(EnemyState newState) {
    if (currentState == newState) return;
    if (currentState == EnemyState::TAKE_HIT && !animManager->isFinished()) return;
    if (currentState == EnemyState::DEATH) return;

    currentState = newState;
    switch (currentState) {
        case EnemyState::IDLE: animManager->play("IDLE", true); break;
        case EnemyState::WALK: animManager->play("WALK", true); break;
        case EnemyState::TAKE_HIT: animManager->play("TAKE_HIT", false); break;
        case EnemyState::DEATH: animManager->play("DEATH", false); break;
        default: break; 
    }
}