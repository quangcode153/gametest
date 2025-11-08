#include "ShieldItem.hpp"
#include "Player.hpp" 
#include <iostream>
#include <cmath> 

ShieldItem::ShieldItem(float x, float y)
    // 1. Vị trí x, y
    // 2. Đường dẫn đến ảnh khiên
    // 3. Thời gian hiệu lực (4 giây)
    : Item(x, y, "assets/items/shieldItem.png", 4.0f), 
      bobbingTimer(0.f)
{
    // === THAY ĐỔI KÍCH THƯỚC GIỐNG HỆT itemSpeed ===
    this->sprite.setScale(0.1f, 0.1f);
    // ============================================

    // Lưu vị trí Y ban đầu
    originalY = y; 
}

void ShieldItem::onCollect(Player& player) {
    if (!collected) {
        // Kích hoạt khiên trên player
        player.activateShield(this->getEffectDuration()); 
        
        // Đánh dấu là đã nhặt
        this->collected = true;
        std::cout << "Shield item collected!" << std::endl;
    }
}

// Hàm update (copy từ HeartItem/SpeedBoostItem để làm nó nhấp nhô)
void ShieldItem::update(float deltaTime) {
    Item::update(deltaTime); 

    if (!collected) {
        bobbingTimer += deltaTime;
        float offsetY = std::sin(bobbingTimer * 5.0f) * 3.0f;
        sprite.setPosition(sprite.getPosition().x, originalY + offsetY);
    }
}