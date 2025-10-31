#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "Player.hpp"
#include "Enemy.hpp"
#include "Platform.hpp"
#include "Mushroom.hpp"
#include "Menu.hpp"

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    WIN
};

class Game {
private:
    sf::RenderWindow window;
    sf::Clock gameClock;
    float totalTime;

    GameState currentState;

    sf::Texture backgroundTexture;
    sf::Sprite background;
    
    // Background cho Game Over
    sf::Texture gameOverBackgroundTexture;
    sf::Sprite  gameOverBackgroundSprite;

    sf::View camera;
    sf::View uiView;

    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Platform>> platforms;
    std::vector<std::unique_ptr<Mushroom>> mushrooms;

    std::unique_ptr<Menu> menu;

    // Font và UI
    sf::Font font;
    sf::Text scoreText; // Giữ lại, có thể dùng cho điểm số
    bool fontLoaded; // Biến thành viên (member variable)

    // UI Trái tim
    sf::Texture heartTexture;
    sf::Sprite heartSprite;

    // --- DÙNG TEXT THAY VÌ SPRITE ---
    sf::Text winText;           // Chữ "YOU WIN!"
    sf::Text gameOverText;      // Chữ "GAME OVER"
    sf::Text restartText;       // Chữ "Restart"
    sf::Text menuText;          // Chữ "Menu"
    // --- HẾT PHẦN SỬA ---

public:
    Game();
    ~Game();

    void run();

private:
    void initialize();
    void loadResources();
    void createLevel();

    void handleEvents();
    void update(float deltaTime);
    void render();

    void updateMenu();
    void updatePlaying(float deltaTime);
    void updateGameOver();
    void updateWin();

    void renderMenu();
    void renderPlaying();
    void renderGameOver();
    void renderWin();

    void startGame();
    void restartGame();
    void returnToMenu();
};

#endif