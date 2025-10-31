#include "Platform.hpp"
#include <iostream>

// Hàm tạo với 1 texture (để lặp lại)
Platform::Platform(const sf::Vector2f& pos, const sf::Vector2f& sz, const std::string& texturePath)
    : position(pos), size(sz), isSolid(true)
{
    shape.setSize(size);
    shape.setOrigin(0.f, 0.f); 
    shape.setPosition(position);

    std::cout << "[DEBUG] Platform dang tai anh: \"" << texturePath << "\"" << std::endl;

    if (!texture.loadFromFile(texturePath)) {
        std::cerr << "!!! LOI: Khong the tai platform texture: \"" << texturePath << "\"" << std::endl;
        shape.setFillColor(sf::Color::Magenta); // Tô màu HỒNG để báo lỗi
    } else {
        std::cout << "✓ Platform texture loaded: \"" << texturePath << "\"" << std::endl;
        texture.setRepeated(true); // Cho phép lặp lại
        shape.setTexture(&texture); // Gán ảnh
        // Đặt vùng lặp bằng kích thước platform
        shape.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
    }
}

// Hàm tạo dự phòng với màu trơn
Platform::Platform(const sf::Vector2f& pos, const sf::Vector2f& sz, const sf::Color& color)
    : position(pos), size(sz), isSolid(true)
{
    shape.setSize(sz);
    shape.setOrigin(0.f, 0.f); 
    shape.setPosition(pos);
    shape.setFillColor(color);
    std::cout << "[DEBUG] Creating platform with color fallback.\n";
}

Platform::~Platform() {
    // Để trống
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}

void Platform::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}