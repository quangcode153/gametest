#ifndef ENEMY_HPP
#define ENEMY_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Platform.hpp"

class Enemy {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    
    sf::Vector2f velocity;
    float speed;
    bool movingRight;

    // === SỬA (Thay thế bool alive) ===
    int health;
    int maxHealth;
    float invulnerabilityTimer; // Giống như Player, để quái không bị dính 50 hit/giây
    // ============================

public:
    Enemy();
    ~Enemy();
    
    bool loadTexture(const std::string& path);
    void setPosition(const sf::Vector2f& pos);
    
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    
    // === SỬA (Hàm isAlive bây giờ sẽ kiểm tra máu) ===
    bool isAlive() const { return health > 0; }
    // ============================================
    
    void update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms);
    void patrol();
    
    void checkBounds(const std::vector<std::unique_ptr<Platform>>& platforms);
    
    void kill(); // Hàm này vẫn giữ, dùng khi bị dậm (nếu muốn) hoặc khi chết
    
    // === THÊM CÁC HÀM CHIẾN ĐẤU MỚI ===
    void takeDamage(int damage);
    bool canBeHit() const;
    void draw(sf::RenderWindow& window);
};

#endif