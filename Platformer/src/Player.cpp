#include "Player.hpp"
#include "AnimationManager.hpp"
#include "Platform.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

Player::Player() :
    animManager(std::make_unique<AnimationManager>(sprite)),
    currentState(State::IDLE),
    facingRight(true),
    velocity(0.f, 0.f),
    moveSpeed(300.0f),
    normalMoveSpeed(300.0f),
    isBoosted(false),
    boostTimer(0.f),
    isShielded(false),
    shieldTimer(0.f),
    jumpStrength(-800.0f),
    gravity(2000.0f),
    maxFallSpeed(700.0f),
    isOnGround(false),
    health(3),
    maxHealth(3),
    invulnerabilityTimer(0.f),
    isPoweredUp(false),
    powerupTimer(0.f),
    normalScale(1.0f),
    poweredScale(1.2f),
    totalCoins(0),
    currentAssetFolder("")
{
    normalMoveSpeed = moveSpeed;
}

Player::~Player() {}

bool Player::loadTexture(const std::string& folderPath) {
    currentAssetFolder = folderPath;
    loadAnimations(folderPath);
    return true;
}

void Player::loadAnimations(const std::string& folder) {
    float currentDesiredHeight = 300.f;
    if (animManager) {
        animManager->clearAnimations();
    }

    if (folder.find("kiem") != std::string::npos) {
        this->frameSize = sf::Vector2i(180, 180);
        this->localHitbox = sf::FloatRect(76.f, 60.f, 40.f, 50.f);

        currentDesiredHeight = 300.f;
        animManager->addAnimation("IDLE", folder + "Idle.png", frameSize, 11, 0.1f);
        animManager->addAnimation("RUNNING", folder + "Run.png", frameSize, 8, 0.08f);
        animManager->addAnimation("JUMPING", folder + "Jump.png", frameSize, 3, 0.1f);
        animManager->addAnimation("FALLING", folder + "Fall.png", frameSize, 3, 0.1f);
        animManager->addAnimation("ATTACK1", folder + "Attack1.png", frameSize, 7, 0.07f);
        animManager->addAnimation("TAKE_HIT", folder + "Take Hit.png", frameSize, 4, 0.1f);
        animManager->addAnimation("DEATH", folder + "Death.png", frameSize, 11, 0.1f);
        animManager->addAnimation("ATTACK2", folder + "Attack2.png", frameSize, 7, 0.07f);

    } else if (folder.find("phep") != std::string::npos) {
        this->frameSize = sf::Vector2i(231, 190);
        this->localHitbox = sf::FloatRect(79.f, 55.f, 55.f, 85.f);

        currentDesiredHeight = 200.f;
        animManager->addAnimation("IDLE", folder + "Idle.png", frameSize, 6, 0.1f);
        animManager->addAnimation("RUNNING", folder + "Run.png", frameSize, 8, 0.08f);
        animManager->addAnimation("JUMPING", folder + "Jump.png", frameSize, 2, 0.1f);
        animManager->addAnimation("FALLING", folder + "Fall.png", frameSize, 2, 0.1f);
        animManager->addAnimation("ATTACK1", folder + "Attack1.png", frameSize, 8, 0.07f);
        animManager->addAnimation("TAKE_HIT", folder + "Take Hit.png", frameSize, 4, 0.1f);
        animManager->addAnimation("DEATH", folder + "Death.png", frameSize, 7, 0.1f);
        animManager->addAnimation("CASTING", folder + "Attack2.png", frameSize, 8, 0.12f);
    }

    normalScale = currentDesiredHeight / this->frameSize.y;
    poweredScale = normalScale * 1.2f;

    sprite.setScale(normalScale, normalScale);
    std::cout << "Player scale set to: " << normalScale << " (Base height: " << this->frameSize.y << ")" << std::endl;

    setAnimation(State::IDLE);
    std::cout << "Player animations loaded from: " << folder << std::endl;
}


void Player::setPosition(const sf::Vector2f& pos) {
    sprite.setPosition(pos);
}

sf::Vector2f Player::getPosition() const {
    return sprite.getPosition();
}

sf::FloatRect Player::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::FloatRect Player::getHitbox() const {
    return sprite.getTransform().transformRect(localHitbox);
}

float Player::getScaledFootOffset() const {
    if (currentAssetFolder.find("phep") != std::string::npos) {
        return 0.f;
    } else {
        return (localHitbox.top + localHitbox.height) * std::abs(sprite.getScale().y);
    }
}


void Player::moveLeft() {
    velocity.x = -moveSpeed;
    facingRight = false;
}

void Player::moveRight() {
    velocity.x = moveSpeed;
    facingRight = true;
}

void Player::jump() {
    if (isOnGround && currentState != State::JUMPING && currentState != State::FALLING) {
        velocity.y = jumpStrength;
        isOnGround = false;
        setAnimation(State::JUMPING);
    }
}

void Player::stopMoving() {
    velocity.x = 0.f;
}

void Player::update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms) {

    if (invulnerabilityTimer > 0.f) invulnerabilityTimer -= deltaTime;
    if (isPoweredUp) {
        powerupTimer -= deltaTime;
        if (powerupTimer <= 0.f) {
            isPoweredUp = false;
        }
    }
    if (isBoosted) {
        boostTimer -= deltaTime;
        if (boostTimer <= 0.f) {
            isBoosted = false;
            moveSpeed = normalMoveSpeed;
            std::cout << "Speed Boost expired!" << std::endl;
        }
    }
    if (isShielded) {
        shieldTimer -= deltaTime;
        if (shieldTimer <= 0.f) {
            isShielded = false;
            std::cout << "Shield expired!" << std::endl;
        }
    }

    if (currentState == State::DEATH) {
        animManager->update(deltaTime);
        return;
    }

    applyGravity(deltaTime);

    sprite.move(velocity.x * deltaTime, 0.f);
    checkCollisionX(platforms);

    sprite.move(0.f, velocity.y * deltaTime);
    checkCollisionY(platforms);

    bool animFinished = animManager->isFinished();
    State nextState = currentState;

    if (currentState == State::ATTACK1 && !animFinished) { /* Giữ nguyên */ }
    else if (currentState == State::ATTACK2 && !animFinished) { /* Giữ nguyên */ }
    else if (currentState == State::CASTING && !animFinished) { /* Giữ nguyên */ }
    else if (currentState == State::TAKE_HIT && !animFinished) { /* Giữ nguyên */ }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::J) && isOnGround) {
        nextState = State::ATTACK1;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::K) && isOnGround) {
        if (currentAssetFolder.find("phep") != std::string::npos) {
            nextState = State::CASTING;
        } else {
            nextState = State::ATTACK2;
        }
    }
    else if (velocity.y < -0.1f) {
        nextState = State::JUMPING;
    }
    else if (velocity.y > 0.1f && !isOnGround) {
        nextState = State::FALLING;
    }
    else if (std::abs(velocity.x) > 0.1f && isOnGround) {
        nextState = State::RUNNING;
    }
    else if (isOnGround) {
        nextState = State::IDLE;
    }

    if (nextState != currentState || (currentState != State::IDLE && currentState != State::RUNNING && currentState != State::FALLING && animFinished)) {
        setAnimation(nextState);
    }


    animManager->update(deltaTime);

    float currentScale = isPoweredUp ? poweredScale : normalScale;
    float scaleX = (facingRight ? currentScale : -currentScale);
    sprite.setScale(scaleX, currentScale);
}

void Player::setAnimation(State newState) {
    if (currentState == State::DEATH) return;
    if (newState == currentState) return;

    if ((currentState == State::ATTACK1 || currentState == State::ATTACK2 || currentState == State::CASTING || currentState == State::TAKE_HIT) && !animManager->isFinished()) {
        if (!(newState == State::JUMPING || newState == State::FALLING || newState == State::DEATH)) {
            return;
        }
    }

    currentState = newState;

    switch (currentState) {
        case State::IDLE:     animManager->play("IDLE", true);   break;
        case State::RUNNING:  animManager->play("RUNNING", true);   break;
        case State::JUMPING:  animManager->play("JUMPING", false);  break;
        case State::FALLING:  animManager->play("FALLING", true);   break;
        case State::ATTACK1:  animManager->play("ATTACK1", false);  break;
        case State::ATTACK2:  animManager->play("ATTACK2", false);  break;
        case State::CASTING:  animManager->play("CASTING", false);  break;
        case State::TAKE_HIT: animManager->play("TAKE_HIT", false); break;
        case State::DEATH:    animManager->play("DEATH", false);   break;
    }
}

void Player::applyGravity(float deltaTime) {
    velocity.y += gravity * deltaTime;
    if (velocity.y > maxFallSpeed)
        velocity.y = maxFallSpeed;
}

void Player::checkCollisionX(const std::vector<std::unique_ptr<Platform>>& platforms) {
    sf::FloatRect playerBounds = getHitbox();

    for (const auto& platform : platforms) {
        sf::FloatRect platformBounds = platform->getBounds();

        if (playerBounds.intersects(platformBounds)) {
            if (velocity.x > 0) {
                sprite.move(-(playerBounds.left + playerBounds.width - platformBounds.left), 0.f);
            } else if (velocity.x < 0) {
                sprite.move(platformBounds.left + platformBounds.width - playerBounds.left, 0.f);
            }
            velocity.x = 0;
            playerBounds = getHitbox();
        }
    }
}

void Player::checkCollisionY(const std::vector<std::unique_ptr<Platform>>& platforms) {
    isOnGround = false;
    sf::FloatRect playerBounds = getHitbox();

    for (const auto& platform : platforms) {
        sf::FloatRect platformBounds = platform->getBounds();

        if (playerBounds.intersects(platformBounds)) {
            if (velocity.y > 0) {
                sprite.move(0.f, -(playerBounds.top + playerBounds.height - platformBounds.top));
                velocity.y = 0;
                isOnGround = true;
            }
            else if (velocity.y < 0) {
                sprite.move(0.f, platformBounds.top + platformBounds.height - playerBounds.top);
                velocity.y = 0;
            }
            playerBounds = getHitbox();
        }
    }
}

void Player::draw(sf::RenderWindow& window) {
    if (invulnerabilityTimer > 0.f) {
        if (static_cast<int>(invulnerabilityTimer * 10) % 2 == 0) return;
    }
    if (isPoweredUp)
        sprite.setColor(sf::Color(255, 255, 150));
    else if (isBoosted)
        sprite.setColor(sf::Color(100, 100, 255));
    else if (isShielded)
        sprite.setColor(sf::Color(200, 200, 255, 200));
    else
        sprite.setColor(sf::Color::White);

    window.draw(sprite);

    
}

void Player::takeDamage() {
    if (currentState == State::DEATH) return;


    if (invulnerabilityTimer > 0.f || isPoweredUp) {
        return;
    }


    if (isShielded) {
        isShielded = false;
        shieldTimer = 0.f;
        std::cout << "Shield blocked 1 hit and broke!" << std::endl;

        invulnerabilityTimer = 1.0f;
        setAnimation(State::TAKE_HIT);
    }

    else {
        health--;
        invulnerabilityTimer = 2.0f;
        std::cout << "Player hit! Health: " << health << "\n";
        if (health <= 0) { setAnimation(State::DEATH); }
        else { setAnimation(State::TAKE_HIT); }
    }
}


bool Player::canTakeDamage() const {
    return invulnerabilityTimer <= 0.f && !isPoweredUp;
}

void Player::activatePowerup(float duration) {
    isPoweredUp = true;
    powerupTimer = duration;
    std::cout << "Powerup activated for " << duration << " seconds!\n";
}

bool Player::isInvulnerable() const {
    return invulnerabilityTimer > 0.f || isPoweredUp || isShielded;
}

void Player::addCoin(int amount) { totalCoins += amount; }
int Player::getTotalCoins() const { return totalCoins; }
void Player::setTotalCoins(int amount) { totalCoins = amount; }
void Player::heal(int amount) {
    health += amount;
    if (health > maxHealth) { health = maxHealth; }
    if (health == maxHealth && currentState == State::DEATH) {
        currentState = State::IDLE;
        animManager->play("IDLE", true);
    }
    std::cout << "Player healed " << amount << " health. Current health: " << health << "\n";
}
void Player::activateSpeedBoost(float duration) {
    if (!isBoosted) { moveSpeed = normalMoveSpeed * 1.5f; }
    isBoosted = true;
    boostTimer = duration;
    std::cout << "Speed Boost activated for " << duration << "s!" << std::endl;
}
void Player::activateShield(float duration) {
    isShielded = true;
    shieldTimer = duration;
    std::cout << "Shield activated for " << duration << "s or 1 hit!" << std::endl;
}

State Player::getCurrentState() const {
    return currentState;
}

bool Player::hasDeathAnimationFinished() const {
    if (currentState == State::DEATH && animManager) {
        return animManager->isFinished();
    }
    return false;
}