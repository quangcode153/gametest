#include "Player.hpp"
#include <iostream>
#include <algorithm>
#include <cmath> 
// #include "Constants.hpp" // (Nếu bạn không có file này, hãy xóa dòng này đi)

Player::Player() :
    animManager(std::make_unique<AnimationManager>(sprite)), 
    currentState(State::IDLE),
    frameSize(180, 180), 
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
    totalCoins(0) 
{
    normalMoveSpeed = moveSpeed; 
    
    float desiredHeight = 300.f; 
    normalScale = desiredHeight / frameSize.y; 
    poweredScale = normalScale * 1.2f; 
    
    sprite.setScale(normalScale, normalScale); 
    
    std::cout << "Player scale set to: " << normalScale << std::endl;

    //
    localHitbox = sf::FloatRect(76.f, 60.f, 40.f, 50.f); 
}

Player::~Player() {}

bool Player::loadTexture(const std::string& path_ignored) {
    loadAnimations();
    return true; 
}

void Player::loadAnimations() {
    std::string folder = "assets/characters/kiem/"; 
    
    animManager->addAnimation("IDLE",     folder + "Idle.png",     frameSize, 11, 0.1f);
    animManager->addAnimation("RUNNING",  folder + "Run.png",      frameSize, 8,  0.08f);
    animManager->addAnimation("JUMPING",  folder + "Jump.png",     frameSize, 3,  0.1f);
    animManager->addAnimation("FALLING",  folder + "Fall.png",     frameSize, 3,  0.1f);
    animManager->addAnimation("ATTACK1",  folder + "Attack1.png",  frameSize, 7,  0.07f);
    animManager->addAnimation("ATTACK2",  folder + "Attack2.png",  frameSize, 7,  0.07f);
    animManager->addAnimation("TAKE_HIT", folder + "Take Hit.png", frameSize, 4,  0.1f);
    animManager->addAnimation("DEATH",    folder + "Death.png",    frameSize, 11, 0.1f);
    
    setAnimation(State::IDLE);
    std::cout << "Player animations loaded!" << std::endl;
}


void Player::setPosition(const sf::Vector2f& pos) {
    sprite.setPosition(pos);
}

sf::Vector2f Player::getPosition() const {
    return sprite.getPosition();
}

// === CÁC HÀM HITBOX MỚI ===

// Hàm getBounds() cũ, trả về khung ảnh 180x180
sf::FloatRect Player::getBounds() const {
    return sprite.getGlobalBounds();
}

// Hàm getHitbox() MỚI, trả về khung va chạm thật
sf::FloatRect Player::getHitbox() const {
    // Áp dụng transform (vị trí, scale) của sprite vào localHitbox
    return sprite.getTransform().transformRect(localHitbox);
}

// Hàm MỚI, lấy khoảng cách từ TÂM sprite đến CHÂN hitbox (đã scale)
float Player::getScaledFootOffset() const {
    // Lấy vị trí Y của chân hitbox (so với tâm 0,0 của hitbox)
    // (localHitbox.top + localHitbox.height)
    // (Ví dụ: -70 + 160 = 90)
    float localFootY = localHitbox.top + localHitbox.height;
    
    // Nhân với scale Y (dùng abs để tránh lỗi lật sprite)
    return localFootY * std::abs(sprite.getScale().y);
}
// =========================


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
    
    // 1. Cập nhật logic hiệu ứng
    if (invulnerabilityTimer > 0.f) invulnerabilityTimer -= deltaTime;
    if (isPoweredUp) {
    powerupTimer -= deltaTime;
    if (powerupTimer <= 0.f) {
        isPoweredUp = false;
        // Tùy chọn: reset scale nếu bạn muốn
        // float currentScale = isPoweredUp ? poweredScale : normalScale;
        // sprite.setScale(currentScale, currentScale); 
    }
}
if (isBoosted) {
    boostTimer -= deltaTime;
    if (boostTimer <= 0.f) {
        isBoosted = false;
        moveSpeed = normalMoveSpeed; // <-- Rất quan trọng: Trả tốc độ về bình thường
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

    // Không cho phép di chuyển nếu đang Chết
    if (currentState == State::DEATH) {
        animManager->update(deltaTime);
        return; 
    }
    
    // 2. Cập nhật vật lý
    applyGravity(deltaTime);
    
    sprite.move(velocity.x * deltaTime, 0.f); 
    checkCollisionX(platforms); 

    sprite.move(0.f, velocity.y * deltaTime); 
    checkCollisionY(platforms); 

    // 3. QUYẾT ĐỊNH TRẠNG THÁI ANIMATION
    bool animFinished = animManager->isFinished();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::J) && currentState != State::JUMPING && currentState != State::FALLING) {
        setAnimation(State::ATTACK1);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::K) && currentState != State::JUMPING && currentState != State::FALLING) {
        setAnimation(State::ATTACK2);
    }
    else if (velocity.y < -0.1f) {
        setAnimation(State::JUMPING);
    } 
    else if (velocity.y > 0.1f && !isOnGround) {
        setAnimation(State::FALLING);
    } 
    else if (std::abs(velocity.x) > 0.1f && isOnGround) {
        setAnimation(State::RUNNING);
    } 
    else if (isOnGround) {
        if ((currentState == State::TAKE_HIT || currentState == State::ATTACK1 || currentState == State::ATTACK2) && animFinished) {
            setAnimation(State::IDLE);
        }
        if (currentState != State::TAKE_HIT && currentState != State::ATTACK1 && currentState != State::ATTACK2) {
             setAnimation(State::IDLE);
        }
    }

    // 4. Cập nhật hình ảnh
    animManager->update(deltaTime);

    // 5. Lật sprite
    float currentScale = isPoweredUp ? poweredScale : normalScale;
    float scaleX = (facingRight ? currentScale : -currentScale);
    sprite.setScale(scaleX, currentScale); 
}

void Player::setAnimation(State newState) {
    if (currentState == newState) return; 
    if ((currentState == State::ATTACK1 || currentState == State::ATTACK2 || currentState == State::TAKE_HIT) && !animManager->isFinished()) return;
    if (currentState == State::DEATH) return;

    currentState = newState;
    
    switch (currentState) {
        case State::IDLE:     animManager->play("IDLE", true);     break;
        case State::RUNNING:  animManager->play("RUNNING", true);  break;
        case State::JUMPING:  animManager->play("JUMPING", false); break; 
        case State::FALLING:  animManager->play("FALLING", true);  break; 
        case State::ATTACK1:  animManager->play("ATTACK1", false); break; 
        case State::ATTACK2:  animManager->play("ATTACK2", false); break; 
        case State::TAKE_HIT: animManager->play("TAKE_HIT", false);break; 
        case State::DEATH:    animManager->play("DEATH", false);   break; 
    }
}

void Player::applyGravity(float deltaTime) {
    velocity.y += gravity * deltaTime; 
    if (velocity.y > maxFallSpeed)
        velocity.y = maxFallSpeed;
}

// ==========================================================
// === VA CHẠM DÙNG HITBOX (THAY VÌ getBounds) ===
// ==========================================================

// Trong file: Player.cpp
// THAY THẾ HÀM NÀY

// Trong file: Player.cpp
// THAY THẾ HÀM NÀY

void Player::checkCollisionX(const std::vector<std::unique_ptr<Platform>>& platforms) {
    sf::FloatRect playerBounds = getHitbox(); 
    
    for (const auto& platform : platforms) {
        sf::FloatRect platformBounds = platform->getBounds(); 
        
        if (playerBounds.intersects(platformBounds)) {
            // Vì chúng ta chỉ di chuyển X trước khi gọi hàm này,
            // nên bất kỳ va chạm nào cũng là va chạm X.
            
            if (velocity.x > 0) { // Di chuyển sang phải
                sprite.move(-(playerBounds.left + playerBounds.width - platformBounds.left), 0.f);
            } else if (velocity.x < 0) { // Di chuyển sang trái
                sprite.move(platformBounds.left + platformBounds.width - playerBounds.left, 0.f);
            }
            velocity.x = 0;

            // Cập nhật lại bounds sau khi di chuyển, để vòng lặp tiếp theo (nếu có) chính xác
            playerBounds = getHitbox(); 
        }
    }
}

// Trong file: Player.cpp
// THAY THẾ TOÀN BỘ HÀM CŨ BẰNG HÀM NÀY

// Trong file: Player.cpp
// THAY THẾ HÀM NÀY

// Trong file: Player.cpp
// THAY THẾ HÀM NÀY

void Player::checkCollisionY(const std::vector<std::unique_ptr<Platform>>& platforms) {
    isOnGround = false;
    sf::FloatRect playerBounds = getHitbox(); 

    for (const auto& platform : platforms) {
        sf::FloatRect platformBounds = platform->getBounds(); 

        if (playerBounds.intersects(platformBounds)) {
            // Vì chúng ta chỉ di chuyển Y trước khi gọi hàm này,
            // nên bất kỳ va chạm nào cũng là va chạm Y.

            if (velocity.y > 0) { // Đang rơi xuống
                sprite.move(0.f, -(playerBounds.top + playerBounds.height - platformBounds.top));
                velocity.y = 0;
                isOnGround = true; // QUAN TRỌNG: Đặt isOnGround ở đây
            } 
            else if (velocity.y < 0) { // Đang nhảy lên (đụng đầu)
                sprite.move(0.f, platformBounds.top + platformBounds.height - playerBounds.top);
                velocity.y = 0;
            }

            // Cập nhật lại bounds sau khi di chuyển
            playerBounds = getHitbox();
        }
    }
}
// ==========================================================

void Player::takeDamage() {
    // ... (code này không đổi)
    if (currentState == State::DEATH) return; 
    if (invulnerabilityTimer <= 0.f && !isPoweredUp) {
        if (isShielded) { /* ... */ } 
        else { 
            health--;
            invulnerabilityTimer = 2.0f; 
            std::cout << "Player hit! Health: " << health << "\n";
            if (health <= 0) { setAnimation(State::DEATH); } 
            else { setAnimation(State::TAKE_HIT); }
        }
    }
}

// ... (code các hàm còn lại: canTakeDamage, activatePowerup, isInvulnerable, draw, addCoin, heal, ... không đổi)
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

void Player::addCoin(int amount) { totalCoins += amount; }
int Player::getTotalCoins() const { return totalCoins; }
void Player::setTotalCoins(int amount) { totalCoins = amount; }
void Player::heal(int amount) {
    health += amount;
    if (health > maxHealth) { health = maxHealth; }
    if (health == maxHealth && currentState == State::DEATH) {
        // Hãy hồi sinh player
        currentState = State::IDLE; 
        animManager->play("IDLE", true); // Buộc nó quay về animation đứng yên
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

// Dán 2 hàm này vào Player.cpp

Player::State Player::getCurrentState() const {
    return currentState;
}

bool Player::hasDeathAnimationFinished() const {
    
    return (currentState == State::DEATH && animManager->isFinished());
}