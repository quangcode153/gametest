#include "HeartItem.hpp"
#include <iostream> 
#include <cmath> 

HeartItem::HeartItem(float x, float y) 
    // Gọi constructor của lớp cha Item. 
    // Item.cpp sẽ đặt origin là đáy giữa và setPosition(x, y).
    : Item(x, y, "assets/items/tim.png", 0.0f),
      bobbingTimer(0.f)
      // originalY sẽ được tính sau trong constructor của HeartItem
{ 
    // --- BƯỚC 1: ĐIỀU CHỈNH KÍCH THƯỚC NHỎ HƠN ---
    sprite.setScale(0.23f, 0.23f); 

    // --- BƯỚC 2: Cập nhật lại vị trí của sprite sau khi scale ---
    // Vì Item::Item đã gọi sprite.setPosition(x,y) với origin là đáy,
    // và sau đó bạn gọi setScale, kích thước sprite thay đổi.
    // Nếu bạn muốn sprite vẫn nằm ở vị trí x, y (tức là đáy sprite ở y),
    // thì bạn KHÔNG cần gọi lại sprite.setPosition(x, y) ở đây.
    // Nó đã được đặt đúng rồi.

    // --- BƯỚC 3 (QUAN TRỌNG): Lưu lại vị trí Y ban đầu của ĐÁY SPRITE làm gốc ---
    // 'y' được truyền vào chính là tọa độ Y của đáy sprite (do setOrigin trong Item.cpp)
    // Chúng ta sẽ lưu lại giá trị 'y' này làm gốc cho chuyển động dao động.
    originalY = y; 
}

void HeartItem::onCollect(Player& player) {
    if (!collected) { 
        if (player.getHealth() < player.getMaxHealth()) {
            player.heal(1); 
            collected = true;  
            std::cout << "Heart collected! Player current health: " << player.getHealth() << std::endl;
            // TODO: Phát âm thanh nhặt tim ở đây
        } else {
            std::cout << "Player already has full health. Heart not collected (full health)." << std::endl;
            // GIỮ NGUYÊN: Nếu không muốn Heart bị biến mất khi máu đầy, không set collected = true ở đây.
        }
    }
}

void HeartItem::update(float deltaTime) {
    // Luôn gọi hàm update của lớp cha trước để xử lý collected state và các logic chung
    Item::update(deltaTime); 

    if (!collected) {
        bobbingTimer += deltaTime;
        
        // Tính toán độ lệch Y từ vị trí đáy ban đầu
        // Sử dụng 'originalY' là vị trí đáy của sprite.
        // Điều này đảm bảo sprite dao động quanh vị trí đáy ban đầu của nó.
        float offsetY = std::sin(bobbingTimer * 5.0f) * 3.0f;
        
        // Đặt lại vị trí Y của sprite.
        // x giữ nguyên, y là 'originalY' (đáy gốc) + 'offsetY'.
        sprite.setPosition(sprite.getPosition().x, originalY + offsetY);
    }
}