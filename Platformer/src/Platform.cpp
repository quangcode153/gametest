#include "Platform.hpp"
#include <iostream>

// *** SỬA LẠI HÀM TẠO: Dùng sf::Color ***
Platform::Platform(const sf::Vector2f& pos, const sf::Vector2f& sz, const sf::Color& color)
    : position(pos), size(sz), isSolid(true)
{
    shape.setSize(size);
    shape.setOrigin(0.f, 0.f);
    shape.setPosition(position);
    
    // Đặt màu trơn
    shape.setFillColor(color); 

    // (Tất cả code liên quan đến texture đã được xóa)

    std::cout << "Platform created at (" << position.x << ", " << position.y
              << ") | size (" << size.x << ", " << size.y << ") | color\n";
}
// *** HẾT SỬA ĐỔI ***


Platform::~Platform() {
    // Để trống
}

// Hàm getBounds (phải có const)
sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}

// Hàm draw (phải có const)
void Platform::draw(sf::RenderWindow& window) const { // <-- Đảm bảo có const
    window.draw(shape);
}