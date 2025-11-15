#include "Boss.hpp"
#include <iostream>
#include <cmath>
#include <SFML/Graphics/RectangleShape.hpp>
Boss::Boss() : attackCount(0), attackCooldown(0.f), hasDealtDamage(false) {
    this->health = 500;
    this->maxHealth = 500;
    this->visionRange = 1000.f;
    
    // [FIX] Tinh chỉnh "Hit-and-Run"
    this->attackRange = 250.f; // Giảm tầm đánh để player áp sát
    
    this->moveSpeed = 100.f; 
    this->gravityEnabled = true; // Boss đi bộ
}

Boss::~Boss() {}

void Boss::init(const std::string& folder) {
    animManager->clearAnimations();
    this->frameSize = sf::Vector2i(250, 250);

    // [FIX] Hitbox THÂN THỂ (Vulnerable) - Nhỏ và căn giữa
    this->hitboxReduceWidth = 450.f;  
    this->hitboxReduceHeight = 450.f; 
    
    // Vì hàm getBounds MỚI (tính từ tâm) đã tự căn giữa,
    // chúng ta PHẢI set offset về 0
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
    // Không cho phép đổi hướng khi đang tấn công
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

// TRONG FILE Boss.cpp
// (Xóa hàm updateAI cũ và thay bằng hàm này)

void Boss::updateAI(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms, Player* player) {
    // 1. Giảm hồi chiêu
    if (attackCooldown > 0.f) attackCooldown -= deltaTime;

    // 2. Reset TRẠNG THÁI (chỉ khi hồi chiêu xong)
    if (currentState == EnemyState::ATTACK && attackCooldown <= 0.f) {
        currentState = EnemyState::IDLE;
    }
    
    bool animFinished = animManager->isFinished();

    // 3. Bị đánh / Chết
    if (currentState == EnemyState::TAKE_HIT || currentState == EnemyState::DEATH) {
        velocity.x = 0;
        sprite.move(velocity.x * deltaTime, 0.f);
        if (animFinished) currentState = EnemyState::IDLE; 
        return;
    }

    // 4. Tính toán Player
    float distanceToPlayer = 0.f;
    float directionToPlayer = 0.f; 
    bool playerDetected = false;
    
    if (player) {
        float dx = player->getPosition().x - this->getPosition().x;
        float dy = player->getPosition().y - this->getPosition().y;
        
        // [FIX LỖI RƯỢT ĐUỔI]
        // Thêm `std::abs(dy)` để Boss chỉ phát hiện Player
        // khi họ ở gần (không quá 300px trên/dưới)
        if (std::abs(dx) < visionRange && std::abs(dy) < 300.f) { 
            playerDetected = true;
            directionToPlayer = (dx > 0) ? 1.f : -1.f;
            // Vẫn tính distanceToPlayer để check Tầm Đánh
            distanceToPlayer = std::sqrt(dx*dx + dy*dy); 
        } else {
            playerDetected = false;
            distanceToPlayer = 9999.f; // Đặt xa vô tận
        }
    }

    // 5. Logic FSM (Máy trạng thái)
    
    // [ƯU TIÊN 1] - LOGIC KHI ĐANG TẤN CÔNG (Bao gồm cả đang hồi chiêu)
    if (currentState == EnemyState::ATTACK) {
        velocity.x = 0; // Luôn đứng im

        if (animFinished) {
            setAnimation(EnemyState::IDLE); // Chạy anim đứng im
        }

        // --- Tạo Hitbox Tấn Công (Giữ nguyên) ---
        sf::FloatRect bodyBounds = this->getBounds(); 

        // 2. Định nghĩa kích thước Hộp Vàng
        float hitboxW = 300.f; 
        float hitboxH = 200.f; 
        float hitboxX, hitboxY;

        // 3. Căn Hộp Vàng theo chiều dọc (Tâm Hộp Vàng = Tâm Hộp Đỏ)
        hitboxY = bodyBounds.top + (bodyBounds.height / 2.f) - (hitboxH / 2.f);

        // 4. Đặt Hộp Vàng ở TRƯỚC MẶT Hộp Đỏ
        if (movingRight) {
            // Đặt Hộp Vàng ngay cạnh phải của Hộp Đỏ
            hitboxX = bodyBounds.left + bodyBounds.width; 
        } else {
            // Đặt Hộp Vàng ngay cạnh trái của Hộp Đỏ
            // (Phải trừ đi chiều rộng của Hộp Vàng)
            hitboxX = bodyBounds.left - hitboxW; 
        }
        
        attackHitbox = sf::FloatRect(hitboxX, hitboxY, hitboxW, hitboxH);
        
        // --- [FIX LỖI KHÔNG GÂY SÁT THƯƠNG] ---
        //
        // Thời gian vung tay (Tell) là 1.2s
        // (Tổng hồi chiêu 3.5s - Mốc 2.3s = 1.2s)
        // 
        // Logic mới:
        // 1. Cửa sổ sát thương CHỈ TỒN TẠI khi `attackCooldown <= 2.3f`
        // 2. Nếu Player ở trong, và `hasDealtDamage` là false -> Gây sát thương.
        //
        if (attackCooldown <= 2.3f) { 
            // Nếu Player đang ở trong Hộp Vàng VÀ chưa bị đánh
            if (player->getHitbox().intersects(attackHitbox) && !hasDealtDamage) {
                player->takeDamage();
                std::cout << "Boss HIT Player (Synced & 1 Hit)!" << std::endl;
                hasDealtDamage = true; // Đánh dấu đã gây sát thương
            }
        }
    }
    // [ƯU TIÊN 2] - BẮT ĐẦU 1 ĐÒN ĐÁNH MỚI
    else if (playerDetected && distanceToPlayer <= attackRange) { 
        velocity.x = 0;
        facePlayer(player);
        
        attackCount++; 
        int attackVariant = attackCount % 2;
        if (attackVariant == 0) animManager->play("ATTACK_1", false);
        else animManager->play("ATTACK_2", false);
        
        currentState = EnemyState::ATTACK; 
        attackCooldown = 3.5f; // Tổng hồi chiêu
        hasDealtDamage = false; // Reset cờ sát thương RẤT QUAN TRỌNG
    } 
    // [ƯU TIÊN 3] - RƯỢT ĐUỔI
    // (Bây giờ đã hoạt động vì Hitbox Thân Thể đã được sửa)
    else if (playerDetected) {
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
    // [ƯU TIÊN 4] - TUẦN TRA
    // (Bây giờ đã hoạt động vì Hitbox Thân Thể đã được sửa)
    else {
        float currentDir = movingRight ? 1.f : -1.f;
        if (!checkPlatformEdge(platforms, currentDir)) {
            movingRight = !movingRight; // Quay đầu
        }
        velocity.x = movingRight ? moveSpeed : -moveSpeed;
        setAnimation(EnemyState::WALK);
    }

    // 6. Cập nhật di chuyển X
    sprite.move(velocity.x * deltaTime, 0.f);

    // 7. Cập nhật hướng Sprite
    float scaleX = std::abs(sprite.getScale().x);
    float scaleY = sprite.getScale().y;
    if (movingRight) sprite.setScale(scaleX, scaleY);
    else sprite.setScale(-scaleX, scaleY);
}

//
// Hàm kiểm tra vách núi (Giữ nguyên)
//
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
    // 1. Vẽ sprite của Boss (gọi hàm draw của lớp cha)
    Enemy::draw(window);

    // --- Vẽ Hitbox THÂN THỂ (Màu đỏ) ---
    sf::RectangleShape bodyHitbox;
    sf::FloatRect bounds = this->getBounds(); // Lấy hitbox thân thể
    bodyHitbox.setSize(sf::Vector2f(bounds.width, bounds.height));
    bodyHitbox.setPosition(bounds.left, bounds.top);
    bodyHitbox.setFillColor(sf::Color::Transparent);
    bodyHitbox.setOutlineColor(sf::Color::Red);
    bodyHitbox.setOutlineThickness(2.f);
    window.draw(bodyHitbox);

    // --- Vẽ Hitbox TẤN CÔNG (Màu vàng, chỉ khi đang đánh) ---
    if (currentState == EnemyState::ATTACK) {
        sf::RectangleShape attackBoxDebug;
        
        // 'attackHitbox' được tạo và cập nhật trong updateAI
        attackBoxDebug.setSize(sf::Vector2f(attackHitbox.width, attackHitbox.height));
        attackBoxDebug.setPosition(attackHitbox.left, attackHitbox.top);
        
        // Tô màu vàng trong suốt để dễ nhìn
        attackBoxDebug.setFillColor(sf::Color(255, 255, 0, 50)); 
        attackBoxDebug.setOutlineColor(sf::Color::Yellow); 
        attackBoxDebug.setOutlineThickness(1.f);
        
        window.draw(attackBoxDebug);
    }
}
// DÁN TOÀN BỘ HÀM NÀY VÀO CUỐI FILE Boss.cpp

sf::FloatRect Boss::getBounds() const { 
    // Kích thước thật của sprite (đã scale)
    float spriteW = sprite.getLocalBounds().width * std::abs(sprite.getScale().x);
    float spriteH = sprite.getLocalBounds().height * std::abs(sprite.getScale().y);

    // Kích thước của Hitbox (Hộp Đỏ) sau khi đã thu nhỏ
    float hitboxW = spriteW - hitboxReduceWidth;
    float hitboxH = spriteH - hitboxReduceHeight;

    // Vị trí TÂM của sprite (getPosition() là tâm)
    sf::Vector2f centerPos = sprite.getPosition();

    // Tính toán góc trên-trái (top-left) của Hộp Đỏ
    // Bằng cách lấy Tâm trừ đi một nửa kích thước, SAU ĐÓ áp dụng offset
    float left = (centerPos.x - hitboxW / 2.f) + hitboxOffsetLeft;
    float top = (centerPos.y - hitboxH / 2.f) + hitboxOffsetTop;

    return sf::FloatRect(left, top, hitboxW, hitboxH);
}