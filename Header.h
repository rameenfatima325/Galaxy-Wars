#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace sf;


// Base class for all game-related errors. Stores an error type and message, inherits from std::exception so it can be caught using standard C++ exception handling.
// std::exception: Standard C++ base class for errors, provides a virtual what() function that lets us return a readable message when an exception is thrown.
class GameException : public exception {
private:
    string message;
    string errorType;
public:
    GameException(const string& type, const string& msg)
        : errorType(type), message("[" + type + "] " + msg) {
    }
    // Replaces the original what() to return our custom error message, noexcept ensures this function never throws another exception itself.
    const char* what() const noexcept override {
        return message.c_str();
    }
    string getType() const { return errorType; }
};
//Specific exception classes for different error scenarios, each inherits from GameException and provides a specific error message.

// Thrown when a required file (save file/audio/etc.) cannot be opened or accessed, helps identify missing or corrupted files.
class FileLoadException : public GameException {
public:
    FileLoadException(const string& filename)
        : GameException("FILE_ERROR", "Failed to load file: " + filename) {
    }
};

// Thrown when a texture (image) fails to load, useful for debugging graphics-related issues, prevents gameplay from continuing with missing graphics.
class TextureLoadException : public GameException {
public:
    TextureLoadException(const string& texturePath)
        : GameException("TEXTURE_ERROR", "Failed to load texture: " + texturePath) {
    }
};

// Thrown when a font fails to load, important for UI text rendering, ensures game text is always properly displayed.
class FontLoadException : public GameException {
public:
    FontLoadException(const string& fontPath)
        : GameException("FONT_ERROR", "Failed to load font: " + fontPath) {
    }
};

// Thrown when an invalid operation is attempted, such as performing an action that is not allowed in the current game state.
class InvalidOperationException : public GameException {
public:
    InvalidOperationException(const string& operation)
        : GameException("INVALID_OPERATION", operation) {
    }
};

//Used when code attempts to access a null pointer unexpectedly, prevents crashes from uninitialized objects.
class NullPointerException : public GameException {
public:
    NullPointerException(const string& context)
        : GameException("NULL_POINTER", "Null pointer encountered in: " + context) {
    }
};


// Thrown when an index is out of the valid range, useful for array or list access, helps catch programming errors.
class IndexOutOfBoundsException : public GameException {
public:
    IndexOutOfBoundsException(int index, int maxSize)
        : GameException("INDEX_ERROR", "Index " + to_string(index) + " out of bounds (max: " + to_string(maxSize - 1) + ")") {
    }
};

// Thrown when saving or loading game data fails, critical for maintaining game progress, alerts to issues with file I/O operations and ensures player progress is safely managed.
class SaveLoadException : public GameException {
public:
    SaveLoadException(const string& operation)
        : GameException("SAVE_LOAD_ERROR", operation) {
    }
};

//Backgound music and sound effects
class SoundManager {
private:
    Music backgroundMusic;

    SoundBuffer bulletBuffer;
    SoundBuffer powerUpBuffer;
    SoundBuffer resourceBuffer;

    Sound bulletSound;
    Sound powerUpSound;
    Sound resourceSound;


    float musicVolume;
    float sfxVolume;
    bool musicEnabled;
    bool sfxEnabled;

public:

    SoundManager()
        : musicVolume(50.f),
        sfxVolume(70.f),
        musicEnabled(true),
        sfxEnabled(true),
        bulletSound(bulletBuffer),
        powerUpSound(powerUpBuffer),
        resourceSound(resourceBuffer)
    {
        if (!backgroundMusic.openFromFile("C:/proj_oop2/bg.mpeg")) {
            throw FileLoadException("C:/proj_oop2/bg.mpeg");
        }

        backgroundMusic.setLooping(true);
        backgroundMusic.setVolume(musicVolume);

        if (!bulletBuffer.loadFromFile("C:/proj_oop2/bullet.mpeg")) {
            throw FileLoadException("C:/proj_oop2/bullet.mpeg");
        }

        if (!powerUpBuffer.loadFromFile("C:/proj_oop2/pu.mpeg")) {
            throw FileLoadException("C:/proj_oop2/pu.mpeg");
        }

        if (!resourceBuffer.loadFromFile("C:/proj_oop2/pu.mpeg")) {
            throw FileLoadException("C:/proj_oop2/pu.mpeg");
        }

        bulletSound.setVolume(sfxVolume);
        powerUpSound.setVolume(sfxVolume);
        resourceSound.setVolume(sfxVolume);
    }

    void playMusic() {
        if (musicEnabled) {
            backgroundMusic.play();
        }
    }

    void stopMusic() {
        backgroundMusic.stop();
    }

    void pauseMusic() {
        backgroundMusic.pause();
    }

    void resumeMusic() {
        if (musicEnabled) {
            backgroundMusic.play();
        }
    }

    void playBulletSound() {
        if (sfxEnabled) {
            bulletSound.play();
        }
    }

    void playPowerUpSound() {
        if (sfxEnabled) {
            powerUpSound.play();
        }
    }

    void playResourceSound() {
        if (sfxEnabled) {
            resourceSound.play();
        }
    }

    void setMusicVolume(float volume) {
        musicVolume = volume;
        backgroundMusic.setVolume(musicVolume);
    }

    void setSfxVolume(float volume) {
        sfxVolume = volume;
        bulletSound.setVolume(sfxVolume);
        powerUpSound.setVolume(sfxVolume);
        resourceSound.setVolume(sfxVolume);

    }

    bool isMusicEnabled() const { return musicEnabled; }
    bool isSfxEnabled() const { return sfxEnabled; }
    float getMusicVolume() const { return musicVolume; }
    float getSfxVolume() const { return sfxVolume; }
};


const int RESOURCE_TYPE_RED = 0;
const int RESOURCE_TYPE_BLUE = 1;

class SlowMotionManager {
private:
    bool isActive;
    float timer;
    float duration;
    float speedMultiplier;
public:
    SlowMotionManager() : isActive(false), timer(0.f), duration(8.0f), speedMultiplier(0.3f) {}
    //deep copy and asignemnt operator:
    // These ensure that when SlowMotionManager objects are copied (e.g., during save/load or level transitions), all state values like timer and active status are transferred correctly. This preserves slow-motion effects across game states.
    SlowMotionManager(const SlowMotionManager& other)
        : isActive(other.isActive), timer(other.timer),
        duration(other.duration), speedMultiplier(other.speedMultiplier) {
    }
    SlowMotionManager& operator=(const SlowMotionManager& other) {
        if (this != &other) {
            isActive = other.isActive;
            timer = other.timer;
            duration = other.duration;
            speedMultiplier = other.speedMultiplier;
        }
        return *this;
    }

    void activate() {
        isActive = true;
        timer = duration;
    }

    void update(float deltaTime) {
        if (isActive) {
            timer -= deltaTime;
            if (timer <= 0.f) {
                isActive = false;
            }
        }
    }

    void reset() {
        isActive = false;
        timer = 0.f;
    }

    bool getIsActive() const { return isActive; }
    float getSpeedMultiplier() const { return isActive ? speedMultiplier : 1.0f; }
    float getRemainingTime() const { return timer; }
    void saveToFile(ofstream& file) const {
        if (!file.good()) {
            throw SaveLoadException("Unable to save slow motion data.");
        }
        file << isActive << " " << timer << " " << duration << " " << speedMultiplier << endl;
    }
    void loadFromFile(ifstream& file) {
        if (!file.good()) {
            throw SaveLoadException("Unable to load slow motion data.");
        }
        file >> isActive >> timer >> duration >> speedMultiplier;
    }
};

// global variable because slow-motion affects the entire game 
SlowMotionManager g_slowMotion;

struct Resource {
    int redCrystals;
    int blueCrystals;

    Resource() : redCrystals(0), blueCrystals(0) {}
    Resource(int red, int blue) : redCrystals(red), blueCrystals(blue) {}

    // Copy constructor
    Resource(const Resource& other) : redCrystals(other.redCrystals), blueCrystals(other.blueCrystals) {}

    // Assignment operator
    Resource& operator=(const Resource& other) {
        if (this != &other) {
            redCrystals = other.redCrystals;
            blueCrystals = other.blueCrystals;
        }
        return *this;
    }

    Resource operator+(const Resource& other) const {
        return Resource(redCrystals + other.redCrystals, blueCrystals + other.blueCrystals);
    }

    Resource& operator+=(const Resource& other) {
        redCrystals += other.redCrystals;
        blueCrystals += other.blueCrystals;
        return *this;
    }

    int getTotal() const {
        return redCrystals + blueCrystals;
    }

    friend ostream& operator<<(ostream& os, const Resource& r);
    friend istream& operator>>(istream& is, Resource& r);
};

ostream& operator<<(ostream& os, const Resource& r) {
    os << "Red: " << r.redCrystals << ", Blue: " << r.blueCrystals << " | Total: " << r.getTotal();
    return os;
}

istream& operator>>(istream& is, Resource& r) {
    is >> r.redCrystals >> r.blueCrystals;
    return is;
}

class ScoreManager;
class ResourceManager;

void processRedCrystalCollection(ResourceManager& rm, ScoreManager& sm);
void processBlueCrystalCollection(ResourceManager& rm, ScoreManager& sm);
void combineResources(ResourceManager& rm, const Resource& bonus, ScoreManager& sm);

//stores game state for saving/loading

struct SaveableState {
    float playerX, playerY;
    bool hasShield;
    float shieldTimer;
    bool hasDoubleFire;
    float fireTimer;
    int score;
    int health;
    int currentLevel;
    int enemiesDefeated;
    int redCrystals;
    int blueCrystals;
    int gameTimeSeconds;
    int currentWave;
    int enemiesPerWave;
    float enemyBaseSpeed;
    int enemiesSpawned;
    int enemiesKilled;
    bool slowMoActive;
    float slowMoTimer;
    int lastMilestone;

    bool asteroidActive[7];
    float asteroidX[7];
    float asteroidY[7];
    int asteroidHP[7];

    bool easyEnemyActive[4];
    float easyEnemyX[4];
    float easyEnemyY[4];
    int easyEnemyHP[4];

    bool mediumEnemyActive[6];
    float mediumEnemyX[6];
    float mediumEnemyY[6];
    int mediumEnemyHP[6];

    bool hardEnemyActive[8];
    float hardEnemyX[8];
    float hardEnemyY[8];
    int hardEnemyHP[8];

    bool bossActive;
    float bossX, bossY;
    int bossHP;
    bool bossVisible;
    float bossDisappearTimer;

    bool resourceActive[8];
    float resourceX[8];
    float resourceY[8];
    int resourceType[8];

    bool powerUpActive[4];
    float powerUpX[4];
    float powerUpY[4];
    int powerUpType[4];

    SaveableState() {
        playerX = 375.f;
        playerY = 285.f;
        hasShield = false;
        shieldTimer = 0.f;
        hasDoubleFire = false;
        fireTimer = 0.f;
        score = 0;
        health = 100;
        currentLevel = 1;
        enemiesDefeated = 0;
        redCrystals = 0;
        blueCrystals = 0;
        gameTimeSeconds = 0;
        currentWave = 1;
        enemiesPerWave = 3;
        enemyBaseSpeed = 2.0f;
        enemiesSpawned = 0;
        enemiesKilled = 0;
        slowMoActive = false;
        slowMoTimer = 0.f;
        lastMilestone = 0;

        for (int i = 0; i < 7; i++) {
            asteroidActive[i] = false;
            asteroidX[i] = 0.f;
            asteroidY[i] = 0.f;
            asteroidHP[i] = 1;
        }
        for (int i = 0; i < 2; i++) {
            easyEnemyActive[i] = false;
            easyEnemyX[i] = 0.f;
            easyEnemyY[i] = 0.f;
            easyEnemyHP[i] = 1;
        }
        for (int i = 0; i < 3; i++) {
            mediumEnemyActive[i] = false;
            mediumEnemyX[i] = 0.f;
            mediumEnemyY[i] = 0.f;
            mediumEnemyHP[i] = 1;
        }
        for (int i = 0; i < 4; i++) {
            hardEnemyActive[i] = false;
            hardEnemyX[i] = 0.f;
            hardEnemyY[i] = 0.f;
            hardEnemyHP[i] = 1;
        }
        bossActive = false;
        bossX = 0.f;
        bossY = 0.f;
        bossHP = 10;
        bossVisible = true;
        bossDisappearTimer = 0.f;

        for (int i = 0; i < 8; i++) {
            resourceActive[i] = false;
            resourceX[i] = 0.f;
            resourceY[i] = 0.f;
            resourceType[i] = 0;
        }
        for (int i = 0; i < 4; i++) {
            powerUpActive[i] = false;
            powerUpX[i] = 0.f;
            powerUpY[i] = 0.f;
            powerUpType[i] = 0;
        }
    }

    SaveableState(const SaveableState& other) {
        playerX = other.playerX;
        playerY = other.playerY;
        hasShield = other.hasShield;
        shieldTimer = other.shieldTimer;
        hasDoubleFire = other.hasDoubleFire;
        fireTimer = other.fireTimer;
        score = other.score;
        health = other.health;
        currentLevel = other.currentLevel;
        enemiesDefeated = other.enemiesDefeated;
        redCrystals = other.redCrystals;
        blueCrystals = other.blueCrystals;
        gameTimeSeconds = other.gameTimeSeconds;
        currentWave = other.currentWave;
        enemiesPerWave = other.enemiesPerWave;
        enemyBaseSpeed = other.enemyBaseSpeed;
        enemiesSpawned = other.enemiesSpawned;
        enemiesKilled = other.enemiesKilled;
        slowMoActive = other.slowMoActive;
        slowMoTimer = other.slowMoTimer;
        lastMilestone = other.lastMilestone;

        for (int i = 0; i < 7; i++) {
            asteroidActive[i] = other.asteroidActive[i];
            asteroidX[i] = other.asteroidX[i];
            asteroidY[i] = other.asteroidY[i];
            asteroidHP[i] = other.asteroidHP[i];
        }
        for (int i = 0; i < 2; i++) {
            easyEnemyActive[i] = other.easyEnemyActive[i];
            easyEnemyX[i] = other.easyEnemyX[i];
            easyEnemyY[i] = other.easyEnemyY[i];
            easyEnemyHP[i] = other.easyEnemyHP[i];
        }
        for (int i = 0; i < 3; i++) {
            mediumEnemyActive[i] = other.mediumEnemyActive[i];
            mediumEnemyX[i] = other.mediumEnemyX[i];
            mediumEnemyY[i] = other.mediumEnemyY[i];
            mediumEnemyHP[i] = other.mediumEnemyHP[i];
        }
        for (int i = 0; i < 4; i++) {
            hardEnemyActive[i] = other.hardEnemyActive[i];
            hardEnemyX[i] = other.hardEnemyX[i];
            hardEnemyY[i] = other.hardEnemyY[i];
            hardEnemyHP[i] = other.hardEnemyHP[i];
        }
        bossActive = other.bossActive;
        bossX = other.bossX;
        bossY = other.bossY;
        bossHP = other.bossHP;
        bossVisible = other.bossVisible;
        bossDisappearTimer = other.bossDisappearTimer;

        for (int i = 0; i < 8; i++) {
            resourceActive[i] = other.resourceActive[i];
            resourceX[i] = other.resourceX[i];
            resourceY[i] = other.resourceY[i];
            resourceType[i] = other.resourceType[i];
        }
        for (int i = 0; i < 4; i++) {
            powerUpActive[i] = other.powerUpActive[i];
            powerUpX[i] = other.powerUpX[i];
            powerUpY[i] = other.powerUpY[i];
            powerUpType[i] = other.powerUpType[i];
        }
    }

    SaveableState& operator=(const SaveableState& other) {
        if (this != &other) {
            SaveableState(other);
        }
        return *this;
    }
};

// Manages saving and loading game states to/from files
class GameSaveManager {
private:
    static const int MAX_SAVE_SLOTS = 3;
    string saveFilenames[MAX_SAVE_SLOTS];
    bool slotOccupied[MAX_SAVE_SLOTS];
    //stores a deep - copied "remembered" version of the last saved state
    SaveableState* savedStates[MAX_SAVE_SLOTS];

    // Helper function that deletes old data and writes a fresh deep copy of a save
    void deepCopyState(SaveableState*& dest, const SaveableState& src) {
        if (dest != nullptr) {
            //if memory already exists for this slot then this remove old data to prevent memory leaks
            delete dest;
        }
        dest = new SaveableState(src);
    }

public:
    GameSaveManager() {
        for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
            saveFilenames[i] = "savegame_slot" + to_string(i + 1) + ".txt";
            slotOccupied[i] = false;
            savedStates[i] = nullptr;
        }
        checkExistingSaves();
    }

    GameSaveManager(const GameSaveManager& other) {
        for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
            saveFilenames[i] = other.saveFilenames[i];
            slotOccupied[i] = other.slotOccupied[i];
            if (other.savedStates[i] != nullptr) {
                savedStates[i] = new SaveableState(*other.savedStates[i]);
            }
            else {
                savedStates[i] = nullptr;
            }
        }
    }

    GameSaveManager& operator=(const GameSaveManager& other) {
        if (this != &other) {
            for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
                delete savedStates[i];
            }
            for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
                saveFilenames[i] = other.saveFilenames[i];
                slotOccupied[i] = other.slotOccupied[i];
                if (other.savedStates[i] != nullptr) {
                    savedStates[i] = new SaveableState(*other.savedStates[i]);
                }
                else {
                    savedStates[i] = nullptr;
                }
            }
        }
        return *this;
    }

    ~GameSaveManager() {
        for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
            delete savedStates[i];
        }
    }

    void checkExistingSaves() {
        for (int i = 0; i < MAX_SAVE_SLOTS; i++) {
            ifstream file(saveFilenames[i]);
            slotOccupied[i] = file.good();
            file.close();
        }
    }

    // Writes full game state to slot text file
    bool saveGame(int slot, const SaveableState& state) {
        try {
            if (slot < 0 || slot >= MAX_SAVE_SLOTS) {
                throw IndexOutOfBoundsException(slot, MAX_SAVE_SLOTS);
            }
            ofstream file(saveFilenames[slot]);
            if (!file.is_open()) {
                throw SaveLoadException("Cannot open save file: " + saveFilenames[slot]);
            }

            file << "GALAXY_WARS_SAVE_V2" << endl;
            file << state.playerX << " " << state.playerY << endl;
            file << state.hasShield << " " << state.shieldTimer << endl;
            file << state.hasDoubleFire << " " << state.fireTimer << endl;
            file << state.score << " " << state.health << endl;
            file << state.currentLevel << " " << state.enemiesDefeated << endl;
            file << state.redCrystals << " " << state.blueCrystals << " " << state.gameTimeSeconds << endl;
            file << state.currentWave << " " << state.enemiesPerWave << endl;
            file << state.enemyBaseSpeed << " " << state.enemiesSpawned << " " << state.enemiesKilled << endl;
            file << state.slowMoActive << " " << state.slowMoTimer << endl;
            file << state.lastMilestone << endl;

            for (int i = 0; i < 7; i++) {
                file << state.asteroidActive[i] << " " << state.asteroidX[i] << " "
                    << state.asteroidY[i] << " " << state.asteroidHP[i] << endl;
            }
            for (int i = 0; i < 4; i++) {
                file << state.easyEnemyActive[i] << " " << state.easyEnemyX[i] << " "
                    << state.easyEnemyY[i] << " " << state.easyEnemyHP[i] << endl;
            }
            for (int i = 0; i < 6; i++) {
                file << state.mediumEnemyActive[i] << " " << state.mediumEnemyX[i] << " "
                    << state.mediumEnemyY[i] << " " << state.mediumEnemyHP[i] << endl;
            }
            for (int i = 0; i < 8; i++) {
                file << state.hardEnemyActive[i] << " " << state.hardEnemyX[i] << " "
                    << state.hardEnemyY[i] << " " << state.hardEnemyHP[i] << endl;
            }
            file << state.bossActive << " " << state.bossX << " " << state.bossY << " "
                << state.bossHP << " " << state.bossVisible << " " << state.bossDisappearTimer << endl;

            for (int i = 0; i < 8; i++) {
                file << state.resourceActive[i] << " " << state.resourceX[i] << " "
                    << state.resourceY[i] << " " << state.resourceType[i] << endl;
            }
            for (int i = 0; i < 4; i++) {
                file << state.powerUpActive[i] << " " << state.powerUpX[i] << " "
                    << state.powerUpY[i] << " " << state.powerUpType[i] << endl;
            }

            file.close();
            slotOccupied[slot] = true;
            deepCopyState(savedStates[slot], state);

            cout << "[SAVE] Game saved to slot " << (slot + 1) << endl;
            return true;
        }
        catch (const GameException& e) {
            cerr << e.what() << endl;
            return false;
        }
    }

    //Reads game data from file back into state
    bool loadGame(int slot, SaveableState& state) {
        try {
            if (slot < 0 || slot >= MAX_SAVE_SLOTS) {
                throw IndexOutOfBoundsException(slot, MAX_SAVE_SLOTS);
            }
            ifstream file(saveFilenames[slot]);
            if (!file.is_open()) {
                throw SaveLoadException("Cannot open save file: " + saveFilenames[slot]);
            }

            string header;
            getline(file, header);
            if (header != "GALAXY_WARS_SAVE_V2" && header != "GALAXY_WARS_SAVE_V1") {
                throw SaveLoadException("Invalid save file format");
            }

            file >> state.playerX >> state.playerY;
            file >> state.hasShield >> state.shieldTimer;
            file >> state.hasDoubleFire >> state.fireTimer;
            file >> state.score >> state.health;
            file >> state.currentLevel >> state.enemiesDefeated;
            file >> state.redCrystals >> state.blueCrystals >> state.gameTimeSeconds;
            file >> state.currentWave >> state.enemiesPerWave;
            file >> state.enemyBaseSpeed >> state.enemiesSpawned >> state.enemiesKilled;
            file >> state.slowMoActive >> state.slowMoTimer;
            file >> state.lastMilestone;

            for (int i = 0; i < 7; i++) {
                file >> state.asteroidActive[i] >> state.asteroidX[i]
                    >> state.asteroidY[i] >> state.asteroidHP[i];
            }
            for (int i = 0; i < 4; i++) {
                file >> state.easyEnemyActive[i] >> state.easyEnemyX[i]
                    >> state.easyEnemyY[i] >> state.easyEnemyHP[i];
            }
            for (int i = 0; i < 6; i++) {
                file >> state.mediumEnemyActive[i] >> state.mediumEnemyX[i]
                    >> state.mediumEnemyY[i] >> state.mediumEnemyHP[i];
            }
            for (int i = 0; i < 8; i++) {
                file >> state.hardEnemyActive[i] >> state.hardEnemyX[i]
                    >> state.hardEnemyY[i] >> state.hardEnemyHP[i];
            }
            file >> state.bossActive >> state.bossX >> state.bossY
                >> state.bossHP >> state.bossVisible >> state.bossDisappearTimer;

            for (int i = 0; i < 8; i++) {
                file >> state.resourceActive[i] >> state.resourceX[i]
                    >> state.resourceY[i] >> state.resourceType[i];
            }
            for (int i = 0; i < 4; i++) {
                file >> state.powerUpActive[i] >> state.powerUpX[i]
                    >> state.powerUpY[i] >> state.powerUpType[i];
            }

            file.close();
            // Update stored memory so UI remains accurate
            deepCopyState(savedStates[slot], state);

            cout << "[LOAD] Game loaded from slot " << (slot + 1) << endl;
            return true;
        }
        catch (const GameException& e) {
            cerr << e.what() << endl;
            return false;
        }
        catch (const exception& e) {
            cerr << "[ERROR] Load failed: " << e.what() << endl;
            return false;
        }
    }

    bool isSlotOccupied(int slot) const {
        if (slot >= 0 && slot < MAX_SAVE_SLOTS) {
            return slotOccupied[slot];
        }
        return false;
    }

    int getMaxSlots() const { return MAX_SAVE_SLOTS; }

    bool deleteSave(int slot) {
        try {
            if (slot < 0 || slot >= MAX_SAVE_SLOTS) {
                throw IndexOutOfBoundsException(slot, MAX_SAVE_SLOTS);
            }
            if (remove(saveFilenames[slot].c_str()) == 0) {
                slotOccupied[slot] = false;
                delete savedStates[slot];
                savedStates[slot] = nullptr;
                cout << "[SAVE] Deleted save from slot " << (slot + 1) << endl;
                return true;
            }
            return false;
        }
        catch (const GameException& e) {
            cerr << e.what() << endl;
            return false;
        }
    }
};

//highscore manager
class HighScoresManager {
public:
    struct ScoreEntry {
        string playerName;
        int score;
        int timeTaken;

        ScoreEntry() : playerName(""), score(0), timeTaken(0) {}
        ScoreEntry(const string& name, int s, int t) : playerName(name), score(s), timeTaken(t) {}
        ScoreEntry(const ScoreEntry& other)
            : playerName(other.playerName), score(other.score), timeTaken(other.timeTaken) {
        }
        ScoreEntry& operator=(const ScoreEntry& other) {
            if (this != &other) {
                playerName = other.playerName;
                score = other.score;
                timeTaken = other.timeTaken;
            }
            return *this;
        }
    };

private:
    static const int MAX_SCORES = 50;
    ScoreEntry scores[MAX_SCORES];
    int scoreCount;
    string textFilename;

    //sorting scores in descending order
    void swapScores(int i, int j) {
        if (i < 0 || i >= scoreCount || j < 0 || j >= scoreCount) {
            throw IndexOutOfBoundsException(max(i, j), scoreCount);
        }
        ScoreEntry temp(scores[i]);
        scores[i] = scores[j];
        scores[j] = temp;
    }
    void sortScoresDescending() {
        for (int i = 0; i < scoreCount - 1; i++) {
            for (int j = 0; j < scoreCount - i - 1; j++) {
                if (scores[j].score < scores[j + 1].score) {
                    swapScores(j, j + 1);
                }
            }
        }
    }

public:
    HighScoresManager() : scoreCount(0), textFilename("highscores.txt") {
        loadScoresFromText();
    }

    HighScoresManager(const HighScoresManager& other)
        : scoreCount(other.scoreCount), textFilename(other.textFilename)
    {
        for (int i = 0; i < scoreCount; i++) {
            scores[i] = ScoreEntry(other.scores[i]);
        }
    }

    HighScoresManager& operator=(const HighScoresManager& other) {
        if (this != &other) {
            scoreCount = other.scoreCount;
            textFilename = other.textFilename;
            for (int i = 0; i < scoreCount; i++) {
                scores[i] = ScoreEntry(other.scores[i]);
            }
        }
        return *this;
    }

    // Adding a new score entry
    void addScore(const string& playerName, int newScore, int timeTaken) {
        try {
            if (playerName.empty()) {
                throw InvalidOperationException("Cannot add score with empty player name");
            }
            if (newScore < 0) {
                throw InvalidOperationException("Score cannot be negative: " + to_string(newScore));
            }

            if (scoreCount < MAX_SCORES) {
                scores[scoreCount] = ScoreEntry(playerName, newScore, timeTaken);
                scoreCount++;
            }
            // If full but score is high enough: replace worst score
            else if (newScore > scores[MAX_SCORES - 1].score) {
                scores[MAX_SCORES - 1] = ScoreEntry(playerName, newScore, timeTaken);
            }
            else {
                return;
            }

            sortScoresDescending();
            saveScoresToText();
        }
        catch (const GameException& e) {
            cerr << "[GAME_ERROR] " << e.what() << "\n";
        }
    }

    ScoreEntry getScore(int index) const {
        try {
            if (index < 0 || index >= scoreCount) {
                throw IndexOutOfBoundsException(index, scoreCount);
            }
            return ScoreEntry(scores[index]);
        }
        catch (const IndexOutOfBoundsException& e) {
            cerr << e.what() << "\n";
            return ScoreEntry("N/A", 0, 0);
        }
    }

    int getScoreCount() const { return scoreCount; }

    void loadScoresFromText() {
        try {
            ifstream file(textFilename);
            if (!file.is_open()) {
                cout << "No highscores file found. Starting fresh." << endl;
                return;
            }

            scoreCount = 0;
            string line;
            if (getline(file, line)) {}

            while (getline(file, line) && scoreCount < MAX_SCORES) {
                if (line.empty()) continue;

                size_t firstComma = line.find(',');
                if (firstComma == string::npos) {
                    throw InvalidOperationException("Invalid score format - missing first comma");
                }

                size_t secondComma = line.find(',', firstComma + 1);
                if (secondComma == string::npos) {
                    throw InvalidOperationException("Invalid score format - missing second comma");
                }

                string playerName = line.substr(0, firstComma);
                string scoreStr = line.substr(firstComma + 1, secondComma - firstComma - 1);
                string timeStr = line.substr(secondComma + 1);

                if (!playerName.empty() && !scoreStr.empty() && !timeStr.empty()) {
                    try {
                        int score = stoi(scoreStr);
                        int timeTaken = stoi(timeStr);
                        scores[scoreCount] = ScoreEntry(playerName, score, timeTaken);
                        scoreCount++;
                    }
                    catch (const invalid_argument& e) {
                        throw InvalidOperationException("Invalid number format in score file: " + string(e.what()));
                    }
                    catch (const out_of_range& e) {
                        throw InvalidOperationException("Number out of range in score file: " + string(e.what()));
                    }
                }
            }
            file.close();
            cout << "[SUCCESS] Loaded " << scoreCount << " scores from " << textFilename << endl;
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
        }
        catch (const exception& e) {
            cerr << "[ERROR] Exception while loading scores: " << e.what() << endl;
        }
    }

    // Saving scores back to text file
    void saveScoresToText() {
        try {
            ofstream file(textFilename);
            if (!file.is_open()) {
                throw FileLoadException(textFilename + " (write mode)");
            }

            file << "PlayerName,Score,TimeTaken" << endl;
            for (int i = 0; i < scoreCount; i++) {
                file << scores[i].playerName << ","
                    << scores[i].score << ","
                    << scores[i].timeTaken << endl;
            }
            file.close();
            cout << "[SUCCESS] Saved " << scoreCount << " scores to " << textFilename << endl;
        }
        catch (const GameException& e) {
            cerr << e.what() << endl;
        }
        catch (const exception& e) {
            cerr << "[ERROR] Exception while saving scores: " << e.what() << endl;
        }
    }

    // Formats time in seconds to MM:SS
    static string formatTime(int seconds) {
        if (seconds < 0) {
            throw InvalidOperationException("Time cannot be negative: " + to_string(seconds));
        }
        int minutes = seconds / 60;
        int secs = seconds % 60;
        string result;
        if (minutes < 10) result += "0";
        result += to_string(minutes) + ":";
        if (secs < 10) result += "0";
        result += to_string(secs);
        return result;
    }
};

void loadFont(Font& font, const string& path) {
    if (!font.openFromFile(path)) {
        throw FontLoadException(path);
    }
}

void loadTexture(Texture& texture, const string& path) {
    if (!texture.loadFromFile(path)) {
        throw TextureLoadException(path);
    }
}


// This class represents a clickable button (shape + text)
class Button {
private:
    RectangleShape buttonShape; // SFML rectangle representing the button's body
    Text buttonText;
    bool isHovered;
public:
    Button(const Vector2f& position, const Vector2f& size,
        const string& text, const Font& font)
        : buttonText(font), isHovered(false)
    {
        try {
            if (size.x <= 0 || size.y <= 0) {
                throw InvalidOperationException("Button size must be positive");
            }

            buttonShape.setSize(size);
            buttonShape.setPosition(position);
            buttonShape.setFillColor(Color::Blue);

            buttonText.setString(text);
            buttonText.setCharacterSize(25);
            buttonText.setFillColor(Color::White);

            FloatRect textRect = buttonText.getLocalBounds();
            buttonText.setOrigin({ textRect.position.x + textRect.size.x / 2.0f,
                textRect.position.y + textRect.size.y / 2.0f });
            buttonText.setPosition({
                position.x + size.x / 2.0f,
                position.y + size.y / 2.0f
                });
        }
        catch (const GameException& e) {
            cerr << e.what() << endl;
        }
    }

    // Update button hover state based on mouse position
    void update(const Vector2f& mousePos) {
        if (buttonShape.getGlobalBounds().contains(mousePos)) {
            buttonShape.setFillColor(Color::Cyan);
            isHovered = true;
        }
        else {
            buttonShape.setFillColor(Color::Blue);
            isHovered = false;
        }
    }
    void setTextSize(unsigned int size) {
        buttonText.setCharacterSize(size);

        // Re-center text after resizing
        FloatRect textRect = buttonText.getLocalBounds();
        buttonText.setOrigin({
            textRect.position.x + textRect.size.x / 2.f,
            textRect.position.y + textRect.size.y / 2.f
            });
    }


    // Detect if player clicked this button 
    bool isClicked(const Vector2f& mousePos) const {
        return isHovered;
    }

    void draw(RenderWindow& window) {
        window.draw(buttonShape);
        window.draw(buttonText);
    }
};

//This class creates and manages the main menu screen
class MainMenu {
private:
    Button* startButton;
    Button* loadGameButton;
    Button* highScoresButton;
    Button* exitButton;
    Font font;
    Text titleText;
public:
    MainMenu() : titleText(font), startButton(nullptr), loadGameButton(nullptr),
        highScoresButton(nullptr), exitButton(nullptr)
    {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << " - Using default font behavior\n";
        }

        titleText.setString("GALAXY WARS");
        titleText.setCharacterSize(150);
        titleText.setFillColor(Color::Yellow);
        titleText.setPosition({ 320.f, 200.f });

        startButton = new Button({ 200.f, 400.f }, { 220.f, 60.f }, "START", font);
        loadGameButton = new Button({ 450.f, 400.f }, { 220.f, 60.f }, "LOAD GAME", font);
        highScoresButton = new Button({ 700.f, 400.f }, { 220.f, 60.f }, "SCORES", font);
        exitButton = new Button({ 950.f, 400.f }, { 220.f, 60.f }, "EXIT", font);

        if (!startButton || !loadGameButton || !highScoresButton || !exitButton) {
            throw NullPointerException("MainMenu button creation");
        }
    }

    ~MainMenu() {
        delete startButton;
        delete loadGameButton;
        delete highScoresButton;
        delete exitButton;
    }

    // Update button states depending on mouse hover location
    void update(const Vector2f& mousePos) {
        try {
            if (!startButton || !loadGameButton || !highScoresButton || !exitButton) {
                throw NullPointerException("MainMenu::update");
            }
            startButton->update(mousePos);
            loadGameButton->update(mousePos);
            highScoresButton->update(mousePos);
            exitButton->update(mousePos);
        }
        catch (const NullPointerException& e) {
            cerr << e.what() << "\n";
        }
    }

    // Handle click and return a number representing which button was pressed
    int handleClick(const Vector2f& mousePos) {
        try {
            if (!startButton || !loadGameButton || !highScoresButton || !exitButton) {
                throw NullPointerException("MainMenu::handleClick");
            }
            if (startButton->isClicked(mousePos)) return 1; // Go to Game
            if (loadGameButton->isClicked(mousePos)) return 4; // Go to Save/Load
            if (highScoresButton->isClicked(mousePos)) return 2; // Go to HighScores
            if (exitButton->isClicked(mousePos)) return 3; // Exit game
        }
        catch (const NullPointerException& e) {
            cerr << e.what() << "\n";
        }
        return 0; // No button clicked
    }

    void draw(RenderWindow& window) {
        window.draw(titleText);
        try {
            if (!startButton || !loadGameButton || !highScoresButton || !exitButton) {
                throw NullPointerException("MainMenu::draw");
            }
            startButton->draw(window);
            loadGameButton->draw(window);
            highScoresButton->draw(window);
            exitButton->draw(window);
        }
        catch (const NullPointerException& e) {
            cerr << e.what() << "\n";
        }
    }
};

//save/load screen class
class SaveLoadScreen {
private:
    Font font;
    Text titleText;
    Text* slotTexts[3];
    Button* slotButtons[3];
    Button* backButton;
    Button* deleteButtons[3];
    bool isSaveMode;
    GameSaveManager* saveManager;
    Text statusText;
    float statusTimer;

public:
    SaveLoadScreen(GameSaveManager* manager, bool saveMode = false)
        : titleText(font), statusText(font), isSaveMode(saveMode),
        saveManager(manager), statusTimer(0.f), backButton(nullptr)
    {
        for (int i = 0; i < 3; ++i) {
            slotTexts[i] = nullptr;
            slotButtons[i] = nullptr;
            deleteButtons[i] = nullptr;
        }

        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }

        titleText.setString(isSaveMode ? "SAVE GAME" : "LOAD GAME");
        titleText.setCharacterSize(100);
        titleText.setFillColor(Color::Yellow);
        titleText.setPosition({ 500.f, 60.f });

        statusText.setCharacterSize(30);
        statusText.setFillColor(Color::Green);
        statusText.setPosition({ 630.f, 600.f });

        for (int i = 0; i < 3; i++) {
      

            string buttonText = "SLOT " + to_string(i + 1);
            if (saveManager && saveManager->isSlotOccupied(i)) {
                buttonText += " (USED)";
            }
            else {
                buttonText += " (EMPTY)";
            }

            slotButtons[i] = new Button({ 450.f, 200.f + i * 150.f }, { 350.f, 60.f }, buttonText, font);
            deleteButtons[i] = new Button({ 820.f, 200.f + i * 150.f }, { 120.f, 60.f }, "DELETE", font);
        }

        backButton = new Button({ 550.f, 670.f }, { 300.f, 60.f }, "BACK", font);
    }

    SaveLoadScreen(const SaveLoadScreen& other)
        : titleText(font), statusText(font), isSaveMode(other.isSaveMode),
        saveManager(other.saveManager), statusTimer(other.statusTimer), backButton(nullptr)
    {
        for (int i = 0; i < 3; ++i) {
            slotTexts[i] = nullptr;
            slotButtons[i] = nullptr;
            deleteButtons[i] = nullptr;
        }

        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }

        titleText.setString(other.titleText.getString());
        titleText.setCharacterSize(other.titleText.getCharacterSize());
        titleText.setFillColor(other.titleText.getFillColor());
        titleText.setPosition(other.titleText.getPosition());

        statusText.setString(other.statusText.getString());
        statusText.setCharacterSize(other.statusText.getCharacterSize());
        statusText.setFillColor(other.statusText.getFillColor());
        statusText.setPosition(other.statusText.getPosition());

        for (int i = 0; i < 3; i++) {
            if (other.slotTexts[i]) {
                slotTexts[i] = new Text(font);
                slotTexts[i]->setString(other.slotTexts[i]->getString());
                slotTexts[i]->setCharacterSize(other.slotTexts[i]->getCharacterSize());
                slotTexts[i]->setFillColor(other.slotTexts[i]->getFillColor());
                slotTexts[i]->setPosition(other.slotTexts[i]->getPosition());
            }

            if (other.slotButtons[i]) {
                string buttonText = "SLOT " + to_string(i + 1);
                if (saveManager && saveManager->isSlotOccupied(i)) {
                    buttonText += " (USED)";
                }
                else {
                    buttonText += " (EMPTY)";
                }
                slotButtons[i] = new Button({ 500.f, 200.f + i * 150.f }, { 300.f, 60.f }, buttonText, font);
            }

            if (other.deleteButtons[i]) {
                deleteButtons[i] = new Button({ 820.f, 200.f + i * 150.f }, { 120.f, 60.f }, "DELETE", font);
            }
        }

        if (other.backButton) {
            backButton = new Button({ 550.f, 720.f }, { 300.f, 60.f }, "BACK", font);
        }
    }

    ~SaveLoadScreen() {
        for (int i = 0; i < 3; i++) {
            delete slotTexts[i];
            delete slotButtons[i];
            delete deleteButtons[i];
        }
        delete backButton;
    }

    void setMode(bool saveMode) {
        isSaveMode = saveMode;
        titleText.setString(isSaveMode ? "SAVE GAME" : "LOAD GAME");
    }

    void refreshSlots() {
        if (saveManager) {
            saveManager->checkExistingSaves();
        }
    }

    void update(const Vector2f& mousePos, float deltaTime) {
        for (int i = 0; i < 3; i++) {
            if (slotButtons[i]) slotButtons[i]->update(mousePos);
            if (deleteButtons[i]) deleteButtons[i]->update(mousePos);
        }
        if (backButton) backButton->update(mousePos);

        if (statusTimer > 0) {
            statusTimer -= deltaTime;
            if (statusTimer <= 0) {
                statusText.setString("");
            }
        }
    }

    int handleClick(const Vector2f& mousePos) {
        for (int i = 0; i < 3; i++) {
            if (slotButtons[i] && slotButtons[i]->isClicked(mousePos)) {
                return i;
            }
            if (deleteButtons[i] && deleteButtons[i]->isClicked(mousePos)) {
                return 10 + i;
            }
        }
        if (backButton && backButton->isClicked(mousePos)) {
            return -1;
        }
        return -2;
    }

    void showStatus(const string& message, bool success) {
        statusText.setString(message);
        statusText.setFillColor(success ? Color::Green : Color::Red);
        statusTimer = 3.0f;
    }

    bool getIsSaveMode() const { return isSaveMode; }

    void draw(RenderWindow& window) {
        window.draw(titleText);

        for (int i = 0; i < 3; i++) {
            string slotInfo = "Slot " + to_string(i + 1) + ": ";
            if (saveManager && saveManager->isSlotOccupied(i)) {
                slotInfo += "Save Data Available";
            }
            else {
                slotInfo += "Empty";
            }

            if (slotTexts[i]) {
                slotTexts[i]->setString(slotInfo);
                window.draw(*slotTexts[i]);
            }

            if (slotButtons[i]) slotButtons[i]->draw(window);
            if (deleteButtons[i] && saveManager && saveManager->isSlotOccupied(i)) {
                deleteButtons[i]->draw(window);
            }
        }

        if (backButton) backButton->draw(window);
        window.draw(statusText);
    }
};

//Highscores screen class
class HighScoresScreen {
private:
    Font font;
    Text titleText;
    Text scoresText;
    Button* backButton;
    HighScoresManager* scoresManager;
    float scrollOffset = 0.f;
    const float scrollSpeed = 30.f;


public:
    HighScoresScreen(HighScoresManager* manager)
        : titleText(font), scoresText(font), backButton(nullptr), scoresManager(manager)
    {
        try {
            if (!manager) {
                throw NullPointerException("HighScoresScreen constructor - manager is null");
            }
            loadFont(font, "C://proj_oop2//PressStart2P-Regular.ttf");
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
        }

        titleText.setString("TOP 50 HIGH SCORES");
        titleText.setCharacterSize(40);
        titleText.setFillColor(Color::Yellow);
        titleText.setPosition({ 360.f, 25.f });

        scoresText.setCharacterSize(15);
        scoresText.setFillColor(Color::White);
        scoresText.setPosition({ 325.f, 100.f + scrollOffset });

        backButton = new Button({ 550.f, 720.f }, { 300.f, 60.f }, "BACK TO MENU", font);
        backButton->setTextSize(10);  


    }

    ~HighScoresScreen() { delete backButton; }

    void update(const Vector2f& mousePos) {
        if (backButton) backButton->update(mousePos);
    }

    bool handleClick(const Vector2f& mousePos) {
        return backButton && backButton->isClicked(mousePos);
    }

    void draw(RenderWindow& window) {
        window.draw(titleText);

        try {
            if (!scoresManager) {
                throw NullPointerException("HighScoresScreen::draw - scoresManager");
            }
            string scoresString;

            scoresString = " RANK  PLAYER NAME                   SCORE    TIME\n";
            scoresString += " =================================================\n";

            for (int i = 0; i < scoresManager->getScoreCount(); i++) {
                HighScoresManager::ScoreEntry entry = scoresManager->getScore(i);

                scoresString += " " + to_string(i + 1);
                if (i + 1 < 10) scoresString += "     ";
                else if (i + 1 < 100) scoresString += "    ";
                else scoresString += "   ";

                scoresString += entry.playerName;
                for (int j = 0; j < 20 - (int)entry.playerName.length(); j++)
                    scoresString += " ";

                string scoreStr = to_string(entry.score);
                for (int j = 0; j < 15 - (int)scoreStr.length(); j++)
                    scoresString += " ";
                scoresString += scoreStr;

                scoresString += "   " + HighScoresManager::formatTime(entry.timeTaken);

                scoresString += "\n";
            }

            if (scoresManager->getScoreCount() == 0)
                scoresString = "No high scores yet!";

            scoresText.setString(scoresString);
            scoresText.setPosition({ 325.f, 100.f + scrollOffset });
            window.draw(scoresText);
        }
        catch (const GameException& e) {
            cerr << e.what() << endl;
        }

        if (backButton) backButton->draw(window);
    }
    void onScroll(float delta) {
        scrollOffset += delta * scrollSpeed;

        if (scrollOffset > 0) scrollOffset = 0;

        float maxScroll = -(scoresManager->getScoreCount() * 11.f) + 400.f;
        if (scrollOffset < maxScroll) scrollOffset = maxScroll;
    }

};

//Pause menu class
class PauseMenu {
private:
    Button* resumeButton;
    Button* saveButton;
    Button* restartButton;
    Button* quitButton;
    Font font;
    Text pauseTitle;
    RectangleShape background;

public:
    PauseMenu() : pauseTitle(font), resumeButton(nullptr), saveButton(nullptr),
        restartButton(nullptr), quitButton(nullptr)
    {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }

        pauseTitle.setString("PAUSED");
        pauseTitle.setCharacterSize(100);
        pauseTitle.setFillColor(Color::Yellow);
        pauseTitle.setPosition({ 570.f, 80.f });

        resumeButton = new Button({ 500.f, 200.f }, { 400.f, 60.f }, "RESUME GAME (P)", font);
        saveButton = new Button({ 500.f, 300.f }, { 400.f, 60.f }, "SAVE GAME", font);
        restartButton = new Button({ 500.f, 400.f }, { 400.f, 60.f }, "RESTART GAME", font);
        quitButton = new Button({ 500.f, 500.f }, { 400.f, 60.f }, "QUIT TO MENU", font);

        background.setSize({ 1400.f, 800.f });
        background.setFillColor(Color(0, 0, 0, 150));
    }

    ~PauseMenu() {
        delete resumeButton;
        delete saveButton;
        delete restartButton;
        delete quitButton;
    }

    void update(const Vector2f& mousePos) {
        if (resumeButton) resumeButton->update(mousePos);
        if (saveButton) saveButton->update(mousePos);
        if (restartButton) restartButton->update(mousePos);
        if (quitButton) quitButton->update(mousePos);
    }

    int handleClick(const Vector2f& mousePos) {
        if (resumeButton && resumeButton->isClicked(mousePos)) return 1;
        if (saveButton && saveButton->isClicked(mousePos)) return 2;
        if (restartButton && restartButton->isClicked(mousePos)) return 3;
        if (quitButton && quitButton->isClicked(mousePos)) return 4;
        return 0;
    }

    void draw(RenderWindow& window) {
        window.draw(background);
        window.draw(pauseTitle);
        if (resumeButton) resumeButton->draw(window);
        if (saveButton) saveButton->draw(window);
        if (restartButton) restartButton->draw(window);
        if (quitButton) quitButton->draw(window);
    }
};

//Game over screen class
class EnhancedGameOverScreen {
private:
    Font font;
    Text gameOverText;
    Text scoreText;
    Text messageText;
    Text playerNamePrompt;
    Button* saveScoreButton;
    Button* menuButton;
    RectangleShape background;
    bool scoreSaved;
    string playerInput;
    bool isInputting;
    int gameTime;

public:
    EnhancedGameOverScreen() : gameOverText(font), scoreText(font), messageText(font),
        playerNamePrompt(font), saveScoreButton(nullptr), menuButton(nullptr),
        scoreSaved(false), isInputting(true), gameTime(0)
    {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << endl;
        }

        gameOverText.setCharacterSize(100);
        gameOverText.setPosition({ 500.f, 60.f });

        scoreText.setCharacterSize(50);
        scoreText.setFillColor(Color::Yellow);
        scoreText.setPosition({ 570.f, 200.f });

        messageText.setCharacterSize(30);
        messageText.setFillColor(Color::White);
        messageText.setPosition({ 590.f, 320.f });

        playerNamePrompt.setCharacterSize(30);
        playerNamePrompt.setFillColor(Color::Cyan);
        playerNamePrompt.setPosition({ 550.f, 400.f });

        saveScoreButton = new Button({ 400.f, 550.f }, { 250.f, 60.f }, "SAVE SCORE", font);
        menuButton = new Button({ 750.f, 550.f }, { 250.f, 60.f }, "MAIN MENU", font);

        background.setSize({ 1400.f, 800.f });
        background.setFillColor(Color(0, 0, 0, 200));
    }

    ~EnhancedGameOverScreen() {
        delete saveScoreButton;
        delete menuButton;
    }

    void displayWin(int finalScore, int level, int timeTaken) {
        try {
            if (finalScore < 0) {
                throw InvalidOperationException("Final score cannot be negative");
            }
            if (level < 1 || level > 4) {
                throw InvalidOperationException("Invalid level number: " + to_string(level));
            }

            gameOverText.setString("YOU DID IT!!");
            gameOverText.setFillColor(Color::Green);
            scoreText.setString("Final Score: " + to_string(finalScore));
            messageText.setString("Level: " + to_string(level) + " - Victory!");
            gameTime = timeTaken;
            scoreSaved = false;
            isInputting = true;
            playerInput = "";
        }
        catch (const GameException& e) {
            cerr << e.what() << endl;
        }
    }

    void displayLose(int finalScore, int level, int timeTaken) {
        try {
            if (finalScore < 0) {
                throw InvalidOperationException("Final score cannot be negative");
            }

            gameOverText.setString("GAME OVER!");
            gameOverText.setFillColor(Color::Red);
            scoreText.setString("Final Score: " + to_string(finalScore));
            messageText.setString("Level: " + to_string(level) + " - Try Again!");
            gameTime = timeTaken;
            scoreSaved = false;
            isInputting = true;
            playerInput = "";
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
        }
    }

    void handleTextInput(char character) {
        if (!isInputting || scoreSaved) return;

        if (character == '\b' && playerInput.length() > 0) {
            playerInput.pop_back();
        }
        else if (character == '\r') {
            if (playerInput.length() > 0) {
                isInputting = false;
            }
        }
        else if (playerInput.length() < 20 && character >= 32 && character <= 126) {
            playerInput += character;
        }
    }

    void update(const Vector2f& mousePos) {
        if (saveScoreButton) saveScoreButton->update(mousePos);
        if (menuButton) menuButton->update(mousePos);
    }

    int handleClick(const Vector2f& mousePos) {
        if (saveScoreButton && saveScoreButton->isClicked(mousePos) && !scoreSaved && !isInputting) {
            scoreSaved = true;
            return 1;
        }
        if (menuButton && menuButton->isClicked(mousePos)) return 2;
        return 0;
    }

    string getPlayerName() const { return playerInput.empty() ? "Player" : playerInput; }
    int getGameTime() const { return gameTime; }

    void draw(RenderWindow& window) {
        window.draw(background);
        window.draw(gameOverText);
        window.draw(scoreText);
        window.draw(messageText);

        if (isInputting && !scoreSaved) {
            playerNamePrompt.setString("Enter Name: " + playerInput + "|");
            window.draw(playerNamePrompt);
        }
        else if (!scoreSaved) {
            if (saveScoreButton) saveScoreButton->draw(window);
        }
        else {
            Text savedText(font);
            savedText.setString("SCORE SAVED!");
            savedText.setCharacterSize(25);
            savedText.setFillColor(Color::Green);
            savedText.setPosition({ 550.f, 560.f });
            window.draw(savedText);
        }

        if (menuButton) menuButton->draw(window);
    }
};

//This class prevents multiple enemies/resources from spawning too close together
class SpawnPositionTracker {
private:
    static const int MAX_TRACKED_POSITIONS = 50;
    float trackedPositionsX[MAX_TRACKED_POSITIONS];
    int positionCount;

public:
    SpawnPositionTracker() : positionCount(0) { clearPositions(); }

    SpawnPositionTracker(const SpawnPositionTracker& other) : positionCount(other.positionCount) {
        for (int i = 0; i < MAX_TRACKED_POSITIONS; i++) {
            trackedPositionsX[i] = other.trackedPositionsX[i];
        }
    }

    SpawnPositionTracker& operator=(const SpawnPositionTracker& other) {
        if (this != &other) {
            positionCount = other.positionCount;
            for (int i = 0; i < MAX_TRACKED_POSITIONS; i++) {
                trackedPositionsX[i] = other.trackedPositionsX[i];
            }
        }
        return *this;
    }


    void addPosition(float x) {
        try {
            if (positionCount >= MAX_TRACKED_POSITIONS) {
                throw IndexOutOfBoundsException(positionCount, MAX_TRACKED_POSITIONS);
            }
            trackedPositionsX[positionCount++] = x;
        }
        catch (const IndexOutOfBoundsException& e) {
            cerr << e.what() << " - Position not added\n";
        }
    }

    // Check if new spawn X is too close to existing ones
    bool isPositionOccupied(float x, float tolerance = 150.0f) {
        for (int i = 0; i < positionCount; i++) {
            if (trackedPositionsX[i] >= 0 && x >= trackedPositionsX[i] - tolerance && x <= trackedPositionsX[i] + tolerance)
                return true;
        }
        return false;
    }

    // Clear all tracked positions at level end
    void clearPositions() {
        positionCount = 0;
        for (int i = 0; i < MAX_TRACKED_POSITIONS; i++) trackedPositionsX[i] = -1;
    }
};

//Asteroid class
class Asteroid {
private:
    Texture texture;
    Sprite* sprite;
    float speed;
    float baseSpeed;
    bool active;
    int hitPoints, maxHitPoints;

public:
    Asteroid(const string& texturePath, float asteroidSpeed)
        : speed(asteroidSpeed), baseSpeed(asteroidSpeed), active(false), hitPoints(1), maxHitPoints(1), sprite(nullptr)
    {
        try {
            loadTexture(texture, texturePath);
            sprite = new Sprite(texture);
            if (!sprite) {
                throw NullPointerException("Asteroid sprite creation");
            }
            sprite->setScale({ 0.15f, 0.15f });
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
            sprite = new Sprite(texture);
        }
    }

    Asteroid(const Asteroid& other)
        : speed(other.speed), baseSpeed(other.baseSpeed), active(other.active),
        hitPoints(other.hitPoints), maxHitPoints(other.maxHitPoints), sprite(nullptr)
    {
        texture = other.texture;
        sprite = new Sprite(texture);
        if (sprite && other.sprite) {
            sprite->setPosition(other.sprite->getPosition());
            sprite->setScale(other.sprite->getScale());
        }
    }

    Asteroid& operator=(const Asteroid& other) {
        if (this != &other) {
            speed = other.speed;
            baseSpeed = other.baseSpeed;
            active = other.active;
            hitPoints = other.hitPoints;
            maxHitPoints = other.maxHitPoints;
            texture = other.texture;
            delete sprite;
            sprite = new Sprite(texture);
            if (sprite && other.sprite) {
                sprite->setPosition(other.sprite->getPosition());
                sprite->setScale(other.sprite->getScale());
            }
        }
        return *this;
    }

    ~Asteroid() { delete sprite; }

    void spawn(float windowWidth) {
        try {
            if (!sprite) {
                throw NullPointerException("Asteroid::spawn - sprite is null");
            }
            if (windowWidth <= 100) {
                throw InvalidOperationException("Window width too small for spawning");
            }
            float randomX = rand() % (int)(windowWidth - 100);
            sprite->setPosition({ randomX, -50.f });
            hitPoints = maxHitPoints;
            active = true;
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
        }
    }

    void setHitPoints(int points) {
        if (points > 0) {
            maxHitPoints = points;
            hitPoints = points;
        }
    }

    int getHitPoints() const { return hitPoints; }

    void setTexture(const string& texturePath) {
        try {
            loadTexture(texture, texturePath);
            if (sprite) sprite->setTexture(texture);
        }
        catch (const TextureLoadException& e) {
            cerr << e.what() << "\n";
        }
    }

    void update() {
        if (active && sprite) {
            float effectiveSpeed = baseSpeed * g_slowMotion.getSpeedMultiplier();
            sprite->move({ 0.f, effectiveSpeed });
            if (sprite->getPosition().y > 850) active = false;
        }
    }

    void draw(RenderWindow& window) {
        if (active && sprite) window.draw(*sprite);
    }

    bool isActive() const { return active; }
    void setActive(bool a) { active = a; }
    FloatRect getBounds() const {
        if (sprite) return sprite->getGlobalBounds();
        return FloatRect();
    }

    bool takeDamage() {
        hitPoints--;
        if (hitPoints <= 0) { active = false; return true; }
        return false;
    }

    void destroy() { active = false; }
    Vector2f getPosition() const {
        if (sprite) return sprite->getPosition();
        return Vector2f(0, 0);
    }

    void setPosition(float x, float y) {
        if (sprite) sprite->setPosition({ x, y });
    }
};


//EnemyShip is a generic enemy spaceship, all specific enemy types (EasyEnemy, MediumEnemy, HardEnemy, BossEnemy) will inherit from this and reuse common logic like
class EnemyShip {
protected:
    Texture texture;
    Sprite* sprite;
    float speedX, speedY;
    float baseSpeedX, baseSpeedY;
    bool active;
    int hitPoints, maxHitPoints;
    float patternOffset;

public:
    EnemyShip(const string& texturePath, float enemySpeedY, float enemySpeedX)
        : speedX(enemySpeedX), speedY(enemySpeedY),
        baseSpeedX(enemySpeedX), baseSpeedY(enemySpeedY),
        active(false), hitPoints(1), maxHitPoints(1), patternOffset(0), sprite(nullptr)
    {
        try {
            loadTexture(texture, texturePath);
            sprite = new Sprite(texture);
            if (!sprite) {
                throw NullPointerException("EnemyShip sprite creation");
            }
            sprite->setScale({ 0.15f, 0.15f });
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
            sprite = new Sprite(texture);
        }
    }

    EnemyShip(const EnemyShip& other)
        : speedX(other.speedX), speedY(other.speedY),
        baseSpeedX(other.baseSpeedX), baseSpeedY(other.baseSpeedY),
        active(other.active), hitPoints(other.hitPoints),
        maxHitPoints(other.maxHitPoints), patternOffset(other.patternOffset), sprite(nullptr)
    {
        texture = other.texture;
        sprite = new Sprite(texture);
        if (sprite && other.sprite) {
            sprite->setPosition(other.sprite->getPosition());
            sprite->setScale(other.sprite->getScale());
        }
    }

    EnemyShip& operator=(const EnemyShip& other) {
        if (this != &other) {
            speedX = other.speedX;
            speedY = other.speedY;
            baseSpeedX = other.baseSpeedX;
            baseSpeedY = other.baseSpeedY;
            active = other.active;
            hitPoints = other.hitPoints;
            maxHitPoints = other.maxHitPoints;
            patternOffset = other.patternOffset;
            texture = other.texture;
            delete sprite;
            sprite = new Sprite(texture);
            if (sprite && other.sprite) {
                sprite->setPosition(other.sprite->getPosition());
                sprite->setScale(other.sprite->getScale());
            }
        }
        return *this;
    }

    virtual ~EnemyShip() { delete sprite; }

    virtual void spawn(float windowWidth, SpawnPositionTracker* tracker = nullptr) = 0;
    virtual void update() = 0;

    void checkBoundaries(float windowWidth = 1400.0f) {
        if (!sprite) return;
        Vector2f pos = sprite->getPosition();
        FloatRect bounds = sprite->getGlobalBounds();

        if (pos.x < 0) {
            sprite->setPosition({ 0.f, pos.y });
            speedX = fabs(baseSpeedX);
        }
        else if (pos.x > windowWidth - bounds.size.x) {
            sprite->setPosition({ windowWidth - bounds.size.x, pos.y });
            speedX = -fabs(baseSpeedX);
        }
        if (pos.y > 850.0f) active = false;
    }

    void draw(RenderWindow& window) {
        if (active && sprite) window.draw(*sprite);
    }

    bool isActive() const { return active; }
    void setActive(bool a) { active = a; }
    FloatRect getBounds() const {
        if (sprite) return sprite->getGlobalBounds();
        return FloatRect();
    }

    virtual bool takeDamage() {
        hitPoints--;
        if (hitPoints <= 0) { active = false; return true; }
        return false;
    }

    void destroy() { active = false; }

    void setHitPoints(int hp) {
        if (hp > 0) {
            maxHitPoints = hp;
            hitPoints = hp;
        }
    }

    int getHitPoints() const { return hitPoints; }

    void setTexture(const string& path) {
        try {
            loadTexture(texture, path);
            if (sprite) sprite->setTexture(texture);
        }
        catch (const TextureLoadException& e) {
            cerr << e.what() << "\n";
        }
    }

    Vector2f getPosition() const {
        if (sprite) return sprite->getPosition();
        return Vector2f(0, 0);
    }

    void setPosition(float x, float y) {
        if (sprite) sprite->setPosition({ x, y });
    }
};

void EnemyShip::spawn(float windowWidth, SpawnPositionTracker* tracker) {
    try {
        if (!sprite) {
            throw NullPointerException("EnemyShip::spawn - sprite is null");
        }

        float randomX;
        int attempts = 0;
        do {
            randomX = rand() % (int)(windowWidth - 100);
            attempts++;
        } while (tracker && tracker->isPositionOccupied(randomX) && attempts < 15);

        float randomY = -(50 + rand() % 150);
        sprite->setPosition({ randomX, randomY });

        if (tracker) tracker->addPosition(randomX);

        speedX = (rand() % 2 == 0) ? fabs(baseSpeedX) : -fabs(baseSpeedX);
        hitPoints = maxHitPoints;
        patternOffset = 0;
        active = true;
    }
    catch (const GameException& e) {
        cerr << e.what() << "\n";
    }
}

void EnemyShip::update() {
    if (!active || !sprite) return;

    float mult = g_slowMotion.getSpeedMultiplier();
    sprite->move({ speedX * mult, baseSpeedY * mult });
    checkBoundaries();
}

//easy enemy derived from enemy ship
class EasyEnemy : public EnemyShip {
public:
    EasyEnemy(const string& texturePath) : EnemyShip(texturePath, 0.10f, 0.03f) {}
    EasyEnemy(const EasyEnemy& other) : EnemyShip(other) {}

    void update() override { EnemyShip::update(); }
    void spawn(float windowWidth, SpawnPositionTracker* tracker = nullptr) override {
        EnemyShip::spawn(windowWidth, tracker);
    }
};

//medium enemy derived from enemy ship
class MediumEnemy : public EnemyShip {
public:
    MediumEnemy(const string& texturePath) : EnemyShip(texturePath, 0.20f, 0.08f) {}
    MediumEnemy(const MediumEnemy& other) : EnemyShip(other) {}

    void update() override { EnemyShip::update(); }
    void spawn(float windowWidth, SpawnPositionTracker* tracker = nullptr) override {
        EnemyShip::spawn(windowWidth, tracker);
    }
};

//hard enemy derived from enemy ship with circular movement pattern
class HardEnemy : public EnemyShip {
protected:
    float originX, originY;

public:
    HardEnemy(const string& texturePath) : EnemyShip(texturePath, 0.0f, 0.0f), originX(0), originY(0) {}
    HardEnemy(const HardEnemy& other) : EnemyShip(other), originX(other.originX), originY(other.originY) {}

    void spawn(float windowWidth, SpawnPositionTracker* tracker = nullptr) override {
        EnemyShip::spawn(windowWidth, tracker);
        if (sprite) {
            originX = sprite->getPosition().x;
            originY = sprite->getPosition().y;
        }
        patternOffset = (rand() % 360) * 0.0017453f;
    }

    void update() override {
        if (!active || !sprite) return;

        float mult = g_slowMotion.getSpeedMultiplier();
        patternOffset += 0.005f * mult;

        float radius = 80.0f;
        float descentSpeed = 0.02f * mult;
        originY += descentSpeed;

        float targetX = originX + cos(patternOffset) * radius;
        float targetY = originY + sin(patternOffset) * radius;

        sprite->setPosition({ targetX, targetY });
        checkBoundaries();

        if (originY > 850.f) active = false;
    }
};

//boss enemy derived from hard enemy with complex movement and visibility mechanics
class BossEnemy : public HardEnemy {
private:
    float centerX;
    float centerY;
    float zigzagAmplitude;
    float zigzagFrequency;
    float zigzagOffset;
    float descentSpeed;
    float horizontalSpeed;
    int horizontalDirection;

    int initialHP;
    float baseScaleX;
    float baseScaleY;

public:
    BossEnemy(const string& texturePath)
        : HardEnemy(texturePath),
        centerX(0.f), centerY(0.f),
        zigzagAmplitude(100.f), zigzagFrequency(0.001f), zigzagOffset(0.f),
        descentSpeed(0.05f), horizontalSpeed(0.45f), horizontalDirection(1),
        initialHP(10), baseScaleX(0.5f), baseScaleY(0.5f)
    {
        if (sprite) sprite->setScale({ baseScaleX, baseScaleY });
        setHitPoints(initialHP);
    }

    BossEnemy(const BossEnemy& other)
        : HardEnemy(other),
        centerX(other.centerX), centerY(other.centerY),
        zigzagAmplitude(other.zigzagAmplitude), zigzagFrequency(other.zigzagFrequency),
        zigzagOffset(other.zigzagOffset), descentSpeed(other.descentSpeed),
        horizontalSpeed(other.horizontalSpeed), horizontalDirection(other.horizontalDirection),
        initialHP(other.initialHP), baseScaleX(other.baseScaleX), baseScaleY(other.baseScaleY)
    {
    }

    int getMaxHitPointsForBoss() const { return initialHP; }

    void spawn(float windowWidth, SpawnPositionTracker* tracker = nullptr) override {
        try {
            if (!sprite) {
                throw NullPointerException("BossEnemy::spawn - sprite is null");
            }

            float spawnX;
            static int spawnIndex = 0;
            if (spawnIndex % 3 == 0) spawnX = 350.f;
            else if (spawnIndex % 3 == 1) spawnX = 700.f;
            else spawnX = 1050.f;
            spawnIndex++;

            float spawnY = 100.f;

            sprite->setPosition({ spawnX, spawnY });
            centerX = spawnX;
            centerY = spawnY;
            zigzagOffset = 0.f;
            horizontalDirection = (rand() % 2 == 0) ? 1 : -1;

            baseScaleX = 0.5f;
            baseScaleY = 0.5f;
            sprite->setScale({ baseScaleX, baseScaleY });

            setHitPoints(initialHP);
            active = true;
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
        }
    }

    void update() override {
        if (!active || !sprite) return;

        float mult = g_slowMotion.getSpeedMultiplier();

        centerY += descentSpeed * mult;

        zigzagOffset += zigzagFrequency * mult;

        float t = fmod(zigzagOffset, 2.0f);
        if (t > 1.0f) t = 2.0f - t;

        float zigzagY = (t * 2.0f - 1.0f) * zigzagAmplitude;

        centerX += horizontalSpeed * horizontalDirection * mult;

        if (centerX < 100.f) {
            centerX = 100.f;
            horizontalDirection = 1;
        }
        else if (centerX > 1300.f) {
            centerX = 1300.f;
            horizontalDirection = -1;
        }

        float finalX = centerX;
        float finalY = centerY + zigzagY;

        if (finalY > 780.f) {
            centerY = 100.f;
        }

        sprite->setPosition({ finalX, finalY });
    }


    void draw(RenderWindow& window) {
        if (active && sprite) {
            window.draw(*sprite);
        }
    }

    bool takeDamage() override {
        hitPoints--;

        if (sprite) {
            float shrinkFactor = 0.88f;
            Vector2f currentScale = sprite->getScale();
            sprite->setScale({ currentScale.x * shrinkFactor, currentScale.y * shrinkFactor });
        }

        if (hitPoints <= 0) {
            active = false;
            return true;
        }
        return false;
    }

    FloatRect getBounds() const {
        if (!sprite) return FloatRect();
        return sprite->getGlobalBounds();
    }
};

//level manager class
class LevelManager {
private:
    int currentLevel;
    int scoreThreshold1, scoreThreshold2, scoreThreshold3, scoreThreshold4;
    int asteroidCount[4];
    float enemySpeedMultiplier[4];
    Font font;
    Text* levelText;
    float transitionTimer;
    bool showTransition;
    int enemyLimit[4];
    int asteroidHitPoints[4];
    int enemyHitPoints[4];
    Text* countdownText;

public:
    LevelManager() : currentLevel(1),
        scoreThreshold1(50),
        scoreThreshold2(100),
        scoreThreshold3(200),
        scoreThreshold4(300),
        transitionTimer(0),
        showTransition(false),
        levelText(nullptr),
        countdownText(nullptr)
    {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }

        levelText = new Text(font);
        levelText->setCharacterSize(100);
        levelText->setFillColor(Color::Yellow);

        countdownText = new Text(font);
        countdownText->setCharacterSize(50);

        asteroidCount[0] = 3; enemySpeedMultiplier[0] = 1.0f; enemyLimit[0] = 2; asteroidHitPoints[0] = 1; enemyHitPoints[0] = 1;
        asteroidCount[1] = 5; enemySpeedMultiplier[1] = 1.5f; enemyLimit[1] = 3; asteroidHitPoints[1] = 2; enemyHitPoints[1] = 2;
        asteroidCount[2] = 7; enemySpeedMultiplier[2] = 2.0f; enemyLimit[2] = 4; asteroidHitPoints[2] = 3; enemyHitPoints[2] = 3;
        asteroidCount[3] = 0; enemySpeedMultiplier[3] = 1.0f; enemyLimit[3] = 3; asteroidHitPoints[3] = 0; enemyHitPoints[3] = 5;
    }

    LevelManager(const LevelManager& other)
        : currentLevel(other.currentLevel),
        scoreThreshold1(other.scoreThreshold1), scoreThreshold2(other.scoreThreshold2),
        scoreThreshold3(other.scoreThreshold3), scoreThreshold4(other.scoreThreshold4),
        transitionTimer(other.transitionTimer), showTransition(other.showTransition),
        levelText(nullptr), countdownText(nullptr)
    {
        for (int i = 0; i < 4; i++) {
            asteroidCount[i] = other.asteroidCount[i];
            enemySpeedMultiplier[i] = other.enemySpeedMultiplier[i];
            enemyLimit[i] = other.enemyLimit[i];
            asteroidHitPoints[i] = other.asteroidHitPoints[i];
            enemyHitPoints[i] = other.enemyHitPoints[i];
        }

        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }
    }

    ~LevelManager() {
        delete levelText;
        delete countdownText;
    }

    void reset() {
        currentLevel = 1;
        transitionTimer = 5;
        showTransition = true;
    }

    void setLevel(int level) {
        if (level >= 1 && level <= 4) {
            currentLevel = level;
        }
    }

    int checkLevelUp(int score) {
        if (showTransition) return -1;

        int previousLevel = currentLevel;
        if (score >= scoreThreshold3 && currentLevel < 4) { currentLevel = 4; }
        else if (score >= scoreThreshold2 && currentLevel < 3) { currentLevel = 3; }
        else if (score >= scoreThreshold1 && currentLevel < 2) { currentLevel = 2; }

        if (previousLevel != currentLevel) {
            showTransition = true;
            transitionTimer = 5.0f;
            return currentLevel;
        }
        return -1;
    }

    void update(float deltaTime) {
        if (showTransition) {
            transitionTimer -= deltaTime;
            if (transitionTimer <= 0) {
                showTransition = false;
                transitionTimer = 0;
            }
        }
    }

    void drawTransition(RenderWindow& window) {
        if (showTransition && levelText && countdownText) {

            RectangleShape bg({ 1400.f, 800.f });
            bg.setFillColor(Color(0, 0, 0, 220));
            window.draw(bg);

            levelText->setString("LEVEL " + to_string(currentLevel));

            FloatRect titleBounds = levelText->getLocalBounds();
            levelText->setOrigin({ titleBounds.size.x / 2.f, titleBounds.size.y / 2.f });
            levelText->setPosition({ 700.f, 160.f }); 
            window.draw(*levelText);

            Text desc(font);
            desc.setCharacterSize(32);
            desc.setFillColor(Color::White);

            string descriptionText;

            if (currentLevel == 1) {
                descriptionText =
                    "EASY - Steady enemies incoming!\n"
                    "Score Required: 50\n"
                    "Enemy HP: 1\n"
                    "Asteroid HP: 1\n"
                    "Press (P) to Pause";
            }
            else if (currentLevel == 2) {
                descriptionText =
                    "MEDIUM - Faster enemies and more asteroids!\n"
                    "Score Required: 100\n"
                    "Enemy HP: 2\n"
                    "Asteroid HP: 2\n"
                    "Press (P) to Pause";
            }
            else if (currentLevel == 3) {
                descriptionText =
                    "HARD - Aggressive enemies with pattern movement!\n"
                    "Score Required: 200\n"
                    "Enemy HP: 3\n"
                    "Asteroid HP: 3\n"
                    "Press (P) to Pause";
            }
            else if (currentLevel == 4) {
                descriptionText =
                    "BOSS LEVEL - Only enemies. Face the Boss!\n"
                    "Score Required: 300\n"
                    "Boss Enemy HP: 10\n"
                    "Press (P) to Pause";
            }

            desc.setString(descriptionText);
            FloatRect descBounds = desc.getLocalBounds();
            desc.setOrigin({ descBounds.size.x / 2.f, descBounds.size.y / 2.f });
            desc.setPosition({ 700.f, 340.f });
            window.draw(desc);

            int secondsLeft = static_cast<int>(transitionTimer) + 1;
            countdownText->setString("Starting in " + to_string(secondsLeft) + "...");
            countdownText->setCharacterSize(42);
            countdownText->setFillColor(Color::Cyan);

            FloatRect timerBounds = countdownText->getLocalBounds();
            countdownText->setOrigin({ timerBounds.size.x / 2.f, timerBounds.size.y / 2.f });
            countdownText->setPosition({ 700.f, 520.f });
            window.draw(*countdownText);

            float barWidth = 450.f;
            float progress = (5.0f - transitionTimer) / 5.0f;
            progress = max(0.f, min(progress, 1.f));

            RectangleShape progressBg({ barWidth, 20.f });
            progressBg.setPosition({ 700.f - barWidth / 2.f, 580.f });
            progressBg.setFillColor(Color(100, 100, 100));
            window.draw(progressBg);

            RectangleShape progressBar({ barWidth * progress, 20.f });
            progressBar.setFillColor(Color::Green);
            progressBar.setPosition({ 700.f - barWidth / 2.f, 580.f });
            window.draw(progressBar);
        }
    }


    int getCurrentLevel() const { return currentLevel; }
    int getAsteroidCount() const {
        try {
            if (currentLevel < 1 || currentLevel > 4) {
                throw IndexOutOfBoundsException(currentLevel - 1, 4);
            }
            return asteroidCount[currentLevel - 1];
        }
        catch (const IndexOutOfBoundsException& e) {
            cerr << e.what() << "\n";
            return 0;
        }
    }

    int getEnemyLimit() const {
        try {
            if (currentLevel < 1 || currentLevel > 4) {
                throw IndexOutOfBoundsException(currentLevel - 1, 4);
            }
            return enemyLimit[currentLevel - 1];
        }
        catch (const IndexOutOfBoundsException& e) {
            cerr << e.what() << "\n";
            return 0;
        }
    }

    bool isTransitioning() const { return showTransition; }

    int getAsteroidHitPoints() const {
        if (currentLevel >= 1 && currentLevel <= 4)
            return asteroidHitPoints[currentLevel - 1];
        return 1;
    }

    int getEnemyHitPoints() const {
        if (currentLevel >= 1 && currentLevel <= 4)
            return enemyHitPoints[currentLevel - 1];
        return 1;
    }
};

// Score/Health/Other Managers

class ScoreManager {
private:
    int score;
    Font font;
    Text* scoreText;

public:
    ScoreManager() : score(0), scoreText(nullptr) {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }
        scoreText = new Text(font);
        scoreText->setCharacterSize(45);
        scoreText->setFillColor(Color::White);
        scoreText->setPosition({ 5.f, 2.f });
    }

    ScoreManager(const ScoreManager& other) : score(other.score), scoreText(nullptr) {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }
        scoreText = new Text(font);
        scoreText->setCharacterSize(45);
        scoreText->setFillColor(Color::White);
        scoreText->setPosition({ 5.f, 2.f });
    }

    ~ScoreManager() { delete scoreText; }

    void reset() { score = 0; }
    void setScore(int s) { score = s; }
    void addAsteroidScore() { score += 10; }
    void addEnemyScore() { score += 50; }
    void addResourceScore(int value = 15) { score += value; }
    void addScore(int value) {
        if (value >= 0) score += value;
    }
    int getScore() const { return score; }

    void draw(RenderWindow& window) {
        if (scoreText) {
            scoreText->setString("Score: " + to_string(score));
            window.draw(*scoreText);
        }
    }
};


class HealthManager {
private:
    int health, maxHealth;
    Font font;
    Text* healthText;
    RectangleShape healthBar, healthBarFill;

public:
    HealthManager() : health(100), maxHealth(100), healthText(nullptr) {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }
        healthText = new Text(font);
        healthText->setCharacterSize(45);
        healthText->setFillColor(Color::White);
        healthText->setPosition({ 1100.f, 2.f });

        healthBar.setSize({ 250.f, 30.f });
        healthBar.setPosition({ 1100.f, 50.f });
        healthBar.setFillColor(Color::Red);

        healthBarFill.setSize({ 250.f, 30.f });
        healthBarFill.setPosition({ 1100.f, 50.f });
        healthBarFill.setFillColor(Color::Green);
    }

    HealthManager(const HealthManager& other)
        : health(other.health), maxHealth(other.maxHealth), healthText(nullptr)
    {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }
        healthText = new Text(font);
        healthText->setCharacterSize(45);
        healthText->setFillColor(Color::White);
        healthText->setPosition({ 1100.f, 2.f });

        healthBar.setSize({ 250.f, 30.f });
        healthBar.setPosition({ 1100.f, 50.f });
        healthBar.setFillColor(Color::Red);

        healthBarFill.setSize({ 250.f, 30.f });
        healthBarFill.setPosition({ 1100.f, 50.f });
        healthBarFill.setFillColor(Color::Green);
    }

    ~HealthManager() { delete healthText; }

    void reset() { health = maxHealth; }
    void setHealth(int h) { health = h; if (health > maxHealth) health = maxHealth; if (health < 0) health = 0; }
    int getHealth() const { return health; }
    void damageFromAsteroid() { health -= 5; if (health < 0) health = 0; }
    void damageFromEnemy() { health -= 10; if (health < 0) health = 0; }
    void instantRepair() { health = maxHealth; }
    bool isAlive() const { return health > 0; }

    void draw(RenderWindow& window) {
        if (healthText) {
            healthText->setString("Health: " + to_string(health) + "/" + to_string(maxHealth));
            healthBarFill.setSize({ (static_cast<float>(health) / maxHealth) * 250.f, 30.f });
            window.draw(healthBar);
            window.draw(healthBarFill);
            window.draw(*healthText);
        }
    }
};

class EnemyCounter {
private:
    int enemiesDefeated, enemiesRequired;
    Font font;
    Text* counterText;

public:
    EnemyCounter() : enemiesDefeated(0), enemiesRequired(100), counterText(nullptr) {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }
        counterText = new Text(font);
        counterText->setCharacterSize(40);
        counterText->setFillColor(Color::Yellow);
        counterText->setPosition({ 550.f, 10.f });
    }

    EnemyCounter(const EnemyCounter& other)
        : enemiesDefeated(other.enemiesDefeated), enemiesRequired(other.enemiesRequired), counterText(nullptr)
    {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }
        counterText = new Text(font);
        counterText->setCharacterSize(40);
        counterText->setFillColor(Color::Yellow);
        counterText->setPosition({ 550.f, 10.f });
    }

    ~EnemyCounter() { delete counterText; }

    void reset() { enemiesDefeated = 0; }
    void setEnemiesDefeated(int e) { enemiesDefeated = e; }
    void incrementEnemyCount() { enemiesDefeated++; }
    bool hasWon() const { return enemiesDefeated >= enemiesRequired; }
    int getEnemiesDefeated() const { return enemiesDefeated; }

    void draw(RenderWindow& window) {
        if (counterText) {
            counterText->setString("Enemies: " + to_string(enemiesDefeated) + "/" + to_string(enemiesRequired));
            window.draw(*counterText);
        }
    }
};

// Resource Manager
class ResourceManager {
private:
    Resource playerResources;
    Font font;
    Text resourceText;
    int lastMilestone;  // Tracks the last milestone reached (0, 10, 20, 30, etc.)

    // Helper to check and award milestone bonus
    int checkMilestoneBonus() {
        int total = playerResources.getTotal();
        int currentMilestone = (total / 10) * 10;  // Round down to nearest 10

        if (currentMilestone > lastMilestone && currentMilestone >= 10) {
            int bonusScore = currentMilestone;  // Bonus equals the milestone (10, 20, 30, etc.)
            lastMilestone = currentMilestone;
            cout << "[MILESTONE BONUS] Total resources reached " << currentMilestone
                << "! Bonus score: +" << bonusScore << endl;
            return bonusScore;
        }
        return 0;
    }

public:
    ResourceManager() : playerResources(), resourceText(font), lastMilestone(0) {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << endl;
        }
        resourceText.setCharacterSize(25);
        resourceText.setFillColor(Color::White);
        resourceText.setPosition({ 5.f, 50.f });
    }

    ResourceManager(const ResourceManager& other)
        : playerResources(other.playerResources), resourceText(font), lastMilestone(other.lastMilestone)
    {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }
        resourceText.setCharacterSize(25);
        resourceText.setFillColor(Color::White);
        resourceText.setPosition({ 5.f, 50.f });
    }

    void reset() {
        playerResources.redCrystals = 0;
        playerResources.blueCrystals = 0;
        lastMilestone = 0;
    }

    void setRedCrystals(int c) { playerResources.redCrystals = c; }
    void setBlueCrystals(int c) { playerResources.blueCrystals = c; }
    void setLastMilestone(int m) { lastMilestone = m; }

    int getRedCrystals() const { return playerResources.redCrystals; }
    int getBlueCrystals() const { return playerResources.blueCrystals; }
    int getLastMilestone() const { return lastMilestone; }
    Resource getResources() const { return playerResources; }

    friend void processRedCrystalCollection(ResourceManager& rm, ScoreManager& sm);
    friend void processBlueCrystalCollection(ResourceManager& rm, ScoreManager& sm);
    friend void combineResources(ResourceManager& rm, const Resource& bonus, ScoreManager& sm);
    friend ostream& operator<<(ostream& os, const ResourceManager& rm);

    int applyMilestoneBonus() {
        return checkMilestoneBonus();
    }

    void draw(RenderWindow& window) {
        resourceText.setString("Red: " + to_string(playerResources.redCrystals) +
            " | Blue: " + to_string(playerResources.blueCrystals) +
            " (Total: " + to_string(playerResources.getTotal()) + ")");
        window.draw(resourceText);
    }
};

void processRedCrystalCollection(ResourceManager& rm, ScoreManager& sm) {
    rm.playerResources.redCrystals++;
    sm.addResourceScore(15);  // Individual score for red crystal
    cout << "[RESOURCE] Collected RED crystal! " << rm.playerResources << "\n";

    int bonus = rm.applyMilestoneBonus();
    if (bonus > 0) {
        sm.addScore(bonus);
    }
}

void processBlueCrystalCollection(ResourceManager& rm, ScoreManager& sm) {
    rm.playerResources.blueCrystals++;
    sm.addResourceScore(20);
    cout << "[RESOURCE] Collected BLUE crystal! " << rm.playerResources << "\n";


    int bonus = rm.applyMilestoneBonus();
    if (bonus > 0) {
        sm.addScore(bonus);
    }
}

void combineResources(ResourceManager& rm, const Resource& bonus, ScoreManager& sm) {
    Resource oldResources = rm.playerResources;
    rm.playerResources = rm.playerResources + bonus;
    cout << "[RESOURCE] Combined resources: " << oldResources << " + " << bonus
        << " = " << rm.playerResources << "\n";

    int milestoneBonus = rm.applyMilestoneBonus();
    if (milestoneBonus > 0) {
        sm.addScore(milestoneBonus);
    }
}

ostream& operator<<(ostream& os, const ResourceManager& rm) {
    os << "Resources: " << rm.playerResources;
    return os;
}

class BaseGameObject {
protected:
    Texture texture;
    Sprite* sprite;
    float speed;
    bool active;

public:
    BaseGameObject(const string& texturePath, float objSpeed)
        : speed(objSpeed), active(false), sprite(nullptr)
    {
        try {
            if (!texturePath.empty()) {
                loadTexture(texture, texturePath);
            }
            sprite = new Sprite(texture);
            if (!sprite) {
                throw NullPointerException("BaseGameObject sprite creation");
            }
            sprite->setScale({ 0.15f, 0.15f });
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
            sprite = new Sprite(texture);
        }
    }

    BaseGameObject(const BaseGameObject& other)
        : speed(other.speed), active(other.active), sprite(nullptr)
    {
        texture = other.texture;
        sprite = new Sprite(texture);
        if (sprite && other.sprite) {
            sprite->setPosition(other.sprite->getPosition());
            sprite->setScale(other.sprite->getScale());
        }
    }

    virtual ~BaseGameObject() { delete sprite; }

    virtual void spawn(float windowWidth) {
        try {
            if (!sprite) {
                throw NullPointerException("BaseGameObject::spawn - sprite is null");
            }
            if (windowWidth <= 50) {
                throw InvalidOperationException("Window width too small for spawning");
            }
            float randomX = rand() % (int)(windowWidth - 50);
            sprite->setPosition({ randomX, -30.f });
            active = true;
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
        }
    }

    void update() {
        if (active && sprite) {
            sprite->move({ 0.f, speed * g_slowMotion.getSpeedMultiplier() });
            if (sprite->getPosition().y > 850) active = false;
        }
    }

    void draw(RenderWindow& window) {
        if (active && sprite) window.draw(*sprite);
    }

    bool isActive() const { return active; }
    void setActive(bool a) { active = a; }
    void destroy() { active = false; }

    FloatRect getBounds() const {
        if (sprite) return sprite->getGlobalBounds();
        return FloatRect();
    }

    Vector2f getPosition() const {
        if (sprite) return sprite->getPosition();
        return Vector2f(0, 0);
    }

    void setPosition(float x, float y) {
        if (sprite) sprite->setPosition({ x, y });
    }
};

// ============= RESOURCE OBJECT WITH TWO TYPES =============
class ResourceObject : public BaseGameObject {
private:
    ConvexShape diamond;
    int resourceType; // 0 = Red, 1 = Blue

public:
    ResourceObject(int type = RESOURCE_TYPE_RED) : BaseGameObject("", 0.15f), resourceType(type) {
        diamond.setPointCount(4);
        diamond.setPoint(0, Vector2f(15.f, 0.f));
        diamond.setPoint(1, Vector2f(30.f, 20.f));
        diamond.setPoint(2, Vector2f(15.f, 40.f));
        diamond.setPoint(3, Vector2f(0.f, 20.f));

        if (resourceType == RESOURCE_TYPE_RED) {
            diamond.setFillColor(Color::Red);
        }
        else {
            diamond.setFillColor(Color::Blue);
        }
        diamond.setOutlineColor(Color::White);
        diamond.setOutlineThickness(2.f);
    }

    ResourceObject(const ResourceObject& other) : BaseGameObject(other), resourceType(other.resourceType) {
        diamond.setPointCount(4);
        diamond.setPoint(0, Vector2f(15.f, 0.f));
        diamond.setPoint(1, Vector2f(30.f, 20.f));
        diamond.setPoint(2, Vector2f(15.f, 40.f));
        diamond.setPoint(3, Vector2f(0.f, 20.f));

        if (resourceType == RESOURCE_TYPE_RED) {
            diamond.setFillColor(Color::Red);
        }
        else {
            diamond.setFillColor(Color::Blue);
        }
        diamond.setOutlineColor(Color::White);
        diamond.setOutlineThickness(2.f);
        diamond.setPosition(other.diamond.getPosition());
    }

    void spawn(float windowWidth) override {
        try {
            if (windowWidth <= 50) {
                throw InvalidOperationException("Window width too small for resource spawning");
            }
            float randomX = rand() % (int)(windowWidth - 50);
            diamond.setPosition({ randomX, -30.f });
            active = true;
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
        }
    }

    void update() {
        if (active) {
            diamond.move({ 0.f, speed * g_slowMotion.getSpeedMultiplier() });
            if (diamond.getPosition().y > 850) active = false;
        }
    }

    void draw(RenderWindow& window) {
        if (active) window.draw(diamond);
    }

    FloatRect getBounds() const {
        return diamond.getGlobalBounds();
    }

    Vector2f getPosition() const {
        return diamond.getPosition();
    }

    void setPosition(float x, float y) {
        diamond.setPosition({ x, y });
    }

    int getResourceType() const { return resourceType; }

    void setResourceType(int type) {
        resourceType = type;
        if (resourceType == RESOURCE_TYPE_RED) {
            diamond.setFillColor(Color::Red);
        }
        else {
            diamond.setFillColor(Color::Blue);
        }
    }

    bool operator==(const ResourceObject& other) const {
        return resourceType == other.resourceType;
    }

    bool operator!=(const ResourceObject& other) const {
        return !(*this == other);
    }
};

enum PowerUpType { SHIELD, DOUBLE_FIRE, INSTANT_REPAIR, SLOW_MO };

class PowerUp : public BaseGameObject {
private:
    PowerUpType type;
    int powerLevel;

public:
    PowerUp(PowerUpType puType)
        : BaseGameObject("", 0.15f), type(puType), powerLevel(1)
    {
        try {
            string path;
            string genericPath = "C:/proj_oop2/asteroid.png";

            if (type == SHIELD)
                path = "C:/proj_oop2/diamond.png";
            else if (type == DOUBLE_FIRE)
                path = "C:/proj_oop2/bullet.png";
            else if (type == INSTANT_REPAIR)
                path = "C:/proj_oop2/repair.png";
            else if (type == SLOW_MO)
                path = "C:/proj_oop2/powerup.png";

            try {
                loadTexture(texture, path);
            }
            catch (const TextureLoadException& e) {
                cerr << e.what() << " - Using fallback texture\n";
                loadTexture(texture, genericPath);
            }

            if (sprite) {
                sprite->setTexture(texture, true);
                sprite->setScale({ 0.15f, 0.15f });

                if (type == SHIELD) sprite->setColor(Color::Cyan);
                else if (type == DOUBLE_FIRE) sprite->setColor(Color::Yellow);
                else if (type == INSTANT_REPAIR) sprite->setColor(Color::Green);
                else if (type == SLOW_MO) sprite->setColor(Color::Magenta);
            }
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
        }
    }

    PowerUp(const PowerUp& other) : BaseGameObject(other), type(other.type), powerLevel(other.powerLevel) {
        if (sprite) {
            if (type == SHIELD) sprite->setColor(Color::Cyan);
            else if (type == DOUBLE_FIRE) sprite->setColor(Color::Yellow);
            else if (type == INSTANT_REPAIR) sprite->setColor(Color::Green);
            else if (type == SLOW_MO) sprite->setColor(Color::Magenta);
        }
    }

    void setScale(float scaleX, float scaleY) {
        if (sprite) {
            sprite->setScale({ scaleX, scaleY });
        }
    }

    void setScale(float scale) {
        if (sprite) {
            sprite->setScale({ scale, scale });
        }
    }

    Vector2f getScale() const {
        if (sprite) {
            return sprite->getScale();
        }
        return Vector2f(0.15f, 0.15f);
    }

    PowerUpType getType() const { return type; }
    int getPowerLevel() const { return powerLevel; }
    void setPowerLevel(int level) { powerLevel = level; }

    bool operator==(const PowerUp& other) const {
        return type == other.type && powerLevel == other.powerLevel;
    }

    bool operator!=(const PowerUp& other) const {
        return !(*this == other);
    }

    bool operator>(const PowerUp& other) const {
        return powerLevel > other.powerLevel;
    }

    bool operator<(const PowerUp& other) const {
        return powerLevel < other.powerLevel;
    }

    PowerUp operator+(const PowerUp& other) const {
        PowerUp result(*this);
        if (type == other.type) {
            result.powerLevel += other.powerLevel;
            if (result.powerLevel > 5) result.powerLevel = 5;
        }
        return result;
    }

    PowerUp& operator+=(const PowerUp& other) {
        if (type == other.type) {
            powerLevel += other.powerLevel;
            if (powerLevel > 5) powerLevel = 5;
        }
        return *this;
    }

    PowerUp operator*(int multiplier) const {
        PowerUp result(*this);
        result.powerLevel *= multiplier;
        if (result.powerLevel > 5) result.powerLevel = 5;
        return result;
    }

    friend ostream& operator<<(ostream& os, const PowerUp& pu);
};

ostream& operator<<(ostream& os, const PowerUp& pu) {
    string typeName;
    switch (pu.type) {
    case SHIELD: typeName = "SHIELD"; break;
    case DOUBLE_FIRE: typeName = "DOUBLE_FIRE"; break;
    case INSTANT_REPAIR: typeName = "INSTANT_REPAIR"; break;
    case SLOW_MO: typeName = "SLOW_MO"; break;
    }
    os << "PowerUp[" << typeName << ", Level: " << pu.powerLevel << "]";
    return os;
}

class WaveSpawnManager {
private:
    int resourceWave;
    float resourceWaveTimer;
    float resourceWaveDelay;
    bool resourceWaveSpawned;

    int powerUpWave;
    float powerUpWaveTimer;
    float powerUpWaveDelay;
    bool powerUpWaveSpawned;

    int maxResources;
    int maxPowerUps;

public:
    WaveSpawnManager(int numResources, int numPowerUps)
        : resourceWave(1), resourceWaveTimer(0.f), resourceWaveDelay(4.0f), resourceWaveSpawned(false),
        powerUpWave(1), powerUpWaveTimer(0.f), powerUpWaveDelay(8.0f), powerUpWaveSpawned(false),
        maxResources(numResources), maxPowerUps(numPowerUps)
    {
        try {
            if (numResources <= 0 || numPowerUps <= 0) {
                throw InvalidOperationException("WaveSpawnManager requires positive resource/powerup counts");
            }
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
        }
    }

    WaveSpawnManager(const WaveSpawnManager& other)
        : resourceWave(other.resourceWave), resourceWaveTimer(other.resourceWaveTimer),
        resourceWaveDelay(other.resourceWaveDelay), resourceWaveSpawned(other.resourceWaveSpawned),
        powerUpWave(other.powerUpWave), powerUpWaveTimer(other.powerUpWaveTimer),
        powerUpWaveDelay(other.powerUpWaveDelay), powerUpWaveSpawned(other.powerUpWaveSpawned),
        maxResources(other.maxResources), maxPowerUps(other.maxPowerUps)
    {
    }

    void reset() {
        resourceWave = 1;
        resourceWaveTimer = 0.f;
        resourceWaveSpawned = false;
        powerUpWave = 1;
        powerUpWaveTimer = 0.f;
        powerUpWaveSpawned = false;
    }

    bool allResourcesInactive(ResourceObject resources[], int count) {
        for (int i = 0; i < count; i++) {
            if (resources[i].isActive()) return false;
        }
        return true;
    }

    bool allPowerUpsInactive(PowerUp powerUps[], int count) {
        for (int i = 0; i < count; i++) {
            if (powerUps[i].isActive()) return false;
        }
        return true;
    }

    void updateResources(ResourceObject resources[], float deltaTime, float windowWidth) {
        resourceWaveTimer += deltaTime;

        int startIdx = 0, endIdx = 0;
        switch (resourceWave) {
        case 1:
            startIdx = 0; endIdx = 4;
            break;
        case 2:
            startIdx = 4; endIdx = maxResources;
            break;
        default:
            if (allResourcesInactive(resources, maxResources)) {
                resourceWave = 1;
                resourceWaveSpawned = false;
                resourceWaveTimer = 0.f;
                cout << "[RESOURCES] All collected! Resetting to Wave 1\n";
            }
            return;
        }

        if (resourceWaveTimer >= resourceWaveDelay && !resourceWaveSpawned) {
            cout << "[RESOURCES] Spawning Wave " << resourceWave
                << " (indices " << startIdx << " to " << endIdx - 1 << ")\n";

            for (int i = startIdx; i < endIdx && i < maxResources; i++) {
                if (!resources[i].isActive()) {
                    resources[i].spawn(windowWidth);
                }
            }
            resourceWaveSpawned = true;
        }

        bool currentWaveComplete = true;
        for (int i = startIdx; i < endIdx && i < maxResources; i++) {
            if (resources[i].isActive()) {
                currentWaveComplete = false;
                break;
            }
        }

        if (currentWaveComplete && resourceWaveSpawned) {
            resourceWave++;
            resourceWaveSpawned = false;
            resourceWaveTimer = 0.f;
            cout << "[RESOURCES] Wave complete! Moving to Wave " << resourceWave << "\n";
        }
    }

    void updatePowerUps(PowerUp powerUps[], float deltaTime, float windowWidth) {
        powerUpWaveTimer += deltaTime;

        int startIdx = 0, endIdx = 0;
        switch (powerUpWave) {
        case 1:
            startIdx = 0; endIdx = 2;
            break;
        case 2:
            startIdx = 2; endIdx = maxPowerUps;
            break;
        default:
            if (allPowerUpsInactive(powerUps, maxPowerUps)) {
                powerUpWave = 1;
                powerUpWaveSpawned = false;
                powerUpWaveTimer = 0.f;
                cout << "[POWER-UPS] All collected! Resetting to Wave 1\n";
            }
            return;
        }

        if (powerUpWaveTimer >= powerUpWaveDelay && !powerUpWaveSpawned) {
            cout << "[POWER-UPS] Spawning Wave " << powerUpWave
                << " (indices " << startIdx << " to " << endIdx - 1 << ")\n";

            for (int i = startIdx; i < endIdx && i < maxPowerUps; i++) {
                if (!powerUps[i].isActive()) {
                    powerUps[i].spawn(windowWidth);
                }
            }
            powerUpWaveSpawned = true;
        }

        bool currentWaveComplete = true;
        for (int i = startIdx; i < endIdx && i < maxPowerUps; i++) {
            if (powerUps[i].isActive()) {
                currentWaveComplete = false;
                break;
            }
        }

        if (currentWaveComplete && powerUpWaveSpawned) {
            powerUpWave++;
            powerUpWaveSpawned = false;
            powerUpWaveTimer = 0.f;
            cout << "[POWER-UPS] Wave complete! Moving to Wave " << powerUpWave << "\n";
        }
    }

    int getResourceWave() const { return resourceWave; }
    int getPowerUpWave() const { return powerUpWave; }
};

//spaceship class
class Spaceship {
private:
    Texture texture;
    Sprite* sprite;
    float moveSpeed;
    float baseMoveSpeed;
    bool hasShield;
    float shieldTimer;
    bool hasDoubleFire;
    float fireTimer;
    float healthFlashTimer;
    Font font;
    Text effectText;
    CircleShape shieldOutline;
    Clock globalClock;

public:
    Spaceship(const string& texturePath, float speed)
        : moveSpeed(speed), baseMoveSpeed(speed),
        hasShield(false), shieldTimer(0.f),
        hasDoubleFire(false), fireTimer(0.f),
        healthFlashTimer(0.f),
        effectText(font), sprite(nullptr)
    {
        try {
            loadTexture(texture, texturePath);
            sprite = new Sprite(texture);
            if (!sprite) {
                throw NullPointerException("Spaceship sprite creation");
            }
            sprite->setPosition({ 375.f, 285.f });
            sprite->setScale({ 0.15f, 0.15f });

            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
            effectText.setFont(font);
            effectText.setCharacterSize(30);
            effectText.setFillColor(Color::Red);

            float radius = sprite->getGlobalBounds().size.x / 2.f + 10.f;
            shieldOutline.setRadius(radius);
            shieldOutline.setFillColor(Color::Transparent);
            shieldOutline.setOutlineThickness(5.f);
            shieldOutline.setOrigin({ radius, radius });
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
            sprite = new Sprite(texture);
        }
    }

    Spaceship(const Spaceship& other)
        : moveSpeed(other.moveSpeed), baseMoveSpeed(other.baseMoveSpeed),
        hasShield(other.hasShield), shieldTimer(other.shieldTimer),
        hasDoubleFire(other.hasDoubleFire), fireTimer(other.fireTimer),
        healthFlashTimer(other.healthFlashTimer), effectText(font), sprite(nullptr)
    {
        texture = other.texture;
        sprite = new Sprite(texture);
        if (sprite && other.sprite) {
            sprite->setPosition(other.sprite->getPosition());
            sprite->setScale(other.sprite->getScale());
            sprite->setColor(other.sprite->getColor());
        }

        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }

        effectText.setFont(font);
        effectText.setCharacterSize(30);
        effectText.setFillColor(Color::Red);

        float radius = sprite ? sprite->getGlobalBounds().size.x / 2.f + 10.f : 30.f;
        shieldOutline.setRadius(radius);
        shieldOutline.setFillColor(Color::Transparent);
        shieldOutline.setOutlineThickness(5.f);
        shieldOutline.setOrigin({ radius, radius });
    }

    ~Spaceship() { delete sprite; }

    void reset() {
        if (sprite) {
            sprite->setPosition({ 375.f, 285.f });
            sprite->setColor(Color::White);
        }
        moveSpeed = baseMoveSpeed;
        hasShield = false;
        shieldTimer = 0.f;
        hasDoubleFire = false;
        fireTimer = 0.f;
        healthFlashTimer = 0.f;
    }

    void handleInput() {
        if (!sprite) return;
        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) sprite->move({ -moveSpeed, 0 });
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) sprite->move({ moveSpeed, 0 });
        if (Keyboard::isKeyPressed(Keyboard::Key::Up)) sprite->move({ 0, -moveSpeed });
        if (Keyboard::isKeyPressed(Keyboard::Key::Down)) sprite->move({ 0, moveSpeed });
    }

    void checkBoundary(float w, float h) {
        if (!sprite) return;
        Vector2f pos = sprite->getPosition();
        FloatRect bounds = sprite->getGlobalBounds();

        if (pos.x < 0) sprite->setPosition({ 0.f, pos.y });
        if (pos.x + bounds.size.x > w) sprite->setPosition({ w - bounds.size.x, pos.y });
        if (pos.y < 0) sprite->setPosition({ pos.x, 0.f });
        if (pos.y + bounds.size.y > h) sprite->setPosition({ pos.x, h - bounds.size.y });
    }

    void update(float deltaTime) {
        if (!sprite) return;

        if (hasShield) {
            shieldTimer -= deltaTime;
            if (shieldTimer <= 0.f) {
                hasShield = false;
            }
        }

        if (hasDoubleFire) {
            fireTimer -= deltaTime;
            if (fireTimer <= 0.f) {
                hasDoubleFire = false;
            }
        }

        if (healthFlashTimer > 0.f) {
            healthFlashTimer -= deltaTime;
        }

        if (healthFlashTimer > 0.f) {
            sprite->setColor(Color::Green);
        }
        else if (g_slowMotion.getIsActive()) {
            sprite->setColor(Color::Magenta);  
        }
        else if (hasDoubleFire) {
            sprite->setColor(Color::Yellow);
        }
        else if (hasShield) {
            sprite->setColor(Color(100, 200, 255));
        }
        else {
            sprite->setColor(Color::White);
        }

        Vector2f center = {
            sprite->getPosition().x + sprite->getGlobalBounds().size.x / 2.f,
            sprite->getPosition().y + sprite->getGlobalBounds().size.y / 2.f
        };
        shieldOutline.setPosition(center);
        shieldOutline.setOutlineColor(Color(255, 255, 255, (int)(hasShield ? 255 * (sin(globalClock.getElapsedTime().asSeconds() * 10.f) * 0.5f + 0.5f) : 0)));
    }
    void activateShield() {
        hasShield = true;
        shieldTimer = 15.0f;
        if (sprite) sprite->setColor(Color(100, 200, 255));
    }

    void activateDoubleFire() {
        hasDoubleFire = true;
        fireTimer = 10.0f;
        if (sprite) {
            sprite->setColor(Color::Yellow);
            effectText.setString("DOUBLE FIRE!");
            effectText.setFillColor(Color::Yellow);
            FloatRect textRect = effectText.getLocalBounds();
            effectText.setOrigin({ textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f });
            effectText.setPosition({
                sprite->getPosition().x + sprite->getGlobalBounds().size.x / 2.f,
                sprite->getPosition().y - 50.f
                });
        }
        globalClock.restart();
    }

    void activateRepair() {
        healthFlashTimer = 0.5f;
        if (sprite) sprite->setColor(Color::Green);
    }

    void activateSlowMo() {
        g_slowMotion.activate();
        if (sprite) sprite->setColor(Color::Magenta);
    }

    void setShieldState(bool shield, float timer) {
        hasShield = shield;
        shieldTimer = timer;
        if (hasShield && sprite) sprite->setColor(Color(100, 200, 255));
    }

    void setDoubleFireState(bool doubleFire, float timer) {
        hasDoubleFire = doubleFire;
        fireTimer = timer;
        if (hasDoubleFire && sprite) sprite->setColor(Color::Yellow);
    }

    bool getHasShield() const { return hasShield; }
    float getShieldTimer() const { return shieldTimer; }
    bool getHasDoubleFire() const { return hasDoubleFire; }
    float getFireTimer() const { return fireTimer; }
    float getFireDelay() const { return hasDoubleFire ? 0.15f : 0.3f; }

    Vector2f getPosition() const {
        if (sprite) return sprite->getPosition();
        return Vector2f(0, 0);
    }

    void setPosition(float x, float y) {
        if (sprite) sprite->setPosition({ x, y });
    }

    FloatRect getGlobalBounds() const {
        if (sprite) return sprite->getGlobalBounds();
        return FloatRect();
    }

    void draw(RenderWindow& window) {
        if (!sprite) return;

        if (hasShield) window.draw(shieldOutline);
        window.draw(*sprite);

        if (hasDoubleFire && globalClock.getElapsedTime().asSeconds() < 2.f) {
            FloatRect textRect = effectText.getLocalBounds();
            effectText.setOrigin({ textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f });
            effectText.setPosition({
                sprite->getPosition().x + sprite->getGlobalBounds().size.x / 2.f,
                sprite->getPosition().y - 50.f
                });
            window.draw(effectText);
        }

        if (healthFlashTimer > 0.f) {
            CircleShape burst(15.f);
            burst.setFillColor(Color(0, 255, 0, (int)(255 * healthFlashTimer * 2)));
            burst.setOrigin({ 15.f, 15.f });
            burst.setPosition({
                sprite->getPosition().x + sprite->getGlobalBounds().size.x / 2.f,
                sprite->getPosition().y + sprite->getGlobalBounds().size.y / 2.f
                });
            window.draw(burst);

            effectText.setString("+HEALTH");
            effectText.setFillColor(Color::Green);
            FloatRect textRect = effectText.getLocalBounds();
            effectText.setOrigin({ textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f });
            effectText.setPosition({
                sprite->getPosition().x + sprite->getGlobalBounds().size.x / 2.f,
                sprite->getPosition().y - 50.f
                });
            window.draw(effectText);
        }
    }
};

//bullet class
class Bullet {
private:
    Texture texture;
    Sprite* sprite;
    float speed;
    bool active;
    bool isDoubleFireBullet;

public:
    Bullet(const string& texturePath, float bulletSpeed)
        : speed(bulletSpeed), active(false), isDoubleFireBullet(false), sprite(nullptr)
    {
        try {
            loadTexture(texture, texturePath);
            sprite = new Sprite(texture);
            if (!sprite) {
                throw NullPointerException("Bullet sprite creation");
            }
            sprite->setScale({ 0.1f, 0.1f });
        }
        catch (const GameException& e) {
            cerr << e.what() << "\n";
            sprite = new Sprite(texture);
        }
    }

    Bullet(const Bullet& other)
        : speed(other.speed), active(other.active),
        isDoubleFireBullet(other.isDoubleFireBullet), sprite(nullptr)
    {
        texture = other.texture;
        sprite = new Sprite(texture);
        if (sprite && other.sprite) {
            sprite->setPosition(other.sprite->getPosition());
            sprite->setScale(other.sprite->getScale());
            sprite->setColor(other.sprite->getColor());
        }
    }

    ~Bullet() { delete sprite; }

    void reset() {
        active = false;
        isDoubleFireBullet = false;
        if (sprite) {
            sprite->setScale({ 0.1f, 0.1f });
            sprite->setColor(Color::White);
        }
    }

    void shoot(Vector2f pos, FloatRect bounds, bool isDoubleFire) {
        if (!active && sprite) {
            sprite->setPosition({ pos.x + bounds.size.x / 2.0f - sprite->getGlobalBounds().size.x / 2.0f, pos.y });
            active = true;
            isDoubleFireBullet = isDoubleFire;

            if (isDoubleFire) {
                sprite->setScale({ 0.15f, 0.15f });
                sprite->setColor(Color::Yellow);
            }
            else {
                sprite->setScale({ 0.1f, 0.1f });
                sprite->setColor(Color::White);
            }
        }
    }

    void update() {
        if (active && sprite) {
            sprite->move({ 0.f, -speed });
            if (sprite->getPosition().y < 0) active = false;
        }
    }

    void draw(RenderWindow& window) {
        if (active && sprite) window.draw(*sprite);
    }

    bool isActive() const { return active; }
    void deactivate() { active = false; }

    bool checkCollision(FloatRect other) {
        if (!sprite) return false;
        return active && sprite->getGlobalBounds().findIntersection(other).has_value();
    }
};

void spawnEnemyWave(EnemyShip* enemies[], int size, float windowWidth, SpawnPositionTracker* tracker) {
    try {
        if (!enemies) {
            throw NullPointerException("spawnEnemyWave - enemies array is null");
        }
        for (int i = 0; i < size; i++) {
            if (!enemies[i]) {
                throw NullPointerException("spawnEnemyWave - enemy at index " + to_string(i) + " is null");
            }
            if (!enemies[i]->isActive()) {
                enemies[i]->spawn(windowWidth, tracker);
            }
        }
    }
    catch (const GameException& e) {
        cerr << e.what() << "\n";
    }
}

// reset Game State 
void resetGameState(
    Spaceship& spaceship,
    Bullet& bullet,
    LevelManager& levelManager,
    SpawnPositionTracker& spawnTracker,
    Asteroid asteroids[],
    EasyEnemy easyEnemies[],
    MediumEnemy mediumEnemies[],
    HardEnemy hardEnemies[],
    BossEnemy bosses[],
    int bossCount,
    ScoreManager& scoreManager,
    HealthManager& healthManager,
    EnemyCounter& enemyCounter,
    ResourceManager& resourceManager,
    ResourceObject resources[],
    PowerUp powerUps[],
    int maxResources,
    int maxPowerUps,
    int& currentWave,
    int& enemiesPerWave,
    float& enemyBaseSpeed,
    int& enemiesSpawned,
    int& enemiesKilled,
    Clock& waveClock,
    bool& spawningWave,
    int& collisionCooldown,
    int& asteroidSpawnTimer,
    bool& isGameActive,
    WaveSpawnManager& waveSpawnManager)
{
    try {
        spaceship.reset();
        bullet.reset();
        levelManager.reset();
        spawnTracker.clearPositions();
        scoreManager.reset();
        healthManager.reset();
        enemyCounter.reset();
        resourceManager.reset();
        waveSpawnManager.reset();
        g_slowMotion.reset();

        for (int i = 0; i < 7; i++) {
            asteroids[i].destroy();
            asteroids[i].setHitPoints(1);
        }
        for (int i = 0; i < 4; i++) {
            easyEnemies[i].destroy();
            easyEnemies[i].setHitPoints(1);
        }
        for (int i = 0; i < 6; i++) {
            mediumEnemies[i].destroy();
            mediumEnemies[i].setHitPoints(1);
        }
        for (int i = 0; i < 8; i++) {
            hardEnemies[i].destroy();
            hardEnemies[i].setHitPoints(1);
        }
        for (int i = 0; i < bossCount; ++i) {
            bosses[i].destroy();
            bosses[i].setHitPoints(bosses[i].getMaxHitPointsForBoss());
        }
        for (int i = 0; i < maxResources; ++i) {
            resources[i].destroy();
        }
        for (int i = 0; i < maxPowerUps; ++i) {
            powerUps[i].destroy();
        }

        currentWave = 1;
        enemiesPerWave = 3;
        enemyBaseSpeed = 2.0f;
        enemiesSpawned = 0;
        enemiesKilled = 0;
        spawningWave = true;
        collisionCooldown = 0;
        asteroidSpawnTimer = 0;
        isGameActive = true;
        waveClock.restart();
    }
    catch (const exception& e) {
        cerr << "[ERROR] Exception in resetGameState: " << e.what() << endl;
    }
}

// class to handle slow motion
class SlowMoIndicator {
private:
    Font font;
    Text slowMoText;
    RectangleShape timerBar;
    RectangleShape timerBarBg;

public:
    SlowMoIndicator() : slowMoText(font) {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << "\n";
        }

        slowMoText.setCharacterSize(30);
        slowMoText.setFillColor(Color::Magenta);
        slowMoText.setString("SLOW-MO ACTIVE!");
        slowMoText.setPosition({ 580.f, 760.f });

        timerBarBg.setSize({ 200.f, 15.f });
        timerBarBg.setPosition({ 600.f, 785.f });
        timerBarBg.setFillColor(Color(50, 50, 50));

        timerBar.setSize({ 200.f, 15.f });
        timerBar.setPosition({ 600.f, 785.f });
        timerBar.setFillColor(Color::Magenta);
    }

    void draw(RenderWindow& window, bool isActive, float currentTime, float duration) {
        if (isActive) {
            window.draw(slowMoText);
            window.draw(timerBarBg);
            float ratio = currentTime / duration;
            timerBar.setSize({ 200.f * ratio, 15.f });
            window.draw(timerBar);
        }
    }
};

// class for dispalying timer
class GameTimer {
private:
    Font font;
    Text timerText;
    Clock gameClock;
    bool isRunning;
    int savedSeconds;

public:
    GameTimer() : timerText(font), isRunning(false), savedSeconds(0) {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << endl;
        }
        timerText.setCharacterSize(30);
        timerText.setFillColor(Color::Yellow);
        timerText.setPosition({ 1200.f, 750.f });
    }

    GameTimer(const GameTimer& other)
        : timerText(font), isRunning(other.isRunning), savedSeconds(other.savedSeconds)
    {
        try {
            loadFont(font, "C:/proj_oop2/ByteBounce.ttf");
        }
        catch (const FontLoadException& e) {
            cerr << e.what() << endl;
        }
        timerText.setCharacterSize(30);
        timerText.setFillColor(Color::Yellow);
        timerText.setPosition({ 1200.f, 750.f });
    }

    void start() {
        gameClock.restart();
        savedSeconds = 0;
        isRunning = true;
    }

    void stop() { isRunning = false; }
    void pause() { isRunning = false; }
    void resume() { isRunning = true; }

    int getElapsedSeconds() const {
        return savedSeconds + static_cast<int>(gameClock.getElapsedTime().asSeconds());
    }

    void setElapsedSeconds(int seconds) {
        savedSeconds = seconds;
        gameClock.restart();
    }

    void update() {
        if (isRunning) {
            int totalSeconds = getElapsedSeconds();
            int minutes = totalSeconds / 60;
            int seconds = totalSeconds % 60;
            string timeStr = "TIME: ";
            if (minutes < 10) timeStr += "0";
            timeStr += to_string(minutes) + ":";
            if (seconds < 10) timeStr += "0";
            timeStr += to_string(seconds);
            timerText.setString(timeStr);
        }
    }

    void draw(RenderWindow& window) { window.draw(timerText); }
    void reset() { gameClock.restart(); savedSeconds = 0; isRunning = false; }
};

// functions to capture and restore game state
SaveableState captureGameState(
    Spaceship& spaceship,
    ScoreManager& scoreManager,
    HealthManager& healthManager,
    LevelManager& levelManager,
    EnemyCounter& enemyCounter,
    ResourceManager& resourceManager,
    GameTimer& gameTimer,
    int currentWave,
    int enemiesPerWave,
    float enemyBaseSpeed,
    int enemiesSpawned,
    int enemiesKilled,
    Asteroid asteroids[],
    EasyEnemy easyEnemies[],
    MediumEnemy mediumEnemies[],
    HardEnemy hardEnemies[],
    BossEnemy bosses[],
    int bossCount,
    ResourceObject resources[],
    PowerUp powerUps[],
    int maxResources,
    int maxPowerUps)
{
    SaveableState state;

    Vector2f pos = spaceship.getPosition();
    state.playerX = pos.x;
    state.playerY = pos.y;
    state.hasShield = spaceship.getHasShield();
    state.shieldTimer = spaceship.getShieldTimer();
    state.hasDoubleFire = spaceship.getHasDoubleFire();
    state.fireTimer = spaceship.getFireTimer();

    state.score = scoreManager.getScore();
    state.health = healthManager.getHealth();
    state.currentLevel = levelManager.getCurrentLevel();
    state.enemiesDefeated = enemyCounter.getEnemiesDefeated();
    state.redCrystals = resourceManager.getRedCrystals();
    state.blueCrystals = resourceManager.getBlueCrystals();
    state.gameTimeSeconds = gameTimer.getElapsedSeconds();
    state.lastMilestone = resourceManager.getLastMilestone();

    state.currentWave = currentWave;
    state.enemiesPerWave = enemiesPerWave;
    state.enemyBaseSpeed = enemyBaseSpeed;
    state.enemiesSpawned = enemiesSpawned;
    state.enemiesKilled = enemiesKilled;

    state.slowMoActive = g_slowMotion.getIsActive();
    state.slowMoTimer = g_slowMotion.getRemainingTime();

    for (int i = 0; i < 7; i++) {
        state.asteroidActive[i] = asteroids[i].isActive();
        Vector2f aPos = asteroids[i].getPosition();
        state.asteroidX[i] = aPos.x;
        state.asteroidY[i] = aPos.y;
        state.asteroidHP[i] = asteroids[i].getHitPoints();
    }

    for (int i = 0; i < 4; i++) {
        state.easyEnemyActive[i] = easyEnemies[i].isActive();
        Vector2f ePos = easyEnemies[i].getPosition();
        state.easyEnemyX[i] = ePos.x;
        state.easyEnemyY[i] = ePos.y;
        state.easyEnemyHP[i] = easyEnemies[i].getHitPoints();
    }

    for (int i = 0; i < 6; i++) {
        state.mediumEnemyActive[i] = mediumEnemies[i].isActive();
        Vector2f mPos = mediumEnemies[i].getPosition();
        state.mediumEnemyX[i] = mPos.x;
        state.mediumEnemyY[i] = mPos.y;
        state.mediumEnemyHP[i] = mediumEnemies[i].getHitPoints();
    }

    for (int i = 0; i < 8; i++) {
        state.hardEnemyActive[i] = hardEnemies[i].isActive();
        Vector2f hPos = hardEnemies[i].getPosition();
        state.hardEnemyX[i] = hPos.x;
        state.hardEnemyY[i] = hPos.y;
        state.hardEnemyHP[i] = hardEnemies[i].getHitPoints();
    }

    if (bossCount > 0) {
        state.bossActive = bosses[0].isActive();
        Vector2f bPos = bosses[0].getPosition();
        state.bossX = bPos.x;
        state.bossY = bPos.y;
        state.bossHP = bosses[0].getHitPoints();
    }

    for (int i = 0; i < maxResources && i < 8; i++) {
        state.resourceActive[i] = resources[i].isActive();
        Vector2f rPos = resources[i].getPosition();
        state.resourceX[i] = rPos.x;
        state.resourceY[i] = rPos.y;
        state.resourceType[i] = resources[i].getResourceType();
    }

    for (int i = 0; i < maxPowerUps && i < 4; i++) {
        state.powerUpActive[i] = powerUps[i].isActive();
        Vector2f pPos = powerUps[i].getPosition();
        state.powerUpX[i] = pPos.x;
        state.powerUpY[i] = pPos.y;
        state.powerUpType[i] = static_cast<int>(powerUps[i].getType());
    }

    return state;
}

void restoreGameState(
    const SaveableState& state,
    Spaceship& spaceship,
    ScoreManager& scoreManager,
    HealthManager& healthManager,
    LevelManager& levelManager,
    EnemyCounter& enemyCounter,
    ResourceManager& resourceManager,
    GameTimer& gameTimer,
    int& currentWave,
    int& enemiesPerWave,
    float& enemyBaseSpeed,
    int& enemiesSpawned,
    int& enemiesKilled,
    Asteroid asteroids[],
    EasyEnemy easyEnemies[],
    MediumEnemy mediumEnemies[],
    HardEnemy hardEnemies[],
    BossEnemy bosses[],
    int bossCount,
    ResourceObject resources[],
    PowerUp powerUps[],
    int maxResources,
    int maxPowerUps)
{
    spaceship.setPosition(state.playerX, state.playerY);
    spaceship.setShieldState(state.hasShield, state.shieldTimer);
    spaceship.setDoubleFireState(state.hasDoubleFire, state.fireTimer);

    scoreManager.setScore(state.score);
    healthManager.setHealth(state.health);
    levelManager.setLevel(state.currentLevel);
    enemyCounter.setEnemiesDefeated(state.enemiesDefeated);
    resourceManager.setRedCrystals(state.redCrystals);
    resourceManager.setBlueCrystals(state.blueCrystals);
    resourceManager.setLastMilestone(state.lastMilestone);
    gameTimer.setElapsedSeconds(state.gameTimeSeconds);

    currentWave = state.currentWave;
    enemiesPerWave = state.enemiesPerWave;
    enemyBaseSpeed = state.enemyBaseSpeed;
    enemiesSpawned = state.enemiesSpawned;
    enemiesKilled = state.enemiesKilled;

    if (state.slowMoActive) {
        g_slowMotion.activate();
    }
    else {
        g_slowMotion.reset();
    }

    for (int i = 0; i < 7; i++) {
        asteroids[i].setActive(state.asteroidActive[i]);
        asteroids[i].setPosition(state.asteroidX[i], state.asteroidY[i]);
        asteroids[i].setHitPoints(state.asteroidHP[i]);
    }

    for (int i = 0; i < 4; i++) {
        easyEnemies[i].setActive(state.easyEnemyActive[i]);
        easyEnemies[i].setPosition(state.easyEnemyX[i], state.easyEnemyY[i]);
        easyEnemies[i].setHitPoints(state.easyEnemyHP[i]);
    }

    for (int i = 0; i < 6; i++) {
        mediumEnemies[i].setActive(state.mediumEnemyActive[i]);
        mediumEnemies[i].setPosition(state.mediumEnemyX[i], state.mediumEnemyY[i]);
        mediumEnemies[i].setHitPoints(state.mediumEnemyHP[i]);
    }

    for (int i = 0; i < 8; i++) {
        hardEnemies[i].setActive(state.hardEnemyActive[i]);
        hardEnemies[i].setPosition(state.hardEnemyX[i], state.hardEnemyY[i]);
        hardEnemies[i].setHitPoints(state.hardEnemyHP[i]);
    }

    if (bossCount > 0) {
        bosses[0].setActive(state.bossActive);
        bosses[0].setPosition(state.bossX, state.bossY);
        bosses[0].setHitPoints(state.bossHP);
    }

    for (int i = 0; i < maxResources && i < 8; i++) {
        resources[i].setActive(state.resourceActive[i]);
        resources[i].setPosition(state.resourceX[i], state.resourceY[i]);
        resources[i].setResourceType(state.resourceType[i]);
    }

    for (int i = 0; i < maxPowerUps && i < 4; i++) {
        powerUps[i].setActive(state.powerUpActive[i]);
        powerUps[i].setPosition(state.powerUpX[i], state.powerUpY[i]);
    }

    cout << "[RESTORE] Game state restored successfully!" << endl;
}
