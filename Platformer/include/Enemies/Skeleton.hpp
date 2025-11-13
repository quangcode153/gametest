#pragma once
#include "Enemy.hpp"

class Skeleton : public Enemy {
public:
    Skeleton();
    virtual ~Skeleton();

    void init(const std::string& assetFolder) override;
    
    void updateAI(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms, Player* player) override;

private:
    int attackCount;
    float moveSpeed;
    float visionRange;
    float attackRange;

    bool checkPlatformEdge(const std::vector<std::unique_ptr<Platform>>& platforms, float directionX);
};