#include "Menu.hpp"
#include <iostream>
#include <cmath>

Menu::Menu() :
    selectedIndex(0),
    fontLoaded(false),
    animationTimer(0.f)
{
    background.setSize(sf::Vector2f(1200.f, 800.f));
    background.setFillColor(sf::Color(20, 30, 50, 230));
    
    // Tải khung gỗ
    if (!frameTexture.loadFromFile("assets/khungMenu.png")) {
        std::cerr << "!!! LOI: Khong the tai assets/khungMenu.png\n";
    }
    
    // === TẢI 6 ẢNH NÚT BẤM ===
    if (!texPlayNormal.loadFromFile("assets/button_play_normal.png")) 
        std::cerr << "Loi tai button_play_normal.png\n";
    if (!texPlayHighlight.loadFromFile("assets/button_play_highlight.png")) 
        std::cerr << "Loi tai button_play_highlight.png\n";
        
    if (!texSettingsNormal.loadFromFile("assets/button_settings_normal.png")) 
        std::cerr << "Loi tai button_settings_normal.png\n";
    if (!texSettingsHighlight.loadFromFile("assets/button_settings_highlight.png")) 
        std::cerr << "Loi tai button_settings_highlight.png\n";
        
    if (!texExitNormal.loadFromFile("assets/button_exit_normal.png")) 
        std::cerr << "Loi tai button_exit_normal.png\n";
    if (!texExitHighlight.loadFromFile("assets/button_exit_highlight.png")) 
        std::cerr << "Loi tai button_exit_highlight.png\n";
    
    std::cout << "Menu initialized\n";
}

Menu::~Menu() {
}

bool Menu::loadFont(const std::string& path) {
    if (!font.loadFromFile(path)) {
        std::cout << "ERROR: Cannot load menu font: " << path << "\n";
        return false;
    }
    fontLoaded = true;
    std::cout << "Menu font loaded: " << path << "\n";
    return true;
}

// === HÀM SCALE SPRITE SỬA LỖI (DÙNG setTextureRect) ===
void Menu::setSpriteToSize(sf::Sprite& sprite, const sf::Vector2f& targetSize) {
    const sf::Texture* texture = sprite.getTexture();
    if (!texture) return; 
    sf::Vector2u texSize = texture->getSize();
    if (texSize.x == 0 || texSize.y == 0) return; 

    // 1. ÉP sprite sử dụng toàn bộ texture (Rất quan trọng)
    sprite.setTextureRect(sf::IntRect(0, 0, texSize.x, texSize.y));
    
    // 2. Đặt origin về TÂM
    sprite.setOrigin(static_cast<float>(texSize.x) / 2.f, static_cast<float>(texSize.y) / 2.f);

    // 3. Tính scale
    float scaleX = targetSize.x / static_cast<float>(texSize.x);
    float scaleY = targetSize.y / static_cast<float>(texSize.y);
    
    // 4. Áp dụng scale (Không giữ tỷ lệ, để lấp đầy nút)
    sprite.setScale(scaleX, scaleY);
}


void Menu::initialize() {
    if (!fontLoaded) {
        std::cout << "WARNING: Font not loaded, menu text will not display\n";
    }
    
    // 1. Cài đặt khung
    frameSprite.setTexture(frameTexture);
    sf::FloatRect frameBounds = frameSprite.getLocalBounds();
    frameSprite.setOrigin(frameBounds.width / 2.f, frameBounds.height / 2.f);
    // Đặt vị trí khung ở giữa (giống ảnh mẫu)
    frameSprite.setPosition(600.f, 400.f); 
    
    // 2. === CÀI ĐẶT 3 SPRITE NÚT BẤM ===
    float frameCenterY = frameSprite.getPosition().y;
    // (Điều chỉnh lại vị trí Y và khoảng cách cho đẹp)
    float spacing = 80.f; 
    float startY = frameCenterY - spacing + 10; // Đẩy lên 1 chút
    
    // (Kích thước nút: Rộng 300, Cao 60)
    sf::Vector2f buttonSize(300.f, 60.f); 

    // Nút PLAY
    spritePlay.setTexture(texPlayHighlight); 
    spritePlay.setPosition(600.f, startY + 0 * spacing);
    setSpriteToSize(spritePlay, buttonSize); 

    // Nút SETTINGS
    spriteSettings.setTexture(texSettingsNormal); 
    spriteSettings.setPosition(600.f, startY + 1 * spacing);
    setSpriteToSize(spriteSettings, buttonSize);

    // Nút EXIT
    spriteExit.setTexture(texExitNormal); 
    spriteExit.setPosition(600.f, startY + 2 * spacing);
    setSpriteToSize(spriteExit, buttonSize);
    
    // 3. Hướng dẫn (Giống ảnh mẫu)
    instructionsText.setFont(font);
    instructionsText.setString("Press Enter to Select");
    instructionsText.setCharacterSize(24);
    instructionsText.setFillColor(sf::Color(255, 215, 0)); // Màu vàng
    sf::FloatRect instBounds = instructionsText.getLocalBounds();
    instructionsText.setOrigin(instBounds.width / 2.f, instBounds.height / 2.f);
    // Đặt vị trí dưới khung
    instructionsText.setPosition(600.f, frameSprite.getPosition().y + frameSprite.getGlobalBounds().height / 2.f + 40.f);

    // 4. Đặt lựa chọn ban đầu
    selectedIndex = 0; 
    
    std::cout << "Menu initialized with 3 sprites\n";
}

// === SỬA LẠI moveUp/moveDown ĐỂ GỌI setTextureRect ===

void Menu::moveUp() {
    selectedIndex--;
    if (selectedIndex < 0) {
        selectedIndex = static_cast<int>(MenuOption::COUNT) - 1;
    }
    
    // Cập nhật texture
    spritePlay.setTexture(selectedIndex == 0 ? texPlayHighlight : texPlayNormal);
    spriteSettings.setTexture(selectedIndex == 1 ? texSettingsHighlight : texSettingsNormal);
    spriteExit.setTexture(selectedIndex == 2 ? texExitHighlight : texExitNormal);
    
    // Ép reset TextureRect
    sf::Vector2u sizeP = spritePlay.getTexture()->getSize();
    spritePlay.setTextureRect(sf::IntRect(0, 0, sizeP.x, sizeP.y));
    sf::Vector2u sizeS = spriteSettings.getTexture()->getSize();
    spriteSettings.setTextureRect(sf::IntRect(0, 0, sizeS.x, sizeS.y));
    sf::Vector2u sizeE = spriteExit.getTexture()->getSize();
    spriteExit.setTextureRect(sf::IntRect(0, 0, sizeE.x, sizeE.y));

    std::cout << "Menu selection: " << selectedIndex << "\n";
}

void Menu::moveDown() {
    selectedIndex++;
    if (selectedIndex >= static_cast<int>(MenuOption::COUNT)) {
        selectedIndex = 0;
    }
    
    // Cập nhật texture
    spritePlay.setTexture(selectedIndex == 0 ? texPlayHighlight : texPlayNormal);
    spriteSettings.setTexture(selectedIndex == 1 ? texSettingsHighlight : texSettingsNormal);
    spriteExit.setTexture(selectedIndex == 2 ? texExitHighlight : texExitNormal);

    // Ép reset TextureRect
    sf::Vector2u sizeP = spritePlay.getTexture()->getSize();
    spritePlay.setTextureRect(sf::IntRect(0, 0, sizeP.x, sizeP.y));
    sf::Vector2u sizeS = spriteSettings.getTexture()->getSize();
    spriteSettings.setTextureRect(sf::IntRect(0, 0, sizeS.x, sizeS.y));
    sf::Vector2u sizeE = spriteExit.getTexture()->getSize();
    spriteExit.setTextureRect(sf::IntRect(0, 0, sizeE.x, sizeE.y));
    
    std::cout << "Menu selection: " << selectedIndex << "\n";
}

MenuOption Menu::getSelectedOption() const {
    return static_cast<MenuOption>(selectedIndex);
}

void Menu::update(float deltaTime) {
    animationTimer += deltaTime;
    
    // Hiệu ứng nhấp nháy (Scale)
    float pulseScale = 1.0f + 0.05f * std::abs(std::sin(animationTimer * 5.f)); 
    sf::Vector2f buttonSize(300.f, 60.f); // Kích thước gốc của nút

    // Ép scale lại mỗi frame (vì setTextureRect có thể đã reset scale)
    setSpriteToSize(spritePlay, (selectedIndex == 0) ? (buttonSize * pulseScale) : buttonSize);
    setSpriteToSize(spriteSettings, (selectedIndex == 1) ? (buttonSize * pulseScale) : buttonSize);
    setSpriteToSize(spriteExit, (selectedIndex == 2) ? (buttonSize * pulseScale) : buttonSize);
}

void Menu::draw(sf::RenderWindow& window) {
    // Vô hiệu hóa lớp phủ màu tối
    // window.draw(background); 
    
    // Bỏ vẽ tiêu đề 
    // window.draw(titleText);
    
    // 1. Vẽ khung gỗ
    window.draw(frameSprite);
    
    // 2. Vẽ 3 nút (là 3 sprite)
    window.draw(spritePlay);
    window.draw(spriteSettings);
    window.draw(spriteExit);
    
    // 3. Vẽ chữ hướng dẫn
    if (fontLoaded) {
        window.draw(instructionsText);
    }
}

bool Menu::isPlayButtonClicked(const sf::Vector2f& mousePos) const {
    return spritePlay.getGlobalBounds().contains(mousePos);
}

bool Menu::isExitButtonClicked(const sf::Vector2f& mousePos) const {
    return spriteExit.getGlobalBounds().contains(mousePos);
}