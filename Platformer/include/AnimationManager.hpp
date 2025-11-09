#ifndef ANIMATIONMANAGER_HPP
#define ANIMATIONMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <iostream> 

// Lớp Animation
class Animation {
public:
    sf::Texture texture;
    int frameCount;
    float frameDuration;
    sf::Vector2i frameSize;

    // Sửa lại constructor (để trống)
    Animation() : frameCount(0), frameDuration(0.f), frameSize(0, 0) {}

    // Hàm load riêng
    void load(const std::string& path, sf::Vector2i fSize, int fCount, float fDuration) {
        if (!texture.loadFromFile(path)) {
            std::cerr << "!!! LOI: Khong tai duoc texture: " << path << std::endl;
        }
        texture.setSmooth(false);
        frameSize = fSize;
        frameCount = fCount;
        frameDuration = fDuration;
    }
};

// Lớp quản lý (cỗ máy)
class AnimationManager {
private:
    std::map<std::string, Animation> animations;
    sf::Sprite& sprite; 
    
    float timer;
    int currentFrame;
    std::string currentState;
    bool isLooping;

public:
    AnimationManager(sf::Sprite& sprite);
    ~AnimationManager();

    void addAnimation(const std::string& name, const std::string& path, 
                      sf::Vector2i frameSize, int frameCount, float frameDuration);

    void play(const std::string& name, bool loop = true);
    void update(float deltaTime);

    const std::string& getCurrentState() const { return currentState; }
    bool isFinished() const; // Hàm mới để kiểm tra animation 1 lần
};

#endif