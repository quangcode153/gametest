#ifndef SPEEDBOOSTITEM_HPP
#define SPEEDBOOSTITEM_HPP

#include "Item.hpp"
#include "Player.hpp"

class SpeedBoostItem : public Item {
private:
    float bobbingTimer;
    float originalY;
public:
    SpeedBoostItem(float x, float y);
    
    virtual void onCollect(Player& player) override;
    virtual void update(float deltaTime) override;
};

#endif