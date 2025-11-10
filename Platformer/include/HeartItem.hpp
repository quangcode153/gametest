#ifndef HEART_ITEM_HPP
#define HEART_ITEM_HPP

#include "Item.hpp"
#include "Player.hpp"

class HeartItem : public Item {
private:
    float bobbingTimer;
    float originalY;
public:
    HeartItem(float x, float y);
    
    void onCollect(Player& player) override; 

    void update(float deltaTime) override; 
};

#endif