#include "ResourceManager.hpp"
#include <filesystem> // Cần để duyệt thư mục

// Định nghĩa cho singleton
ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

// === HÀM TẢI TÀI NGUYÊN TỔNG ===
// (Chúng ta sẽ tự động nạp tất cả âm thanh ở đây)
void ResourceManager::loadResources() {
    std::cout << "--- Loading All Sounds ---" << std::endl;
    
    // 1. NẠP TẤT CẢ SFX (trong thư mục sfx)
    std::string sfxPath = "assets/sfx/";
    // Tự động quét thư mục sfx và nạp file .wav
    // (Lưu ý: Cần C++17 trở lên cho std::filesystem)
    try {
        for (const auto& entry : std::filesystem::directory_iterator(sfxPath)) {
            if (entry.path().extension() == ".wav") {
                std::string filename = entry.path().filename().string();
                std::cout << "Loading SFX: " << filename << " from " << entry.path().string() << std::endl;
                loadSoundBuffer(filename, entry.path().string());
            }
        }
    } catch (std::exception& e) {
        std::cerr << "!!! LOI: Khong the quet thu muc 'assets/sfx/'. " << e.what() << std::endl;
        std::cerr << "Vui long kiem tra lai duong dan." << std::endl;
    }

    // 2. NẠP NHẠC NỀN (trong thư mục music)
    // Nhạc nền sẽ được stream, không nạp trước
    std::cout << "--- Sound loading complete ---" << std::endl;
}

// === Triển khai các hàm âm thanh ===

void ResourceManager::loadSoundBuffer(const std::string& name, const std::string& path) {
    auto buffer = std::make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(path)) {
        std::cerr << "!!! LOI: Khong the tai sound buffer: " << path << std::endl;
    } else {
        mSoundBuffers[name] = std::move(buffer);
    }
}

sf::SoundBuffer& ResourceManager::getSoundBuffer(const std::string& name) {
    auto found = mSoundBuffers.find(name);
    if (found == mSoundBuffers.end()) {
        std::cerr << "!!! LOI: Khong tim thay sound buffer ten: " << name << std::endl;
        // Trả về một buffer rỗng để tránh crash (cần xử lý tốt hơn)
        static sf::SoundBuffer emptyBuffer;
        return emptyBuffer; 
    }
    return *found->second;
}

void ResourceManager::playSound(const std::string& name) {
    // Xóa các âm thanh đã phát xong ra khỏi list
    mSounds.remove_if([](const sf::Sound& s) {
        return s.getStatus() == sf::Sound::Stopped;
    });

    // Thêm âm thanh mới vào list và phát
    try {
        mSounds.emplace_back(getSoundBuffer(name));
        mSounds.back().play();
    } catch (std::exception& e) {
        std::cerr << "!!! LOI khi playSound(" << name << "): " << e.what() << std::endl;
    }
}

void ResourceManager::playMusic(const std::string& filename, bool loop) {
    std::string path = "assets/music/" + filename;
    if (!mMusic.openFromFile(path)) {
        std::cerr << "!!! LOI: Khong the mo file music: " << path << std::endl;
    } else {
        mMusic.setLoop(loop);
        if (mMusicMuted) {
            mMusic.setVolume(0.f);
        } else {
            mMusic.setVolume(mCurrentMusicVolume);
        }
        mMusic.play();
        std::cout << "Playing music: " << path << std::endl;
    }
}

void ResourceManager::stopMusic() {
    mMusic.stop();
}

// (Các hàm Texture, nếu bạn muốn dùng ResourceManager cho cả ảnh)
sf::Texture& ResourceManager::getTexture(const std::string& name) {
    auto found = mTextures.find(name);
    if (found == mTextures.end()) {
        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile(name)) {
            std::cerr << "!!! LOI: Khong the tai texture: " << name << std::endl;
        }
        mTextures[name] = std::move(texture);
        return *mTextures[name];
    }
    return *found->second;
}
// === DÁN 3 HÀM MỚI NÀY VÀO CUỐI FILE ===

void ResourceManager::toggleMusicMute() {
    mMusicMuted = !mMusicMuted; // Đảo ngược trạng thái
    if (mMusicMuted) {
        mMusic.setVolume(0.f); // Nếu tắt -> đặt âm lượng = 0
        std::cout << "Music Muted" << std::endl;
    } else {
        mMusic.setVolume(mCurrentMusicVolume); // Nếu bật -> đặt lại âm lượng cũ
        std::cout << "Music Unmuted" << std::endl;
    }
}

void ResourceManager::setMusicVolume(float volume) {
    mCurrentMusicVolume = volume;
    if (!mMusicMuted) {
        mMusic.setVolume(mCurrentMusicVolume);
    }
}

bool ResourceManager::isMusicMuted() const {
    return mMusicMuted;
}