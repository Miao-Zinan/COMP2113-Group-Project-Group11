#include "GameRenderer.h"
#include "GameManager.h"
#include "Player.h"
#include "ghost.h"
#include <sstream>
#include <iostream>

GameRenderer::GameRenderer() 
    : window(nullptr), cellSize(20), offsetX(50), offsetY(50),
      wallColor(50, 50, 50), pathColor(240, 240, 240),
      playerColor(0, 200, 0), ghostColor(200, 0, 0),
      chestColor(255, 215, 0), exitColor(0, 0, 255),
      spawnpointColor(0, 255, 255), backgroundColor(30, 30, 30), 
      textColor(255, 255, 255), fontLoaded(false) {
}

GameRenderer::~GameRenderer() {
    close();
}

bool GameRenderer::initialize(int windowWidth, int windowHeight, const std::string& title) {
    window = new sf::RenderWindow(sf::VideoMode(windowWidth, windowHeight), title);
    window->setFramerateLimit(60);
    
    // Try to load font - if it fails, we'll use default
    fontLoaded = font.loadFromFile("arial.ttf");
    if (!fontLoaded) {
        // Try alternative fonts (Windows)
        fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }
    if (!fontLoaded) {
        // Try Linux font paths
        fontLoaded = font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
    }
    if (!fontLoaded) {
        fontLoaded = font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    }
    if (!fontLoaded) {
        fontLoaded = font.loadFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf");
    }
    if (!fontLoaded) {
        std::cerr << "Warning: Could not load font. Text may not display correctly." << std::endl;
        std::cerr << "Game will still run, but text may not be visible." << std::endl;
    }
    
    initializeText();
    
    return true;
}

void GameRenderer::initializeText() {
    if (fontLoaded) {
        healthText.setFont(font);
        movesText.setFont(font);
        difficultyText.setFont(font);
        pausedText.setFont(font);
        gameOverText.setFont(font);
        winText.setFont(font);
        instructionsText.setFont(font);
    }
    
    healthText.setCharacterSize(20);
    healthText.setFillColor(textColor);
    healthText.setPosition(10, 10);
    
    movesText.setCharacterSize(20);
    movesText.setFillColor(textColor);
    movesText.setPosition(10, 35);
    
    difficultyText.setCharacterSize(20);
    difficultyText.setFillColor(textColor);
    difficultyText.setPosition(10, 60);
    
    pausedText.setCharacterSize(40);
    pausedText.setFillColor(sf::Color::Yellow);
    pausedText.setString("PAUSED\nPress P to resume");
    pausedText.setPosition(400, 300);
    
    gameOverText.setCharacterSize(40);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("GAME OVER!");
    gameOverText.setPosition(400, 300);
    
    winText.setCharacterSize(40);
    winText.setFillColor(sf::Color::Green);
    winText.setString("YOU WIN!");
    winText.setPosition(450, 300);
    
    instructionsText.setCharacterSize(16);
    instructionsText.setFillColor(textColor);
    instructionsText.setPosition(10, 750);
}

void GameRenderer::close() {
    if (window) {
        window->close();
        delete window;
        window = nullptr;
    }
}

bool GameRenderer::isOpen() const {
    return window && window->isOpen();
}

bool GameRenderer::pollEvent(sf::Event& event) {
    if (window) {
        return window->pollEvent(event);
    }
    return false;
}

void GameRenderer::clear() {
    if (window) {
        window->clear(backgroundColor);
    } else {
        std::cerr << "Warning: clear() called but window is null!" << std::endl;
    }
}

void GameRenderer::display() {
    if (window) {
        window->display();
    }
}

void GameRenderer::renderGame(const GameManager& game) {
    if (!window) return;
    
    clear();
    
    // Calculate cell size based on maze dimensions
    int mazeWidth = game.getWidth();
    int mazeHeight = game.getHeight();
    int availableWidth = window->getSize().x - offsetX * 2;
    int availableHeight = window->getSize().y - offsetY * 2 - 100; // Reserve space for UI
    
    int cellSizeX = availableWidth / mazeWidth;
    int cellSizeY = availableHeight / mazeHeight;
    cellSize = std::min(cellSizeX, cellSizeY);
    
    // Draw game elements
    drawMaze(game);
    drawChests(game);
    drawExit(game);
    drawSpawnpoint(game);
    drawGhosts(game);
    drawPlayer(game);
    drawUI(game);
    
    // Draw pause overlay if paused
    if (game.isGamePaused()) {
        pausedText.setPosition(window->getSize().x / 2 - 150, window->getSize().y / 2 - 50);
        window->draw(pausedText);
    }
    
    // Draw game over overlay
    if (game.isGameOver()) {
        if (game.isGameWon()) {
            winText.setPosition(window->getSize().x / 2 - 100, window->getSize().y / 2 - 50);
            window->draw(winText);
        } else {
            gameOverText.setPosition(window->getSize().x / 2 - 150, window->getSize().y / 2 - 50);
            window->draw(gameOverText);
        }
    }
    
    display();
}

void GameRenderer::drawMaze(const GameManager& game) {
    const auto& maze = game.getMaze();
    
    for (int y = 0; y < game.getHeight(); y++) {
        for (int x = 0; x < game.getWidth(); x++) {
            int screenX = offsetX + x * cellSize;
            int screenY = offsetY + y * cellSize;
            
            if (maze[y][x] == '#') {
                drawRectangle(screenX, screenY, cellSize, cellSize, wallColor);
            } else {
                drawRectangle(screenX, screenY, cellSize, cellSize, pathColor);
            }
        }
    }
}

void GameRenderer::drawPlayer(const GameManager& game) {
    const Player* player = game.getPlayer();
    if (!player) return;
    
    int screenX = offsetX + player->getX() * cellSize;
    int screenY = offsetY + player->getY() * cellSize;
    
    // Draw player as a circle
    sf::CircleShape playerShape(cellSize * 0.4f);
    playerShape.setFillColor(playerColor);
    playerShape.setPosition(screenX + cellSize * 0.1f, screenY + cellSize * 0.1f);
    window->draw(playerShape);
}

void GameRenderer::drawGhosts(const GameManager& game) {
    const auto& ghosts = game.getGhosts();
    
    for (const auto& ghost : ghosts) {
        Position pos = ghost.getPosition();
        int screenX = offsetX + pos.x * cellSize;
        int screenY = offsetY + pos.y * cellSize;
        
        // Draw ghost as a square
        sf::RectangleShape ghostShape(sf::Vector2f(cellSize * 0.8f, cellSize * 0.8f));
        ghostShape.setFillColor(ghostColor);
        ghostShape.setPosition(screenX + cellSize * 0.1f, screenY + cellSize * 0.1f);
        window->draw(ghostShape);
    }
}

void GameRenderer::drawChests(const GameManager& game) {
    const auto& chests = game.getChests();
    
    for (const auto& chest : chests) {
        int screenX = offsetX + chest.x * cellSize;
        int screenY = offsetY + chest.y * cellSize;
        
        // Draw chest as a gold rectangle
        sf::RectangleShape chestShape(sf::Vector2f(cellSize * 0.7f, cellSize * 0.7f));
        chestShape.setFillColor(chestColor);
        chestShape.setPosition(screenX + cellSize * 0.15f, screenY + cellSize * 0.15f);
        window->draw(chestShape);
    }
}

void GameRenderer::drawExit(const GameManager& game) {
    int screenX = offsetX + game.getExitX() * cellSize;
    int screenY = offsetY + game.getExitY() * cellSize;
    
    // Draw exit as a blue rectangle
    sf::RectangleShape exitShape(sf::Vector2f(cellSize * 0.8f, cellSize * 0.8f));
    exitShape.setFillColor(exitColor);
    exitShape.setPosition(screenX + cellSize * 0.1f, screenY + cellSize * 0.1f);
    window->draw(exitShape);
}

void GameRenderer::drawSpawnpoint(const GameManager& game) {
    if (!game.hasSpawnpoint()) return;
    
    int screenX = offsetX + game.getSpawnpointX() * cellSize;
    int screenY = offsetY + game.getSpawnpointY() * cellSize;
    
    // Draw spawnpoint as a cyan circle with a border
    sf::CircleShape spawnpointShape(cellSize * 0.25f);
    spawnpointShape.setFillColor(sf::Color::Transparent);
    spawnpointShape.setOutlineColor(spawnpointColor);
    spawnpointShape.setOutlineThickness(2.0f);
    spawnpointShape.setPosition(screenX + cellSize * 0.25f, screenY + cellSize * 0.25f);
    window->draw(spawnpointShape);
    
    // Draw a small dot in the center
    sf::CircleShape centerDot(cellSize * 0.1f);
    centerDot.setFillColor(spawnpointColor);
    centerDot.setPosition(screenX + cellSize * 0.4f, screenY + cellSize * 0.4f);
    window->draw(centerDot);
}

void GameRenderer::drawUI(const GameManager& game) {
    if (!fontLoaded) return;
    
    const Player* player = game.getPlayer();
    if (!player) return;
    
    // Health
    std::ostringstream healthStream;
    healthStream << "Health: " << player->getHealth() << "/" << player->getMaxHealth();
    healthText.setString(healthStream.str());
    window->draw(healthText);
    
    // Moves
    std::ostringstream movesStream;
    movesStream << "Moves: " << game.getMoves();
    movesText.setString(movesStream.str());
    window->draw(movesText);
    
    // Difficulty
    std::ostringstream diffStream;
    diffStream << "Difficulty: ";
    switch (game.getDifficulty()) {
        case 1: diffStream << "Easy"; break;
        case 2: diffStream << "Medium"; break;
        case 3: diffStream << "Hard"; break;
    }
    difficultyText.setString(diffStream.str());
    window->draw(difficultyText);
    
    // Instructions
    std::string instructions = "Arrow Keys: Move | P: Pause | S: Save | L: Load | M: Mark Spawn | R: Return to Spawn | ESC: Menu";
    instructionsText.setString(instructions);
    window->draw(instructionsText);
}

void GameRenderer::drawRectangle(int x, int y, int w, int h, const sf::Color& color) {
    sf::RectangleShape rect(sf::Vector2f(w, h));
    rect.setFillColor(color);
    rect.setPosition(x, y);
    window->draw(rect);
}

void GameRenderer::renderMenu(int selectedDifficulty) {
    if (!window) {
        std::cerr << "Error: renderMenu called but window is null!" << std::endl;
        return;
    }
    
    clear();
    
    // Draw background rectangles for menu items (even without font)
    int menuX = 450;
    int menuY = 250;
    int itemHeight = 50;
    int itemWidth = 300;
    
    // Draw menu item backgrounds
    for (int i = 0; i < 5; i++) {
        sf::Color bgColor = (i == selectedDifficulty - 1) ? sf::Color(100, 100, 100) : sf::Color(60, 60, 60);
        drawRectangle(menuX, menuY + i * itemHeight, itemWidth, itemHeight - 5, bgColor);
    }
    
    if (fontLoaded) {
        sf::Text title("MAZE GAME", font, 60);
        title.setFillColor(sf::Color::White);
        title.setPosition(400, 100);
        window->draw(title);
        
        sf::Text easy("1. Easy", font, 40);
        sf::Text medium("2. Medium", font, 40);
        sf::Text hard("3. Hard", font, 40);
        sf::Text load("4. Load Game", font, 40);
        sf::Text quit("Q. Quit", font, 40);
        
        easy.setPosition(menuX + 10, menuY);
        medium.setPosition(menuX + 10, menuY + itemHeight);
        hard.setPosition(menuX + 10, menuY + itemHeight * 2);
        load.setPosition(menuX + 10, menuY + itemHeight * 3);
        quit.setPosition(menuX + 10, menuY + itemHeight * 4);
        
        if (selectedDifficulty == 1) easy.setFillColor(sf::Color::Yellow);
        else easy.setFillColor(sf::Color::White);
        
        if (selectedDifficulty == 2) medium.setFillColor(sf::Color::Yellow);
        else medium.setFillColor(sf::Color::White);
        
        if (selectedDifficulty == 3) hard.setFillColor(sf::Color::Yellow);
        else hard.setFillColor(sf::Color::White);
        
        load.setFillColor(sf::Color::White);
        quit.setFillColor(sf::Color::White);
        
        window->draw(easy);
        window->draw(medium);
        window->draw(hard);
        window->draw(load);
        window->draw(quit);
    } else {
        // Draw simple shapes as fallback when font is not available
        // Draw colored rectangles to indicate menu items
        for (int i = 0; i < 5; i++) {
            sf::Color indicatorColor = (i == selectedDifficulty - 1) ? sf::Color::Yellow : sf::Color::White;
            sf::RectangleShape indicator(sf::Vector2f(20, 20));
            indicator.setFillColor(indicatorColor);
            indicator.setPosition(menuX + 10, menuY + i * itemHeight + 15);
            window->draw(indicator);
        }
        
        // Draw a title rectangle
        drawRectangle(350, 100, 500, 80, sf::Color(80, 80, 80));
    }
    
    display();
}

void GameRenderer::renderPauseMenu() {
    if (!window || !fontLoaded) return;
    
    sf::Text pause("PAUSED", font, 50);
    pause.setFillColor(sf::Color::Yellow);
    pause.setPosition(window->getSize().x / 2 - 100, window->getSize().y / 2 - 100);
    window->draw(pause);
}

void GameRenderer::renderGameOver(const GameManager& game) {
    // This is handled in renderGame method
}

