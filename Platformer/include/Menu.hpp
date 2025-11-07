#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// Định nghĩa các lựa chọn có trong menu
enum class MenuOption {
    PLAY, // Đổi tên cho khớp
    SETTINGS, 
    EXIT,
    COUNT // Đếm số lượng
};


class Menu {
public:
    Menu();
    ~Menu();

    bool loadFont(const std::string& path);
    void initialize();
    void draw(sf::RenderWindow& window);
    void update(float deltaTime);

    void moveUp();
    void moveDown();
    MenuOption getSelectedOption() const;

    bool isPlayButtonClicked(const sf::Vector2f& mousePos) const; // Đổi tên
    bool isExitButtonClicked(const sf::Vector2f& mousePos) const;

private:
    sf::Font font;
    sf::Text titleText; // Tiêu đề "ADVENTURES OF A PRINCE"
    sf::Text instructionsText; // Chữ "Press Enter..."

    // === THAY ĐỔI: Bỏ vector Text và Shape ===
    // std::vector<sf::Text> menuOptions;
    // std::vector<sf::RectangleShape> menuButtonShapes; 
    
    // === THÊM VÀO: 6 TEXTURES VÀ 3 SPRITES ===
    sf::Texture texPlayNormal, texPlayHighlight;
    sf::Texture texSettingsNormal, texSettingsHighlight;
    sf::Texture texExitNormal, texExitHighlight;
    
    sf::Sprite spritePlay;
    sf::Sprite spriteSettings;
    sf::Sprite spriteExit;
    // ======================================

    sf::RectangleShape background; // Lớp phủ màu tối (đã vô hiệu hóa)

    sf::Texture frameTexture;
    sf::Sprite frameSprite;

    int selectedIndex;
    bool fontLoaded;
    float animationTimer;

    // Hàm helper để scale sprite cho vừa
    void setSpriteToSize(sf::Sprite& sprite, const sf::Vector2f& size);
};

#endif // MENU_HPP