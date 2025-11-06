#ifndef COIN_ITEM_HPP
#define COIN_ITEM_HPP

#include "Item.hpp"
#include "Player.hpp"

class CoinItem : public Item {
public:
    float bobbingTimer;
    float originalY;
    // Constructor của CoinItem. 
    // Sẽ gọi constructor của lớp cha Item với đường dẫn texture "assets/items/Coin.png"
    // và duration = 0.0f (vì coin không có hiệu ứng thời gian đặc biệt)
    CoinItem(float x, float y);
    
    // Triển khai (override) hàm onCollect từ lớp cha Item
    void onCollect(Player& player) override; 

    
    void update(float deltaTime) override;
};

#endif // COIN_ITEM_HPP