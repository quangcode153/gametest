#include "Platform.hpp"
#include <iostream>

Platform::Platform(const sf::Vector2f& pos, const sf::Vector2f& sz, const sf::Color& color)
    : position(pos), size(sz), isSolid(true)
{
    shape.setSize(size);
    shape.setFillColor(color);
    // SỬA LỖI: Đặt gốc ở (0, 0) (góc trên-trái) để tọa độ chính xác
    shape.setOrigin(0.f, 0.f); 
    shape.setPosition(position);

    std::cout << "Platform created at (" << position.x << ", " << position.y
              << ") | size (" << size.x << ", " << size.y << ")\n";
}

Platform::~Platform() {
    // Không cần giải phóng thủ công vì SFML tự xử lý
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}

void Platform::draw(sf::RenderWindow& window) {
    window.draw(shape);
}