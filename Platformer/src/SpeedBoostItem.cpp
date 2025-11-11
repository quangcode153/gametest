#include "SpeedBoostItem.hpp"
#include "Player.hpp"
#include <iostream>
#include <cmath>
#include "ResourceManager.hpp"
SpeedBoostItem::SpeedBoostItem(float x, float y)
    : Item(x, y, "assets/items/itemSpeed.png", 5.0f),
      bobbingTimer(0.f)
{
    sprite.setScale(0.1f, 0.1f);
    originalY = y;
}

void SpeedBoostItem::onCollect(Player& player) {
    if (!collected) {
        player.activateSpeedBoost(this->getEffectDuration());
        this->collected = true;
        ResourceManager::getInstance().playSound("speedItemsfx.wav");
        std::cout << "Speed Boost collected!" << std::endl;
    }
}

void SpeedBoostItem::update(float deltaTime) {
    Item::update(deltaTime);

    if (!collected) {
        bobbingTimer += deltaTime;

        float offsetY = std::sin(bobbingTimer * 5.0f) * 3.0f;

        sprite.setPosition(sprite.getPosition().x, originalY + offsetY);
    }
}