#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <list> // Dùng để quản lý nhiều âm thanh phát cùng lúc

class ResourceManager {
public:
    // Hàm để truy cập duy nhất vào trình quản lý
    static ResourceManager& getInstance();

    // Hàm nạp tài nguyên (sẽ được gọi 1 lần trong Game)
    void loadResources();

    // Hàm lấy Texture (nếu bạn muốn dùng)
    sf::Texture& getTexture(const std::string& name);
    
    // === CÁC HÀM ÂM THANH MỚI ===

    // Nạp một SoundBuffer
    void loadSoundBuffer(const std::string& name, const std::string& path);
    // Lấy một SoundBuffer
    sf::SoundBuffer& getSoundBuffer(const std::string& name);

    // Phát SFX (dùng cho tiếng nhảy, ăn xu, v.v.)
    void playSound(const std::string& name);

    // Phát Music (dùng cho nhạc nền)
    void playMusic(const std::string& filename, bool loop = true);

    // Dừng Music
    void stopMusic();
    void toggleMusicMute();
    bool isMusicMuted() const;
    void setMusicVolume(float volume);

    // Cấm sao chép và gán (cho singleton)
    ResourceManager(const ResourceManager&) = delete;
    void operator=(const ResourceManager&) = delete;

private:
    // Hàm khởi tạo riêng tư (cho singleton)
    ResourceManager() : mCurrentMusicVolume(100.f), mMusicMuted(false){} 
    
    std::map<std::string, std::unique_ptr<sf::Texture>> mTextures;
    
    // === CÁC BIẾN ÂM THANH MỚI ===
    std::map<std::string, std::unique_ptr<sf::SoundBuffer>> mSoundBuffers;
    sf::Music mMusic; // Chỉ có 1 nhạc nền phát tại 1 thời điểm
    
    // Dùng list để có thể phát nhiều SFX chồng lên nhau
    std::list<sf::Sound> mSounds; 
    float mCurrentMusicVolume;
    bool mMusicMuted;
};