#include "Header.h"

int main()
{
    try {
        srand(static_cast<unsigned int>(time(0)));

        RenderWindow window(VideoMode({ 1400, 800 }), "Galaxy Wars");
        RectangleShape screenRect({ 1400.f, 800.f });
        screenRect.setFillColor(Color::Black);

        HighScoresManager highScores;
        GameSaveManager saveManager;
        MainMenu mainMenu;
        HighScoresScreen highScoresScreen(&highScores);
        SaveLoadScreen saveLoadScreen(&saveManager, false);
        PauseMenu pauseMenu;
        EnhancedGameOverScreen gameOverScreen;
        GameTimer gameClock;
        SlowMoIndicator slowMoIndicator;

        int gameState = 0;
        int previousGameState = 0;

        Spaceship spaceship("C:/proj_oop2/spaceship.png", 0.5f);
        Bullet bullet("C:/proj_oop2/bullet.png", 0.7f);
        LevelManager levelManager;
        SpawnPositionTracker spawnTracker;

        const int MAX_RESOURCES = 8;
        ResourceObject resources[MAX_RESOURCES] = {
            ResourceObject(RESOURCE_TYPE_RED),
            ResourceObject(RESOURCE_TYPE_RED),
            ResourceObject(RESOURCE_TYPE_BLUE),
            ResourceObject(RESOURCE_TYPE_BLUE),
            ResourceObject(RESOURCE_TYPE_RED),
            ResourceObject(RESOURCE_TYPE_RED),
            ResourceObject(RESOURCE_TYPE_BLUE),
            ResourceObject(RESOURCE_TYPE_BLUE)
        };

        const int MAX_POWERUPS = 4;
        PowerUp powerUps[MAX_POWERUPS] = {
            PowerUp(SHIELD),
            PowerUp(DOUBLE_FIRE),
            PowerUp(INSTANT_REPAIR),
            PowerUp(SLOW_MO)
        };
        powerUps[0].setScale(0.5f);        
        powerUps[1].setScale(0.1f);       
        powerUps[2].setScale(0.3f, 0.3f); 
        powerUps[3].setScale(0.25f);       

        WaveSpawnManager waveSpawnManager(MAX_RESOURCES, MAX_POWERUPS);

        Asteroid asteroids[7] = {
            Asteroid("C:/proj_oop2/asteroid.png", 0.1f),
            Asteroid("C:/proj_oop2/asteroid.png", 0.2f),
            Asteroid("C:/proj_oop2/asteroid.png", 0.15f),
            Asteroid("C:/proj_oop2/asteroid.png", 0.12f),
            Asteroid("C:/proj_oop2/asteroid.png", 0.18f),
            Asteroid("C:/proj_oop2/asteroid.png", 0.11f),
            Asteroid("C:/proj_oop2/asteroid.png", 0.16f)
        };

        EasyEnemy easyEnemies[4] = {
            EasyEnemy("C:/proj_oop2/enemy.png"),
            EasyEnemy("C:/proj_oop2/enemy.png"),
            EasyEnemy("C:/proj_oop2/enemy.png"),
            EasyEnemy("C:/proj_oop2/enemy.png")
        };

        MediumEnemy mediumEnemies[6] = {
            MediumEnemy("C:/proj_oop2/enemy2.png"),
            MediumEnemy("C:/proj_oop2/enemy2.png"),
            MediumEnemy("C:/proj_oop2/enemy2.png"),
            MediumEnemy("C:/proj_oop2/enemy2.png"),
            MediumEnemy("C:/proj_oop2/enemy2.png"),
            MediumEnemy("C:/proj_oop2/enemy2.png"),
        };

        HardEnemy hardEnemies[8] = {
            HardEnemy("C:/proj_oop2/enemy3.png"),
            HardEnemy("C:/proj_oop2/enemy3.png"),
            HardEnemy("C:/proj_oop2/enemy3.png"),
            HardEnemy("C:/proj_oop2/enemy3.png"),
            HardEnemy("C:/proj_oop2/enemy3.png"),
            HardEnemy("C:/proj_oop2/enemy3.png"),
            HardEnemy("C:/proj_oop2/enemy3.png"),
            HardEnemy("C:/proj_oop2/enemy3.png"),
        };

        const int BOSS_COUNT = 1;
        BossEnemy bosses[BOSS_COUNT] = {
            BossEnemy("C:/proj_oop2/bossenemy.png")
        };

        EnemyShip* currentEnemiesArray[10];
        int currentEnemyCount = 0;

        int currentWave = 1;
        int enemiesPerWave = 3;
        float enemyBaseSpeed = 2.0f;
        int enemiesSpawned = 0;
        int enemiesKilled = 0;
        float waveDelay = 5.0f;
        Clock waveClock;
        Clock updateClock;
        float deltaTime = 0.f;
        bool spawningWave = true;

        ScoreManager scoreManager;
        HealthManager healthManager;
        EnemyCounter enemyCounter;
        ResourceManager resourceManager;

        SoundManager soundManager;
        soundManager.playMusic();

        bool isGameActive = true;
        int collisionCooldown = 0;
        int asteroidSpawnTimer = 0;
        int previousLevel = 1;
        float fireCooldown = 0.f;

        for (int i = 0; i < 7; i++)
            asteroids[i].setHitPoints(levelManager.getAsteroidHitPoints());
        for (int i = 0; i < 4; i++)
            easyEnemies[i].setHitPoints(levelManager.getEnemyHitPoints());
        for (int i = 0; i < 6; i++)
            mediumEnemies[i].setHitPoints(levelManager.getEnemyHitPoints());
        for (int i = 0; i < 8; i++)
            hardEnemies[i].setHitPoints(levelManager.getEnemyHitPoints());
        for (int i = 0; i < BOSS_COUNT; ++i) {
            bosses[i].setHitPoints(bosses[i].getMaxHitPointsForBoss());
        }

        while (window.isOpen())
        {
            deltaTime = updateClock.restart().asSeconds();

            while (auto event = window.pollEvent())
            {
                if (event->is<Event::Closed>())
                    window.close();

                if (event->is<Event::MouseButtonPressed>())
                {
                    auto* mouseClick = event->getIf<Event::MouseButtonPressed>();
                    Vector2f mousePos(static_cast<float>(mouseClick->position.x), static_cast<float>(mouseClick->position.y));

                    if (gameState == 0)
                    {
                        int choice = mainMenu.handleClick(mousePos);
                        if (choice == 1)
                        {
                            gameState = 1;
                            gameClock.start();
                            resetGameState(spaceship, bullet, levelManager, spawnTracker,
                                asteroids, easyEnemies, mediumEnemies, hardEnemies, bosses, BOSS_COUNT,
                                scoreManager, healthManager, enemyCounter,
                                resourceManager, resources, powerUps, MAX_RESOURCES, MAX_POWERUPS,
                                currentWave, enemiesPerWave, enemyBaseSpeed,
                                enemiesSpawned, enemiesKilled, waveClock, spawningWave,
                                collisionCooldown, asteroidSpawnTimer, isGameActive, waveSpawnManager);
                            levelManager.checkLevelUp(1);
                        }
                        else if (choice == 4)
                        {
                            saveLoadScreen.setMode(false);
                            saveLoadScreen.refreshSlots();
                            previousGameState = 0;
                            gameState = 5;
                        }
                        else if (choice == 2)
                            gameState = 2;
                        else if (choice == 3)
                            window.close();
                    }
                    else if (gameState == 2)
                    {
                        if (highScoresScreen.handleClick(mousePos))
                            gameState = 0;
                    }
                    else if (gameState == 5)
                    {
                        int choice = saveLoadScreen.handleClick(mousePos);
                        if (choice == -1) {
                            gameState = previousGameState;
                            if (previousGameState == 4) {
                                gameState = 4;
                            }
                        }
                        else if (choice >= 0 && choice <= 2) {
                            if (saveLoadScreen.getIsSaveMode()) {
                                SaveableState state = captureGameState(
                                    spaceship, scoreManager, healthManager, levelManager,
                                    enemyCounter, resourceManager, gameClock,
                                    currentWave, enemiesPerWave, enemyBaseSpeed,
                                    enemiesSpawned, enemiesKilled,
                                    asteroids, easyEnemies, mediumEnemies, hardEnemies,
                                    bosses, BOSS_COUNT, resources, powerUps,
                                    MAX_RESOURCES, MAX_POWERUPS);

                                if (saveManager.saveGame(choice, state)) {
                                    saveLoadScreen.showStatus("Game saved to slot " + to_string(choice + 1) + "!", true);
                                    saveLoadScreen.refreshSlots();
                                }
                                else {
                                    saveLoadScreen.showStatus("Failed to save game!", false);
                                }
                            }
                            else {
                                if (saveManager.isSlotOccupied(choice)) {
                                    SaveableState state;
                                    if (saveManager.loadGame(choice, state)) {
                                        restoreGameState(state, spaceship, scoreManager, healthManager,
                                            levelManager, enemyCounter, resourceManager, gameClock,
                                            currentWave, enemiesPerWave, enemyBaseSpeed,
                                            enemiesSpawned, enemiesKilled,
                                            asteroids, easyEnemies, mediumEnemies, hardEnemies,
                                            bosses, BOSS_COUNT, resources, powerUps,
                                            MAX_RESOURCES, MAX_POWERUPS);
                                        isGameActive = true;
                                        gameState = 1;
                                        gameClock.resume();
                                        saveLoadScreen.showStatus("Game loaded!", true);
                                    }
                                    else {
                                        saveLoadScreen.showStatus("Failed to load game!", false);
                                    }
                                }
                                else {
                                    saveLoadScreen.showStatus("Slot is empty!", false);
                                }
                            }
                        }
                        else if (choice >= 10 && choice <= 12) {
                            int slot = choice - 10;
                            if (saveManager.deleteSave(slot)) {
                                saveLoadScreen.showStatus("Save deleted!", true);
                                saveLoadScreen.refreshSlots();
                            }
                            else {
                                saveLoadScreen.showStatus("Failed to delete save!", false);
                            }
                        }
                    }
                    else if (gameState == 4)
                    {
                        int choice = pauseMenu.handleClick(mousePos);
                        if (choice == 1) {
                            gameState = 1;
                            gameClock.resume();
                        }
                        else if (choice == 2) {
                            saveLoadScreen.setMode(true);
                            saveLoadScreen.refreshSlots();
                            previousGameState = 4;
                            gameState = 5;
                        }
                        else if (choice == 3)
                        {
                            gameState = 1;
                            gameClock.start();
                            resetGameState(spaceship, bullet, levelManager, spawnTracker,
                                asteroids, easyEnemies, mediumEnemies, hardEnemies, bosses, BOSS_COUNT,
                                scoreManager, healthManager, enemyCounter,
                                resourceManager, resources, powerUps, MAX_RESOURCES, MAX_POWERUPS,
                                currentWave, enemiesPerWave, enemyBaseSpeed,
                                enemiesSpawned, enemiesKilled, waveClock, spawningWave,
                                collisionCooldown, asteroidSpawnTimer, isGameActive, waveSpawnManager);
                        }
                        else if (choice == 4)
                            gameState = 0;
                    }
                    else if (gameState == 3)
                    {
                        int choice = gameOverScreen.handleClick(mousePos);
                        if (choice == 1)
                        {
                            highScores.addScore(
                                gameOverScreen.getPlayerName(),
                                scoreManager.getScore(),
                                gameOverScreen.getGameTime()
                            );
                        }
                        else if (choice == 2)
                        {
                            gameState = 0;
                            resetGameState(spaceship, bullet, levelManager, spawnTracker,
                                asteroids, easyEnemies, mediumEnemies, hardEnemies, bosses, BOSS_COUNT,
                                scoreManager, healthManager, enemyCounter,
                                resourceManager, resources, powerUps, MAX_RESOURCES, MAX_POWERUPS,
                                currentWave, enemiesPerWave, enemyBaseSpeed,
                                enemiesSpawned, enemiesKilled, waveClock, spawningWave,
                                collisionCooldown, asteroidSpawnTimer, isGameActive, waveSpawnManager);
                        }
                    }
                }

                if (event->is<Event::TextEntered>() && gameState == 3)
                {
                    auto* textEvent = event->getIf<Event::TextEntered>();
                    gameOverScreen.handleTextInput(static_cast<char>(textEvent->unicode));
                }

                if (event->is<Event::KeyPressed>())
                {
                    auto* key = event->getIf<Event::KeyPressed>();
                    if (key->code == Keyboard::Key::P && gameState == 1)
                    {
                        gameState = 4;
                        gameClock.pause();
                    }
                    else if (key->code == Keyboard::Key::P && gameState == 4)
                    {
                        gameState = 1;
                        gameClock.resume();
                    }

                    if (key->code == Keyboard::Key::F5 && gameState == 1)
                    {
                        SaveableState state = captureGameState(
                            spaceship, scoreManager, healthManager, levelManager,
                            enemyCounter, resourceManager, gameClock,
                            currentWave, enemiesPerWave, enemyBaseSpeed,
                            enemiesSpawned, enemiesKilled,
                            asteroids, easyEnemies, mediumEnemies, hardEnemies,
                            bosses, BOSS_COUNT, resources, powerUps,
                            MAX_RESOURCES, MAX_POWERUPS);
                        if (saveManager.saveGame(0, state)) {
                            cout << "[QUICKSAVE] Game saved to slot 1!\n";
                        }
                    }

                    if (key->code == Keyboard::Key::F9 && (gameState == 1 || gameState == 0))
                    {
                        if (saveManager.isSlotOccupied(0)) {
                            SaveableState state;
                            if (saveManager.loadGame(0, state)) {
                                restoreGameState(state, spaceship, scoreManager, healthManager,
                                    levelManager, enemyCounter, resourceManager, gameClock,
                                    currentWave, enemiesPerWave, enemyBaseSpeed,
                                    enemiesSpawned, enemiesKilled,
                                    asteroids, easyEnemies, mediumEnemies, hardEnemies,
                                    bosses, BOSS_COUNT, resources, powerUps,
                                    MAX_RESOURCES, MAX_POWERUPS);
                                isGameActive = true;
                                gameState = 1;
                                gameClock.resume();
                                cout << "[QUICKLOAD] Game loaded from slot 1!\n";
                            }
                        }
                    }


                    if (gameState == 1 && key->code == Keyboard::Key::Space && isGameActive && fireCooldown <= 0.f)
                    {
                        bullet.shoot(spaceship.getPosition(), spaceship.getGlobalBounds(), spaceship.getHasDoubleFire());
                        fireCooldown = spaceship.getFireDelay();
                        if (spaceship.getHasDoubleFire())
                            fireCooldown = spaceship.getFireDelay() * 0.5f;
                        else
                            fireCooldown = spaceship.getFireDelay();

                        soundManager.playBulletSound();
                    }
                }

                if (event->is<Event::MouseWheelScrolled>())
                {
                    auto* scrollEvent = event->getIf<Event::MouseWheelScrolled>();
                    float scrollDelta = scrollEvent->delta;

                    if (gameState == 2)
                    {
                        highScoresScreen.onScroll(scrollDelta);
                    }
                }
            }



            Vector2i mousePosI = Mouse::getPosition(window);
            Vector2f mousePos(static_cast<float>(mousePosI.x), static_cast<float>(mousePosI.y));

            if (gameState == 0)
                mainMenu.update(mousePos);
            else if (gameState == 2)
                highScoresScreen.update(mousePos);
            else if (gameState == 4)
                pauseMenu.update(mousePos);
            else if (gameState == 3)
                gameOverScreen.update(mousePos);
            else if (gameState == 5)
                saveLoadScreen.update(mousePos, deltaTime);

            if (collisionCooldown > 0)
                collisionCooldown--;

            if (gameState == 1 && isGameActive)
            {

                gameClock.update();
                g_slowMotion.update(deltaTime);

                if (fireCooldown > 0.f) fireCooldown -= deltaTime;

                spaceship.update(deltaTime);
                levelManager.update(deltaTime);

                int newLevel = levelManager.checkLevelUp(scoreManager.getScore());
                if (levelManager.isTransitioning()) {
                    if (newLevel != -1) {
                        for (int i = 0; i < 7; i++)
                        {
                            string asteroidSprite = "C:/proj_oop2/asteroid.png";
                            if (levelManager.getCurrentLevel() == 2)
                                asteroidSprite = "C:/proj_oop2/asteroid2.png";
                            else if (levelManager.getCurrentLevel() >= 3)
                                asteroidSprite = "C:/proj_oop2/asteroid3.png";
                            asteroids[i].setTexture(asteroidSprite);
                            asteroids[i].setHitPoints(levelManager.getAsteroidHitPoints());
                            asteroids[i].destroy();
                        }

                        for (int i = 0; i < 4; i++) easyEnemies[i].setHitPoints(levelManager.getEnemyHitPoints());
                        for (int i = 0; i < 6; i++) mediumEnemies[i].setHitPoints(levelManager.getEnemyHitPoints());
                        for (int i = 0; i < 8; i++) hardEnemies[i].setHitPoints(levelManager.getEnemyHitPoints());

                        if (levelManager.getCurrentLevel() == 4) {
                            for (int i = 0; i < BOSS_COUNT; ++i) {
                                bosses[i].setHitPoints(bosses[i].getMaxHitPointsForBoss());
                                bosses[i].spawn(1400, &spawnTracker);
                            }
                        }

                        spawnTracker.clearPositions();
                        for (int i = 0; i < 4; i++) easyEnemies[i].destroy();
                        for (int i = 0; i < 6; i++) mediumEnemies[i].destroy();
                        for (int i = 0; i < 8; i++) hardEnemies[i].destroy();
                        for (int i = 0; i < MAX_RESOURCES; ++i) resources[i].destroy();
                        for (int i = 0; i < MAX_POWERUPS; ++i) powerUps[i].destroy();
                    }

                    window.clear();
                    window.draw(screenRect);
                    spaceship.draw(window);
                    healthManager.draw(window);
                    scoreManager.draw(window);
                    resourceManager.draw(window);
                    enemyCounter.draw(window);
                    slowMoIndicator.draw(window, g_slowMotion.getIsActive(), g_slowMotion.getRemainingTime(), 8.0f);
                    gameClock.draw(window);
                    levelManager.drawTransition(window);
                    window.display();
                    continue;
                }

                spaceship.handleInput();
                spaceship.checkBoundary(1400, 800);
                bullet.update();

                int currentLevel = levelManager.getCurrentLevel();
                if (currentLevel == 1)
                {
                    currentEnemyCount = 4;
                    for (int i = 0; i < currentEnemyCount; i++)
                        currentEnemiesArray[i] = &easyEnemies[i];
                }
                else if (currentLevel == 2)
                {
                    currentEnemyCount = 6;
                    for (int i = 0; i < currentEnemyCount; i++)
                        currentEnemiesArray[i] = &mediumEnemies[i];
                }
                else if (currentLevel == 3)
                {
                    currentEnemyCount = 8;
                    for (int i = 0; i < currentEnemyCount; i++)
                        currentEnemiesArray[i] = &hardEnemies[i];
                }
                else if (currentLevel == 4)
                {
                    currentEnemyCount = BOSS_COUNT;
                    for (int i = 0; i < BOSS_COUNT; ++i)
                        currentEnemiesArray[i] = &bosses[i];
                }

                waveSpawnManager.updateResources(resources, deltaTime, 1400.f);
                waveSpawnManager.updatePowerUps(powerUps, deltaTime, 1400.f);

                for (int i = 0; i < MAX_RESOURCES; ++i) {
                    resources[i].update();
                    if (resources[i].isActive() &&
                        spaceship.getGlobalBounds().findIntersection(resources[i].getBounds()).has_value())
                    {

                        if (resources[i].getResourceType() == RESOURCE_TYPE_RED) {
                            processRedCrystalCollection(resourceManager, scoreManager);
                            soundManager.playPowerUpSound();
                        }
                        else {
                            processBlueCrystalCollection(resourceManager, scoreManager);
                            soundManager.playPowerUpSound();
                        }
                        resources[i].destroy();
                    }
                }

                for (int i = 0; i < MAX_POWERUPS; ++i) {
                    powerUps[i].update();
                    if (powerUps[i].isActive() &&
                        spaceship.getGlobalBounds().findIntersection(powerUps[i].getBounds()).has_value())
                    {
                        PowerUpType type = powerUps[i].getType();
                        cout << "[POWERUP] Collected: " << powerUps[i] << "\n";


                        if (type == SHIELD) spaceship.activateShield();
                        else if (type == DOUBLE_FIRE) spaceship.activateDoubleFire();
                        else if (type == INSTANT_REPAIR) {
                            spaceship.activateRepair();
                            healthManager.instantRepair();
                        }
                        else if (type == SLOW_MO) spaceship.activateSlowMo();
                        soundManager.playPowerUpSound();
                        powerUps[i].destroy();
                    }
                }

                int maxAsteroids = levelManager.getAsteroidCount();
                asteroidSpawnTimer++;
                int spawnRate = 200 - (levelManager.getCurrentLevel() * 20);
                if (asteroidSpawnTimer > spawnRate)
                {
                    for (int i = 0; i < maxAsteroids; i++)
                    {
                        if (!asteroids[i].isActive())
                        {
                            asteroids[i].spawn(1400);
                            break;
                        }
                    }
                    asteroidSpawnTimer = 0;
                }

                for (int i = 0; i < maxAsteroids; i++)
                {
                    asteroids[i].update();
                    if (bullet.checkCollision(asteroids[i].getBounds()))
                    {
                        if (asteroids[i].takeDamage())
                            scoreManager.addAsteroidScore();
                        bullet.deactivate();
                    }

                    if (asteroids[i].isActive() && collisionCooldown == 0 &&
                        spaceship.getGlobalBounds().findIntersection(asteroids[i].getBounds()).has_value())
                    {
                        if (!spaceship.getHasShield()) {
                            healthManager.damageFromAsteroid();
                            collisionCooldown = 30;
                        }
                        asteroids[i].destroy();
                    }
                }

                for (int i = 0; i < currentEnemyCount; i++)
                {
                    if (!currentEnemiesArray[i]->isActive())
                        currentEnemiesArray[i]->spawn(1400, &spawnTracker);

                    currentEnemiesArray[i]->update();

                    if (bullet.checkCollision(currentEnemiesArray[i]->getBounds()))
                    {
                        if (currentEnemiesArray[i]->takeDamage())
                        {
                            scoreManager.addEnemyScore();
                            enemyCounter.incrementEnemyCount();
                        }
                        bullet.deactivate();
                    }

                    if (currentEnemiesArray[i]->isActive() && collisionCooldown == 0 &&
                        spaceship.getGlobalBounds().findIntersection(currentEnemiesArray[i]->getBounds()).has_value())
                    {
                        if (!spaceship.getHasShield()) {
                            healthManager.damageFromEnemy();
                            collisionCooldown = 30;
                        }
                        currentEnemiesArray[i]->destroy();
                    }
                }

                if (levelManager.getCurrentLevel() == 4 && scoreManager.getScore() >= 300) {
                    isGameActive = false;
                    gameState = 3;
                    gameClock.stop();
                    int timeTaken = gameClock.getElapsedSeconds();
                    gameOverScreen.displayWin(scoreManager.getScore(), levelManager.getCurrentLevel(), timeTaken);
                }
                else if (enemyCounter.hasWon() || !healthManager.isAlive())
                {
                    isGameActive = false;
                    gameState = 3;
                    gameClock.stop();
                    int timeTaken = gameClock.getElapsedSeconds();
                    if (enemyCounter.hasWon())
                        gameOverScreen.displayWin(scoreManager.getScore(), levelManager.getCurrentLevel(), timeTaken);
                    else
                        gameOverScreen.displayLose(scoreManager.getScore(), levelManager.getCurrentLevel(), timeTaken);
                }
            }

            window.clear();
            window.draw(screenRect);

            if (gameState == 0)
            {
                mainMenu.draw(window);
            }
            else if (gameState == 2)
            {
                highScoresScreen.draw(window);
            }
            else if (gameState == 5)
            {
                saveLoadScreen.draw(window);
            }
            else if (gameState == 1 || gameState == 4)
            {
                for (int i = 0; i < MAX_RESOURCES; ++i) resources[i].draw(window);
                for (int i = 0; i < MAX_POWERUPS; ++i) powerUps[i].draw(window);

                for (int i = 0; i < levelManager.getAsteroidCount(); i++)
                    asteroids[i].draw(window);

                for (int i = 0; i < currentEnemyCount; i++)
                {
                    if (currentEnemiesArray[i]->isActive())
                        currentEnemiesArray[i]->draw(window);
                }

                spaceship.draw(window);
                bullet.draw(window);
                scoreManager.draw(window);
                healthManager.draw(window);
                enemyCounter.draw(window);
                resourceManager.draw(window);
                gameClock.draw(window);
                slowMoIndicator.draw(window, g_slowMotion.getIsActive(), g_slowMotion.getRemainingTime(), 8.0f);
                levelManager.drawTransition(window);

                if (gameState == 4)
                    pauseMenu.draw(window);
            }
            else if (gameState == 3)
            {
                gameOverScreen.draw(window);
            }

            window.display();
        }

        return 0;
    }
    catch (const GameException& e) {
        cerr << "[FATAL GAME ERROR] " << e.what() << endl;
        return 1;
    }
    catch (const exception& e) {
        cerr << "[FATAL ERROR] " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "[FATAL ERROR] Unknown exception occurred" << endl;
        return 1;
    }
}