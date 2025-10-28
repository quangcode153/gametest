#include "Game.hpp"
#include <iostream>
#include <numeric> // Dùng cho std::all_of
#include <algorithm> // Dùng cho std::all_of

Game::Game() :
    totalTime(0.f),
    currentState(GameState::MENU),
    fontLoaded(false)
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
    menu->loadFont("C:\\Windows\\Fonts\\arial.ttf"); // Cân nhắc dùng đường dẫn tương đối
    menu->initialize();
    std::cout << "Game initialized!\n";
}

void Game::loadResources() {
    // Load background
    std::vector<std::string> bgPaths = {"assets/background.png", "../assets/background.png"}; // Ví dụ
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

    // Load và xử lý trái tim
    sf::Image heartImage; // Dùng sf::Image để xử lý màu
    if (!heartImage.loadFromFile("assets/heart.png")) {
        std::cout << "✗ Heart image NOT FOUND! (assets/heart.png)\n";
    } else {
        std::cout << "✓ Heart image loaded.\n";
        heartImage.createMaskFromColor(sf::Color::White); // Xóa nền trắng
        
        if (!heartTexture.loadFromImage(heartImage)) {
             std::cerr << "Loi: Khong the nap heart texture tu image da xu ly." << std::endl;
        } else {
             std::cout << "✓ Heart texture created from image.\n";
             heartTexture.setSmooth(false); // Giữ sắc nét (không bắt buộc với ảnh này)
        }
    }
    heartSprite.setTexture(heartTexture);
    heartSprite.setScale(0.5f, 0.5f); // Đặt kích thước trái tim (ví dụ 50%)

    // Load font
    std::vector<std::string> fontPaths = {"C:\\Windows\\Fonts\\arial.ttf"}; // Ví dụ
    bool fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (font.loadFromFile(path)) {
            std::cout << "✓ Font loaded: " << path << "\n";
            fontLoaded = true;
            break;
        }
    }
     if (!fontLoaded) {
        std::cout << "✗ Font NOT FOUND! UI text might not display.\n";
    }

    if (fontLoaded) {
        // Setup gameOverText
        gameOverText.setFont(font);
        gameOverText.setString("GAME OVER!\nPress R to Restart\nPress ESC for Menu");
        gameOverText.setCharacterSize(48);
        gameOverText.setFillColor(sf::Color::Red);
        sf::FloatRect boundsGO = gameOverText.getLocalBounds();
        gameOverText.setOrigin(boundsGO.width / 2.f, boundsGO.height / 2.f);
        gameOverText.setPosition(600.f, 400.f);

        // Setup winText
        winText.setFont(font);
        winText.setString("YOU WIN!\nPress Enter for Menu\nPress R to Restart");
        winText.setCharacterSize(48);
        winText.setFillColor(sf::Color::Green);
        sf::FloatRect boundsWin = winText.getLocalBounds();
        winText.setOrigin(boundsWin.width / 2.f, boundsWin.height / 2.f);
        winText.setPosition(600.f, 400.f);
    }
}

void Game::createLevel() {
    std::cout << "Creating level...\n";
    platforms.clear(); enemies.clear(); mushrooms.clear();
    // Platforms...
    platforms.push_back(std::make_unique<Platform>(sf::Vector2f(0.f, 740.f), sf::Vector2f(2400.f, 60.f), sf::Color(100, 70, 40)));
    platforms.push_back(std::make_unique<Platform>(sf::Vector2f(400.f, 600.f), sf::Vector2f(250.f, 30.f), sf::Color(80, 60, 30)));
    platforms.push_back(std::make_unique<Platform>(sf::Vector2f(700.f, 500.f), sf::Vector2f(200.f, 30.f), sf::Color(80, 60, 30)));
    platforms.push_back(std::make_unique<Platform>(sf::Vector2f(950.f, 400.f), sf::Vector2f(220.f, 30.f), sf::Color(80, 60, 30)));
    platforms.push_back(std::make_unique<Platform>(sf::Vector2f(1220.f, 550.f), sf::Vector2f(300.f, 30.f), sf::Color(80, 60, 30)));
    // Player...
    player = std::make_unique<Player>();
    player->loadTexture("assets/player.png");
    player->setPosition(sf::Vector2f(200.f, 680.f));
    // Enemies...
    {
        auto enemy1 = std::make_unique<Enemy>(); enemy1->loadTexture("assets/npc.png"); enemy1->setPosition({525.f, 600.f}); enemies.push_back(std::move(enemy1));
        auto enemy2 = std::make_unique<Enemy>(); enemy2->loadTexture("assets/npc.png"); enemy2->setPosition({750.f, 500.f}); enemies.push_back(std::move(enemy2));
        auto enemy3 = std::make_unique<Enemy>(); enemy3->loadTexture("assets/npc.png"); enemy3->setPosition({1370.f, 550.f}); enemies.push_back(std::move(enemy3));
    }
    // Mushrooms...
    mushrooms.push_back(std::make_unique<Mushroom>(sf::Vector2f(750.f, 460.f)));
    mushrooms.push_back(std::make_unique<Mushroom>(sf::Vector2f(1050.f, 360.f)));
    mushrooms.push_back(std::make_unique<Mushroom>(sf::Vector2f(2000.f, 420.f)));
    std::cout << "Level created!\n";
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();

        if (event.type == sf::Event::KeyPressed) {
            // Xử lý input khi WIN
            if (currentState == GameState::WIN) {
                if (event.key.code == sf::Keyboard::Return) returnToMenu();
                else if (event.key.code == sf::Keyboard::R) restartGame();
                continue; // Bỏ qua input khác
            }

            switch (event.key.code) {
                case sf::Keyboard::Escape: if (currentState == GameState::PLAYING) returnToMenu(); else window.close(); break;
                case sf::Keyboard::R: if (currentState == GameState::GAME_OVER || currentState == GameState::WIN) restartGame(); break; // Chỉ restart khi Game Over hoặc Win
                case sf::Keyboard::Up: if (currentState == GameState::MENU) menu->moveUp(); break;
                case sf::Keyboard::Down: if (currentState == GameState::MENU) menu->moveDown(); break;
                case sf::Keyboard::Return:
                     if (currentState == GameState::MENU) { MenuOption option = menu->getSelectedOption(); if (option == MenuOption::START_GAME) startGame(); else if (option == MenuOption::EXIT) window.close(); }
                     else if (currentState == GameState::WIN) { returnToMenu(); } // Enter khi Win cũng về Menu
                     break;
                case sf::Keyboard::Space: if (currentState == GameState::PLAYING && player) player->jump(); break; // Thêm kiểm tra player
                default: break;
            }
        }
        // Xử lý mouse input cho MENU
        if (currentState == GameState::MENU && event.type == sf::Event::MouseButtonPressed) { if (event.mouseButton.button == sf::Mouse::Left) { sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), uiView); if (menu->isStartButtonClicked(mousePos)) startGame(); else if (menu->isExitButtonClicked(mousePos)) window.close(); } }
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
    if (!player) return; // Thoát nếu player chưa được tạo

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
    for (auto& enemy : enemies) { if (!enemy->isAlive()) continue; sf::FloatRect enemyBounds = enemy->getBounds(); if (playerBounds.intersects(enemyBounds)) { float playerBottom = playerBounds.top + playerBounds.height; float enemyTop = enemyBounds.top; float overlap = playerBottom - enemyTop; if (player->getVelocity().y > 0 && overlap < 25.f && enemyBounds.top > playerBounds.top) enemy->kill(); else if (player->canTakeDamage()) { player->takeDamage(); if (player->getHealth() <= 0) currentState = GameState::GAME_OVER; } } }
    for (auto& mushroom : mushrooms) { if (mushroom->isCollected()) continue; if (playerBounds.intersects(mushroom->getBounds())) { mushroom->collect(); player->activatePowerup(5.0f); } }

    // Kiểm tra điều kiện thắng
    if (currentState == GameState::PLAYING) {
        bool allEnemiesDead = std::all_of(enemies.begin(), enemies.end(),
                                         [](const std::unique_ptr<Enemy>& e){ return !e->isAlive(); });
        if (allEnemiesDead && !enemies.empty()) {
            currentState = GameState::WIN;
            std::cout << "--- Player Wins! ---" << std::endl;
        }
    }

    // Camera
    sf::Vector2f playerPos = player->getPosition(); float cameraX = playerPos.x;
    if (cameraX < 600.f) cameraX = 600.f; if (cameraX > 1800.f) cameraX = 1800.f;
    camera.setCenter(cameraX, 400.f);

    // Check rơi khỏi map
    if (playerPos.y > window.getSize().y + 100.f) { // Rơi sâu hơn mới chết
         currentState = GameState::GAME_OVER;
         std::cout << "--- Player fell off map! ---" << std::endl;
    }
}

void Game::updateGameOver() {}
void Game::updateWin() {} // Không cần làm gì

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
    window.setView(uiView); // Đảm bảo dùng UI view cho menu
    if(menu) menu->draw(window);
}

void Game::renderPlaying() {
    window.setView(camera); // View camera cho thế giới game
    window.draw(background);
    for (const auto& platform : platforms) platform->draw(window);
    for (const auto& mushroom : mushrooms) mushroom->draw(window);
    for (const auto& enemy : enemies) enemy->draw(window);
    if(player) player->draw(window);

    // Vẽ UI Trái tim
    window.setView(uiView); // Chuyển sang View UI
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
            powerupText.setPosition(window.getSize().x - 250.f, 30.f); // Căn phải
             window.draw(powerupText);
        }
    }
}

void Game::renderGameOver() {
    // Vẽ lại game state mờ đi
    window.setView(camera); window.draw(background);
    for (const auto& platform : platforms) platform->draw(window);
    for (const auto& mushroom : mushrooms) mushroom->draw(window);
    for (const auto& enemy : enemies) enemy->draw(window);
    if(player) player->draw(window);

    // Vẽ lớp phủ tối và text Game Over
    window.setView(uiView); sf::RectangleShape overlay(sf::Vector2f(window.getSize())); overlay.setFillColor(sf::Color(0, 0, 0, 180)); window.draw(overlay);
    if (fontLoaded) window.draw(gameOverText);
}

void Game::renderWin() {
    // Vẽ lại game state mờ đi
    window.setView(camera); window.draw(background);
    for (const auto& platform : platforms) platform->draw(window);
    for (const auto& mushroom : mushrooms) mushroom->draw(window);
    // Không cần vẽ enemies
    if(player) player->draw(window);

    // Vẽ lớp phủ tối và text You Win
    window.setView(uiView); sf::RectangleShape overlay(sf::Vector2f(window.getSize())); overlay.setFillColor(sf::Color(0, 0, 0, 150)); window.draw(overlay);
    if (fontLoaded) window.draw(winText);
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
        if (deltaTime > 0.1f) deltaTime = 0.1f; // Giới hạn delta time
        update(deltaTime);
        render();
    }
    std::cout << "=== Game Ended ===\n";
}