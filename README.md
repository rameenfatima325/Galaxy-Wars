# Galaxy Wars – The Object-Oriented Space Battle

Galaxy Wars is a 2D space survival shooter game developed in **C++ using SFML** as part of the Object-Oriented Programming course project at FAST-NUCES.

The game combines action-packed gameplay with advanced Object-Oriented Programming concepts including inheritance, polymorphism, abstraction, encapsulation, operator overloading, exception handling, file handling, deep copying, dynamic memory management, and more.

---

# Developers

- Rameen Fatima (24I-0782)
- Aizah Atif (24I-0840)

---

# Game Overview

In Galaxy Wars, the player controls a spaceship that must survive increasingly difficult enemy waves while collecting resources and power-ups.

The game features:
- Multiple enemy types
- Boss battles
- Dynamic level progression
- Power-ups
- Save/Load system
- High score system
- Background music and sound effects
- Complete GUI system

The objective is to:
- Survive enemy attacks
- Destroy asteroids and enemy ships
- Collect crystals and power-ups
- Reach score milestones
- Defeat enemies and win the final boss battle

---

# Controls

| Key | Action |
|------|--------|
| Arrow Keys | Move spaceship |
| Spacebar | Shoot bullets |
| P | Pause / Resume game |
| F5 | Quick Save |
| F9 | Quick Load |

---

# Features

## Core Gameplay
- Smooth spaceship movement
- Bullet shooting system
- Increasing difficulty progression
- Collision detection system
- Real-time HUD

---

## Enemy System

### Easy Enemies
- Basic movement
- 1 HP
- Slow speed

### Medium Enemies
- Faster movement
- 2 HP
- More aggressive behavior

### Hard Enemies
- Circular movement patterns
- 3 HP
- Unpredictable paths

### Boss Enemy
- 10 HP
- Zigzag movement
- Teleportation mechanics
- Shrinks as it takes damage

---

## Asteroids
- Random spawning
- Different textures for levels
- Increasing HP by level
- Damage player on collision

---

# Resource System

## Collectible Crystals
- Red Crystal → +15 points
- Blue Crystal → +20 points

### Milestone Bonus System
Every 10 collected resources gives bonus score:
- 10 resources → +10 bonus
- 20 resources → +20 bonus
- 30 resources → +30 bonus

---

# Power-Ups

## Shield
- Protects from damage for 15 seconds
- Cyan spaceship glow
- Shield animation

## Double Fire
- Faster shooting
- Larger bullets
- Yellow spaceship effect

## Instant Repair
- Restores health instantly

## Slow Motion
- Slows all enemies and obstacles
- Magenta visual effect
- Timer indicator

---

# Level System

## Level 1 – Easy Mode
- Easy enemies
- 3 asteroids maximum
- Beginner-friendly gameplay

## Level 2 – Medium Mode
- Faster enemies
- 5 asteroids
- Increased HP

## Level 3 – Hard Mode
- Circular enemy movement
- 7 asteroids
- Intense gameplay

## Level 4 – Boss Battle
- Final boss enemy
- No asteroids
- Reach 300 score to win

---

# Health System

- Player starts with 100 HP
- Asteroid collision → -5 HP
- Enemy collision → -10 HP
- Real-time health bar
- Game over at 0 HP

---

# Save & Load System

Features:
- 3 save slots
- Quick save/load support
- Delete save option
- Full game state saving

Saved data includes:
- Player position
- Health
- Score
- Power-up states
- Enemy states
- Resources
- Timer
- Level progression

---

# High Scores System

- Stores Top 50 scores
- Saves:
  - Player name
  - Score
  - Time taken
- Automatically sorted

---

# GUI System

The game contains multiple interactive screens:

- Main Menu
- Pause Menu
- Save/Load Screen
- High Scores Screen
- Victory Screen
- Game Over Screen
- Level Transition Screens
- Real-time HUD

---

# Sound System

The game includes:
- Background music
- Shooting sounds
- Menu click sounds
- Power-up sounds
- Interactive audio feedback

---

# OOP Concepts Implemented

This project demonstrates major Object-Oriented Programming concepts:

## Classes & Objects
## Encapsulation
## Inheritance
## Multilevel Inheritance
## Polymorphism
## Abstraction
## Friend Functions
## Static Members
## Const Correctness
## Operator Overloading
## Composition & Aggregation
## Copy Constructors
## Deep Copying
## Virtual Destructors
## Dynamic Memory Management
## Function Overloading
## Exception Handling
## File Handling

---

# Project Architecture

The project contains **40+ classes and structs**, including:

## Manager Classes
- SoundManager
- ScoreManager
- HealthManager
- ResourceManager
- GameSaveManager
- HighScoresManager
- SlowMotionManager
- EnemyCounter

## Game Object Classes
- Spaceship
- Bullet
- Asteroid
- EnemyShip
- EasyEnemy
- MediumEnemy
- HardEnemy
- BossEnemy
- PowerUp
- ResourceObject

## UI Classes
- Button
- MainMenu
- PauseMenu
- SaveLoadScreen
- HighScoresScreen
- GameOverScreen

---

# Exception Handling

Custom exception hierarchy implemented for:
- File loading errors
- Texture loading errors
- Invalid operations
- Save/load failures
- Null pointer access
- Index out-of-bounds access

The game uses try-catch blocks throughout the project to prevent crashes and ensure stability.

---

# Technologies Used

- C++
- SFML
- Object-Oriented Programming
- File Handling
- Dynamic Memory Management

---

# How to Run the Game

## Requirements
- Visual Studio 2022
- SFML 3.0.2
- Windows OS

---

## Setup Instructions

### Clone the Repository

```bash
git clone https://github.com/rameenfatima325/Galaxy-Wars.git
```

---

### Open the Project

Navigate to:

```txt
proj_oop2/proj_oop2/
```

Open:

```txt
proj_oop2.sln
```

in Visual Studio.

---

### Keep Folder Structure Intact

Do NOT rename or move any folders/files.

The game relies on the existing directory structure for:
- sprites
- textures
- sounds
- fonts
- save files

---

### Configure Visual Studio

Set:

```txt
Configuration: Debug
Platform: x64
```

---

### Run the Game

Press:

```txt
Ctrl + F5
```

or click:

```txt
Local Windows Debugger
```

The game should launch successfully.

---

# Important Notes

- SFML DLL files are already included in the project folder.
- Save files and high scores are automatically created and updated.
- If textures or sounds fail to load, ensure the folder structure has not been modified.

---


# Challenges Solved

Some major technical challenges solved during development:

- Enemy spawn overlap prevention
- Slow motion affecting all game objects
- Boss teleportation system
- Deep copy game saving
- Wave-based spawning
- Multiple simultaneous power-ups
- Level transition countdown system
- Comprehensive exception handling

---

# Academic Purpose

This project was developed for the **Object-Oriented Programming** course at FAST-NUCES and demonstrates practical application of advanced OOP concepts in game development.

---

# Conclusion

Galaxy Wars successfully combines engaging gameplay with strong software engineering principles and Object-Oriented Design.

The project exceeds the course requirements by implementing:
- 40+ classes
- Advanced inheritance hierarchies
- Save/load system
- GUI framework
- Power-up mechanics
- Boss AI
- Audio system
- Exception handling
- File handling
- Dynamic gameplay systems

It represents a complete and scalable object-oriented game architecture built entirely in C++ and SFML.

---
