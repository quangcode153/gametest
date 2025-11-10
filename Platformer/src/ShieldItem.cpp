#include "ShieldItem.hpp"
#include "Player.hpp"
#include <iostream>
#include <cmath>

ShieldItem::ShieldItem(float x, float y)
    : Item(x, y, "assets/items/shieldItem.png", 4.0f),
      bobbingTimer(0.f)
{
    this->sprite.setScale(0.1f, 0.1f);
    originalY = y;
}

void ShieldItem::onCollect(Player& player) {
    if (!collected) {
        player.activateShield(this->getEffectDuration());
        this->collected = true;
        std::cout << "Shield item collected!" << std::endl;
    }
}

void ShieldItem::update(float deltaTime) {
    Item::update(deltaTime);

    if (!collected) {
        bobbingTimer += deltaTime;
        float offsetY = std::sin(bobbingTimer * 5.0f) * 3.0f;
        sprite.setPosition(sprite.getPosition().x, originalY + offsetY);
    }
}