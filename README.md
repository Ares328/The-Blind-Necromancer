# The Blind Necromancer

_The Blind Necromancer_ is a turn-based, text-driven dungeon game written in modern C++ (C++20).  
You play as a necromancer navigating in darkness, “seeing” the world through magical pulses, elemental interactions, and the movements of your undead summons.

This project is designed both as a playable prototype and as a portfolio piece demonstrating game systems and test-driven development in C++.
It is very much still a demo and is still being worked on.

---

## Core Fantasy

You cannot see the world directly.

Instead, you:

- **Pulse** to sense doors, hostiles, traps, and environmental features around you.
- **Manipulate elements**: fire, poison, water, and more via spells and traps.
- **Raise and command undead minions** who can scout, fight, guard, and move on your orders.

Everything is described through text output, making world feedback and combat narration central to the experience.

---

## Features

### Movement & Exploration

- Grid-based movement with 8 directions:
  - `north`, `south`, `east`, `west`, `north-east`, `north-west`, `south-east`, `south-west`.
- Map loaded from ASCII layouts (see `Game::InitializeMap` and `Map::LoadFromAscii`).
- Walkable and non-walkable tiles:
  - Floor, wall, door, trap, fireplace.

### Pulse (Echolocation)

The `pulse` command sends out a magical wave that:

- Finds tiles reachable within a radius (BFS).
- Describes:
  - **Doors**: where they are and in which direction.
  - **Fireplaces**: when burning, with flavor text about warmth and crackling.
  - **Entities**: counts and directions of hostiles and friendlies.
  - **Traps**: including whether they are fire or poison traps.

Pulse is the primary way you “see” as a blind necromancer.

### Elements & Spells

- **Status effects** managed via `StatusEffect` and `EnvironmentSystem`:
  - `OnFire`, `Poisoned`, `Wet`, etc.
- **Spells** via the `cast` command (`CastSpell`):
  - `cast water <direction>`:
    - Extinguishes burning tiles and entities.
    - Clears fire traps (and converts them to floor).
    - Extinguishes fireplaces and can turn them into safe tiles, depending on state.
  - `cast fire <direction>`:
    - Ignites tiles / fireplaces (where supported).
- Status effects do damage over time and produce per-turn flavor text, with separate descriptions for player and entities.

### Traps

- Fire and poison traps placed on the map (from ASCII or spawned in code).
- Stepping onto a trap applies the corresponding status effect:
  - Handled via `EnvironmentSystem`.
- Pulse reveals trap locations and type (fire vs poison) in a radius, if reachable.

### Fireplaces

- Special tile type `Fireplace`.
- Can be:
  - **Lit**: deals fire status when stepped on.
  - **Extinguished**: safe to stand on.
- Interacts with spells:
  - Water can extinguish a lit fireplace.
  - Fire can ignite an unlit fireplace (depending on your current logic).
- Pulse describes nearby lit fireplaces with directional, atmospheric text.

### Summons & Commands

You can summon undead allies and issue commands:

- `summon skeleton` – calls a friendly undead next to you (or nearest free adjacent tile).
- Summons have:
  - HP, attack damage, AI state (`FollowPlayer`, `Guard`, `Attack`), and aggro range.
- `command` system (`CommandAction::SummonCommand`):
  - `command all follow` – minions follow you.
  - `command all guard` – minions guard their current positions.
  - `command all attack` – minions seek and attack hostiles.
  - `command <name> move <direction>` – move a specific minion one tile (e.g. `command skely move east`).
    - Used for precise positioning, e.g. walking a minion into a trap or fireplace.
- Summon and hostile behavior per turn is handled by:
  - `SummonsAISystem::ProcessSummonedTurn`
  - `HostileAISystem::ProcessHostileTurn`

### Hostile AI

- Hostiles have an aggro radius and will approach / attack the player or summons.
- Basic chase and attack behavior built on the map/pathfinding systems.
- Integrates with status effects and environment (fire, poison, traps). (they will walk into traps)

---

## Commands Overview

Examples of in-game commands:

- **Pulse**
  - `pulse`  
  - `pulse 5`  
- **Movement**
  - `move north`
  - `move south-east`
- **Spells**
  - `cast fire east`
  - `cast water self`
- **Summoning**
  - `summon skeleton`
- **Melee**
  - `attack north`
- **Minion Commands**
  - `command all follow`
  - `command all guard`
  - `command all attack`
  - `command skely move east`
- **Wait**
  - `wait` (you gather your thoughts and uh, yup that's it)

---

## Architecture Highlights & diagram

- **Core types**:
  - `Actor` – shared base for `Player` and `Entity`, with HP, status, and position.
  - `Player` – the necromancer.
  - `Entity` – hostile or friendly units (AI state, faction, combat stats).
  - `Map` – tile data, tile states, ASCII loading, and direction utilities.
- **Systems**:
  - `HostileAISystem` – enemy turns.
  - `SummonsAISystem` – minion turns.
  - `EnvironmentSystem` – applies status effects, damage over time, and related messaging each turn.
- **Pathfinding**:
  - `Pathfinder::BfsReachable` – BFS for reachable tiles (used by pulse).
  - `Pathfinder::FindFirstStepTowards` – pathfinding for AI movement.
  
- **High-level architecture diagram**:
- [Show class diagram (Google Drive)](https://drive.google.com/file/d/1LnlGdhzL7S6p9jxrtBrhepLgKeYekFbF/view?usp=sharing)
- **Turn sequence diagram**:
- [Show class diagram (Google Drive)](https://drive.google.com/file/d/1Q83ZMkKHPUaamGqYvijAsCFLLKr5EHM5/view?usp=sharing)

---

## Testing

The project includes unit tests using **GoogleTest**:

- **Summons**: `SummonCommandTest.cpp`
  - Verifies summon follow/guard/attack behavior, movement commands, and error handling.
- **Traps**: `TrapTest.cpp`
  - Ensures stepping on fire/poison traps applies correct status and messages.
- **Fireplaces**: `Fireplace.cpp`
  - Player and summons taking damage from lit fireplaces.
  - Fireplaces interacting with water/fire spells.
- **Movement & Pulse**:
  - Movement edge cases and collision.
  - Pulse output and detection counts for entities, traps, and doors.

Tests act as executable documentation of the rules and interactions.

---

## Building & Running

### Requirements

- C++20-capable compiler (MSVC in Visual Studio 2022).
- GoogleTest (already integrated via the test project).
- httplib only needed for NecroClient

## Possible Future Work

Some ideas that could be explored next:

- More environmental objects (e.g. wells, braziers, crumbling walls).
- Inventory management.
- Additional elements and status effects (e.g. frost, curses).
- More complex AI behaviors (fleeing, formations, smarter guarding) and/or ranged enemies.
- Multiple maps (progression) or Procedural map generation on top of the ASCII-driven system.
- Enhanced narrative / story beats triggered by progression.
