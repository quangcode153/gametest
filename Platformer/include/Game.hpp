#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Enemies/Boss.hpp"

class Player;
class Platform;
class Enemy;
class Item;
class Menu; 
class CoinItem;
class HeartItem;
class SpeedBoostItem;
class ShieldItem;


enum class GameState {
    MENU,
    PLAYING,
    GAME_OVER,
    WIN,
    CHARACTER_SELECTION, 
    SETTINGS
};

// Cấu trúc để lưu dữ liệu thẻ bài
struct CharacterData {
    std::string name;
    std::string assetFolder;
    sf::Texture previewTexture;
    sf::Sprite previewSprite;
};

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    void initialize();
    void loadResources();
    void loadLevel(int levelNumber);
    void handleEvents();
    void update(float deltaTime);
    void render();

    // Các hàm update con
    void updateMenu();
    void updatePlaying(float deltaTime);
    void updateGameOver();
    void updateWin();

    // Các hàm render con
    void renderMenu();
    void renderPlaying();
    void renderGameOver();
    void renderWin();

    // Các hàm logic
    void startGame();
    void restartGame();
    void returnToMenu();

    sf::RenderWindow window;
    sf::View camera;
    sf::View uiView;
    sf::Font font;
    bool fontLoaded;
    float totalTime;
    sf::Clock gameClock;

    GameState currentState;
    int currentLevel;
    int maxLevels;

    
    sf::Texture backgroundTexture;
    sf::Sprite background;
    sf::Texture heartTexture;
    sf::Sprite heartSprite;

    
    std::unique_ptr<Menu> menu;
    sf::Texture menuBackgroundTexture;
    sf::Sprite menuBackgroundSprite;

    
    sf::Texture gameOverBackgroundTexture;
    sf::Sprite gameOverBackgroundSprite;
    sf::Text gameOverText;
    sf::Text winText;
    sf::Text restartText;
    sf::Text menuText;

    
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Platform>> platforms;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Item>> items;

    
    std::vector<CharacterData> characterList;
    int selectedCharacterIndex;

    sf::Texture charSelectBackgroundTexture;
    sf::Sprite charSelectBackgroundSprite;
    sf::Texture selectorTexture; 
    sf::Sprite selectorSprite;
    
    sf::Text charSelectTitle; 
    sf::Text charNameText;    
    sf::Text charSelectHelpText; 
    sf::Texture texMusic;       
    sf::Texture texMusicMuted;    
    sf::Sprite spriteMusicButton; 
    sf::Texture texBackArrow;       
    sf::Sprite spriteBackArrow;
   
    void updateCharacterSelection(float deltaTime);
    void renderCharacterSelection();
    void updateSelectorPosition(); 
    void updateSettings(float deltaTime);
    void renderSettings();
    void updateMusicButtonTexture();
};