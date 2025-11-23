#include <SFML/Graphics.hpp>
#include "GameManager.h"
#include "GameRenderer.h"
#include <iostream>
#include <string>

enum AppState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

int main() {
    GameManager gameManager;
    GameRenderer renderer;
    AppState currentState = MENU;
    int selectedDifficulty = 1;
    
    // Initialize renderer
    if (!renderer.initialize(1200, 800, "Maze Game")) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return -1;
    }
    
    std::cout << "Renderer initialized successfully!" << std::endl;
    std::cout << "Window should be visible. Press 1, 2, or 3 to start a game." << std::endl;
    
    sf::Clock gameClock;
    float ghostUpdateInterval = 0.5f; // Update ghosts every 0.5 seconds
    float lastGhostUpdate = 0.0f;
    
    while (renderer.isOpen()) {
        sf::Event event;
        float deltaTime = gameClock.restart().asSeconds();
        lastGhostUpdate += deltaTime;
        
        // Poll events
        while (renderer.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                renderer.close();
                break;
            }
            
            if (event.type == sf::Event::KeyPressed) {
                if (currentState == MENU) {
                    switch (event.key.code) {
                        case sf::Keyboard::Num1:
                        case sf::Keyboard::Numpad1:
                            selectedDifficulty = 1;
                            currentState = PLAYING;
                            gameManager.initializeGame(1);
                            break;
                        case sf::Keyboard::Num2:
                        case sf::Keyboard::Numpad2:
                            selectedDifficulty = 2;
                            currentState = PLAYING;
                            gameManager.initializeGame(2);
                            break;
                        case sf::Keyboard::Num3:
                        case sf::Keyboard::Numpad3:
                            selectedDifficulty = 3;
                            currentState = PLAYING;
                            gameManager.initializeGame(3);
                            break;
                        case sf::Keyboard::Num4:
                        case sf::Keyboard::Numpad4:
                            if (gameManager.loadGame("savegame.txt")) {
                                currentState = PLAYING;
                            } else {
                                std::cout << "Failed to load game!" << std::endl;
                            }
                            break;
                        case sf::Keyboard::Q:
                            renderer.close();
                            break;
                        default:
                            break;
                    }
                }
                else if (currentState == PLAYING) {
                    switch (event.key.code) {
                        case sf::Keyboard::Up:
                            gameManager.handlePlayerMove(0, -1);
                            break;
                        case sf::Keyboard::Down:
                            gameManager.handlePlayerMove(0, 1);
                            break;
                        case sf::Keyboard::Left:
                            gameManager.handlePlayerMove(-1, 0);
                            break;
                        case sf::Keyboard::Right:
                            gameManager.handlePlayerMove(1, 0);
                            break;
                        case sf::Keyboard::P:
                            currentState = PAUSED;
                            gameManager.setPaused(true);
                            break;
                        case sf::Keyboard::S:
                            if (gameManager.saveGame("savegame.txt")) {
                                std::cout << "Game saved!" << std::endl;
                            } else {
                                std::cout << "Failed to save game!" << std::endl;
                            }
                            break;
                        case sf::Keyboard::L:
                            if (gameManager.loadGame("savegame.txt")) {
                                currentState = PLAYING;
                                std::cout << "Game loaded!" << std::endl;
                            } else {
                                std::cout << "Failed to load game!" << std::endl;
                            }
                            break;
                        case sf::Keyboard::M:
                            // Mark current position as spawnpoint
                            gameManager.markSpawnpoint();
                            break;
                        case sf::Keyboard::R:
                            // Return to spawnpoint
                            gameManager.goToSpawnpoint();
                            break;
                        case sf::Keyboard::Escape:
                            currentState = MENU;
                            break;
                        default:
                            break;
                    }
                }
                else if (currentState == PAUSED) {
                    switch (event.key.code) {
                        case sf::Keyboard::P:
                            currentState = PLAYING;
                            gameManager.setPaused(false);
                            break;
                        case sf::Keyboard::Escape:
                            currentState = MENU;
                            break;
                        default:
                            break;
                    }
                }
                else if (currentState == GAME_OVER) {
                    switch (event.key.code) {
                        case sf::Keyboard::R:
                            currentState = PLAYING;
                            gameManager.resetGame();
                            break;
                        case sf::Keyboard::Escape:
                        case sf::Keyboard::M:
                            currentState = MENU;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        
        // Update game logic
        if (currentState == PLAYING && !gameManager.isGamePaused()) {
            // Update ghosts periodically
            if (lastGhostUpdate >= ghostUpdateInterval) {
                gameManager.update();
                lastGhostUpdate = 0.0f;
            }
            
            // Check game over/win conditions
            if (gameManager.isGameOver()) {
                currentState = GAME_OVER;
            }
        }
        
        // Render
        if (currentState == MENU) {
            renderer.renderMenu(selectedDifficulty);
        } else {
            renderer.renderGame(gameManager);
        }
    }
    
    return 0;
}

