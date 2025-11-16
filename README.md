# oop-project
# Chrome Dino Game

A C++ recreation of the classic Chrome offline dinosaur game with enhanced features including difficulty levels, player statistics, and high score tracking.

## Features

### Gameplay
- **Three Difficulty Levels**: Easy, Medium, and Hard modes with varying obstacle speeds and spawn rates
- **Smooth Jump Mechanics**: Physics-based jumping with realistic gravity
- **Dynamic Obstacles**: Randomly spawning obstacles that increase challenge over time
- **Real-time Scoring**: Your score increases as you survive longer

### Player System
- **Player Profiles**: Track individual player statistics across multiple sessions
- **Persistent Storage**: All scores and stats are saved to disk
- **Comprehensive Statistics**: 
  - Total games played
  - Games played per difficulty
  - Best scores for each difficulty level
  - Cumulative total score

### High Scores
- **Difficulty-Based Leaderboards**: Separate high score tables for each difficulty
- **Score History**: View all recorded scores for any difficulty level

## How to Play

### Controls
- **SPACEBAR**: Jump to avoid obstacles
- Game automatically scrolls, focus on timing your jumps!

### Difficulty Levels

| Difficulty | Obstacle Speed | Spawn Interval | Description |
|-----------|---------------|----------------|-------------|
| Easy | 4 pixels/frame | 2.0s | Perfect for beginners |
| Medium | 7 pixels/frame | 1.5s | Balanced challenge |
| Hard | 10 pixels/frame | 1.0s | For experienced players |

## Technical Requirements

### Dependencies
- **SFML 3.x** (Simple and Fast Multimedia Library)
  - Graphics module for rendering
  - Window module for display
  - System module for timing

### Compiler Requirements
- C++11 or later
- Support for standard library features

## Installation

### 1. Install SFML

#### Windows (with vcpkg)
```bash
vcpkg install sfml
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install libsfml-dev
```

#### macOS (with Homebrew)
```bash
brew install sfml
```

### 2. Compile the Game

#### Using g++
```bash
g++ -std=c++11 main.cpp -o DinoGame -lsfml-graphics -lsfml-window -lsfml-system
```

#### Using CMake (recommended)
Create a `CMakeLists.txt`:
```cmake
cmake_minimum_required(VERSION 3.10)
project(DinoGame)

set(CMAKE_CXX_STANDARD 11)

find_package(SFML 3 COMPONENTS graphics window system REQUIRED)

add_executable(DinoGame main.cpp)
target_link_libraries(DinoGame sfml-graphics sfml-window sfml-system)
```

Then compile:
```bash
mkdir build
cd build
cmake ..
make
```

### 3. Run the Game
```bash
./DinoGame
```

## File Structure

```
DinoGame/
├── main.cpp           # Main game source code
├── easy.txt           # Easy mode high scores
├── medium.txt         # Medium mode high scores
├── hard.txt           # Hard mode high scores
└── players.dat        # Binary player statistics database
```

## Game Modes

### Main Menu Options
1. **New Game** - Start a new game session
2. **Show High Scores** - View leaderboards by difficulty
3. **Show Player Scores** - View detailed statistics for a specific player
4. **Help** - Display game instructions
5. **Exit** - Quit the game

## Data Storage

### High Scores
- Stored in plain text files (`easy.txt`, `medium.txt`, `hard.txt`)
- Format: `PlayerName Score`

### Player Statistics
- Stored in binary format (`players.dat`)
- Includes:
  - Player name
  - Total games played
  - Games per difficulty
  - Best scores per difficulty
  - Cumulative total score

## Code Architecture

### Module Organization

#### Group A: Player, Menu, and File Management
- Menu system and user interface
- Player name validation and input handling
- File I/O operations for scores and statistics
- Player data management (load, save, update)

#### Group B: Gameplay Logic
- Game state management
- Physics calculations (jumping, gravity)
- Obstacle spawning and movement
- Collision detection
- Score tracking

#### Group C: Graphics and Rendering (SFML)
- Window management
- Sprite rendering (dinosaur, obstacles, ground)
- Game loop with frame rate control
- Event handling

### Key Data Structures

```cpp
struct GameState {
    int playerX, playerY;          // Player position
    float jumpVelocity;            // Current jump velocity
    bool isJumping;                // Jump state
    vector<Obstacle> obstacles;    // Active obstacles
    int score;                     // Current score
    int obstacleSpeed;             // Speed setting
    float spawnInterval;           // Spawn timing
    bool isRunning;                // Game active flag
};

struct PlayerStats {
    char name[50];                 // Player name
    int gamesPlayed;               // Total games
    int easyPlayed, mediumPlayed, hardPlayed;
    int totalScore;                // Cumulative score
    int bestEasy, bestMedium, bestHard;
};
```

## Known Limitations

- Font rendering for score display is not implemented (score shown in console)
- Text-based fallback mode included for testing without graphics
- Player names limited to 50 characters

## Customization

You can easily modify game parameters in the global constants section:

```cpp
const int WINDOW_WIDTH = 800;        // Window width
const int WINDOW_HEIGHT = 400;       // Window height
const int GROUND_LEVEL = 300;        // Ground position
const int DINO_WIDTH = 50;           // Dinosaur size
const int DINO_HEIGHT = 50;

// Difficulty settings
const int EASY_SPEED = 4;            // Easy obstacle speed
const int MEDIUM_SPEED = 7;          // Medium obstacle speed
const int HARD_SPEED = 10;           // Hard obstacle speed
const float EASY_SPAWN = 2.0f;       // Easy spawn interval
const float MEDIUM_SPAWN = 1.5f;     // Medium spawn interval
const float HARD_SPAWN = 1.0f;       // Hard spawn interval
```

## Contributing

Feel free to fork this project and submit pull requests for:
- Bug fixes
- New features (power-ups, different obstacle types)
- Performance improvements
- Better graphics and animations

## License

This project is open source and available for educational purposes.

## Learning Objectives

This project demonstrates:
- C++ object-oriented programming
- File I/O and binary data handling
- Game loop implementation
- Physics simulation
- SFML graphics library usage
- Memory management with dynamic arrays
- Cross-platform development considerations

## Acknowledgments

Inspired by the Chrome offline dinosaur game created by Google.

---

**Happy Gaming!!**
