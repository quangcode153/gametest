#include "Enemy.hpp"
#include <iostream>

Enemy::Enemy() :
    velocity(0.f, 0.f),
    speed(100.f),
    movingRight(true),
    alive(true)
{
}

Enemy::~Enemy() {
}

bool Enemy::loadTexture(const std::string& path) {
    sf::Image image;
    if (!image.loadFromFile(path)) {
        std::cerr << "Loi: Khong the tai file enemy: " << path << std::endl;
        return false;
    }

    image.createMaskFromColor(sf::Color::White);

    if (!texture.loadFromImage(image)) {
        std::cerr << "Loi: Khong the nap texture tu image enemy da xu ly." << std::endl;
        return false;
    }

    sprite.setTexture(texture);

    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height); 
    
    sprite.setScale(0.15f, 0.15f); 

    return true;
}

void Enemy::setPosition(const sf::Vector2f& pos) {
    sprite.setPosition(pos);
}

sf::Vector2f Enemy::getPosition() const {
    return sprite.getPosition();
}

sf::FloatRect Enemy::getBounds() const {
    return sprite.getGlobalBounds();
}

void Enemy::update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms) {
    if (!alive) return;
    
    velocity.y += 1000.f * deltaTime;
    
    patrol(); 
    
    sprite.move(velocity * deltaTime);
    
    sf::FloatRect enemyBounds = getBounds();
    
    for (const auto& platform : platforms) {
        sf::FloatRect platformBounds = platform->getBounds();

        if (enemyBounds.intersects(platformBounds)) {
            if (velocity.y > 0) {
                float previousBottom = enemyBounds.top + enemyBounds.height - (velocity.y * deltaTime);

                if (previousBottom <= platformBounds.top) {
                    sprite.setPosition(sprite.getPosition().x, platformBounds.top);
                    velocity.y = 0;
                }
            }
        }
    }
    
    checkBounds(platforms);
}

void Enemy::patrol() {
    if (movingRight) {
        velocity.x = speed;
    } else {
        velocity.x = -speed;
    }
}

void Enemy::checkBounds(const std::vector<std::unique_ptr<Platform>>& platforms) {
    sf::FloatRect enemyBounds = getBounds();
    sf::Vector2f currentPos = sprite.getPosition();

    sf::Vector2f feelerPoint = currentPos;
    float feelerXOffset = (enemyBounds.width / 2.f) + 5.f;
    feelerPoint.y += 5.f;

    if (movingRight) {
        feelerPoint.x += feelerXOffset;
    } else {
        feelerPoint.x -= feelerXOffset;
    }

    bool groundInFront = false;
    for (const auto& platform : platforms) {
        if (platform->getBounds().contains(feelerPoint)) {
            groundInFront = true;
            break;
        }
    }

    if (!groundInFront && velocity.y == 0) { 
        movingRight = !movingRight;
    }
    
    float currentScale = sprite.getScale().x;
    if (currentScale < 0) currentScale = -currentScale; 

    if (movingRight) {
        sprite.setScale(currentScale, currentScale);
    } else {
        sprite.setScale(-currentScale, currentScale);
    }
}


void Enemy::kill() {
    alive = false;
    sprite.setPosition(-1000.f, -1000.f);
}

void Enemy::draw(sf::RenderWindow& window) {
    if (alive) {
        window.draw(sprite);
    }
}