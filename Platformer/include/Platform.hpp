#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <SFML/Graphics.hpp>
// #include <string> // Không cần string nữa

class Platform {
private:
    sf::RectangleShape shape;
    // sf::Texture texture; // <-- XÓA DÒNG NÀY
    sf::Vector2f position;
    sf::Vector2f size;
    bool isSolid;

public:
    // *** SỬA LẠI HÀM TẠO: Nhận sf::Color ***
    Platform(const sf::Vector2f& pos, const sf::Vector2f& sz, const sf::Color& color);

    ~Platform();
    sf::FloatRect getBounds() const;
    
    // (Thêm các hàm getter này nếu bạn cần, file .hpp cũ của bạn có)
    sf::Vector2f getPosition() const { return position; }
    sf::Vector2f getSize() const { return size; }
    bool getIsSolid() const { return isSolid; }

    // SỬA: Đảm bảo hàm draw có const
    void draw(sf::RenderWindow& window) const;
};

#endif