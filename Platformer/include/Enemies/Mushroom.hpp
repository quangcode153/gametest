#pragma once
#include "Enemy.hpp"

class Mushroom : public Enemy {
public:
    Mushroom();
    virtual ~Mushroom();

    void init(const std::string& assetFolder) override;
    void updateAI(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms, Player* player) override;

private:
    int attackCount;
    float moveSpeed;
    float visionRange;
    float attackRange;
    float attackCooldown;

    bool checkPlatformEdge(const std::vector<std::unique_ptr<Platform>>& platforms, float directionX);
};