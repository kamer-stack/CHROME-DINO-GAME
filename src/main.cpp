#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cstring>

using namespace std;

const int windowWidth = 800;
const int windowHeight = 400;

const int groundLevel = 300;
const int dinoWidth = 50;
const int dinoHeight = 50;

const int obstacleWidth = 30;
const int obstacleHeight = 50;
const int maxObstacles = 100;

const int easySpeed = 4;
const int mediumSpeed = 7;
const int hardSpeed = 10;

const float easySpawn = 2.0f;
const float mediumSpawn = 1.5f;
const float hardSpawn = 1.0f;

struct Obstacle
{
    int x, y;
    bool active;
};

struct PlayerStats
{
    char name[50];
    int gamesPlayed;
    int easyPlayed;
    int mediumPlayed;
    int hardPlayed;
    int totalScore;
    int bestEasy;
    int bestMedium;
    int bestHard;

    PlayerStats ()
    {
        name[0] = '\0';
        gamesPlayed = 0;
        easyPlayed = 0;
        mediumPlayed = 0;
        hardPlayed = 0;
        totalScore = 0;
        bestEasy = 0;
        bestMedium = 0;
        bestHard = 0;
    }
};

struct GameState
{
    int playerX;
    int playerY;
    float jumpVelocity;
    bool isJumping;
    Obstacle obstacles [maxObstacles];
    int obstacleCount;
    int score;
    int obstacleSpeed;
    float spawnInterval;
    bool isRunning;

    GameState ()
        : playerX(0), playerY(0), jumpVelocity(0.0f), isJumping(false), obstacleCount(0), score(0), obstacleSpeed(0), spawnInterval(0.0f), isRunning(false)
    {
        for (int i = 0; i < maxObstacles; i++)
        {
            obstacles[i].active = false;
        }
    }
};


void showMainMenu ();
int getMenuChoice ();
void handleMenuChoice(int choice);
void showHelp ();
void getPlayerName(char playerName[]);
int chooseDifficulty ();
void showHighScoresMenu ();
void showHighScores(int difficulty);
void showPlayerScores ();
bool fileExists (const char *filename);
void saveHighScore (int difficulty, const char name[], int score);
void savePlayerStats (const char name[], int difficulty, int score);
void loadAllPlayers (PlayerStats *&arr, int &count);
void saveAllPlayers (PlayerStats *arr, int count);
int findPlayerIndex (PlayerStats *arr, int count, const char name[]);
void updatePlayerRecord (PlayerStats &p, int difficulty, int score);
void expandPlayerArray (PlayerStats *&arr, int &count);
void showPlayerStats (const PlayerStats &p);

void clearScreen ();
void pauseScreen ();
void clearInputBuffer ();
bool isValidName(const char *name);

void startNewGame ();
void startGame (int difficulty, const char playerName[]);
void initializeGame (GameState &game, int difficulty);
void updateDino(GameState &game);
void updateObstacles(GameState &game);
bool checkCollision(const GameState &game);
void updateScore (GameState &game);
void gameOverScreen (int score, const char playerName[], int difficulty);

void setDifficultyParams(GameState &game, int difficulty);
void spawnObstacle(GameState &game);

void renderGame (sf::RenderWindow &window, const GameState &game);
void drawGround (sf::RenderWindow &window, const sf::Sprite &bgSprite);
void drawDino (sf::RenderWindow &window, const GameState &game);
void drawObstacles (sf::RenderWindow &window, const GameState &game);
void drawScore (sf::RenderWindow &window, int score);
void gameLoop (int difficulty, const char playerName[]);

void textBasedGameLoop (int difficulty, const char playerName[]);

// ====================== MAIN FUNCTION ======================
int main ()
{
    srand(static_cast<unsigned int>(time(0)));

    while (true)
    {
        showMainMenu ();
        int choice = getMenuChoice ();
        if (choice == 5)
        {
            cout << "\nThank you for playing Goodbye!\n";
            break;
        }
        handleMenuChoice(choice);
    }

    return 0;
}

// ====================== UTILITY FUNCTIONS ======================
void clearScreen ()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen ()
{
    cout << "\nPress Enter to continue...";
    clearInputBuffer ();
    cin.get ();
}

void clearInputBuffer ()
{
    cin.clear ();
    while (cin.get () != '\n')
        ;
}

bool isValidName(const char *name)
{
    int len = 0;
    while (name[len] != '\0')
        len++;

    if (len == 0 || len >= 50)
        return false;

    for (int i = 0; name[i] != '\0'; i++)
    {
        if (!isalnum(name[i]) && name[i] != '_' && name[i] != ' ')
        {
            return false;
        }
    }
    return true;
}

void showMainMenu ()
{
    clearScreen ();
    cout << "========================================\n";
    cout << "          CHROME DINO GAME             \n";
    cout << "========================================\n";
    cout << "1. New Game\n";
    cout << "2. Show High Scores\n";
    cout << "3. Show Player Scores\n";
    cout << "4. Help\n";
    cout << "5. Exit\n";
    cout << "========================================\n";
    cout << "Enter your choice (1-5): ";
}

int getMenuChoice ()
{
    int choice;
    while (!(cin >> choice) || choice < 1 || choice > 5)
    {
        clearInputBuffer ();
        cout << "Invalid input! Please enter a number between 1-5: ";
    }
    clearInputBuffer ();
    return choice;
}

void handleMenuChoice(int choice)
{
    char playerName[50];
    int difficulty;

    switch (choice)
    {
    case 1:
        getPlayerName(playerName);
        difficulty = chooseDifficulty ();
        startGame(difficulty, playerName);
        break;
    case 2:
        showHighScoresMenu ();
        pauseScreen ();
        break;
    case 3:
        showPlayerScores ();
        pauseScreen ();
        break;
    case 4:
        showHelp ();
        pauseScreen ();
        break;
    case 5:
        break;
    default:
        cout << "Invalid choice.\n";
        pauseScreen ();
    }
}

void showHelp ()
{
    clearScreen ();
    cout << "========================================\n";
    cout << "              HOW TO PLAY              \n";
    cout << "========================================\n";
    cout << "\n1. Press SPACEBAR to jump\n";
    cout << "2. Avoid the red obstacles (cacti)\n";
    cout << "3. Your score increases with distance\n";
    cout << "4. Game ends when you hit an obstacle\n";
    cout << "\nDIFFICULTY LEVELS:\n";
    cout << "  - Easy: Slower obstacles, more space\n";
    cout << "  - Medium: Moderate speed and spacing\n";
    cout << "  - Hard: Fast obstacles, tight spacing\n";
    cout << "\nGood luck and have fun!\n";
    cout << "========================================\n";
}

void getPlayerName(char playerName[])
{
    do
    {
        cout << "\nEnter your name (letters, numbers, spaces only): ";
        cin.getline(playerName, 50);

        if (!isValidName(playerName))
        {
            cout << "Invalid name! Please use only letters, numbers, and spaces.\n";
        }
    } while (!isValidName(playerName));
}

int chooseDifficulty ()
{
    int diff;
    cout << "\nSelect Difficulty:\n";
    cout << "1. Easy\n";
    cout << "2. Medium\n";
    cout << "3. Hard\n";
    cout << "Enter choice (1-3): ";

    while (!(cin >> diff) || diff < 1 || diff > 3)
    {
        clearInputBuffer ();
        cout << "Invalid input! Please enter 1, 2, or 3: ";
    }
    clearInputBuffer ();
    return diff;
}

void showHighScoresMenu ()
{
    clearScreen ();
    int diff;
    cout << "========================================\n";
    cout << "           HIGH SCORES MENU            \n";
    cout << "========================================\n";
    cout << "Select Difficulty:\n";
    cout << "1. Easy\n";
    cout << "2. Medium\n";
    cout << "3. Hard\n";
    cout << "Enter choice (1-3): ";

    while (!(cin >> diff) || diff < 1 || diff > 3)
    {
        clearInputBuffer ();
        cout << "Invalid input! Please enter 1, 2, or 3: ";
    }
    clearInputBuffer ();
    showHighScores(diff);
}

void showHighScores(int difficulty)
{
    string filename;
    string diffName;

    if (difficulty == 1)
    {
        filename = "easy.txt";
        diffName = "EASY";
    }
    else if (difficulty == 2)
    {
        filename = "medium.txt";
        diffName = "MEDIUM";
    }
    else
    {
        filename = "hard.txt";
        diffName = "HARD";
    }

    ifstream fin(filename.c_str());
    if (!fin)
    {
        cout << "\n--- No scores recorded yet for " << diffName << " difficulty ---\n";
        return;
    }

    cout << "\n========================================\n";
    cout << "      HIGH SCORES - " << diffName << " MODE\n";
    cout << "========================================\n";

    string names[100];
    int scores[100];
    int count = 0;

    while (fin >> names[count] >> scores[count] && count < 100)
    {
        count++;
    }
    fin.close();

    if (count == 0)
    {
        cout << "No scores yet!\n";
        cout << "========================================\n";
        return;
    }

    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (scores[j] < scores[j + 1])
            {
                int tempScore = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = tempScore;

                string tempName = names[j];
                names[j] = names[j + 1];
                names[j + 1] = tempName;
            }
        }
    }

    int display = count;
    if (display > 10) display = 10;

    for (int i = 0; i < display; i++)
    {
        cout << (i + 1) << ". " << names[i] << " - " << scores[i] << "\n";
    }

    cout << "========================================\n";
}
void showPlayerScores ()
{
    clearScreen ();
    char name[50];
    cout << "========================================\n";
    cout << "          PLAYER STATISTICS            \n";
    cout << "========================================\n";
    cout << "Enter Player Name: ";
    cin.getline(name, 50);

    if (!isValidName(name))
    {
        cout << "Invalid name format!\n";
        return;
    }

    PlayerStats *arr = nullptr;
    int count = 0;
    loadAllPlayers(arr, count);

    int idx = findPlayerIndex(arr, count, name);
    if (idx == -1)
    {
        cout << "\nPlayer '" << name << "' not found in records.\n";
    }
    else
    {
        showPlayerStats(arr[idx]);
    }

    delete[] arr;
}

bool fileExists(const char *filename)
{
    ifstream f(filename);
    return f.good ();
}

void saveHighScore(int difficulty, const char name[], int score)
{
    string filename;
    switch (difficulty)
    {
    case 1:
        filename = "easy.txt";
        break;
    case 2:
        filename = "medium.txt";
        break;
    case 3:
        filename = "hard.txt";
        break;
    default:
        filename = "easy.txt";
    }

    ofstream fout(filename.c_str (), ios::app);
    if (fout)
    {
        fout << name << " " << score << endl;
        fout.close ();
    }
    else
    {
        cerr << "Error: Could not save high score to file.\n";
    }
}

void savePlayerStats(const char name[], int difficulty, int score)
{
    PlayerStats *arr = nullptr;
    int count = 0;
    loadAllPlayers(arr, count);

    int idx = findPlayerIndex(arr, count, name);

    if (idx == -1)
    {
        expandPlayerArray(arr, count);
#ifdef _WIN32
        strcpy_s(arr[count - 1].name, 50, name);
#else
        strncpy(arr[count - 1].name, name, 49);
        arr[count - 1].name[49] = '\0';
#endif
        arr[count - 1].gamesPlayed = 1;
        updatePlayerRecord(arr[count - 1], difficulty, score);
    }
    else
    {
        arr[idx].gamesPlayed++;
        updatePlayerRecord(arr[idx], difficulty, score);
    }

    saveAllPlayers(arr, count);
    delete[] arr;
}

void loadAllPlayers(PlayerStats *&arr, int &count)
{
    ifstream fin("players.dat", ios::binary);
    if (!fin)
    {
        arr = nullptr;
        count = 0;
        return;
    }

    fin.seekg(0, ios::end);
    streampos fileSize = fin.tellg ();

    if (fileSize == 0)
    {
        arr = nullptr;
        count = 0;
        fin.close ();
        return;
    }

    int sz = static_cast<int>(fileSize / sizeof(PlayerStats));
    fin.seekg(0, ios::beg);

    arr = new PlayerStats[sz];
    fin.read(reinterpret_cast<char *>(arr), sizeof(PlayerStats) * sz);
    count = sz;
    fin.close ();
}

void saveAllPlayers(PlayerStats *arr, int count)
{
    if (count == 0 || arr == nullptr)
        return;

    ofstream fout("players.dat", ios::binary);
    if (fout)
    {
        fout.write(reinterpret_cast<const char *>(arr), sizeof(PlayerStats) * count);
        fout.close ();
    }
    else
    {
        cerr << "Error: Could not save player data.\n";
    }
}

int findPlayerIndex(PlayerStats *arr, int count, const char name[])
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(arr[i].name, name) == 0)
            return i;
    }
    return -1;
}

void updatePlayerRecord(PlayerStats &p, int difficulty, int score)
{
    p.totalScore += score;

    switch (difficulty)
    {
    case 1:
        p.easyPlayed++;
        if (score > p.bestEasy)
            p.bestEasy = score;
        break;
    case 2:
        p.mediumPlayed++;
        if (score > p.bestMedium)
            p.bestMedium = score;
        break;
    case 3:
        p.hardPlayed++;
        if (score > p.bestHard)
            p.bestHard = score;
        break;
    }
}

void expandPlayerArray(PlayerStats *&arr, int &count)
{
    if (count < 0)
    {
        count = 0;
        return;
    }

    const int newCapacity = count + 1;
    PlayerStats *newArr = new PlayerStats[newCapacity];

    if (arr != nullptr)
    {
        for (int i = 0; i < count; ++i)
        {
            if (i < newCapacity)
            {
                newArr[i] = arr[i];
            }
        }
        delete[] arr;
    }

    arr = newArr;
    count = newCapacity;
}

void showPlayerStats(const PlayerStats &p)
{
    cout << "\n========================================\n";
    cout << "Player Name: " << p.name << "\n";
    cout << "========================================\n";
    cout << "Total Games Played: " << p.gamesPlayed << "\n";
    cout << "Total Score: " << p.totalScore << "\n";
    cout << "\nGames by Difficulty:\n";
    cout << "  Easy:   " << p.easyPlayed << " games\n";
    cout << "  Medium: " << p.mediumPlayed << " games\n";
    cout << "  Hard:   " << p.hardPlayed << " games\n";
    cout << "\nBest Scores:\n";
    cout << "  Easy:   " << p.bestEasy << "\n";
    cout << "  Medium: " << p.bestMedium << "\n";
    cout << "  Hard:   " << p.bestHard << "\n";
    cout << "========================================\n";
}

// ====================== GAMEPLAY LOGIC ======================
void startNewGame ()
{
    char playerName[50];
    getPlayerName(playerName);
    int diff = chooseDifficulty ();
    startGame(diff, playerName);
}

void startGame(int difficulty, const char playerName[])
{
    gameLoop(difficulty, playerName);
}

void initializeGame(GameState &game, int difficulty)
{
    game.playerX = 50;
    game.playerY = groundLevel;
    game.isJumping = false;
    game.jumpVelocity = 0;
    game.score = 0;
    game.obstacleCount = 0;
    game.isRunning = true;

    for (int i = 0; i < maxObstacles; i++)
    {
        game.obstacles[i].active = false;
    }

    setDifficultyParams(game, difficulty);
}

void setDifficultyParams(GameState &game, int difficulty)
{
    switch (difficulty)
    {
    case 1:
        game.obstacleSpeed = easySpeed;
        game.spawnInterval = easySpawn;
        break;
    case 2:
        game.obstacleSpeed = mediumSpeed;
        game.spawnInterval = mediumSpawn;
        break;
    case 3:
        game.obstacleSpeed = hardSpeed;
        game.spawnInterval = hardSpawn;
        break;
    default:
        game.obstacleSpeed = mediumSpeed;
        game.spawnInterval = mediumSpawn;
    }
}

void updateDino(GameState &game)
{
    if (game.isJumping)
    {
        game.playerY += static_cast<int>(game.jumpVelocity);
        game.jumpVelocity += 0.8f;

        if (game.playerY >= groundLevel)
        {
            game.playerY = groundLevel;
            game.isJumping = false;
            game.jumpVelocity = 0;
        }
    }
}

void updateObstacles(GameState &game)
{
    for (int i = 0; i < maxObstacles; i++)
    {
        if (game.obstacles[i].active)
        {
            game.obstacles[i].x -= game.obstacleSpeed;

            if (game.obstacles[i].x < -obstacleWidth)
            {
                game.obstacles[i].active = false;
                game.obstacleCount--;
            }
        }
    }
}

bool checkCollision(const GameState &game)
{
    for (int i = 0; i < maxObstacles; i++)
    {
        if (game.obstacles[i].active)
        {
            if (game.playerX + dinoWidth > game.obstacles[i].x &&
                game.playerX < game.obstacles[i].x + obstacleWidth &&
                game.playerY + dinoHeight > game.obstacles[i].y)
            {
                return true;
            }
        }
    }
    return false;
}

void updateScore(GameState &game)
{
    game.score++;
}

void spawnObstacle(GameState &game)
{
    if (game.obstacleCount >= maxObstacles)
        return;

    for (int i = 0; i < maxObstacles; i++)
    {
        if (!game.obstacles[i].active)
        {
            game.obstacles[i].x = windowWidth;
            game.obstacles[i].y = groundLevel;
            game.obstacles[i].active = true;
            game.obstacleCount++;
            break;
        }
    }
}

void gameOverScreen(int score, const char playerName[], int difficulty)
{
    clearScreen ();
    cout << "\n========================================\n";
    cout << "             GAME OVER!                \n";
    cout << "========================================\n";
    cout << "Player: " << playerName << "\n";
    cout << "Final Score: " << score << "\n";

    string diffName;
    switch (difficulty)
    {
    case 1:
        diffName = "Easy";
        break;
    case 2:
        diffName = "Medium";
        break;
    case 3:
        diffName = "Hard";
        break;
    }
    cout << "Difficulty: " << diffName << "\n";
    cout << "========================================\n";

    saveHighScore(difficulty, playerName, score);
    savePlayerStats(playerName, difficulty, score);

    cout << "Score saved successfully!\n";
    pauseScreen ();
}

// ====================== GRAPHICS + SFML ======================

void renderGame(sf::RenderWindow &window, const GameState &game)
{
    static sf::Texture bgTexture;
    static sf::Texture dinoTexture;
    static bool loaded = false;

    if (!loaded)
    {
        if (!bgTexture.loadFromFile("background.png"))
        {
            std::cerr << "Failed to load background.png\n";
        }
        if (!dinoTexture.loadFromFile("dino.png"))
        {
            std::cerr << "Failed to load dino.png (will use rectangle instead)\n";
        }
        loaded = true;
    }

    sf::Sprite bgSprite(bgTexture);

    sf::FloatRect bounds = bgSprite.getGlobalBounds ();
    float xScale = static_cast<float>(window.getSize ().x) / bounds.size.x;
    float yScale = static_cast<float>(window.getSize ().y) / bounds.size.y;

    bgSprite.setScale(sf::Vector2f(xScale, yScale));

    window.draw(bgSprite);
    drawGround(window, bgSprite);

    if (dinoTexture.getNativeHandle ())
    {
        sf::Sprite dinoSprite(dinoTexture);
        dinoSprite.setPosition(sf::Vector2f(static_cast<float>(game.playerX),
                                            static_cast<float>(game.playerY)));

        float scaleX = static_cast<float>(dinoWidth) / dinoTexture.getSize ().x;
        float scaleY = static_cast<float>(dinoHeight) / dinoTexture.getSize ().y;
        dinoSprite.setScale(sf::Vector2f(scaleX, scaleY));

        window.draw(dinoSprite);
    }
    else
    {
        drawDino(window, game);
    }

    drawObstacles(window, game);
    drawScore(window, game.score);
}

void drawGround(sf::RenderWindow &window, const sf::Sprite &bgSprite)
{
    window.draw(bgSprite);
    sf::RectangleShape ground(sf::Vector2f(static_cast<float>(windowWidth), 20.f));
    ground.setFillColor(sf::Color(150, 75, 0));
    ground.setPosition(sf::Vector2f(0.f, static_cast<float>(groundLevel + dinoHeight)));
    window.draw(ground);
}

void drawDino(sf::RenderWindow &window, const GameState &game)
{
    sf::RectangleShape playerShape(sf::Vector2f(static_cast<float>(dinoWidth),
                                                static_cast<float>(dinoHeight)));
    playerShape.setFillColor(sf::Color::Green);
    playerShape.setPosition(sf::Vector2f(static_cast<float>(game.playerX),
                                         static_cast<float>(game.playerY)));
    window.draw(playerShape);
}

void drawObstacles(sf::RenderWindow &window, const GameState &game)
{
    sf::RectangleShape obsShape(sf::Vector2f(static_cast<float>(obstacleWidth),
                                             static_cast<float>(obstacleHeight)));
    obsShape.setFillColor(sf::Color::Red);

    for (int i = 0; i < maxObstacles; i++)
    {
        if (game.obstacles[i].active)
        {
            obsShape.setPosition(sf::Vector2f(static_cast<float>(game.obstacles[i].x),
                                              static_cast<float>(game.obstacles[i].y)));
            window.draw(obsShape);
        }
    }
}

void drawScore(sf::RenderWindow &window, int score)
{
    
}

void gameLoop(int difficulty, const char playerName[])
{
    GameState game;
    initializeGame(game, difficulty);

    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(static_cast<unsigned int>(windowWidth),
                                                       static_cast<unsigned int>(windowHeight))),
                            "Chrome Dino Game");
    window.setFramerateLimit(60);

    sf::SoundBuffer jumpBuffer, gameOverBuffer;
    sf::Sound *jumpSound = nullptr;
    sf::Sound *gameOverSound = nullptr;

    if (jumpBuffer.loadFromFile("jump.wav"))
    {
        jumpSound = new sf::Sound(jumpBuffer);
    }
    else
    {
        std::cerr << "Failed to load jump.wav\n";
    }

    if (gameOverBuffer.loadFromFile("gameover.wav"))
    {
        gameOverSound = new sf::Sound(gameOverBuffer);
    }
    else
    {
        std::cerr << "Failed to load gameover.wav\n";
    }

    sf::Clock clock;
    float spawnTimer = 0;

    while (window.isOpen () && game.isRunning)
    {
        while (auto event = window.pollEvent ())
        {
            if (event->is<sf::Event::Closed> ())
            {
                window.close ();
                game.isRunning = false;
            }

            if (const auto *keyPress = event->getIf<sf::Event::KeyPressed> ())
            {
                if (keyPress->code == sf::Keyboard::Key::Space && !game.isJumping)
                {
                    game.isJumping = true;
                    game.jumpVelocity = -15.0f;

                    if (jumpSound != nullptr)
                    {
                        jumpSound->play ();
                    }
                }
            }
        }

        float dt = clock.restart ().asSeconds ();
        spawnTimer += dt;

        updateDino(game);
        updateObstacles(game);

        if (spawnTimer > game.spawnInterval)
        {
            spawnObstacle(game);
            spawnTimer = 0;
        }

        if (checkCollision(game))
        {
            if (gameOverSound != nullptr)
            {
                gameOverSound->play ();
                while (gameOverSound->getStatus () == sf::SoundSource::Status::Playing)
                {
                    sf::sleep(sf::milliseconds(100));
                }
            }

            window.close ();
            gameOverScreen(game.score, playerName, difficulty);
            game.isRunning = false;
            break;
        }

        updateScore(game);

        window.clear(sf::Color::White);
        renderGame(window, game);
        window.display ();
    }

    delete jumpSound;
    delete gameOverSound;
}

void textBasedGameLoop(int difficulty, const char playerName[])
{
    GameState game;
    initializeGame(game, difficulty);

    cout << "\n========================================\n";
    cout << "        GAME STARTED (TEXT MODE)       \n";
    cout << "========================================\n";
    cout << "Player: " << playerName << "\n";

    string diffName;
    switch (difficulty)
    {
    case 1:
        diffName = "Easy";
        break;
    case 2:
        diffName = "Medium";
        break;
    case 3:
        diffName = "Hard";
        break;
    }
    cout << "Difficulty: " << diffName << "\n";
    cout << "Speed: " << game.obstacleSpeed << "\n";
    cout << "Spawn Interval: " << game.spawnInterval << "s\n";
    cout << "========================================\n";
    cout << "\nSimulating game...\n";

    float gameTime = 0;
    float spawnTimer = 0;
    const float timeStep = 0.1f;
    const float maxGameTime = 10.0f;

    while (gameTime < maxGameTime && game.isRunning)
    {
        gameTime += timeStep;
        spawnTimer += timeStep;

        if (static_cast<int>(gameTime) % 2 == 0 && !game.isJumping && gameTime > 0.5f)
        {
            game.isJumping = true;
            game.jumpVelocity = -15.0f;
            cout << "[" << static_cast<int>(gameTime) << "s] JUMP!\n";
        }

        updateDino(game);
        updateObstacles(game);

        if (spawnTimer > game.spawnInterval)
        {
            spawnObstacle(game);
            spawnTimer = 0;
            cout << "[" << static_cast<int>(gameTime) << "s] Obstacle spawned!\n";
        }

        if (checkCollision(game))
        {
            cout << "\n[" << static_cast<int>(gameTime) << "s] COLLISION DETECTED!\n";
            game.isRunning = false;
            break;
        }

        updateScore(game);

        if (static_cast<int>(gameTime * 10) % 20 == 0)
        {
            cout << "[" << static_cast<int>(gameTime) << "s] Score: " << game.score
                 << " | Obstacles: " << game.obstacleCount
                 << " | Dino Y: " << game.playerY << "\n";
        }
    }

    if (gameTime >= maxGameTime)
    {
        cout << "\nSimulation completed successfully!\n";
    }

    cout << "\n========================================\n";
    cout << "         SIMULATION ENDED              \n";
    cout << "========================================\n";
    cout << "Time Survived: " << static_cast<int>(gameTime) << " seconds\n";
    cout << "Final Score: " << game.score << "\n";
    cout << "Obstacles Dodged: " << game.obstacleCount << "\n";
    cout << "========================================\n";

    gameOverScreen(game.score, playerName, difficulty);
}