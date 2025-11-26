# COMP2113-Group-Project-Group11

# Shadow Maze

# Team Members
Gong Zihan   
Guo Xinrui   3036292520\n
He Jixiang   3036290871\n
Miao Zinan   3036292829\n
Sun Gao      3036292491\n
Xiao Jingzhe 3036253689\n

## Application Description
Maze Game is a modular C++17 terminal application that renders ANSI-colored mazes directly in the console. Players pick a difficulty, explore a fully procedural labyrinth, dodge autonomous ghosts, collect random chests, and optionally teleport back to a user-marked spawnpoint. The loop supports saving/loading progress so a run can be resumed at any time.

## Gameplay Loop
- **Objective**: Reach the exit tile before losing all health to ghosts. Collect chests for buffs and optionally mark a spawnpoint for emergency teleports.
- **Progression**: Every move consumes a step, ghosts update on a timer even if the player waits, and win/loss banners appear once the exit is reached or health reaches zero.
- **Randomness**: DFS-based maze carving, ghost patrol shuffles, chest placement, and chest rewards introduce new layouts and outcomes every run.
- **HUD Feedback**: ANSI UI shows health, move count, difficulty, active chest effects, and spawnpoint location so the player can make tactical decisions without leaving the terminal.

## Controls
- **Menu**: `1-3` start Easy/Medium/Hard, `4` loads `savegame.txt`, `Q` quits.
- **In-Game**: Arrow keys move, `P` toggles pause, `S` saves, `M` stores the current tile as spawnpoint, `R` returns to the spawnpoint, `ESC` goes back to menu.
- **Game Over**: `R` restarts at the same difficulty, `M` or `ESC` returns to menu.

## Features
- Multiple preset difficulties (31×21, 51×31, 71×41) scaling maze size, ghost density, and chest frequency.
- Depth-first-search maze generator with BFS validation, additional passage drilling, and open-area pruning for replayable layouts.
- Player health system with damage, healing caps, shield indicator, and spawnpoint teleportation.
- Ghost manager that instantiates patrol, hunter, random, and teleport ghosts; movement automatically continues using timers and respects temporary freeze/shield states.
- Chest subsystem that scatters loot off the main path, removes claimed chests, and randomly issues health boosts, global ghost freeze, or shield buffs.
- Save/load pipeline that writes the entire maze, metadata, and entity positions to disk via atomic file swaps.
- Pause overlay plus throttled rendering to prevent flicker while keeping the simulation deterministic.

## Non-Standard Libraries
- None. The codebase relies solely on the C++17 standard library for containers, random engines, threads, filesystem interaction, and ANSI escape sequences for coloring.

## Compilation & Execution
1. Ensure a C++17-capable toolchain (e.g., `clang++` or `g++`) is available on macOS/Linux. No third-party libraries are required.
2. From the project root run `make` to build the terminal executable described in `makefile`.
3. Launch the game with `./main`. Interact via the keyboard controls listed above; progress is stored in `savegame.txt`. Play the game in fullscreen mode for best experience!!!!!!!!

## Code Requirements Coverage
- **Generation of random events**: `maze_generate.cpp`, `chest_generate.cpp`, and `chest.cpp` use `std::mt19937` to randomize mazes, chest slots, and chest rewards.
- **Data structures for storing data**: `std::vector`, `std::queue`, and custom structs (`pos`, `Position`) hold maze grids, entities, and BFS parents throughout the engine.
- **Dynamic memory management**: `GameManager` allocates `Player` and `GhostManager` on the heap, recreating them per difficulty/reset to refresh state.
- **File input/output**: `fileio.cpp` reads/writes `savegame.txt` with validation and atomic renames; `GameManager` serializes maze/chest data during saves.
- **Program codes in multiple files**: Logic is split into dedicated headers/implementations (`main_game.cpp`, `GameManager.*`, `ghost.*`, etc.) to isolate rendering, AI, input, persistence, and utilities.
- **Multiple difficulty levels**: Menu commands `1-3` call `GameManager::initializeGame` with distinct maze sizes, ghost counts, and chest ratios.

## File Responsibilities
- `main_game.cpp`: Owns the application state machine (Menu/Playing/Paused/Game Over), drives the main loop, links input, manager, and renderer, and throttles ghost/render timing.
- `GameManager.h/cpp`: Central coordinator that spawns the maze, player, ghosts, and chests; handles movement, win/loss checks, spawnpoints, chest effects, and save/load orchestration.
- `GameRenderer.h/cpp`: Builds ANSI buffers for the maze, entities, UI, pause/game-over overlays, and applies colors/borders before writing to the console.
- `InputHandler.h/cpp`: Configures terminal modes (termios on Unix, `_kbhit` on Windows) to support non-blocking, cross-platform keyboard polling.
- `Player.h/cpp`: Tracks coordinates, max health, live/dead state, and exposes damage/heal helpers.
- `ghost.h/cpp`: Defines `Position`, ghost types, AI behaviors (random walkers, patrol routes, hunters, teleporters), movement cooldowns, collision checks, and the `GhostManager`.
- `maze_generate.h/cpp`: Implements the DFS maze generator, BFS reachability checks, extra passage drilling, and open-area pruning while storing start/exit metadata.
- `chest_generate.h/cpp`: Uses BFS to avoid shortest paths and entrance/exit tiles, then randomly distributes chest positions filtered by difficulty ratio.
- `chest.h/cpp`: Legacy helpers for chest placement plus the `benefit` routine that randomly awards healing, ghost freeze, or shield effects via atomic flags.
- `fileio.h/cpp`: Declares and implements the `GameState` serializer/deserializer with strict validation, CR stripping, and atomic save-file replacement.
- `spawnpoint.h/cpp`: Stores a global spawnpoint, exposes `mark_spawnpoint`/`go_to_spawnpoint`, and logs teleport actions for player feedback.
- `pos.h`: Lightweight struct shared across systems to reference grid coordinates.
- `GameRenderer.o`, `*.o`, `main`: Build outputs generated by `make`.
- `makefile`: Defines compilation targets and dependencies for building the multi-file project.
