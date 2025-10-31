#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Platform {
private:
    sf::RectangleShape shape; // Chỉ dùng 1 shape
    sf::Texture texture;      // Chỉ dùng 1 texture
    bool isSolid;
    sf::Vector2f position;
    sf::Vector2f size;

public:
    // Hàm tạo nhận 1 đường dẫn texture (để lặp lại)
    Platform(const sf::Vector2f& pos, const sf::Vector2f& sz, const std::string& texturePath); 
    
    // Hàm tạo dự phòng với màu trơn
    Platform(const sf::Vector2f& pos, const sf::Vector2f& sz, const sf::Color& color); 

    ~Platform();
    
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getSize() const { return size; }
    bool getIsSolid() const { return isSolid; }
    
    void draw(sf::RenderWindow& window) const;
};

#endif // PLATFORM_HPP