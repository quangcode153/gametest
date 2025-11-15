#include "Enemies/Goblin.hpp"
#include "Player.hpp"
#include <iostream>
#include <cmath>


Goblin::Goblin() : moveSpeed(130.f), visionRange(600.f), attackRange(40.f), attackCount(0), attackCooldown(0.f) {
    this->health = 100;
    this->maxHealth = 100;
}

Goblin::~Goblin() {}

void Goblin::init(const std::string& folder) {
    animManager->clearAnimations();
    this->frameSize = sf::Vector2i(150, 150); 

    
    this->hitboxOffsetTop = 50.f; 
    this->hitboxReduceWidth = 120.f;
    this->hitboxReduceHeight = 100.f; 
    
    this->hitboxOffsetLeft = 75.f; 
  
    animManager->addAnimation("IDLE", folder + "Idle.png", frameSize, 4, 0.15f);
    animManager->addAnimation("WALK", folder + "Run.png", frameSize, 8, 0.1f);

    animManager->addAnimation("ATTACK_1", folder + "Attack1.png", frameSize, 8, 0.1f);
    animManager->addAnimation("ATTACK_2", folder + "Attack2.png", frameSize, 8, 0.1f);
    animManager->addAnimation("ATTACK_3", folder + "Attack3.png", frameSize, 8, 0.1f);
    
    animManager->addAnimation("TAKE_HIT", folder + "Take Hit.png", frameSize, 4, 0.1f);
    animManager->addAnimation("DEATH", folder + "Death.png", frameSize, 4, 0.15f);

    sprite.setOrigin(frameSize.x / 2.f, frameSize.y / 2.f);
    sprite.setScale(1.0f, 1.0f);
    setAnimation(EnemyState::WALK);
}

void Goblin::updateAI(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms, Player* player) {
    // Giảm hồi chiêu
    if (attackCooldown > 0.f) attackCooldown -= deltaTime;

    // Logic Reset trạng thái
    if ((currentState == EnemyState::ATTACK || currentState == EnemyState::TAKE_HIT) && animManager->isFinished()) {
        currentState = EnemyState::IDLE; 
    }

    if (currentState == EnemyState::TAKE_HIT || currentState == EnemyState::DEATH) {
        velocity.x = 0;
        return;
    }

    float distanceToPlayer = 0.f;
    float directionToPlayer = 0.f;
    bool playerDetected = false;

    if (player) {
        float dx = player->getPosition().x - this->getPosition().x;
        float dy = player->getPosition().y - this->getPosition().y;
        distanceToPlayer = std::sqrt(dx*dx + dy*dy);
        
        if (distanceToPlayer < visionRange && std::abs(dy) < 50.f) { 
            playerDetected = true;
            directionToPlayer = (dx > 0) ? 1.f : -1.f;
        }
    }

    // 1. TẤN CÔNG
    if (playerDetected && distanceToPlayer <= attackRange) {
        velocity.x = 0;
        if (currentState != EnemyState::ATTACK && attackCooldown <= 0.f) {
            attackCount++; 
            int attackVariant = attackCount % 3; 

            if (attackVariant == 0) animManager->play("ATTACK_1", false);
            else if (attackVariant == 1) animManager->play("ATTACK_2", false);
            else animManager->play("ATTACK_3", false);

            currentState = EnemyState::ATTACK; 
            attackCooldown = 1.0f; // Goblin đánh nhanh, nghỉ 1s
        }
    }
    // 2. RƯỢT ĐUỔI
    else if (playerDetected && currentState != EnemyState::ATTACK) {
        bool safeToChase = checkPlatformEdge(platforms, directionToPlayer);

        if (safeToChase) {
            if (directionToPlayer > 0) {
                velocity.x = moveSpeed;
                movingRight = true;
            } else {
                velocity.x = -moveSpeed;
                movingRight = false;
            }
            setAnimation(EnemyState::WALK);
        } else {
            velocity.x = 0;
            setAnimation(EnemyState::IDLE);
            if (directionToPlayer > 0) movingRight = true;
            else movingRight = false;
        }
    }
    // 3. TUẦN TRA
    else if (currentState != EnemyState::ATTACK) {
        float currentDir = movingRight ? 1.f : -1.f;
        if (!checkPlatformEdge(platforms, currentDir)) {
            movingRight = !movingRight;
        }
        
        if (movingRight) velocity.x = moveSpeed;
        else velocity.x = -moveSpeed;
        
        setAnimation(EnemyState::WALK);
    }

    sprite.move(velocity.x * deltaTime, 0.f);
    
    float scaleX = std::abs(sprite.getScale().x);
    float scaleY = sprite.getScale().y;
    if (movingRight) sprite.setScale(scaleX, scaleY);
    else sprite.setScale(-scaleX, scaleY);
}

bool Goblin::checkPlatformEdge(const std::vector<std::unique_ptr<Platform>>& platforms, float directionX) {
    sf::FloatRect bounds = getBounds();
    sf::Vector2f feelerPoint;
    feelerPoint.y = bounds.top + bounds.height + 10.f;

    if (directionX > 0) feelerPoint.x = bounds.left + bounds.width + 20.f;
    else feelerPoint.x = bounds.left - 20.f;

    bool groundAhead = false;
    for (const auto& platform : platforms) {
        if (platform->getBounds().contains(feelerPoint)) {
            groundAhead = true;
            break;
        }
    }
    return groundAhead;
}