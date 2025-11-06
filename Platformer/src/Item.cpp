#include "Item.hpp"
#include <iostream> 

Item::Item(float x, float y, const std::string& texturePath, float duration) 
    : collected(false),
      effectDuration(duration), 
      currentEffectTime(0.0f) 
{
    std::cout << "[DEBUG Item] Creating Item at (" << x << ", " << y << ") with texture: " << texturePath << std::endl; 
    
    sf::Image img; // Tải vào sf::Image trước
    if (!img.loadFromFile(texturePath)) {
        std::cerr << "ERROR: Could not load item IMAGE: " << texturePath << std::endl;
        // Fallback: Tạo một hình chữ nhật màu đỏ nếu không tải được ảnh
        img.create(32, 32, sf::Color::Magenta); 
        std::cerr << "Using magenta fallback texture." << std::endl;
    } else {
        std::cout << "SUCCESS: Loaded item IMAGE: " << texturePath << std::endl;
        
        // Tạo mask từ màu trắng để làm trong suốt nền
        img.createMaskFromColor(sf::Color::White); 
    }
    
    if (!texture.loadFromImage(img)) { // Tải texture từ ảnh đã xử lý
        std::cerr << "ERROR: Could not load item TEXTURE from image: " << texturePath << std::endl;
    } else {
        std::cout << "SUCCESS: Loaded item TEXTURE from image: " << texturePath << std::endl;
    }

    sprite.setTexture(texture);
    sprite.setPosition(x, y);

    // Đặt gốc của sprite ở giữa để vị trí (x,y) là tâm của coin
    // và để dễ dàng thay đổi tỷ lệ hoặc xoay sau này
    sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f); 
}

void Item::update(float deltaTime) {
    if (collected && effectDuration > 0) {
        currentEffectTime -= deltaTime;
        if (currentEffectTime < 0) {
            currentEffectTime = 0;
        }
    }
}

void Item::draw(sf::RenderWindow& window) {
    if (!collected) {
        window.draw(sprite);
    }
}

sf::FloatRect Item::getBounds() const {
    return sprite.getGlobalBounds();
}

bool Item::isCollected() const {
    return collected;
}

void Item::setCollected(bool val) {
    collected = val;
}