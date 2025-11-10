#ifndef SHIELDITEM_HPP
#define SHIELDITEM_HPP

#include "Item.hpp"
#include "Player.hpp"

class ShieldItem : public Item {
private:
    float bobbingTimer;
    float originalY;

public:
    ShieldItem(float x, float y);
    
    virtual void onCollect(Player& player) override;
    virtual void update(float deltaTime) override;
};

#endif