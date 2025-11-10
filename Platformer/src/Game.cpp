#include "Game.hpp"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>

#include "Player.hpp"
#include "Enemy.hpp"
#include "Platform.hpp"
#include "Menu.hpp"
#include "Item.hpp"
#include "CoinItem.hpp"
#include "HeartItem.hpp"
#include "SpeedBoostItem.hpp"
#include "ShieldItem.hpp"

Game::Game() :
    totalTime(0.f),
    currentState(GameState::MENU),
    fontLoaded(false),
    currentLevel(1),
    maxLevels(4),
    selectedCharacterIndex(0)
{
    window.create(sf::VideoMode(1200, 800), "game test");
    window.setFramerateLimit(60);
    camera.setSize(1200.f, 800.f); camera.setCenter(600.f, 400.f);
    uiView.setSize(1200.f, 800.f); uiView.setCenter(600.f, 400.f);
    initialize();

    player = std::make_unique<Player>();
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

    if (!menuBackgroundTexture.loadFromFile("assets/backgroundMenu.png")) {
        std::cerr << "!!! LOI: Khong tai duoc assets/backgroundMenu.png\n";
    } else {
        std::cout << "✓ Menu Background loaded: assets/backgroundMenu.png\n";
        menuBackgroundSprite.setTexture(menuBackgroundTexture);
        sf::Vector2u bgSize = menuBackgroundTexture.getSize();
        float scaleX = 1200.f / bgSize.x;
        float scaleY = 800.f / bgSize.y;
        float scale = std::max(scaleX, scaleY);
        menuBackgroundSprite.setScale(scale, scale);
        menuBackgroundSprite.setOrigin(bgSize.x / 2.f, bgSize.y / 2.f);
        menuBackgroundSprite.setPosition(1200.f / 2.f, 800.f / 2.f);
    }

    std::cout << "Background (in-game) se duoc tai trong loadLevel().\n";

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

        charSelectTitle.setFont(font);
        charSelectTitle.setString("CHON NHAN VAT");
        charSelectTitle.setCharacterSize(60);
        charSelectTitle.setFillColor(sf::Color::White);
        sf::FloatRect bounds = charSelectTitle.getLocalBounds();
        charSelectTitle.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        charSelectTitle.setPosition(600.f, 100.f);

        charNameText.setFont(font);
        charNameText.setCharacterSize(40);
        charNameText.setFillColor(sf::Color::Yellow);

        charSelectHelpText.setFont(font);
        charSelectHelpText.setString("Nhan trai/phai de chon. Nhan Enter de xac nhan.");
        charSelectHelpText.setCharacterSize(24);
        charSelectHelpText.setFillColor(sf::Color::White);
        bounds = charSelectHelpText.getLocalBounds();
        charSelectHelpText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        charSelectHelpText.setPosition(600.f, 700.f);

    } else {
        std::cerr << "✗ UI Texts NOT initialized due to font loading failure.\n";
    }

    if (!charSelectBackgroundTexture.loadFromFile("assets/backgroundSelect.png")) {
        std::cerr << "!!! LOI: Khong tai duoc assets/backgroundSelect.png\n";
    } else {
        charSelectBackgroundSprite.setTexture(charSelectBackgroundTexture);
        sf::Vector2u bgSize = charSelectBackgroundTexture.getSize();
        charSelectBackgroundSprite.setOrigin(bgSize.x / 2.f, bgSize.y / 2.f);
        charSelectBackgroundSprite.setScale(1200.f / bgSize.x, 800.f / bgSize.y);
        charSelectBackgroundSprite.setPosition(600.f, 400.f);
    }

    if (!selectorTexture.loadFromFile("assets/selector_arrow.png")) {
        std::cerr << "!!! LOI: Khong tai duoc assets/selector_arrow.png\n";
    }
    selectorSprite.setTexture(selectorTexture);
    selectorSprite.setOrigin(selectorTexture.getSize().x / 2.f, selectorTexture.getSize().y / 2.f);
    selectorSprite.setScale(0.25f, 0.25f);

    // === ĐỊNH NGHĨA CÁC NHÂN VẬT (ĐÃ SỬA LỖI WHITE BOX) ===
    characterList.clear();
    float cardScale = 0.5f;

    // --- BƯỚC 1: THÊM TẤT CẢ NHÂN VẬT VÀO VECTOR TRƯỚC ---

    // Nhân vật 1: Kiếm Sĩ
    CharacterData kiem;
    kiem.name = "Kiem Si";
    kiem.assetFolder = "assets/characters/kiem/";
    characterList.push_back(kiem); // Thêm Kiem Si

    // Nhân vật 2: Pháp Sư
    CharacterData phep;
    phep.name = "Phap Su";
    phep.assetFolder = "assets/characters/phep/";
    characterList.push_back(phep); // Thêm Phap Su

    // (Thêm các nhân vật khác ở đây nếu có)

    // --- BƯỚC 2: BÂY GIỜ MỚI TẢI TEXTURE BẰNG INDEX (SAU KHI VECTOR ỔN ĐỊNH) ---

    // Tải cho Kiếm Sĩ (index 0)
    if (characterList[0].previewTexture.loadFromFile("assets/characters/kiem/Preview.png")) {
        std::cout << "✓ Kiem Si Preview loaded: assets/characters/kiem/Preview.png\n";
        characterList[0].previewSprite.setTexture(characterList[0].previewTexture); 
        characterList[0].previewSprite.setOrigin(characterList[0].previewTexture.getSize().x / 2.f, characterList[0].previewTexture.getSize().y / 2.f);
        characterList[0].previewSprite.setPosition(400.f, 400.f); 
        characterList[0].previewSprite.setScale(cardScale, cardScale);
    } else {
        std::cerr << "!!! LOI: Khong tai duoc assets/characters/kiem/Preview.png\n";
    }

    // Tải cho Pháp Sư (index 1)
    if (characterList[1].previewTexture.loadFromFile("assets/characters/phep/Preview.png")) { 
        std::cout << "✓ Phap Su Preview loaded: assets/characters/phep/Preview.png\n";
        characterList[1].previewSprite.setTexture(characterList[1].previewTexture); 
        characterList[1].previewSprite.setOrigin(characterList[1].previewTexture.getSize().x / 2.f, characterList[1].previewTexture.getSize().y / 2.f);
        characterList[1].previewSprite.setPosition(800.f, 400.f); 
        characterList[1].previewSprite.setScale(cardScale, cardScale); 
    } else {
        std::cerr << "!!! LOI: Khong tai duoc assets/characters/phep/Preview.png\n";
    }

    // Đặt nhân vật mặc định
    selectedCharacterIndex = 0;
    updateSelectorPosition();
}


void Game::loadLevel(int levelNumber) {
    std::cout << "Loading level " << levelNumber << "...\n";
    platforms.clear();
    enemies.clear();
    items.clear();

    std::string bgPath;
    if (levelNumber == 1) bgPath = "assets/background.png";
    else if (levelNumber == 2) bgPath = "assets/background2.png";
    else if (levelNumber == 3) bgPath = "assets/background3.png";
    else if (levelNumber == 4) bgPath = "assets/background4.png";
    else bgPath = "assets/background.png";

    if (!backgroundTexture.loadFromFile(bgPath)) {
        std::cerr << "!!! LOI: Khong tai duoc background cho level: " << bgPath << std::endl;
    } else {
        std::cout << "✓ Background for level " << levelNumber << " loaded: " << bgPath << "\n";
        background.setTexture(backgroundTexture);
        sf::Vector2u bgSize = backgroundTexture.getSize();
        float scaleX = 2400.f / bgSize.x;
        float scaleY = 800.f / bgSize.y;
        background.setScale(scaleX, scaleY);
    }

    std::string levelPath = "assets/levels/level" + std::to_string(levelNumber) + ".txt";

    std::ifstream file(levelPath);
    if (!file.is_open()) {
        std::cerr << "!!! LOI: Khong the mo file level: " << levelPath << std::endl;
        std::cout << "Khong tim thay level, quay ve Menu." << std::endl;
        currentState = GameState::MENU;
        return;
    }

    std::string platformTexturePath;
    if (levelNumber == 1) platformTexturePath = "assets/ground_forest.png";
    else if (levelNumber == 2) platformTexturePath = "assets/ground_forest2.png";
    else if (levelNumber == 3) platformTexturePath = "assets/ground_forest3.jpg";
    else if (levelNumber == 4) platformTexturePath = "assets/ground_forest4.jpg";
    else platformTexturePath = "assets/ground_forest.png";

    std::string enemyTexturePath = "assets/npc.png";

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string type;
        float x, y, w, h;

        iss >> type;
        if (type == "PLAYER") {
            iss >> x >> y;
            if (player) {
                float footOffset = player->getScaledFootOffset();
                player->setPosition({x, y - footOffset});
                player->setTotalCoins(0);
                player->heal(player->getMaxHealth());
            }
        }
        else if (type == "PLATFORM") {
            iss >> x >> y >> w >> h;
            platforms.push_back(std::make_unique<Platform>(sf::Vector2f(x, y), sf::Vector2f(w, h), platformTexturePath));
        }
        else if (type == "ENEMY") {
            iss >> x >> y;
            auto enemy = std::make_unique<Enemy>();
            if (enemy->loadTexture(enemyTexturePath)) {
                enemy->setPosition({x, y});
                enemies.push_back(std::move(enemy));
            } else {
                std::cerr << "!!! LOI: Khong the tai enemy texture: " << enemyTexturePath << std::endl;
            }
        }
        else if (type == "ITEM") {
            std::string itemType;
            iss >> itemType >> x >> y;
            if (itemType == "COIN") items.push_back(std::make_unique<CoinItem>(x, y));
            else if (itemType == "HEART") items.push_back(std::make_unique<HeartItem>(x, y));
            else if (itemType == "SPEED") items.push_back(std::make_unique<SpeedBoostItem>(x, y));
            else if (itemType == "SHIELD") items.push_back(std::make_unique<ShieldItem>(x, y));
        }
        else {
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
                    if (restartBounds.contains(mousePos)) { restartGame(); continue; }
                    if (menuBounds.contains(mousePos)) { returnToMenu(); continue; }
                }

                if (currentState == GameState::MENU && menu) {
                    if (menu->isPlayButtonClicked(mousePos)) {
                        currentState = GameState::CHARACTER_SELECTION;
                    }
                    else if (menu->isExitButtonClicked(mousePos)) {
                        window.close();
                    }
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
                if (event.key.code == sf::Keyboard::R) { restartGame(); }
                else if (event.key.code == sf::Keyboard::Escape) { returnToMenu(); }
                continue;
            }

            else if (currentState == GameState::CHARACTER_SELECTION) {
                if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
                    selectedCharacterIndex = (selectedCharacterIndex > 0) ? selectedCharacterIndex - 1 : characterList.size() - 1;
                    updateSelectorPosition();
                } else if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
                    selectedCharacterIndex = (selectedCharacterIndex < (int)characterList.size() - 1) ? selectedCharacterIndex + 1 : 0;
                    updateSelectorPosition();
                } else if (event.key.code == sf::Keyboard::Return) {
                    startGame();
                } else if (event.key.code == sf::Keyboard::Escape) {
                    returnToMenu();
                }
            }

            else if (currentState == GameState::MENU) {
                if (event.key.code == sf::Keyboard::Up) menu->moveUp();
                else if (event.key.code == sf::Keyboard::Down) menu->moveDown();
                else if (event.key.code == sf::Keyboard::Return) {
                    MenuOption option = menu->getSelectedOption();
                    if (option == MenuOption::PLAY) {
                        currentState = GameState::CHARACTER_SELECTION;
                    }
                    else if (option == MenuOption::EXIT) window.close();
                }
                else if (event.key.code == sf::Keyboard::Escape) window.close();
            }

            else if (currentState == GameState::PLAYING) {
                if (event.key.code == sf::Keyboard::Space) player->jump();
                else if (event.key.code == sf::Keyboard::Escape) returnToMenu();
            }
        }
    }
}

void Game::update(float deltaTime) {
    totalTime += deltaTime;
    switch (currentState) {
        case GameState::MENU: updateMenu(); break;
        case GameState::CHARACTER_SELECTION: updateCharacterSelection(deltaTime); break;
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

    for (const auto& item : items) {
        if (!item->isCollected()) {
            item->update(deltaTime);
            if (player->getHitbox().intersects(item->getBounds())) {
                item->onCollect(*player);
            }
        }
    }
    auto removeIt = std::remove_if(items.begin(), items.end(),
                                   [](const std::unique_ptr<Item>& i){ return i && i->isCollected(); });
    items.erase(removeIt, items.end());

    sf::FloatRect playerBounds = player->getHitbox();
    for (auto& enemy : enemies) {
        if (!enemy || !enemy->isAlive()) continue;
        sf::FloatRect enemyBounds = enemy->getBounds();
        if (playerBounds.intersects(enemyBounds)) {
            float playerBottom = playerBounds.top + playerBounds.height;
            float enemyTop = enemyBounds.top;
            float overlap = playerBottom - enemyTop;
            if (player->getVelocity().y > 0 && overlap < 25.f && enemyBounds.top > playerBounds.top) {
                enemy->kill();
            }
            else{
                player->takeDamage();
            }
        }
    }

    if (player->hasDeathAnimationFinished()) {
        currentState = GameState::GAME_OVER;
        std::cout << "--- Player DEATH animation finished! State -> GAME_OVER ---" << std::endl;
    }
    if (player->getPosition().y > window.getSize().y + 100.f) {
        if (currentState == GameState::PLAYING) {
            currentState = GameState::GAME_OVER;
            std::cout << "--- Player fell off map! State -> GAME_OVER ---" << std::endl;
        }
    }

    if (currentState == GameState::PLAYING) {
        bool allEnemiesDead = std::all_of(enemies.begin(), enemies.end(),
                                         [](const std::unique_ptr<Enemy>& e){ return e && !e->isAlive(); });

        bool levelProgressCondition = allEnemiesDead;

        if (levelProgressCondition) {
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
    if (cameraX < 600.f) cameraX = 600.f;
    if (cameraX > 2400.f - 600.f) cameraX = 2400.f - 600.f;
    camera.setCenter(cameraX, 400.f);
}

void Game::updateGameOver() {}
void Game::updateWin() {}

void Game::render() {
    window.clear(sf::Color(50, 150, 200));
    switch (currentState) {
        case GameState::MENU:
            renderMenu();
            break;
        case GameState::CHARACTER_SELECTION:
            renderCharacterSelection();
            break;
        case GameState::PLAYING:
            renderPlaying();
            break;
        case GameState::GAME_OVER:
            renderGameOver();
            break;
        case GameState::WIN:
            renderWin();
            break;
        default: break;
    }
    window.display();
}

void Game::renderMenu() {
    window.setView(uiView);
    window.draw(menuBackgroundSprite);
    if(menu) menu->draw(window);
}

void Game::renderPlaying() {
    window.setView(camera);
    window.draw(background);
    for (const auto& platform : platforms) platform->draw(window);
    for (const auto& enemy : enemies) enemy->draw(window);
    for (const auto& item : items) item->draw(window);
    if(player) player->draw(window);

    window.setView(uiView);
    float heartX = 10.f; float heartY = 10.f;
    float heartPadding = 70.f;

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

        if (fontLoaded) {
            sf::Text coinText;
            coinText.setFont(font);
            coinText.setString("Coins: " + std::to_string(player->getTotalCoins()));
            coinText.setCharacterSize(24);
            coinText.setFillColor(sf::Color::Yellow);
            coinText.setPosition(window.getSize().x - 200.f, heartY);
            window.draw(coinText);
        }
    }

    if (fontLoaded) {
        if (player && player->getIsPoweredUp()) {
            sf::Text powerupText; powerupText.setFont(font);
            powerupText.setString("POWERED UP: " + std::to_string(static_cast<int>(player->getPowerupTimer() + 1)) + "s");
            powerupText.setCharacterSize(24); powerupText.setFillColor(sf::Color::Yellow);
            powerupText.setPosition(window.getSize().x - 200.f, heartY + 30.f);
            window.draw(powerupText);
        }
        if (player && player->getIsBoosted()) {
            sf::Text boostText;
            boostText.setFont(font);
            boostText.setString("SPEED BOOST: " + std::to_string(static_cast<int>(player->getBoostTimer() + 1)) + "s");
            boostText.setCharacterSize(24);
            boostText.setFillColor(sf::Color::Cyan);
            boostText.setPosition(window.getSize().x - 200.f, heartY + 60.f);
            window.draw(boostText);
        }
        if (player && player->getIsShielded()) {
            sf::Text shieldText;
            shieldText.setFont(font);
            shieldText.setString("SHIELD: " + std::to_string(static_cast<int>(player->getShieldTimer() + 1)) + "s");
            shieldText.setCharacterSize(24);
            shieldText.setFillColor(sf::Color(200, 200, 255));
            shieldText.setPosition(window.getSize().x - 200.f, heartY + 90.f);
            window.draw(shieldText);
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

    if (characterList.empty()) {
        std::cerr << "!!! LOI: Khong co nhan vat nao de bat dau game!" << std::endl;
        returnToMenu();
        return;
    }

    std::string selectedFolder = characterList[selectedCharacterIndex].assetFolder;

    std::cout << "Loading character from: " << selectedFolder << std::endl;

    player->loadTexture(selectedFolder);

    currentLevel = 1;
    loadLevel(currentLevel);
    currentState = GameState::PLAYING;
}

void Game::restartGame() {
    std::cout << "Restarting game...\n";

    if (currentState == GameState::WIN) {
        currentLevel = maxLevels;
    }

    loadLevel(currentLevel);
    currentState = GameState::PLAYING;
}


void Game::returnToMenu() {
    std::cout << "Returning to menu...\n";
    currentState = GameState::MENU;
    if (player) {
        player->setTotalCoins(0);
    }
}

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

void Game::updateCharacterSelection(float deltaTime) {
    if (characterList.empty()) return;

    updateSelectorPosition();

    float timer = totalTime * 5.f;
    float alpha = 150.f + 105.f * std::sin(timer);
    selectorSprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(alpha)));
}
void Game::renderCharacterSelection() {
    window.setView(uiView);

    window.draw(charSelectBackgroundSprite);

    for (const auto& character : characterList) {
        window.draw(character.previewSprite);
    }

    window.draw(selectorSprite);

    window.draw(charSelectTitle);
    window.draw(charNameText);
    window.draw(charSelectHelpText);
}

void Game::updateSelectorPosition() {
    if (characterList.empty()) return;

    sf::Vector2f charPos = characterList[selectedCharacterIndex].previewSprite.getPosition();

    float selectorX = charPos.x;
    float selectorY = charPos.y - 250.f;

    selectorSprite.setPosition(selectorX, selectorY);

    if (fontLoaded) {
        charNameText.setString(characterList[selectedCharacterIndex].name);
        sf::FloatRect bounds = charNameText.getLocalBounds();
        charNameText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        charNameText.setPosition(selectorX, selectorY + 80.f);
    }
}