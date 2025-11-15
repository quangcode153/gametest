#pragma once
#include "Enemy.hpp"
#include "Player.hpp" 
#include <SFML/Graphics/RenderWindow.hpp>
class Boss : public Enemy {
public:
    Boss();
    ~Boss();

    void init(const std::string& folder) override;
    void updateAI(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms, Player* player) override;
    void draw(sf::RenderWindow& window) override;
private:
    float attackCooldown;
    float attackRange;
    float visionRange;
    int attackCount;
    
    // Biến này để đảm bảo mỗi lần múa chỉ chém trúng 1 nhát
    bool hasDealtDamage; 
    
    // [FIX] Thêm biến tốc độ di chuyển
    float moveSpeed;
    
    sf::FloatRect attackHitbox;
    sf::FloatRect getBounds() const override;
    void facePlayer(Player* player);
    bool checkPlatformEdge(const std::vector<std::unique_ptr<Platform>>& platforms, float directionX);
};