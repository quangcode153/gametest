#include "AnimationManager.hpp"
#include <iostream>

// --- ĐÃ SỬA LỖI GÕ PHÍM "isLoopLooing" ---
AnimationManager::AnimationManager(sf::Sprite& sprite)
    : sprite(sprite), timer(0.f), currentFrame(0), currentState(""), isLooping(true) {}

AnimationManager::~AnimationManager() {}

void AnimationManager::addAnimation(const std::string& name, const std::string& path, 
                                    sf::Vector2i frameSize, int frameCount, float frameDuration)
{
    animations[name].load(path, frameSize, frameCount, frameDuration);
}

void AnimationManager::play(const std::string& name, bool loop) {
    if (animations.find(name) == animations.end()) {
        std::cerr << "AnimationManager: Animation '" << name << "' not found!" << std::endl;
        return;
    }

    // Chỉ reset nếu là animation mới
    if (currentState != name) {
        currentState = name;
        timer = 0.f;
        currentFrame = 0;
        isLooping = loop;
        
        sprite.setTexture(animations[name].texture);
        
        // Cập nhật Origin cho frameSize MỚI
        sf::Vector2i frameSize = animations[name].frameSize;
        sprite.setOrigin(frameSize.x / 2.f, frameSize.y / 2.f); 
    }
}

void AnimationManager::update(float deltaTime) {
    if (currentState.empty() || animations.find(currentState) == animations.end()) {
        return; // Không có animation để chạy
    }

    Animation& anim = animations[currentState];
    if (anim.frameCount == 0) return; // Tránh lỗi chia cho 0

    timer += deltaTime;
    if (timer >= anim.frameDuration) {
        timer -= anim.frameDuration;
        
        currentFrame++;
        
        if (currentFrame >= anim.frameCount) {
            if (isLooping) {
                currentFrame = 0; // Quay về đầu nếu là animation lặp
            } else {
                currentFrame = anim.frameCount - 1; // Giữ ở frame cuối
            }
        }
    }
    
    // Cắt ảnh (Sử dụng frameSize từ animation)
    sf::IntRect textureRect;
    textureRect.top = 0; 
    textureRect.left = currentFrame * anim.frameSize.x;
    textureRect.width = anim.frameSize.x;
    textureRect.height = anim.frameSize.y;
    
    sprite.setTextureRect(textureRect);
}

bool AnimationManager::isFinished() const {
    if (isLooping || currentState.empty()) {
        return false;
    }
    if (animations.find(currentState) == animations.end()) {
        return true; 
    }
    const Animation& anim = animations.at(currentState);
    return (currentFrame >= anim.frameCount - 1);
}

// <-- ĐÃ THÊM: Hàm clearAnimations (Để sửa lỗi model) ---
void AnimationManager::clearAnimations() {
    animations.clear(); // Xóa tất cả hoạt ảnh đã nạp
    
    // Reset trạng thái
    currentState = "";
    currentFrame = 0;
    timer = 0.f;
    sprite.setTexture(sf::Texture()); // Xóa texture của sprite
}
int AnimationManager::getCurrentFrameIndex() const {
    return currentFrame;
}