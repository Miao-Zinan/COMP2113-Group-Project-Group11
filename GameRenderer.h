#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include <string>
#include <vector>
#include <sstream>

// Forward declarations
class GameManager;

class GameRenderer {
public:
    GameRenderer();
    ~GameRenderer();
    
    void initialize();
    void clearScreen();
    void renderGame(const GameManager& game);
    void renderMenu(int selectedDifficulty);
    void renderPauseOverlay();
    void renderGameOver(const GameManager& game);
    
private:
    void drawMaze(const GameManager& game, std::vector<std::string>& buffer);
    void drawPlayer(const GameManager& game, std::vector<std::string>& buffer);
    void drawGhosts(const GameManager& game, std::vector<std::string>& buffer);
    void drawChests(const GameManager& game, std::vector<std::string>& buffer);
    void drawExit(const GameManager& game, std::vector<std::string>& buffer);
    void drawSpawnpoint(const GameManager& game, std::vector<std::string>& buffer);
    void drawUI(const GameManager& game, std::ostringstream& buffer);
    void drawBorder(std::vector<std::string>& buffer);
    void drawPauseOverlay(std::ostringstream& buffer);
    void drawGameOver(const GameManager& game, std::ostringstream& buffer);
    
    // Color codes (ANSI)
    std::string resetColor() const { return "\033[0m"; }
    std::string colorWall() const { return "\033[48;5;235m\033[38;5;240m"; }     // Dark gray background
    std::string colorPath() const { return "\033[48;5;233m\033[38;5;248m"; }     // Dark background
    std::string colorPlayer() const { return "\033[1;32m"; }                     // Bright green
    std::string colorPlayerShield() const { return "\033[1;36m"; }               // Bright cyan
    std::string colorGhost() const { return "\033[1;31m"; }                      // Bright red
    std::string colorChest() const { return "\033[1;33m"; }                      // Bright yellow
    std::string colorExit() const { return "\033[1;34m"; }                       // Bright blue
    std::string colorSpawnpoint() const { return "\033[1;35m"; }                 // Bright magenta
    std::string colorText() const { return "\033[1;37m"; }                       // Bright white
    std::string colorHealthGood() const { return "\033[1;32m"; }                 // Green
    std::string colorHealthMedium() const { return "\033[1;33m"; }               // Yellow
    std::string colorHealthLow() const { return "\033[1;31m"; }                  // Red
    std::string colorEffect() const { return "\033[1;36m"; }                     // Cyan
    std::string colorTitle() const { return "\033[1;35m"; }                      // Magenta
    std::string colorSelected() const { return "\033[1;33m"; }                   // Yellow
    std::string colorPaused() const { return "\033[1;33m"; }                     // Yellow
    std::string colorWin() const { return "\033[1;32m"; }                        // Green
    std::string colorLose() const { return "\033[1;31m"; }                       // Red
};

#endif // GAMERENDERER_H
