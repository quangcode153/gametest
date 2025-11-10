#include "CoinItem.hpp"
#include <iostream>
#include <cmath>

CoinItem::CoinItem(float x, float y)
    : Item(x, y, "assets/items/Coin.png", 0.0f),
    bobbingTimer(0.f),
    originalY(y)
{
    sprite.setScale(0.25f, 0.25f);
}

void CoinItem::onCollect(Player& player) {
    if (!collected) {
        player.addCoin(1);
        collected = true;
        std::cout << "Coin collected! Player total coins: " << player.getTotalCoins() << std::endl;
    }
}

void CoinItem::update(float deltaTime) {
    Item::update(deltaTime);

    if (!collected) {
        bobbingTimer += deltaTime;

        float offsetY = std::sin(bobbingTimer * 5.0f) * 3.0f;

        sprite.setPosition(sprite.getPosition().x, originalY + offsetY);
    }
}