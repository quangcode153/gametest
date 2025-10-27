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
    WIN // <-- THÊM TRẠNG THÁI NÀY
};

class Game {
private:
    sf::RenderWindow window;
    sf::Clock gameClock;
    float totalTime;

    GameState currentState;

    sf::Texture backgroundTexture;
    sf::Sprite background;

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
    sf::Text gameOverText;
    sf::Text winText; // <-- THÊM BIẾN NÀY ĐỂ HIỂN THỊ CHỮ WIN
    bool fontLoaded;

    // UI Trái tim
    sf::Texture heartTexture;
    sf::Sprite heartSprite;

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
    void updateWin(); // <-- THÊM HÀM NÀY

    void renderMenu();
    void renderPlaying();
    void renderGameOver();
    void renderWin(); // <-- THÊM HÀM NÀY

    void startGame();
    void restartGame();
    void returnToMenu();
};

#endif