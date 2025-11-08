#include "SpeedBoostItem.hpp"
#include "Player.hpp" 
#include <iostream>
#include <cmath> // <-- Cần thêm thư viện <cmath> để dùng std::sin

// Constructor
SpeedBoostItem::SpeedBoostItem(float x, float y)
    : Item(x, y, "assets/items/itemSpeed.png", 5.0f),
      bobbingTimer(0.f) // <-- Khởi tạo bobbingTimer
{
   
    sprite.setScale(0.1f, 0.1f); 
    
    originalY = y; 
    // =====================================
}

// Hàm onCollect (Logic của Speed Boost)
void SpeedBoostItem::onCollect(Player& player) {
    if (!collected) {
        player.activateSpeedBoost(this->getEffectDuration()); 
        this->collected = true;
        std::cout << "Speed Boost collected!" << std::endl;
    }
}

// === HÀM UPDATE MỚI (Copy y hệt từ HeartItem) ===
void SpeedBoostItem::update(float deltaTime) {
    // Luôn gọi hàm update của lớp cha trước
    Item::update(deltaTime); 

    if (!collected) {
        bobbingTimer += deltaTime;
        
        // Tính toán độ lệch Y
        float offsetY = std::sin(bobbingTimer * 5.0f) * 3.0f;
        
        // Đặt lại vị trí Y của sprite.
        sprite.setPosition(sprite.getPosition().x, originalY + offsetY);
    }
}