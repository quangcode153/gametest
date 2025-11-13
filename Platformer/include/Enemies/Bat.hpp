#pragma once
#include "Enemy.hpp"

class Bat : public Enemy {
public:
    Bat();
    virtual ~Bat();

    void init(const std::string& assetFolder) override;
    void updateAI(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms, Player* player) override;

private:
    int attackCount;
    float flySpeed;
    float visionRange;
    float attackRange;
    
    // === BIẾN MỚI QUAN TRỌNG ===
    float attackCooldown; // Thời gian nghỉ giữa các đòn đánh
};