#include "Game.hpp"
#include <iostream>
#include <numeric> // Dùng cho std::all_of
#include <algorithm> // Dùng cho std::all_of

Game::Game() :
    totalTime(0.f),
    currentState(GameState::MENU),
    fontLoaded(false) // Khởi tạo biến thành viên
{
    window.create(sf::VideoMode(1200, 800), "game test");
    window.setFramerateLimit(60);
    camera.setSize(1200.f, 800.f); camera.setCenter(600.f, 400.f);
    uiView.setSize(1200.f, 800.f); uiView.setCenter(600.f, 400.f);
    initialize();
}

Game::~Game() {}

// *** HÀM INITIALIZE ĐÃ SỬA LỖI ***
void Game::initialize() {
    std::cout << "=== Initializing Game ===\n";
    loadResources(); // Tải TẤT CẢ tài nguyên (bao gồm font của Game)
    
    menu = std::make_unique<Menu>();
    
    // Cung cấp đường dẫn font cho Menu
    // (Menu sẽ tự tải font riêng của nó, không ảnh hưởng đến font của Game)
    std::string menuFontPath = "assets/fonts/arial.ttf";
    if (!menu->loadFont(menuFontPath)) { // Thử tải font tương đối
        // Nếu thất bại, thử tải font hệ thống
        std::cout << "Menu failed to load relative font, trying system font..." << std::endl;
        menu->loadFont("C:\\Windows\\Fonts\\arial.ttf"); 
    }
    
    menu->initialize();
    std::cout << "Game initialized!\n";
}


void Game::loadResources() {
    // 1. Load background chính
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

    // 2. Load background cho màn hình Game Over (backgroundEnd.png)
    if (!gameOverBackgroundTexture.loadFromFile("assets/backgroundEnd.png")) {
        std::cerr << "LOI: Khong tai duoc assets/backgroundEnd.png\n";
    } else {
        std::cout << "✓ Game Over Background loaded: assets/backgroundEnd.png\n";
        sf::Vector2u bgSize = gameOverBackgroundTexture.getSize();
        gameOverBackgroundSprite.setTexture(gameOverBackgroundTexture);
        gameOverBackgroundSprite.setScale(1200.f / bgSize.x, 800.f / bgSize.y);
    }

    // 3. Load và xử lý trái tim (heart.png)
    sf::Image heartImage;
    if (!heartImage.loadFromFile("assets/heart.png")) {
        std::cout << "✗ Heart image NOT FOUND! (assets/heart.png)\n";
    } else {
        std::cout << "✓ Heart image loaded.\n";
        heartImage.createMaskFromColor(sf::Color::White); // Cắt nền trắng
        if (!heartTexture.loadFromImage(heartImage)) {
             std::cerr << "Loi: Khong the nap heart texture tu image da xu ly." << std::endl;
        } else {
             std::cout << "✓ Heart texture created from image.\n";
        }
    }
    heartSprite.setTexture(heartTexture);
    heartSprite.setScale(0.5f, 0.5f); // Đặt kích thước trái tim

    // 4. Load font
    std::vector<std::string> fontPaths = {
        "assets/fonts/arial.ttf", // Ưu tiên tải font từ thư mục game
        "C:\\Windows\\Fonts\\arial.ttf" // Đường dẫn dự phòng
    };
    
    fontLoaded = false; // Gán cho biến thành viên (member variable)
    
    for (const auto& path : fontPaths) {
        std::cout << "Trying font: " << path << "... ";
        if (font.loadFromFile(path)) {
            std::cout << "SUCCESS!\n";
            fontLoaded = true; // Gán cho biến thành viên
            break;
        } else {
            std::cout << "failed.\n";
        }
    }
     if (!fontLoaded) {
        std::cerr << "✗ ERROR: Font NOT FOUND! UI text might not display.\n";
    }

    // --- 5. Setup UI Text (ĐÃ SỬA LỖI THỨ TỰ) ---
    if (fontLoaded) {
        // Setup gameOverText
        gameOverText.setFont(font); // <-- SET FONT TRƯỚC
        gameOverText.setString("GAME OVER");
        gameOverText.setCharacterSize(72);
        gameOverText.setFillColor(sf::Color::Red);
        sf::FloatRect boundsGO = gameOverText.getLocalBounds(); // <-- Lấy bounds SAU
        gameOverText.setOrigin(boundsGO.width / 2.f, boundsGO.height / 2.f);
        gameOverText.setPosition(600.f, 250.f);

        // Setup winText
        winText.setFont(font); // <-- SET FONT TRƯỚC
        winText.setString("YOU WIN!");
        winText.setCharacterSize(72);
        winText.setFillColor(sf::Color::Green);
        sf::FloatRect boundsWin = winText.getLocalBounds(); // <-- Lấy bounds SAU
        winText.setOrigin(boundsWin.width / 2.f, boundsWin.height / 2.f);
        winText.setPosition(600.f, 250.f);

        // Setup restartText
        restartText.setFont(font); // <-- SET FONT TRƯỚC
        restartText.setString("Restart");
        restartText.setCharacterSize(40);
        restartText.setFillColor(sf::Color::White); // Đổi lại màu trắng (sẽ đổi thành vàng khi hover)
        sf::FloatRect boundsRst = restartText.getLocalBounds(); // <-- Lấy bounds SAU
        restartText.setOrigin(boundsRst.width / 2.f, boundsRst.height / 2.f);
        restartText.setPosition(480.f, 400.f);

        // Setup menuText
        menuText.setFont(font); // <-- SET FONT TRƯỚC
        menuText.setString("Menu");
        menuText.setCharacterSize(40);
        menuText.setFillColor(sf::Color::White); // Đổi lại màu trắng (sẽ đổi thành vàng khi hover)
        sf::FloatRect boundsMenu = menuText.getLocalBounds(); // <-- Lấy bounds SAU
        menuText.setOrigin(boundsMenu.width / 2.f, boundsMenu.height / 2.f);
        menuText.setPosition(720.f, 400.f);
        
        std::cout << "✓ UI Texts initialized.\n";
    } else {
         std::cerr << "✗ UI Texts NOT initialized due to font loading failure.\n";
         // Nếu font lỗi, chúng ta vẫn nên setup text để SFML dùng font mặc định
         // (Code bên dưới lặp lại, nhưng đảm bảo text được tạo)
         gameOverText.setString("GAME OVER");
         gameOverText.setCharacterSize(72);
         gameOverText.setFillColor(sf::Color::Red);
         sf::FloatRect boundsGO = gameOverText.getLocalBounds();
         gameOverText.setOrigin(boundsGO.width / 2.f, boundsGO.height / 2.f);
         gameOverText.setPosition(600.f, 250.f);

         winText.setString("YOU WIN!");
         winText.setCharacterSize(72);
         winText.setFillColor(sf::Color::Green);
         sf::FloatRect boundsWin = winText.getLocalBounds();
         winText.setOrigin(boundsWin.width / 2.f, boundsWin.height / 2.f);
         winText.setPosition(600.f, 250.f);

         restartText.setString("Restart");
         restartText.setCharacterSize(40);
         restartText.setFillColor(sf::Color::White);
         sf::FloatRect boundsRst = restartText.getLocalBounds();
         restartText.setOrigin(boundsRst.width / 2.f, boundsRst.height / 2.f);
         restartText.setPosition(480.f, 400.f);

         menuText.setString("Menu");
         menuText.setCharacterSize(40);
         menuText.setFillColor(sf::Color::White);
         sf::FloatRect boundsMenu = menuText.getLocalBounds();
         menuText.setOrigin(boundsMenu.width / 2.f, boundsMenu.height / 2.f);
         menuText.setPosition(720.f, 400.f);
    }
}

void Game::createLevel() {
    std::cout << "Creating level...\n";
    platforms.clear(); enemies.clear(); mushrooms.clear();
    currentState = GameState::PLAYING; // Đặt trạng thái ở đầu

    // Platforms (Dùng màu trơn)
    platforms.push_back(std::make_unique<Platform>(sf::Vector2f(0.f, 740.f), sf::Vector2f(2400.f, 60.f), sf::Color(100, 70, 40)));
    platforms.push_back(std::make_unique<Platform>(sf::Vector2f(400.f, 600.f), sf::Vector2f(250.f, 30.f), sf::Color(80, 60, 30)));
    platforms.push_back(std::make_unique<Platform>(sf::Vector2f(700.f, 500.f), sf::Vector2f(200.f, 30.f), sf::Color(80, 60, 30)));
    platforms.push_back(std::make_unique<Platform>(sf::Vector2f(950.f, 400.f), sf::Vector2f(220.f, 30.f), sf::Color(80, 60, 30)));
    platforms.push_back(std::make_unique<Platform>(sf::Vector2f(1220.f, 550.f), sf::Vector2f(300.f, 30.f), sf::Color(80, 60, 30)));
    
    // Player
    player = std::make_unique<Player>();
    player->loadTexture("assets/player.png");
    player->setPosition(sf::Vector2f(200.f, 680.f));
    
    // Enemies
    enemies.clear();
    {
        auto enemy1 = std::make_unique<Enemy>(); 
        if(enemy1->loadTexture("assets/npc.png")) { 
            enemy1->setPosition({525.f, 600.f}); 
            enemies.push_back(std::move(enemy1)); 
        }
        auto enemy2 = std::make_unique<Enemy>(); 
        if(enemy2->loadTexture("assets/npc.png")) { 
            enemy2->setPosition({750.f, 500.f}); 
            enemies.push_back(std::move(enemy2)); 
        }
        auto enemy3 = std::make_unique<Enemy>(); 
        if(enemy3->loadTexture("assets/npc.png")) { 
            enemy3->setPosition({1370.f, 550.f}); 
            enemies.push_back(std::move(enemy3)); 
        }
    }
    
    // Mushrooms
    mushrooms.push_back(std::make_unique<Mushroom>(sf::Vector2f(750.f, 460.f)));
    mushrooms.push_back(std::make_unique<Mushroom>(sf::Vector2f(1050.f, 360.f)));
    mushrooms.push_back(std::make_unique<Mushroom>(sf::Vector2f(2000.f, 420.f)));
    std::cout << "Level created!\n";
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();

        // Xử lý CLICK CHUỘT
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), uiView);

                // A. Xử lý click khi Win hoặc Game Over
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
                
                // B. Xử lý click chuột cho MENU
                if (currentState == GameState::MENU && menu) { 
                    if (menu->isStartButtonClicked(mousePos)) startGame(); 
                    else if (menu->isExitButtonClicked(mousePos)) window.close();
                }
            }
        }

        // Xử lý Hover chuột
        if (currentState == GameState::WIN || currentState == GameState::GAME_OVER) {
            if (event.type == sf::Event::MouseMoved) {
                 sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), uiView);
                 
                 sf::FloatRect restartBounds = fontLoaded ? restartText.getGlobalBounds() : sf::FloatRect(430.f, 370.f, 120.f, 60.f);
                 sf::FloatRect menuBounds = fontLoaded ? menuText.getGlobalBounds() : sf::FloatRect(670.f, 370.f, 120.f, 60.f);
                 
                 if (fontLoaded) { // Chỉ đổi màu/scale nếu font tải thành công
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

        // Xử lý NHẤN PHÍM
        if (event.type == sf::Event::KeyPressed) {
             // Thêm xử lý phím R và Escape cho Win/Game Over
             if (currentState == GameState::GAME_OVER || currentState == GameState::WIN) {
                 if (event.key.code == sf::Keyboard::R) {
                     std::cout << "R pressed: Restart!\n";
                     restartGame();
                 } else if (event.key.code == sf::Keyboard::Escape) {
                     std::cout << "ESC pressed: Menu!\n";
                     returnToMenu();
                 }
                 continue; // Bỏ qua các phím khác
             }
            
            // Code switch cho MENU và PLAYING
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

    // Player input
    bool moving = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { player->moveLeft(); moving = true; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { player->moveRight(); moving = true; }
    if (!moving) player->stopMoving();

    // Updates
    player->update(deltaTime, platforms);
    for (auto& enemy : enemies) enemy->update(deltaTime, platforms);
    for (auto& mushroom : mushrooms) mushroom->update(deltaTime);

    // Collisions
    sf::FloatRect playerBounds = player->getBounds();
    for (auto& enemy : enemies) { if (!enemy) continue; if (!enemy->isAlive()) continue; sf::FloatRect enemyBounds = enemy->getBounds(); if (playerBounds.intersects(enemyBounds)) { float playerBottom = playerBounds.top + playerBounds.height; float enemyTop = enemyBounds.top; float overlap = playerBottom - enemyTop; if (player->getVelocity().y > 0 && overlap < 25.f && enemyBounds.top > playerBounds.top) enemy->kill(); else if (player->canTakeDamage()) { player->takeDamage(); if (player->getHealth() <= 0) { currentState = GameState::GAME_OVER; std::cout << "--- Player health <=0! State -> GAME_OVER ---" << std::endl; } } } }
    for (auto& mushroom : mushrooms) { if (!mushroom) continue; if (mushroom->isCollected()) continue; if (playerBounds.intersects(mushroom->getBounds())) { mushroom->collect(); player->activatePowerup(5.0f); } }

    // Kiểm tra điều kiện thắng
    if (currentState == GameState::PLAYING) {
        bool allEnemiesDead = std::all_of(enemies.begin(), enemies.end(),
                                         [](const std::unique_ptr<Enemy>& e){ return e && !e->isAlive(); });
        if (allEnemiesDead && !enemies.empty()) {
            currentState = GameState::WIN;
            std::cout << "--- Player Wins! State -> WIN ---" << std::endl;
        }
    }

    // Camera
    sf::Vector2f playerPos = player->getPosition(); float cameraX = playerPos.x;
    if (cameraX < 600.f) cameraX = 600.f; if (cameraX > 1800.f) cameraX = 1800.f;
    camera.setCenter(cameraX, 400.f);

    // Check rơi khỏi map
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

    // Vẽ UI Trái tim
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

    // Vẽ UI khác (powerup timer)
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
    // 1. Vẽ BACKGROUND (backgroundEnd.png)
    window.setView(uiView);
    window.draw(gameOverBackgroundSprite); 

    // 2. Vẽ UI (Chữ)
    // Luôn vẽ text (nếu fontLoaded=false, SFML dùng font mặc định)
    window.draw(gameOverText); 
    window.draw(restartText);
    window.draw(menuText);
    
    if (!fontLoaded) {
         std::cerr << "[DEBUG] Cannot draw GAME OVER text properly: Font not loaded.\n";
    }
}

void Game::renderWin() {
    // 1. Vẽ lại game state mờ đi
    window.setView(camera); 
    window.draw(background);
    for (const auto& platform : platforms) platform->draw(window);
    for (const auto& mushroom : mushrooms) mushroom->draw(window);
    if(player) player->draw(window);

    // 2. Vẽ lớp phủ tối
    window.setView(uiView); 
    sf::RectangleShape overlay(sf::Vector2f(window.getSize())); 
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
    
    // 3. Vẽ UI (Chữ)
    // Luôn vẽ text (nếu fontLoaded=false, SFML dùng font mặc định)
    window.draw(winText);
    window.draw(restartText);
    window.draw(menuText);
    
    if (!fontLoaded) {
         std::cerr << "[DEBUG] Cannot draw WIN text properly: Font not loaded.\n";
    }
}

void Game::startGame() { std::cout << "Starting game...\n"; createLevel(); currentState = GameState::PLAYING; }
void Game::restartGame() { std::cout << "Restarting game...\n"; createLevel(); currentState = GameState::PLAYING; }
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