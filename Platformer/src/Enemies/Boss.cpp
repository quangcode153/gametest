#include "Boss.hpp"
#include <iostream>
#include <cmath>
#include <SFML/Graphics/RectangleShape.hpp>

Boss::Boss() : attackCount(0), attackCooldown(0.f), hasDealtDamage(false) {
    this->health = 1000;
    this->maxHealth = 1000;
    this->visionRange = 1000.f;
    
    this->attackRange = 300.f;
    
    this->moveSpeed = 100.f; 
    this->gravityEnabled = true;
}

Boss::~Boss() {}

void Boss::init(const std::string& folder) {
    animManager->clearAnimations();
    this->frameSize = sf::Vector2i(250, 250);

    this->hitboxReduceWidth = 450.f;  
    this->hitboxReduceHeight = 450.f; 
    this->hitboxOffsetTop = 0.f;
    this->hitboxOffsetLeft = 0.f;

    std::string spriteFolder = folder + "/Sprites/";

    animManager->addAnimation("IDLE", spriteFolder + "Idle.png", frameSize, 8, 0.15f);
    animManager->addAnimation("WALK", spriteFolder + "Run.png", frameSize, 6, 0.1f);
    animManager->addAnimation("ATTACK_1", spriteFolder + "Attack1.png", frameSize, 8, 0.1f);
    animManager->addAnimation("ATTACK_2", spriteFolder + "Attack2.png", frameSize, 8, 0.1f);
    animManager->addAnimation("TAKE_HIT", spriteFolder + "Take hit.png", frameSize, 3, 0.1f);
    animManager->addAnimation("DEATH", spriteFolder + "Death.png", frameSize, 5, 0.15f);

    sprite.setOrigin(frameSize.x / 2.f, frameSize.y / 2.f);
    sprite.setScale(2.5f, 2.5f); 
    
    setAnimation(EnemyState::IDLE);
}

void Boss::facePlayer(Player* player) {
    if (!player) return;
    if (currentState == EnemyState::ATTACK) return; 

    if (player->getPosition().x < this->getPosition().x) {
        movingRight = false; 
    } else {
        movingRight = true; 
    }

    float scaleX = std::abs(sprite.getScale().x);
    float scaleY = sprite.getScale().y;
    if (movingRight) sprite.setScale(scaleX, scaleY);
    else sprite.setScale(-scaleX, scaleY);
}

void Boss::updateAI(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms, Player* player) {
    if (attackCooldown > 0.f) attackCooldown -= deltaTime;

    if (currentState == EnemyState::ATTACK && animManager->isFinished()) {
        setAnimation(EnemyState::IDLE);
    }

    if (currentState == EnemyState::TAKE_HIT || currentState == EnemyState::DEATH) {
        velocity.x = 0;
        sprite.move(velocity.x * deltaTime, 0.f);
        if (animManager->isFinished()) currentState = EnemyState::IDLE; 
        return;
    }

    float distanceToPlayer = 0.f;
    float directionToPlayer = 0.f; 
    bool playerDetected = false;
    
    if (player) {
        float dx = player->getPosition().x - this->getPosition().x;
        float dy = player->getPosition().y - this->getPosition().y;
        
        if (std::abs(dx) < visionRange && std::abs(dy) < 300.f) { 
            playerDetected = true;
            directionToPlayer = (dx > 0) ? 1.f : -1.f;
            distanceToPlayer = std::sqrt(dx*dx + dy*dy); 
        } else {
            playerDetected = false;
            distanceToPlayer = 9999.f; 
        }
    }

    if (currentState == EnemyState::ATTACK) {
        velocity.x = 0; 

        int currentFrame = animManager->getCurrentFrameIndex();
        
        bool inDamageWindow = (currentFrame >= 3 && currentFrame <= 5);
        
        if (inDamageWindow && !hasDealtDamage && player) {
            sf::FloatRect attackHitbox = this->calculateAttackHitbox();
            sf::FloatRect playerBox = player->getHitbox();
            
            if (attackHitbox.intersects(playerBox)) {
                if (!player->isInvulnerable()) {
                    player->takeDamage();
                    std::cout << "========================================" << std::endl;
                    std::cout << "🗡️  BOSS HIT PLAYER! (Frame " << currentFrame << ")" << std::endl;
                    std::cout << "========================================" << std::endl;
                    hasDealtDamage = true;
                } else {
                    std::cout << "[Boss] Player is invulnerable/shielded - no damage" << std::endl;
                }
            }
        }
    }
    else if (playerDetected && distanceToPlayer <= attackRange && attackCooldown <= 0.f) { 
        velocity.x = 0;
        facePlayer(player);
        
        attackCount++; 
        int attackVariant = attackCount % 2;
        if (attackVariant == 0) animManager->play("ATTACK_1", false);
        else animManager->play("ATTACK_2", false);
        
        currentState = EnemyState::ATTACK; 
        attackCooldown = 3.0f;
        hasDealtDamage = false;
    } 
    else if (playerDetected && attackCooldown <= 0.f) {
        bool safeToChase = checkPlatformEdge(platforms, directionToPlayer);
        if (safeToChase) {
            velocity.x = (directionToPlayer > 0) ? moveSpeed : -moveSpeed;
            movingRight = (directionToPlayer > 0);
            setAnimation(EnemyState::WALK);
        } else {
            velocity.x = 0;
            setAnimation(EnemyState::IDLE);
            movingRight = (directionToPlayer > 0);
        }
    }
    else if (attackCooldown <= 0.f) {
        float currentDir = movingRight ? 1.f : -1.f;
        if (!checkPlatformEdge(platforms, currentDir)) {
            movingRight = !movingRight;
        }
        velocity.x = movingRight ? moveSpeed : -moveSpeed;
        setAnimation(EnemyState::WALK);
    }

    sprite.move(velocity.x * deltaTime, 0.f);

    float scaleX = std::abs(sprite.getScale().x);
    float scaleY = sprite.getScale().y;
    if (movingRight) sprite.setScale(scaleX, scaleY);
    else sprite.setScale(-scaleX, scaleY);
}

bool Boss::checkPlatformEdge(const std::vector<std::unique_ptr<Platform>>& platforms, float directionX) {
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

void Boss::draw(sf::RenderWindow& window) {
    Enemy::draw(window);
}

sf::FloatRect Boss::getBounds() const {
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();
    float newWidth = spriteBounds.width - hitboxReduceWidth;
    float newHeight = spriteBounds.height - hitboxReduceHeight;
    float newLeft = (spriteBounds.left + spriteBounds.width / 2.f) - (newWidth / 2.f);
    float newTop = (spriteBounds.top + spriteBounds.height / 2.f) - (newHeight / 2.f);
    return sf::FloatRect(newLeft, newTop, newWidth, newHeight);
}

sf::FloatRect Boss::calculateAttackHitbox() const {
    float hitboxW = 200.f;
    float hitboxH = 150.f;
    float hitboxX, hitboxY;

    hitboxY = getPosition().y - (hitboxH / 2.f); 
    
    if (movingRight) {
        hitboxX = getPosition().x + 30.f;
    } else {
        hitboxX = getPosition().x - hitboxW - 30.f;
    }
    
    return sf::FloatRect(hitboxX, hitboxY, hitboxW, hitboxH);
}

void Boss::takeDamage(int damage) {
    if (health <= 0 || !canBeHit()) return;

    health -= damage;
    invulnerabilityTimer = 0.4f;

    if (health <= 0) {
        health = 0;
        setAnimation(EnemyState::DEATH);
    }
}