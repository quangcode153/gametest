#include "Player.hpp"
#include <iostream>
#include <algorithm>
#include "Constants.hpp" 

Player::Player() :
    frameCount(1),
    currentFrameIndex(0),
    animationTimer(0.f),
    frameTime(0.1f),
    facingRight(true),
    velocity(0.f, 0.f),
    moveSpeed(300.0f),
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
    poweredScale(1.5f),
    totalCoins(0) // <-- THÊM DÒNG NÀY: Khởi tạo totalCoins
{
}

Player::~Player() {}

bool Player::loadTexture(const std::string& path) {
    std::vector<std::string> paths = {
        path,
        "../" + path,
        "assets/player.png" // Fallback path, you might need to adjust this
    };
    
    sf::Image playerImage; 
    bool loaded = false;

    for (const auto& tryPath : paths) {
        std::cout << "Trying player: " << tryPath << "... ";
        if (playerImage.loadFromFile(tryPath)) { 
            std::cout << "SUCCESS!\n";
            loaded = true;
            break;
        } else {
            std::cout << "failed\n";
        }
    }
    
    if (!loaded) {
        std::cout << "✗ Cannot load player texture from any path\n";
        
        sf::Image fallbackImage;
        fallbackImage.create(50, 50, sf::Color::Transparent); // Create a transparent fallback
        
        texture.loadFromImage(fallbackImage);
        std::cout << "✓ Fallback player texture created\n";

    } else {
        playerImage.createMaskFromColor(sf::Color::White); // Assuming white is the background to be masked
        if (!texture.loadFromImage(playerImage)) {
             std::cerr << "Loi: Khong the nap texture tu image player da xu ly." << std::endl;
             return false;
        }
    }

    texture.setSmooth(false); 
    sprite.setTexture(texture);

    sf::Vector2u textureSize = texture.getSize();
    frameCount = 1; // Assuming a single frame for now, adjust if you have animation in player.png
    currentFrame = sf::IntRect(0, 0, textureSize.x / frameCount, textureSize.y);
    sprite.setTextureRect(currentFrame);

    sprite.setOrigin(currentFrame.width / 2.f, currentFrame.height);

    float baseSize = 32.f; 
    #ifdef TILE_SIZE
        baseSize = TILE_SIZE;
    #endif
    
    float scaleX = baseSize / currentFrame.width;
    float scaleY = baseSize / currentFrame.height;
    normalScale = std::min(scaleX, scaleY) * 2.0f; // Scale to be about 2 tiles high
    poweredScale = normalScale * 1.2f; // Bigger when powered up
    sprite.setScale(normalScale, normalScale);
    
    std::cout << "Player sprite configured: scale=" << normalScale 
              << ", size=" << sprite.getGlobalBounds().width << "x" 
              << sprite.getGlobalBounds().height << "\n";
    
    return true;
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

void Player::moveLeft() {
    velocity.x = -moveSpeed;
    facingRight = false;
}

void Player::moveRight() {
    velocity.x = moveSpeed;
    facingRight = true;
}

void Player::jump() {
    if (isOnGround) {
        velocity.y = jumpStrength;
        isOnGround = false;
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

    
    sprite.move(velocity.x * deltaTime, 0.f); 
    checkCollisionX(platforms);

    
    applyGravity(deltaTime);
    sprite.move(0.f, velocity.y * deltaTime); 
    checkCollisionY(platforms);

    
    updateAnimation(deltaTime);

    
    float currentScale = isPoweredUp ? poweredScale : normalScale;
    float scaleX = (facingRight ? currentScale : -currentScale);
    sprite.setScale(scaleX, currentScale);
}

void Player::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;
    if (animationTimer >= frameTime) {
        animationTimer = 0.f;
        if (std::abs(velocity.x) > 0.1f) {
            currentFrameIndex = (currentFrameIndex + 1) % frameCount;
        } else {
            currentFrameIndex = 0;
        }
        currentFrame.left = currentFrameIndex * currentFrame.width;
        sprite.setTextureRect(currentFrame);
    }
}

void Player::applyGravity(float deltaTime) {
    velocity.y += gravity * deltaTime; 
    if (velocity.y > maxFallSpeed)
        velocity.y = maxFallSpeed;
}


void Player::checkCollisionX(const std::vector<std::unique_ptr<Platform>>& platforms) {
    sf::FloatRect playerBounds = getBounds();
    for (const auto& platform : platforms) {
        sf::FloatRect platformBounds = platform->getBounds();
        if (playerBounds.intersects(platformBounds)) {
            if (velocity.x > 0) { 
                sprite.move(-(playerBounds.left + playerBounds.width - platformBounds.left), 0.f);
            } else if (velocity.x < 0) { 
                sprite.move(platformBounds.left + platformBounds.width - playerBounds.left, 0.f);
            }
            velocity.x = 0;
        }
    }
}

void Player::checkCollisionY(const std::vector<std::unique_ptr<Platform>>& platforms) {
    isOnGround = false;
    sf::FloatRect playerBounds = getBounds();
    for (const auto& platform : platforms) {
        sf::FloatRect platformBounds = platform->getBounds();
        if (playerBounds.intersects(platformBounds)) {
            if (velocity.y > 0) { 
                sprite.move(0.f, -(playerBounds.top + playerBounds.height - platformBounds.top));
                velocity.y = 0;
                isOnGround = true;
            } else if (velocity.y < 0) { 
                sprite.move(0.f, platformBounds.top + platformBounds.height - playerBounds.top);
                velocity.y = 0;
            }
        }
    }
}


void Player::takeDamage() {
    if (invulnerabilityTimer <= 0.f && !isPoweredUp) {
        health--;
        invulnerabilityTimer = 2.0f;
        std::cout << "Player hit! Health: " << health << "\n";
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
    return invulnerabilityTimer > 0.f || isPoweredUp;
}

void Player::draw(sf::RenderWindow& window) {
    if (invulnerabilityTimer > 0.f) {
        if (static_cast<int>(invulnerabilityTimer * 10) % 2 == 0) return; // Blinking effect
    }
    if (isPoweredUp)
        sprite.setColor(sf::Color(255, 255, 150)); // Yellowish when powered up
    else
        sprite.setColor(sf::Color::White); // Normal color
    window.draw(sprite);
}

// CÁC HÀM MỚI CHO COIN
void Player::addCoin(int amount) {
    totalCoins += amount;
    // std::cout << "Player collected " << amount << " coin(s). Total: " << totalCoins << std::endl; // Debug output
}

int Player::getTotalCoins() const {
    return totalCoins;
}

void Player::setTotalCoins(int amount) {
    totalCoins = amount;
}
void Player::heal(int amount) {
    health += amount;
    if (health > maxHealth) {
        health = maxHealth; // Đảm bảo máu không vượt quá tối đa
    }
    std::cout << "Player healed " << amount << " health. Current health: " << health << "\n";
}