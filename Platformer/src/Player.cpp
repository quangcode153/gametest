#include "Player.hpp"
#include <iostream>
#include <algorithm>
#include "Constants.hpp" // (Giả sử file này định nghĩa TILE_SIZE)

// --- HÀM KHỞI TẠO (ĐÃ SỬA VẬT LÝ + TĂNG ĐỘ NHẢY) ---
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
    poweredScale(1.5f)
{
}

Player::~Player() {}

// --- LOAD TEXTURE (ĐÃ SỬA ĐỂ XÓA VIỀN TRẮNG) ---
bool Player::loadTexture(const std::string& path) {
    std::vector<std::string> paths = {
        path,
        "../" + path,
        "assets/player.png"
    };
    
    sf::Image playerImage; // <-- Dùng sf::Image để xử lý
    bool loaded = false;

    for (const auto& tryPath : paths) {
        std::cout << "Trying player: " << tryPath << "... ";
        if (playerImage.loadFromFile(tryPath)) { // <-- Tải vào Image
            std::cout << "SUCCESS!\n";
            loaded = true;
            break;
        } else {
            std::cout << "failed\n";
        }
    }
    
    if (!loaded) {
        std::cout << "✗ Cannot load player texture from any path\n";
        
        // (Code fallback của bạn)
        sf::Image fallbackImage;
        fallbackImage.create(50, 50, sf::Color::Transparent);
        // ... (phần code vẽ fallback của bạn) ...
        texture.loadFromImage(fallbackImage);
        std::cout << "✓ Fallback player texture created\n";

    } else {
        // Tải thành công vào playerImage
        // *** ĐÂY LÀ PHẦN SỬA LỖI VIỀN TRẮNG ***
        playerImage.createMaskFromColor(sf::Color::White);
        // ************************************

        if (!texture.loadFromImage(playerImage)) {
             std::cerr << "Loi: Khong the nap texture tu image player da xu ly." << std::endl;
             return false;
        }
    }

    texture.setSmooth(false); // Dành cho pixel art
    sprite.setTexture(texture);

    // Setup animation (đã sửa cho ảnh tĩnh)
    sf::Vector2u textureSize = texture.getSize();
    frameCount = 1; 
    currentFrame = sf::IntRect(0, 0, textureSize.x / frameCount, textureSize.y);
    sprite.setTextureRect(currentFrame);

    // Đặt gốc ở chân
    sprite.setOrigin(currentFrame.width / 2.f, currentFrame.height);

    // Scale
    float baseSize = 32.f; // (Giả sử TILE_SIZE = 32)
    #ifdef TILE_SIZE
        baseSize = TILE_SIZE;
    #endif
    
    float scaleX = baseSize / currentFrame.width;
    float scaleY = baseSize / currentFrame.height;
    normalScale = std::min(scaleX, scaleY) * 2.0f; 
    poweredScale = normalScale * 1.2f;
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

// --- UPDATE (VẬT LÝ CHÍNH XÁC) ---
void Player::update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms) {
    // Timers
    if (invulnerabilityTimer > 0.f) invulnerabilityTimer -= deltaTime;
    if (isPoweredUp) {
        powerupTimer -= deltaTime;
        if (powerupTimer <= 0.f) {
            isPoweredUp = false;
        }
    }

    // 1. Di chuyển X
    sprite.move(velocity.x * deltaTime, 0.f); 
    checkCollisionX(platforms);

    // 2. Di chuyển Y (Trọng lực)
    applyGravity(deltaTime);
    sprite.move(0.f, velocity.y * deltaTime); 
    checkCollisionY(platforms);

    // 3. Animation
    updateAnimation(deltaTime);

    // 4. Lật sprite
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

// --- VA CHẠM CHÍNH XÁC ---

void Player::checkCollisionX(const std::vector<std::unique_ptr<Platform>>& platforms) {
    sf::FloatRect playerBounds = getBounds();
    for (const auto& platform : platforms) {
        sf::FloatRect platformBounds = platform->getBounds();
        if (playerBounds.intersects(platformBounds)) {
            if (velocity.x > 0) { // Đang đi sang phải
                sprite.move(-(playerBounds.left + playerBounds.width - platformBounds.left), 0.f);
            } else if (velocity.x < 0) { // Đang đi sang trái
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
            if (velocity.y > 0) { // Đang rơi xuống
                sprite.move(0.f, -(playerBounds.top + playerBounds.height - platformBounds.top));
                velocity.y = 0;
                isOnGround = true;
            } else if (velocity.y < 0) { // Đang nhảy (đụng trần)
                sprite.move(0.f, platformBounds.top + platformBounds.height - playerBounds.top);
                velocity.y = 0;
            }
        }
    }
}
// --- CÁC HÀM CÒN LẠI ---

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
        if (static_cast<int>(invulnerabilityTimer * 10) % 2 == 0) return;
    }
    if (isPoweredUp)
        sprite.setColor(sf::Color(255, 255, 150));
    else
        sprite.setColor(sf::Color::White);
    window.draw(sprite);
}