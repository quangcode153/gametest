#include "Enemies/Bat.hpp"
#include "Player.hpp"
#include <iostream>
#include <cmath>

Bat::Bat() : flySpeed(300.f), visionRange(600.f), attackRange(40.f), attackCount(0), attackCooldown(0.f) {
    this->health = 50;
    this->maxHealth = 50;
    this->gravityEnabled = false;
}

Bat::~Bat() {}

void Bat::init(const std::string& folder) {
    animManager->clearAnimations();
    this->frameSize = sf::Vector2i(150, 150);

    this->hitboxOffsetTop = 53.f;
    this->hitboxReduceWidth = 113.f;
    this->hitboxReduceHeight = 95.f;
    this->hitboxOffsetLeft = 25.f;

    animManager->addAnimation("IDLE", folder + "Flight.png", frameSize, 4, 0.1f);
    animManager->addAnimation("WALK", folder + "Flight.png", frameSize, 4, 0.1f);
    animManager->addAnimation("ATTACK_1", folder + "Attack1.png", frameSize, 4, 0.1f);
    animManager->addAnimation("ATTACK_2", folder + "Attack2.png", frameSize, 4, 0.1f);
    animManager->addAnimation("ATTACK_3", folder + "Attack3.png", frameSize, 4, 0.1f);
    animManager->addAnimation("TAKE_HIT", folder + "Take Hit.png", frameSize, 4, 0.1f);
    animManager->addAnimation("DEATH", folder + "Death.png", frameSize, 4, 0.15f);

    sprite.setOrigin(frameSize.x / 2.f, frameSize.y / 2.f);
    sprite.setScale(1.0f, 1.0f);
    setAnimation(EnemyState::WALK);
}

void Bat::updateAI(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms, Player* player) {
    if (attackCooldown > 0.f) {
        attackCooldown -= deltaTime;
    }

    if ((currentState == EnemyState::ATTACK || currentState == EnemyState::TAKE_HIT) && animManager->isFinished()) {
        currentState = EnemyState::WALK;
        setAnimation(EnemyState::WALK);
    }

    if (currentState == EnemyState::TAKE_HIT || currentState == EnemyState::DEATH) {
        velocity.x = 0;
        velocity.y = 0;
        return;
    }

    float distanceToPlayer = 0.f;
    bool playerDetected = false;
    sf::Vector2f direction(0.f, 0.f);

    if (player) {
        sf::Vector2f playerPos = player->getPosition();
        sf::Vector2f targetPos = {playerPos.x, playerPos.y - 30.f};
        sf::Vector2f myPos = getPosition();

        float dx = targetPos.x - myPos.x;
        float dy = targetPos.y - myPos.y;
        distanceToPlayer = std::sqrt(dx * dx + dy * dy);

        if (distanceToPlayer < visionRange) {
            playerDetected = true;
            if (distanceToPlayer > 0) {
                direction.x = dx / distanceToPlayer;
                direction.y = dy / distanceToPlayer;
            }
        }
    }

    if (playerDetected && distanceToPlayer <= attackRange) {
        velocity.x = 0;
        velocity.y = 0;

        if (currentState != EnemyState::ATTACK && attackCooldown <= 0.f) {
            attackCount++;
            int attackVariant = attackCount % 3;

            if (attackVariant == 0) animManager->play("ATTACK_1", false);
            else if (attackVariant == 1) animManager->play("ATTACK_2", false);
            else animManager->play("ATTACK_3", false);

            currentState = EnemyState::ATTACK;
            attackCooldown = 1.5f;
        } else if (currentState != EnemyState::ATTACK) {
            setAnimation(EnemyState::IDLE);
        }
    } else if (playerDetected && currentState != EnemyState::ATTACK) {
        velocity.x = direction.x * flySpeed;
        velocity.y = direction.y * flySpeed;
        setAnimation(EnemyState::WALK);

        if (velocity.x > 0) movingRight = true;
        else movingRight = false;
    } else if (currentState != EnemyState::ATTACK) {
        velocity.x = 0;
        velocity.y = 0;
        setAnimation(EnemyState::IDLE);
    }

    sprite.move(velocity * deltaTime);

    float scaleX = std::abs(sprite.getScale().x);
    float scaleY = sprite.getScale().y;
    if (movingRight) sprite.setScale(scaleX, scaleY);
    else sprite.setScale(-scaleX, scaleY);
}
