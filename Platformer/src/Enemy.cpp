#include "Enemy.hpp"
#include <iostream>

// --- Constructor (Hàm khởi tạo) ---
Enemy::Enemy() :
    velocity(0.f, 0.f), // <-- SỬA: Khởi tạo (x, y)
    speed(100.f),       // Tốc độ di chuyển
    movingRight(true),
    alive(true)
{
    // Để trống
}

// --- Destructor (Hàm hủy) ---
Enemy::~Enemy() {
    // Để trống
}

// --- Hàm loadTexture (Sửa lỗi nền trắng VÀ ĐẶT GỐC Ở CHÂN) ---
bool Enemy::loadTexture(const std::string& path) {
    sf::Image image; // Dùng sf::Image để xử lý
    if (!image.loadFromFile(path)) {
        std::cerr << "Loi: Khong the tai file enemy: " << path << std::endl;
        return false;
    }

    // *** SỬA LỖI NỀN TRẮNG ***
    image.createMaskFromColor(sf::Color::White);
    // ************************

    if (!texture.loadFromImage(image)) {
        std::cerr << "Loi: Khong the nap texture tu image enemy da xu ly." << std::endl;
        return false;
    }

    sprite.setTexture(texture);

    // *** SỬA LỖI LƠ LỬNG ***
    sf::FloatRect bounds = sprite.getLocalBounds();
    // Đặt tâm (origin) ở GIỮA-DƯỚI (CHÂN)
    sprite.setOrigin(bounds.width / 2.f, bounds.height); 
    // ************************
    
    sprite.setScale(0.15f, 0.15f); 

    return true;
}

void Enemy::setPosition(const sf::Vector2f& pos) {
    sprite.setPosition(pos);
}

// (Hàm setPatrolBounds không còn cần thiết)
// void Enemy::setPatrolBounds(float left, float right) { }

sf::Vector2f Enemy::getPosition() const {
    return sprite.getPosition();
}

sf::FloatRect Enemy::getBounds() const {
    return sprite.getGlobalBounds();
}

// --- Hàm Update chính (THÊM TRỌNG LỰC VÀ VA CHẠM) ---
void Enemy::update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms) {
    if (!alive) return;
    
    // 1. Thêm trọng lực
    velocity.y += 1000.f * deltaTime; // Bạn có thể chỉnh số 1000
    
    // 2. AI Tuần tra (Set vận tốc X)
    patrol(); 
    
    // 3. Di chuyển (theo cả X và Y)
    sprite.move(velocity * deltaTime);
    
    // 4. Kiểm tra va chạm với platform
    sf::FloatRect enemyBounds = getBounds();
    
    for (const auto& platform : platforms) {
        sf::FloatRect platformBounds = platform->getBounds();

        if (enemyBounds.intersects(platformBounds)) {
            // Va chạm từ phía trên (đang rơi xuống)
            if (velocity.y > 0) {
                float previousBottom = enemyBounds.top + enemyBounds.height - (velocity.y * deltaTime);

                if (previousBottom <= platformBounds.top) {
                    // Đặt enemy đứng trên platform
                    sprite.setPosition(sprite.getPosition().x, platformBounds.top);
                    velocity.y = 0; // Dừng rơi
                }
            }
        }
    }
    
    // 5. Kiểm tra biên (AI thông minh)
    checkBounds(platforms);
}

// --- AI Tuần tra ---
void Enemy::patrol() {
    if (movingRight) {
        velocity.x = speed;
    } else {
        velocity.x = -speed;
    }
}

// --- Kiểm tra biên (AI "THÔNG MINH" TỰ PHÁT HIỆN MÉP) ---
void Enemy::checkBounds(const std::vector<std::unique_ptr<Platform>>& platforms) {
    sf::FloatRect enemyBounds = getBounds();
    sf::Vector2f currentPos = sprite.getPosition(); // Đây là gốc (center-bottom)

    // --- 1. Phát hiện Mép (Vực) ---
    // Tạo một điểm "cảm biến" (feeler) ở ngay trước chân con quái
    sf::Vector2f feelerPoint = currentPos;
    float feelerXOffset = (enemyBounds.width / 2.f) + 5.f; // 5px phía trước mép
    feelerPoint.y += 5.f; // 5px phía dưới chân (để chắc chắn nó ở dưới)

    if (movingRight) {
        feelerPoint.x += feelerXOffset; // Cảm biến bên phải
    } else {
        feelerPoint.x -= feelerXOffset; // Cảm biến bên trái
    }

    // Kiểm tra xem điểm cảm biến có đang nằm trên platform nào không
    bool groundInFront = false;
    for (const auto& platform : platforms) {
        if (platform->getBounds().contains(feelerPoint)) {
            groundInFront = true;
            break;
        }
    }

    // Nếu KHÔNG có đất ở phía trước (tức là sắp rơi) VÀ đang đứng yên (không rơi) -> Quay đầu
    if (!groundInFront && velocity.y == 0) { 
        movingRight = !movingRight;
    }
    
    // (Bạn có thể thêm code phát hiện tường ở đây nếu muốn)
    
    // --- 2. Lật Sprite (Hiển thị) ---
    float currentScale = sprite.getScale().x;
    if (currentScale < 0) currentScale = -currentScale; 

    if (movingRight) {
        sprite.setScale(currentScale, currentScale); // Nhìn bên phải
    } else {
        sprite.setScale(-currentScale, currentScale); // Nhìn bên trái (lật ảnh)
    }
}


// --- Khi Enemy chết ---
void Enemy::kill() {
    alive = false;
    sprite.setPosition(-1000.f, -1000.f);
}

// --- Hàm Vẽ ---
void Enemy::draw(sf::RenderWindow& window) {
    if (alive) {
        window.draw(sprite);
    }
}