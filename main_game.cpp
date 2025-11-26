#include "GameManager.h"
#include "GameRenderer.h"
#include "InputHandler.h"
#include <iostream>
#include <chrono>
#include <thread>

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
    
    // Initialize input handler
    InputHandler::initialize();
    
    // Initialize renderer
    renderer.initialize();
    
    // Timing variables
    auto lastGhostUpdate = std::chrono::steady_clock::now();
    auto lastRenderTime = std::chrono::steady_clock::now();
    const auto ghostUpdateInterval = std::chrono::milliseconds(500);  // Update ghosts every 500ms
    const auto renderInterval = std::chrono::milliseconds(150);      // Render every 150ms to prevent flickering
    
    // Track game over state to prevent flickering
    bool gameOverRendered = false;
    
    // Track paused state to prevent flickering
    bool pausedRendered = false;
    
    // Main game loop
    bool running = true;
    
    while (running) {
        auto currentTime = std::chrono::steady_clock::now();
        
        // Handle input (non-blocking)
        KeyCode key = InputHandler::getNonBlockingKey();
        
        if (currentState == MENU) {
            switch (key) {
                case KEY_1:
                    selectedDifficulty = 1;
                    currentState = PLAYING;
                    gameManager.initializeGame(1);
                    lastGhostUpdate = currentTime;
                    break;
                case KEY_2:
                    selectedDifficulty = 2;
                    currentState = PLAYING;
                    gameManager.initializeGame(2);
                    lastGhostUpdate = currentTime;
                    break;
                case KEY_3:
                    selectedDifficulty = 3;
                    currentState = PLAYING;
                    gameManager.initializeGame(3);
                    lastGhostUpdate = currentTime;
                    break;
                case KEY_4:
                    if (gameManager.loadGame("savegame.txt")) {
                        currentState = PLAYING;
                        lastGhostUpdate = currentTime;
                    } else {
                        std::cout << "Failed to load game!" << std::endl;
                    }
                    break;
                case KEY_Q:
                    running = false;
                    break;
                default:
                    break;
            }
            
            // Render menu
            renderer.renderMenu(selectedDifficulty);
        }
        else if (currentState == PLAYING) {
            switch (key) {
                case KEY_UP:
                    gameManager.handlePlayerMove(0, -1);
                    break;
                case KEY_DOWN:
                    gameManager.handlePlayerMove(0, 1);
                    break;
                case KEY_LEFT:
                    gameManager.handlePlayerMove(-1, 0);
                    break;
                case KEY_RIGHT:
                    gameManager.handlePlayerMove(1, 0);
                    break;
                case KEY_P:
                    currentState = PAUSED;
                    gameManager.setPaused(true);
                    pausedRendered = false;  // Reset flag when entering paused state
                    // Render immediately when pause is activated
                    renderer.renderGame(gameManager);
                    pausedRendered = true;
                    break;
                case KEY_S:
                    if (gameManager.saveGame("savegame.txt")) {
                        // Show save message temporarily (could be improved with overlay)
                    } else {
                        // Show error message
                    }
                    break;
                case KEY_M:
                    gameManager.markSpawnpoint();
                    break;
                case KEY_R:
                    gameManager.goToSpawnpoint();
                    break;
                case KEY_ESCAPE:
                    currentState = MENU;
                    break;
                default:
                    break;
            }
            
            // Update game logic (ghosts move automatically even if player doesn't move)
            if (!gameManager.isGamePaused()) {
                // Update ghosts periodically
                if (currentTime - lastGhostUpdate >= ghostUpdateInterval) {
                    gameManager.update();
                    lastGhostUpdate = currentTime;
                }
                
                // Check game over/win conditions
                if (gameManager.isGameOver()) {
                    currentState = GAME_OVER;
                    gameOverRendered = false;  // Reset flag when entering game over state
                    // Render immediately when game over is detected
                    renderer.renderGame(gameManager);
                    gameOverRendered = true;
                }
            }
            
            // Render game at fixed interval for smooth animation (only if not game over)
            if (!gameManager.isGameOver() && currentTime - lastRenderTime >= renderInterval) {
                renderer.renderGame(gameManager);
                lastRenderTime = currentTime;
            }
        }
        else if (currentState == PAUSED) {
            switch (key) {
                case KEY_P:
                    currentState = PLAYING;
                    gameManager.setPaused(false);
                    pausedRendered = false;  // Reset flag when leaving paused state
                    break;
                case KEY_ESCAPE:
                    currentState = MENU;
                    pausedRendered = false;  // Reset flag when leaving paused state
                    break;
                default:
                    break;
            }
            
            // Only render once when entering paused state to prevent flickering
            if (!pausedRendered) {
                renderer.renderGame(gameManager);
                pausedRendered = true;
            }
        }
        else if (currentState == GAME_OVER) {
            switch (key) {
                case KEY_R:
                    currentState = PLAYING;
                    gameManager.resetGame();
                    lastGhostUpdate = std::chrono::steady_clock::now();
                    gameOverRendered = false;  // Reset flag when leaving game over state
                    break;
                case KEY_ESCAPE:
                case KEY_M:
                    currentState = MENU;
                    gameOverRendered = false;  // Reset flag when leaving game over state
                    break;
                default:
                    break;
            }
            
            // Only render once when entering game over state to prevent flickering
            if (!gameOverRendered) {
                renderer.renderGame(gameManager);
                gameOverRendered = true;
            }
        }
        
        // Small sleep to prevent 100% CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    
    // Cleanup
    InputHandler::restore();
    renderer.clearScreen();
    std::cout << "\033[?25h";  // Show cursor
    
    return 0;
}
