#ifndef SPEEDBOOSTITEM_HPP
#define SPEEDBOOSTITEM_HPP

#include "Item.hpp"

class SpeedBoostItem : public Item {
private:
    float bobbingTimer;
    float originalY;
public:
    // Constructor: nhận vị trí x, y
    SpeedBoostItem(float x, float y);
    
    // Ghi đè hàm onCollect
    virtual void onCollect(Player& player) override;
    virtual void update(float deltaTime) override;
};

#endif