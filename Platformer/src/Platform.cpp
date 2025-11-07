#include "Platform.hpp"
#include <iostream>

Platform::Platform(const sf::Vector2f& pos, const sf::Vector2f& sz, const std::string& texturePath)
    : position(pos), size(sz), isSolid(true)
{
    shape.setSize(size);
    shape.setOrigin(0.f, 0.f); 
    shape.setPosition(position);

    // std::cout << "[DEBUG] Platform dang tai anh: \"" << texturePath << "\"" << std::endl;

    if (!texture.loadFromFile(texturePath)) {
        std::cerr << "!!! LOI: Khong the tai platform texture: \"" << texturePath << "\"" << std::endl;
        shape.setFillColor(sf::Color::Black); // Đổi thành màu đen nếu không tải được
    } else {
        // std::cout << "✓ Platform texture loaded: \"" << texturePath << "\"" << std::endl;
        texture.setRepeated(true); // Cho phép lặp lại texture
        shape.setTexture(&texture);
        
        // === DÒNG QUAN TRỌNG NHẤT ===
        // Đặt TextureRect để lặp lại (tile) texture
        // (0, 0) là góc bắt đầu, (size.x, size.y) là kích thước lặp lại
        shape.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
    }
}

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
}

sf::FloatRect Platform::getBounds() const {
    return shape.getGlobalBounds();
}

void Platform::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}