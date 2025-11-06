#include "CoinItem.hpp"
#include <iostream> 
#include<math.h>

CoinItem::CoinItem(float x, float y) 
    : Item(x, y, "assets/items/Coin.png", 0.0f),
    bobbingTimer(0.f), // Khởi tạo timer
    originalY(y) 
{ 
    // Ở đây bạn có thể điều chỉnh tỷ lệ của đồng xu nếu ảnh gốc quá lớn hoặc quá nhỏ
    // Ví dụ: sprite.setScale(0.8f, 0.8f); 
    // Hoặc giữ nguyên nếu kích thước ảnh đã phù hợp (như 32x32)
    // sprite.setScale(1.0f, 1.0f); // Giữ nguyên kích thước 
    // Đảm bảo vị trí được thiết lập sau khi setOrigin trong Item constructor
    sprite.setScale(0.25f, 0.25f);
}

void CoinItem::onCollect(Player& player) {
    if (!collected) { 
        player.addCoin(1); 
        collected = true;  
        std::cout << "Coin collected! Player total coins: " << player.getTotalCoins() << std::endl;
        // TODO: Phát âm thanh nhặt coin ở đây
    }
}

void CoinItem::update(float deltaTime) {
    // Gọi hàm update của lớp cha (quan trọng nếu có effectDuration)
    Item::update(deltaTime); 

    if (!collected) {
        // 1. Cập nhật timer (dùng biến thành viên, không dùng static)
        bobbingTimer += deltaTime;

        // 2. Tính toán độ lệch Y (offsetY)
        // std::sin(bobbingTimer * 5.0f) -> tạo dao động (số 5.0f là tốc độ)
        // * 3.0f -> là biên độ (sẽ dao động 3 pixel lên và 3 pixel xuống)
        float offsetY = std::sin(bobbingTimer * 5.0f) * 3.0f;
        
        // 3. Đặt vị trí MỚI dựa trên vị trí Y GỐC + độ lệch
        // (Giữ nguyên vị trí X đã được set trong constructor của Item)
        sprite.setPosition(sprite.getPosition().x, originalY + offsetY);
    }
}
