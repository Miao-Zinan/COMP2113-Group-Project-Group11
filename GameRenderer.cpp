
#include "GameRenderer.h"
#include "GameManager.h"
#include "Player.h"
#include "ghost.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

GameRenderer::GameRenderer() {
}

GameRenderer::~GameRenderer() {
    // Restore terminal on exit: reset colors and show cursor
    std::cout << resetColor() << "\033[?25h";
}

void GameRenderer::initialize() {
    // Hide cursor for smoother display
    std::cout << "\033[?25l";
    clearScreen();
}

void GameRenderer::clearScreen() {
    // Initial clear when starting the game
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

/**
 * Render the main game screen.
 * Uses buffered output and cursor repositioning to reduce flicker.
 */
void GameRenderer::renderGame(const GameManager& game) {
    std::ostringstream screenBuffer;

    // Move cursor to top-left without clearing the screen
    screenBuffer << "\033[H";

    const int bufferWidth = game.getWidth() + 2;

    // Draw top border
    std::string topBorder = colorWall();
    for (int i = 0; i < bufferWidth; i++) {
        topBorder += "██";
    }
    topBorder += resetColor();
    screenBuffer << topBorder << "\n";

    // Draw maze rows
    for (int y = 0; y < game.getHeight(); y++) {
        std::string row = colorWall() + "█";
        for (int x = 0; x < game.getWidth(); x++) {
            // Player rendering
            const Player* player = game.getPlayer();
            if (player && player->getX() == x && player->getY() == y) {
                row += game.isPlayerShielded() ? colorPlayerShield() + "@@" : colorPlayer() + "@@";
                continue;
            }

            // Ghost rendering
            bool ghostFound = false;
            for (const auto& ghost : game.getGhosts()) {
                Position pos = ghost.getPosition();
                if (pos.x == x && pos.y == y) {
                    row += colorGhost() + std::string(2, ghost.getDisplayChar());
                    ghostFound = true;
                    break;
                }
            }
            if (ghostFound) continue;

            // Exit rendering
            if (game.getExitX() == x && game.getExitY() == y) {
                row += colorExit() + "><";
                continue;
            }

            // Chest rendering
            bool chestFound = false;
            for (const auto& chest : game.getChests()) {
                if (chest.x == x && chest.y == y) {
                    row += colorChest() + "$ ";
                    chestFound = true;
                    break;
                }
            }
            if (chestFound) continue;

            // Spawnpoint rendering
            if (game.hasSpawnpoint() && game.getSpawnpointX() == x && game.getSpawnpointY() == y) {
                row += colorSpawnpoint() + "**";
                continue;
            }

            // Default: wall or path
            const auto& maze = game.getMaze();
            row += (maze[y][x] == '#') ? colorWall() + "██" : colorPath() + "  ";
        }
        row += colorWall() + "█" + resetColor();
        screenBuffer << row << "\n";
    }

    // Draw bottom border
    std::string bottomBorder = colorWall();
    for (int i = 0; i < bufferWidth; i++) {
        bottomBorder += "██";
    }
    bottomBorder += resetColor();
    screenBuffer << bottomBorder << "\n";

    // Draw UI and overlays
    drawUI(game, screenBuffer);
    if (game.isGamePaused()) drawPauseOverlay(screenBuffer);
    if (game.isGameOver()) drawGameOver(game, screenBuffer);

    // Output the complete buffer at once
    std::cout << screenBuffer.str();
    std::cout.flush();
}

/**
 * Render the main menu screen.
 */

 void GameRenderer::renderMenu(int selectedDifficulty) {
    std::ostringstream menuBuffer;

    // Full clear when switching to menu
    menuBuffer << "\033[2J\033[H";  // Clear screen and move cursor to top-left
    menuBuffer << resetColor() << "\n\n\n";

    // Title - centered
    const int titleWidth = 24;  // Width of the title box
    const int screenWidth = 80;  // Assume standard terminal width
    const int padding = (screenWidth - titleWidth) / 2;
    menuBuffer << std::string(padding, ' ') << colorTitle() << "╔════════════════════════╗\n";
    menuBuffer << std::string(padding, ' ') << colorTitle() << "║      MAZE GAME         ║\n";
    menuBuffer << std::string(padding, ' ') << colorTitle() << "╚════════════════════════╝\n\n\n" << resetColor();
    
    // Menu items
    for (int i = 1; i <= 5; i++) {
        std::string prefix = (i == selectedDifficulty) ? colorSelected() + "→ " : colorText() + "  ";
        std::string itemColor = (i == selectedDifficulty) ? colorSelected() : colorText();
        menuBuffer << std::setw(40) << "";
        switch (i) {
            case 1:
                menuBuffer << prefix << itemColor << "1. Easy" << resetColor() << "\n";
                break;
            case 2:
                menuBuffer << prefix << itemColor << "2. Medium" << resetColor() << "\n";
                break;
            case 3:
                menuBuffer << prefix << itemColor << "3. Hard" << resetColor() << "\n";
                break;
            case 4:
                menuBuffer << prefix << itemColor << "4. Continue Saved Game" << resetColor() << "\n";
                break;
            case 5:
                menuBuffer << prefix << itemColor << "Q. Quit" << resetColor() << "\n";
                break;
        }
        menuBuffer << "\n";
    }

    menuBuffer << resetColor() << "\n\n";

    // Output complete buffer at once
    std::cout << menuBuffer.str();
    std::cout.flush();
}

/**
 * Draw UI with health and controls (without box border).
 */
void GameRenderer::drawUI(const GameManager& game, std::ostringstream& buffer) {
    const Player* player = game.getPlayer();
    if (!player) return;

    // Health info
    int health = player->getHealth();
    int maxHealth = player->getMaxHealth();
    std::string healthColor = (health == maxHealth) ? colorHealthGood() :
                              (health > maxHealth / 2) ? colorHealthMedium() : colorHealthLow();

    buffer << "\n";
    buffer << colorText() << "Health: " << healthColor << health << "/" << maxHealth << resetColor() << "\n";

    // Chest effect message - always reserve a line to prevent Controls from jumping
    const std::string effectMessage = game.getActiveChestEffectMessage();
    if (!effectMessage.empty()) {
        buffer << colorText() << "Effect: " << colorEffect() << effectMessage << resetColor() << "\n";
    } else {
        // Reserve empty line when no effect message to keep Controls in fixed position
        buffer << "\n";
    }

    // Controls info - always on third line
    buffer << colorText() << "Controls: Arrow Keys: Move | P: Pause | S: Save | M: Mark | R: Return | ESC: Menu" << resetColor() << "\n";
}

/**
 * Draw pause overlay.
 */
void GameRenderer::drawPauseOverlay(std::ostringstream& buffer) {
    int overlayY = 10;
    int overlayX = 40;
    buffer << "\033[" << overlayY << ";" << overlayX << "H";
    buffer << colorPaused() << "╔════════════════╗\n";
    buffer << "\033[" << (overlayY + 1) << ";" << overlayX << "H" << colorPaused() << "║                ║\n";
    buffer << "\033[" << (overlayY + 2) << ";" << overlayX << "H" << colorPaused() << "║    PAUSED      ║\n";
    buffer << "\033[" << (overlayY + 3) << ";" << overlayX << "H" << colorPaused() << "║                ║\n";
    buffer << "\033[" << (overlayY + 4) << ";" << overlayX << "H" << colorPaused() << "║ Press P to     ║\n";
    buffer << "\033[" << (overlayY + 5) << ";" << overlayX << "H" << colorPaused() << "║ resume         ║\n";
    buffer << "\033[" << (overlayY + 6) << ";" << overlayX << "H" << colorPaused() << "╚════════════════╝" << resetColor();
}

/**
 * Draw game over overlay.
 */
void GameRenderer::drawGameOver(const GameManager& game, std::ostringstream& buffer) {
    int overlayY = 10;
    int overlayX = 35;
    buffer << "\033[" << overlayY << ";" << overlayX << "H";
    if (game.isGameWon()) {
        buffer << colorWin() << "╔════════════════════════╗\n";
        buffer << "\033[" << (overlayY + 1) << ";" << overlayX << "H" << colorWin() << "║                        ║\n";
        buffer << "\033[" << (overlayY + 2) << ";" << overlayX << "H" << colorWin() << "║       YOU WIN!         ║\n";
        buffer << "\033[" << (overlayY + 3) << ";" << overlayX << "H" << colorWin() << "║                        ║\n";
        buffer << "\033[" << (overlayY + 4) << ";" << overlayX << "H" << colorWin() << "╚════════════════════════╝" << resetColor();
    } else {
        buffer << colorLose() << "╔════════════════════════╗\n";
        buffer << "\033[" << (overlayY + 1) << ";" << overlayX << "H" << colorLose() << "║                        ║\n";
        buffer << "\033[" << (overlayY + 2) << ";" << overlayX << "H" << colorLose() << "║      GAME OVER!        ║\n";
        buffer << "\033[" << (overlayY + 3) << ";" << overlayX << "H" << colorLose() << "║                        ║\n";
        buffer << "\033[" << (overlayY + 4) << ";" << overlayX << "H" << colorLose() << "╚════════════════════════╝" << resetColor();
    }
}
