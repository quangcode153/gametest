#ifndef COIN_ITEM_HPP
#define COIN_ITEM_HPP

#include "Item.hpp"
#include "Player.hpp"

class CoinItem : public Item {
public:
    float bobbingTimer;
    float originalY;
    
    CoinItem(float x, float y);
    
    void onCollect(Player& player) override; 

    void update(float deltaTime) override;
};

#endif