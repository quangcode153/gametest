#ifndef HEART_ITEM_HPP
#define HEART_ITEM_HPP

#include "Item.hpp" // Kế thừa từ lớp Item cơ sở
#include "Player.hpp" // Cần Player để gọi hàm heal

class HeartItem : public Item {
private:
    float bobbingTimer; // Timer riêng cho mỗi trái tim
    float originalY;
public:
    // Constructor của HeartItem
    // Sẽ gọi constructor của lớp cha Item với đường dẫn texture và duration = 0.0f
    HeartItem(float x, float y);
    
    // Triển khai (override) hàm onCollect từ lớp cha Item
    // Khi Player nhặt HeartItem, Player sẽ được hồi máu
    void onCollect(Player& player) override; 

    // Không cần override update nếu bạn không muốn animation cho HeartItem
    // Nếu muốn tim nhấp nháy hoặc dao động nhẹ, có thể override ở đây.
    // Hiện tại, không override để giữ cho nó tĩnh.
    void update(float deltaTime) override; 
};

#endif // HEART_ITEM_HPP