#ifndef ITEM_HPP
#define ITEM_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

class Player;

class Item {
protected:
    sf::Sprite sprite;
    sf::Texture texture;
    bool collected;
    float effectDuration;
    float currentEffectTime;

public:
    Item(float x, float y, const std::string& texturePath, float duration = 0.0f);
    virtual ~Item() = default;

    virtual void onCollect(Player& player) = 0;
    
    virtual void update(float deltaTime);

    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    bool isCollected() const;
    void setCollected(bool val);

    float getEffectDuration() const { return effectDuration; }
    float getCurrentEffectTime() const { return currentEffectTime; }
    void resetEffectTime() { currentEffectTime = effectDuration; }
};

#endif