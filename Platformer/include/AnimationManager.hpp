#pragma once
#include <iostream> 
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

// (Struct Animation)
struct Animation {
    sf::Texture texture;
    sf::Vector2i frameSize;
    int frameCount;
    float frameDuration;
    
    bool load(const std::string& path, sf::Vector2i fSize, int fCount, float fDur) {
        if (!texture.loadFromFile(path)) {
            std::cerr << "Failed to load animation texture: " << path << std::endl;
            return false;
        }
        frameSize = fSize;
        frameCount = fCount;
        frameDuration = fDur;
        return true;
    }
};


class AnimationManager {
public:
    AnimationManager(sf::Sprite& sprite);
    ~AnimationManager();

    void addAnimation(const std::string& name, const std::string& path, 
                      sf::Vector2i frameSize, int frameCount, float frameDuration);
    void play(const std::string& name, bool loop);
    void update(float deltaTime);
    bool isFinished() const;

    // <-- THÊM HÀM NÀY ĐỂ SỬA LỖI MODEL
    void clearAnimations(); 

private:
    sf::Sprite& sprite;
    std::map<std::string, Animation> animations;
    float timer;
    int currentFrame;
    std::string currentState;
    bool isLooping; // <-- Tên biến chính xác
};