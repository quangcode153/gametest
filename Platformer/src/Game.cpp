#include "Game.hpp"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <fstream> 
#include <sstream>

Game::Game() :
    totalTime(0.f),
    currentState(GameState::MENU),
    fontLoaded(false),
    currentLevel(1),
    maxLevels(2)
{
    window.create(sf::VideoMode(1200, 800), "game test");
    window.setFramerateLimit(60);
    camera.setSize(1200.f, 800.f); camera.setCenter(600.f, 400.f);
    uiView.setSize(1200.f, 800.f); uiView.setCenter(600.f, 400.f);
    initialize();
}

Game::~Game() {}

void Game::initialize() {
    std::cout << "=== Initializing Game ===\n";
    loadResources(); 
    
    menu = std::make_unique<Menu>();
    
    std::string menuFontPath = "assets/fonts/arial.ttf"; 
    if (!menu->loadFont(menuFontPath)) { 
        std::cout << "Menu failed to load relative font, trying system font..." << std::endl;
        menu->loadFont("C:\\Windows\\Fonts\\arial.ttf"); 
    }
    
    menu->initialize();
    std::cout << "Game initialized!\n";
}

void Game::loadResources() {
    
    std::vector<std::string> bgPaths = {"assets/background.png", "../assets/background.png"};
    bool bgLoaded = false;
    for (const auto& path : bgPaths) {
        if (backgroundTexture.loadFromFile(path)) {
            std::cout << "✓ Background loaded: " << path << "\n";
            bgLoaded = true;
            break;
        }
    }
    if (!bgLoaded) {
        std::cout << "✗ Background not found, using solid color\n";
    } else {
        background.setTexture(backgroundTexture);
        sf::Vector2u bgSize = backgroundTexture.getSize();
        float scaleX = 2400.f / bgSize.x;
        float scaleY = 800.f / bgSize.y;
        background.setScale(scaleX, scaleY);
    }

    
    if (!gameOverBackgroundTexture.loadFromFile("assets/backgroundEnd.png")) {
        std::cerr << "LOI: Khong tai duoc assets/backgroundEnd.png\n";
    } else {
        std::cout << "✓ Game Over Background loaded: assets/backgroundEnd.png\n";
        sf::Vector2u bgSize = gameOverBackgroundTexture.getSize();
        gameOverBackgroundSprite.setTexture(gameOverBackgroundTexture);
        gameOverBackgroundSprite.setScale(1200.f / bgSize.x, 800.f / bgSize.y);
    }

    
    sf::Image heartImage;
    if (!heartImage.loadFromFile("assets/heart.png")) {
        std::cout << "✗ Heart image NOT FOUND! (assets/heart.png)\n";
    } else {
        std::cout << "✓ Heart image loaded.\n";
        heartImage.createMaskFromColor(sf::Color::White); 
        if (!heartTexture.loadFromImage(heartImage)) {
             std::cerr << "Loi: Khong the nap heart texture tu image da xu ly." << std::endl;
        } else {
             std::cout << "✓ Heart texture created from image.\n";
        }
    }
    heartSprite.setTexture(heartTexture);
    heartSprite.setScale(0.5f, 0.5f); 

    
    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf", 
        "C:\\Windows\\Fonts\\arial.ttf", 
        "/System/Library/Fonts/Arial.ttf", 
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" 
    };
    
    fontLoaded = false; 
    
    for (const auto& path : fontPaths) {
        std::cout << "Trying font: " << path << "... ";
        if (font.loadFromFile(path)) {
            std::cout << "SUCCESS!\n";
            fontLoaded = true; 
            break;
        } else {
            std::cout << "failed.\n";
        }
    }
     if (!fontLoaded) {
        std::cerr << "✗ ERROR: Font NOT FOUND! UI text might not display.\n";
    }

    
    if (fontLoaded) {
        
        gameOverText.setFont(font); 
        gameOverText.setString("GAME OVER");
        gameOverText.setCharacterSize(72);
        gameOverText.setFillColor(sf::Color::Red);
        sf::FloatRect boundsGO = gameOverText.getLocalBounds();
        gameOverText.setOrigin(boundsGO.width / 2.f, boundsGO.height / 2.f);
        gameOverText.setPosition(600.f, 250.f);

        
        winText.setFont(font); 
        winText.setString("YOU WIN!");
        winText.setCharacterSize(72);
        winText.setFillColor(sf::Color::Green);
        sf::FloatRect boundsWin = winText.getLocalBounds();
        winText.setOrigin(boundsWin.width / 2.f, boundsWin.height / 2.f);
        winText.setPosition(600.f, 250.f);

        
        restartText.setFont(font); 
        restartText.setString("Restart");
        restartText.setCharacterSize(40);
        restartText.setFillColor(sf::Color::White);
        sf::FloatRect boundsRst = restartText.getLocalBounds();
        restartText.setOrigin(boundsRst.width / 2.f, boundsRst.height / 2.f);
        restartText.setPosition(480.f, 400.f);

        
        menuText.setFont(font); 
        menuText.setString("Menu");
        menuText.setCharacterSize(40);
        menuText.setFillColor(sf::Color::White);
        sf::FloatRect boundsMenu = menuText.getLocalBounds();
        menuText.setOrigin(boundsMenu.width / 2.f, boundsMenu.height / 2.f);
        menuText.setPosition(720.f, 400.f);
        
        std::cout << "✓ UI Texts initialized with font.\n";
    } else {
         std::cerr << "✗ UI Texts NOT initialized due to font loading failure.\n";
    }
}

// Trong Game.cpp

// Hàm loadLevel MỚI
// Trong Game.cpp

// Hàm loadLevel MỚI
void Game::loadLevel(int levelNumber) {
    std::cout << "Loading level " << levelNumber << "...\n";
    platforms.clear();
    enemies.clear();
    mushrooms.clear();
    // (Không clear Player, chỉ đặt lại vị trí)

    currentState = GameState::PLAYING; // Đặt trạng thái playing khi tải màn chơi

    std::string levelPath = "assets/levels/level" + std::to_string(levelNumber) + ".txt";

    std::ifstream file(levelPath);
    if (!file.is_open()) {
        std::cerr << "!!! LOI: Khong the mo file level: " << levelPath << std::endl;
        std::cout << "Khong tim thay level, quay ve Menu." << std::endl;
        currentState = GameState::MENU; // Nếu không tìm thấy file, quay về menu
        return;
    }

    // Lấy đường dẫn texture cho platform (CHẮC CHẮN ĐÂY LÀ ĐƯỜNG DẪN ĐÚNG CỦA ANH)
    std::string platformTexturePath = "assets/ground_forest.png"; 
    std::string enemyTexturePath = "assets/npc.png";
    std::string playerTexturePath = "assets/player.png";

    std::string line;
    while (std::getline(file, line)) {
        // Bỏ qua dòng trống hoặc dòng ghi chú
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string type;
        float x, y, w, h;

        iss >> type; // Đọc từ đầu tiên (PLAYER, PLATFORM, ...)

        if (type == "PLAYER") {
            iss >> x >> y;
            if (player) { // Nếu player đã tồn tại, chỉ đặt lại vị trí
                player->setPosition({x, y});
            } else { // Nếu player chưa được tạo (lần đầu tiên game chạy)
                player = std::make_unique<Player>();
                player->loadTexture(playerTexturePath);
                player->setPosition({x, y});
            }
        } 
        else if (type == "PLATFORM") {
            iss >> x >> y >> w >> h;
            platforms.push_back(std::make_unique<Platform>(sf::Vector2f(x, y), sf::Vector2f(w, h), platformTexturePath));
        } 
        else if (type == "ENEMY") {
            iss >> x >> y;
            auto enemy = std::make_unique<Enemy>(); 
            if (enemy->loadTexture(enemyTexturePath)) { // Dùng đường dẫn texture đã định nghĩa
                enemy->setPosition({x, y}); 
                enemies.push_back(std::move(enemy)); 
            } else {
                 std::cerr << "!!! LOI: Khong the tai enemy texture: " << enemyTexturePath << std::endl;
            }
        } 
        else if (type == "MUSHROOM") {
            iss >> x >> y;
            mushrooms.push_back(std::make_unique<Mushroom>(sf::Vector2f(x, y)));
        } else {
            std::cerr << "!!! LOI: Loai doi tuong khong hop le trong file level: " << type << " (dong: " << line << ")" << std::endl;
        }
    }

    file.close();
    std::cout << "Level " << levelNumber << " loaded!" << std::endl;
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();

        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), uiView);

                
                if (currentState == GameState::WIN || currentState == GameState::GAME_OVER) {
                    sf::FloatRect restartBounds = fontLoaded ? restartText.getGlobalBounds() : sf::FloatRect(430.f, 370.f, 120.f, 60.f);
                    sf::FloatRect menuBounds = fontLoaded ? menuText.getGlobalBounds() : sf::FloatRect(670.f, 370.f, 120.f, 60.f);
                    
                    if (restartBounds.contains(mousePos)) {
                        std::cout << "Restart clicked!\n";
                        restartGame();
                        continue; 
                    }
                    if (menuBounds.contains(mousePos)) {
                        std::cout << "Menu clicked!\n";
                        returnToMenu();
                        continue; 
                    }
                }
                
                
                if (currentState == GameState::MENU && menu) { 
                    if (menu->isStartButtonClicked(mousePos)) startGame(); 
                    else if (menu->isExitButtonClicked(mousePos)) window.close();
                }
            }
        }

        
        if (currentState == GameState::WIN || currentState == GameState::GAME_OVER) {
            if (event.type == sf::Event::MouseMoved) {
                 sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), uiView);
                 
                 sf::FloatRect restartBounds = fontLoaded ? restartText.getGlobalBounds() : sf::FloatRect(430.f, 370.f, 120.f, 60.f);
                 sf::FloatRect menuBounds = fontLoaded ? menuText.getGlobalBounds() : sf::FloatRect(670.f, 370.f, 120.f, 60.f);
                 
                 if (fontLoaded) { 
                      if (restartBounds.contains(mousePos)) {
                          restartText.setFillColor(sf::Color::Yellow);
                          restartText.setScale(1.1f, 1.1f);
                      } else {
                          restartText.setFillColor(sf::Color::White);
                          restartText.setScale(1.0f, 1.0f);
                      }
                      if (menuBounds.contains(mousePos)) {
                          menuText.setFillColor(sf::Color::Yellow);
                          menuText.setScale(1.1f, 1.1f);
                      } else {
                          menuText.setFillColor(sf::Color::White);
                          menuText.setScale(1.0f, 1.0f);
                      }
                 }
            }
        }

        
        if (event.type == sf::Event::KeyPressed) {
             
             if (currentState == GameState::GAME_OVER || currentState == GameState::WIN) {
                 if (event.key.code == sf::Keyboard::R) {
                     std::cout << "R pressed: Restart!\n";
                     restartGame();
                 } else if (event.key.code == sf::Keyboard::Escape) {
                     std::cout << "ESC pressed: Menu!\n";
                     returnToMenu();
                 }
                 continue; 
             }
            
            
            switch (event.key.code) {
                case sf::Keyboard::Escape: if (currentState == GameState::PLAYING) returnToMenu(); else window.close(); break;
                case sf::Keyboard::Up: if (currentState == GameState::MENU && menu) menu->moveUp(); break;
                case sf::Keyboard::Down: if (currentState == GameState::MENU && menu) menu->moveDown(); break;
                case sf::Keyboard::Return:
                     if (currentState == GameState::MENU && menu) { MenuOption option = menu->getSelectedOption(); if (option == MenuOption::START_GAME) startGame(); else if (option == MenuOption::EXIT) window.close(); }
                     break;
                case sf::Keyboard::Space: if (currentState == GameState::PLAYING && player) player->jump(); break;
                default: break;
            }
        }
    }
}

void Game::update(float deltaTime) {
    totalTime += deltaTime;
    switch (currentState) {
        case GameState::MENU: updateMenu(); break;
        case GameState::PLAYING: updatePlaying(deltaTime); break;
        case GameState::GAME_OVER: updateGameOver(); break;
        case GameState::WIN: updateWin(); break;
        default: break;
    }
}

void Game::updateMenu() {
    if(menu) menu->update(gameClock.restart().asSeconds());
}

void Game::updatePlaying(float deltaTime) {
    if (!player) return;

    
    bool moving = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { player->moveLeft(); moving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { player->moveRight(); moving = true; }
    if (!moving) player->stopMoving();

    
    player->update(deltaTime, platforms);
    for (auto& enemy : enemies) enemy->update(deltaTime, platforms);
    for (auto& mushroom : mushrooms) mushroom->update(deltaTime);

    
    sf::FloatRect playerBounds = player->getBounds();
    for (auto& enemy : enemies) { if (!enemy) continue; if (!enemy->isAlive()) continue; sf::FloatRect enemyBounds = enemy->getBounds(); if (playerBounds.intersects(enemyBounds)) { float playerBottom = playerBounds.top + playerBounds.height; float enemyTop = enemyBounds.top; float overlap = playerBottom - enemyTop; if (player->getVelocity().y > 0 && overlap < 25.f && enemyBounds.top > playerBounds.top) enemy->kill(); else if (player->canTakeDamage()) { player->takeDamage(); if (player->getHealth() <= 0) { currentState = GameState::GAME_OVER; std::cout << "--- Player health <=0! State -> GAME_OVER ---" << std::endl; } } } }
    for (auto& mushroom : mushrooms) { if (!mushroom) continue; if (mushroom->isCollected()) continue; if (playerBounds.intersects(mushroom->getBounds())) { mushroom->collect(); player->activatePowerup(5.0f); } }

    
    if (currentState == GameState::PLAYING) {
        bool allEnemiesDead = std::all_of(enemies.begin(), enemies.end(),
                                        [](const std::unique_ptr<Enemy>& e){ return e && !e->isAlive(); });
        if (allEnemiesDead && !enemies.empty()) {
            currentLevel++;
            if (currentLevel > maxLevels) {
                currentState = GameState::WIN; 
                std::cout << "--- Player Wins! (All levels complete) ---" << std::endl;
            } else {
                std::cout << "--- Level complete! Loading level " << currentLevel << " ---" << std::endl;
                loadLevel(currentLevel);
            }
        }
    }

    
    sf::Vector2f playerPos = player->getPosition(); float cameraX = playerPos.x;
    if (cameraX < 600.f) cameraX = 600.f; if (cameraX > 1800.f) cameraX = 1800.f;
    camera.setCenter(cameraX, 400.f);

    
    if (playerPos.y > window.getSize().y + 100.f) { 
         if (currentState == GameState::PLAYING) {
             currentState = GameState::GAME_OVER;
             std::cout << "--- Player fell off map! State -> GAME_OVER ---" << std::endl;
         }
    }
}

void Game::updateGameOver() {}
void Game::updateWin() {}

void Game::render() {
    window.clear(sf::Color(50, 150, 200));
    switch (currentState) {
        case GameState::MENU: renderMenu(); break;
        case GameState::PLAYING: renderPlaying(); break;
        case GameState::GAME_OVER: renderGameOver(); break;
        case GameState::WIN: renderWin(); break;
        default: break;
    }
    window.display();
}

void Game::renderMenu() {
    window.setView(uiView);
    if(menu) menu->draw(window);
}

void Game::renderPlaying() {
    window.setView(camera);
    window.draw(background);
    for (const auto& platform : platforms) platform->draw(window);
    for (const auto& mushroom : mushrooms) mushroom->draw(window);
    for (const auto& enemy : enemies) enemy->draw(window);
    if(player) player->draw(window);

    
    window.setView(uiView);
    float heartX = 50.f; float heartY = 30.f;
    float heartPadding = heartSprite.getGlobalBounds().width + 10.f;

    if (player) {
        heartSprite.setColor(sf::Color(100, 100, 100, 150));
        for (int i = 0; i < player->getMaxHealth(); ++i) {
            heartSprite.setPosition(heartX + (i * heartPadding), heartY);
            window.draw(heartSprite);
        }
        heartSprite.setColor(sf::Color::White);
        for (int i = 0; i < player->getHealth(); ++i) {
            heartSprite.setPosition(heartX + (i * heartPadding), heartY);
            window.draw(heartSprite);
        }
    }

    
    if (fontLoaded) {
        if (player && player->getIsPoweredUp()) {
            sf::Text powerupText; powerupText.setFont(font);
            powerupText.setString("POWERED UP: " + std::to_string(static_cast<int>(player->getPowerupTimer() + 1)) + "s");
            powerupText.setCharacterSize(24); powerupText.setFillColor(sf::Color::Yellow);
            powerupText.setPosition(window.getSize().x - 250.f, 30.f);
             window.draw(powerupText);
        }
    }
}

void Game::renderGameOver() {
    
    window.setView(uiView);
    window.draw(gameOverBackgroundSprite); 

    
    if (fontLoaded) {
        window.draw(gameOverText); 
        window.draw(restartText);  
        window.draw(menuText);     
    } else {
        std::cerr << "[DEBUG] Cannot draw GAME OVER text properly: Font not loaded.\n";
    }
}

void Game::renderWin() {
    
    window.setView(camera); 
    window.draw(background);
    for (const auto& platform : platforms) platform->draw(window);
    for (const auto& mushroom : mushrooms) mushroom->draw(window);
    if(player) player->draw(window);

    
    window.setView(uiView); 
    sf::RectangleShape overlay(sf::Vector2f(window.getSize())); 
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
    
    
    if (fontLoaded) {
        window.draw(winText);     
        window.draw(restartText); 
        window.draw(menuText);    
    } else {
         std::cerr << "[DEBUG] Cannot draw WIN text properly: Font not loaded.\n";
    }
}

void Game::startGame() {
    std::cout << "Starting game...\n";
    currentLevel = 1;
    loadLevel(currentLevel);
    currentState = GameState::PLAYING;
}
void Game::restartGame() { 
    std::cout << "Restarting game...\n"; 
    loadLevel(currentLevel);
    currentState = GameState::PLAYING; }
void Game::returnToMenu() { std::cout << "Returning to menu...\n"; currentState = GameState::MENU; }

void Game::run() {
    std::cout << "\n=== Game Loop Started ===\n";
    sf::Clock deltaClock;
    while (window.isOpen()) {
        handleEvents();
        float deltaTime = deltaClock.restart().asSeconds();
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        update(deltaTime);
        render();
    }
    std::cout << "=== Game Ended ===\n";
}