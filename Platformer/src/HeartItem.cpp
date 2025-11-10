#include "HeartItem.hpp"
#include <iostream>
#include <cmath>

HeartItem::HeartItem(float x, float y)
    : Item(x, y, "assets/items/tim.png", 0.0f),
      bobbingTimer(0.f)
{
    sprite.setScale(0.23f, 0.23f);
    originalY = y;
}

void HeartItem::onCollect(Player& player) {
    if (!collected) {
        if (player.getHealth() < player.getMaxHealth()) {
            player.heal(1);
            this->collected = true;
            std::cout << "Heart collected! Player current health: " << player.getHealth() << std::endl;
        } else {
            std::cout << "Player already has full health. Heart not collected (full health)." << std::endl;
        }
    }
}

void HeartItem::update(float deltaTime) {
    Item::update(deltaTime);

    if (!collected) {
        bobbingTimer += deltaTime;

        float offsetY = std::sin(bobbingTimer * 5.0f) * 3.0f;

        sprite.setPosition(sprite.getPosition().x, originalY + offsetY);
    }
}