#include "Enemy.hpp"
#include <iostream>

Enemy::Enemy() :
    velocity(0.f, 0.f),
    speed(100.f),
    movingRight(true),
    // === SỬA (Khởi tạo máu) ===
    health(100), // Ví dụ: Quái có 100 máu
    maxHealth(100),
    invulnerabilityTimer(0.f)
    // alive(true) // Xóa dòng này
    // ========================
{
}

Enemy::~Enemy() {
}

bool Enemy::loadTexture(const std::string& path) {
    // ... (code loadTexture của bạn giữ nguyên, không cần sửa) ...
    sf::Image image;
    if (!image.loadFromFile(path)) {
        std::cerr << "Loi: Khong the tai file enemy: " << path << std::endl;
        return false;
    }

    image.createMaskFromColor(sf::Color::White);

    if (!texture.loadFromImage(image)) {
        std::cerr << "Loi: Khong the nap texture tu image enemy da xu ly." << std::endl;
        return false;
    }

    sprite.setTexture(texture);

    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height);

    sprite.setScale(0.15f, 0.15f);

    return true;
}

void Enemy::setPosition(const sf::Vector2f& pos) {
    sprite.setPosition(pos);
}

sf::Vector2f Enemy::getPosition() const {
    return sprite.getPosition();
}

sf::FloatRect Enemy::getBounds() const {
    return sprite.getGlobalBounds();
}

void Enemy::update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms) {
    // === SỬA (Kiểm tra máu thay vì 'alive') ===
    if (health <= 0) return;
    // =======================================

    // === THÊM (Đếm ngược thời gian bất tử) ===
    if (invulnerabilityTimer > 0.f) {
        invulnerabilityTimer -= deltaTime;
    }
    // ======================================

    velocity.y += 1000.f * deltaTime;

    patrol();

    sprite.move(velocity * deltaTime);

    sf::FloatRect enemyBounds = getBounds();

    for (const auto& platform : platforms) {
        // ... (code xử lý va chạm platform giữ nguyên) ...
        sf::FloatRect platformBounds = platform->getBounds();

        if (enemyBounds.intersects(platformBounds)) {
            if (velocity.y > 0) {
                float previousBottom = enemyBounds.top + enemyBounds.height - (velocity.y * deltaTime);

                if (previousBottom <= platformBounds.top) {
                    sprite.setPosition(sprite.getPosition().x, platformBounds.top);
                    velocity.y = 0;
                }
            }
        }
    }

    checkBounds(platforms);
}

void Enemy::patrol() {
    // ... (code patrol của bạn giữ nguyên) ...
    if (movingRight) {
        velocity.x = speed;
    } else {
        velocity.x = -speed;
    }
}

void Enemy::checkBounds(const std::vector<std::unique_ptr<Platform>>& platforms) {
    // ... (code checkBounds của bạn giữ nguyên) ...
    sf::FloatRect enemyBounds = getBounds();
    sf::Vector2f currentPos = sprite.getPosition();

    sf::Vector2f feelerPoint = currentPos;
    float feelerXOffset = (enemyBounds.width / 2.f) + 5.f;
    feelerPoint.y += 5.f;

    if (movingRight) {
        feelerPoint.x += feelerXOffset;
    } else {
        feelerPoint.x -= feelerXOffset;
    }

    bool groundInFront = false;
    for (const auto& platform : platforms) {
        if (platform->getBounds().contains(feelerPoint)) {
            groundInFront = true;
            break;
        }
    }

    if (!groundInFront && velocity.y == 0) {
        movingRight = !movingRight;
    }

    float currentScale = sprite.getScale().x;
    if (currentScale < 0) currentScale = -currentScale;

    if (movingRight) {
        sprite.setScale(currentScale, currentScale);
    } else {
        sprite.setScale(-currentScale, currentScale);
    }
}


void Enemy::kill() {
    // === SỬA (Đặt máu về 0) ===
    health = 0;
    // alive = false; // Xóa dòng này
    // ======================
    sprite.setPosition(-1000.f, -1000.f); // Giữ lại cái này, nó rất hữu ích
}

void Enemy::draw(sf::RenderWindow& window) {
    // === SỬA (Kiểm tra máu) ===
    if (health > 0) {
        // === THÊM (Hiệu ứng nhấp nháy khi bị đánh) ===
        if (invulnerabilityTimer > 0.f) {
            // Cứ 0.1 giây lại ẩn/hiện
            if (static_cast<int>(invulnerabilityTimer * 10) % 2 == 0) {
                // Bỏ qua việc vẽ (tạo hiệu ứng nhấp nháy)
                return; 
            }
        }
        // ===========================================
        window.draw(sprite);
    }
}

// === THÊM CÁC HÀM MỚI VÀO CUỐI FILE ===

void Enemy::takeDamage(int damage) {
    if (health <= 0 || !canBeHit()) {
        return; // Đã chết hoặc đang bất tử
    }

    health -= damage;
    invulnerabilityTimer = 0.5f; // Quái bất tử trong 0.5 giây sau khi bị đánh
    
    std::cout << "Enemy bi danh! Mau con lai: " << health << std::endl;

    if (health <= 0) {
        kill(); // Quái chết
    }
}

bool Enemy::canBeHit() const {
    // Có thể bị đánh nếu timer <= 0
    return invulnerabilityTimer <= 0.f;
}