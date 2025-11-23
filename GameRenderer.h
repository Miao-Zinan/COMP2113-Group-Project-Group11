#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>

// Forward declarations
class GameManager;

class GameRenderer {
private:
    sf::RenderWindow* window;
    sf::Font font;
    
    // Cell size for rendering
    int cellSize;
    int offsetX, offsetY;
    
    // Colors
    sf::Color wallColor;
    sf::Color pathColor;
    sf::Color playerColor;
    sf::Color ghostColor;
    sf::Color chestColor;
    sf::Color exitColor;
    sf::Color spawnpointColor;
    sf::Color backgroundColor;
    sf::Color textColor;
    
    // Text elements
    sf::Text healthText;
    sf::Text movesText;
    sf::Text difficultyText;
    sf::Text pausedText;
    sf::Text gameOverText;
    sf::Text winText;
    sf::Text instructionsText;
    
    bool fontLoaded;
    
public:
    GameRenderer();
    ~GameRenderer();
    
    bool initialize(int windowWidth = 1200, int windowHeight = 800, const std::string& title = "Maze Game");
    void close();
    bool isOpen() const;
    bool pollEvent(sf::Event& event);
    void clear();
    void display();
    
    void renderGame(const GameManager& game);
    void renderMenu(int selectedDifficulty);
    void renderPauseMenu();
    void renderGameOver(const GameManager& game);
    
    void setCellSize(int size) { cellSize = size; }
    
private:
    void drawMaze(const GameManager& game);
    void drawPlayer(const GameManager& game);
    void drawGhosts(const GameManager& game);
    void drawChests(const GameManager& game);
    void drawExit(const GameManager& game);
    void drawSpawnpoint(const GameManager& game);
    void drawUI(const GameManager& game);
    void drawRectangle(int x, int y, int w, int h, const sf::Color& color);
    
    void initializeText();
};

#endif // GAMERENDERER_H

