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
    
    sf::Texture gameOverBackgroundTexture;
    sf::Sprite  gameOverBackgroundSprite;

    sf::View camera;
    sf::View uiView;

    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Platform>> platforms;
    std::vector<std::unique_ptr<Mushroom>> mushrooms;

    std::unique_ptr<Menu> menu;

    sf::Font font;
    sf::Text scoreText; 
    bool fontLoaded; 

    sf::Texture heartTexture;
    sf::Sprite heartSprite;

    sf::Text winText;         
    sf::Text gameOverText;    
    sf::Text restartText;     
    sf::Text menuText;        

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