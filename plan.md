# ATFATF — *And Thanks for All the Fish*

- parse map (Sun) ok
- print map in terminal (temp) (Sun) ok
- logger ok
- ===GRAPHICS===
- display map in window (Tue) ok
- move player (Tue) ok
- collect fish (Tue) ok
- ===SOUNDS===
- exit to next level (Wed) ok
- sharks_pt1 (Wed) ok
- time per level (Wed) ok
- cheats (Wed) ok
- ===TEXT===
- score (Wed) ok
- menu pt1 (Thu) ok
- menu pt2 (Sat) ok
- lives (Sun) ok
- mini reset and game loss / victory (Sun) ok
- enter name (and campus and choose coalition) (Sun) ok
- record score (Sun) ok 
- highscore State (Sun) ok
- sharks_pt2 (Mon)
- ===FUTURE===
- license (Mon)
- random level generation ()
- launcher
- recording
- replay
- quicksave/quickload
- big maps
- fine movements
- new enemies (always chase)
- stun shark with sonic burst
- shark dies if it cannot move for too long (no oxygen) - 3 consequtive hits or so

## Game concept

ATFATF is a 2D, grid-based arcade game that combines Pac-Man's constant
movement through a maze with Bomberman's tactical, temporary hazards. The
player is a small fish escaping an aquarium labyrinth before it is drained.
Collect stranded fish, open routes, avoid roaming keepers, and use bubbles to
briefly clear space or expose a safe path.

The aim is to rescue every fish and reach the exit. A level should reward
quick navigation, but never make progress depend only on reflexes: well-placed
bubbles and clever route choices create safe opportunities.

## Core game loop

1. Explore the maze and collect fish.
2. Avoid enemies and environmental hazards.
3. Place a limited bubble charge on the current tile.
4. After a short fuse, its cross-shaped burst travels through open tiles,
   stopping at walls and breaking fragile obstacles.
5. Use the newly opened route and the momentarily stunned enemies to collect
   the remaining fish and reach the exit.

The player is harmed by enemies and their own bubble burst, so each charge is
a tactical decision rather than a universal attack.

## Proposed modular structure

### 1. Core / engine

- `engine`
  - main loop, state switching, frame timing
  - game states: menu, playing, paused, level complete, game over
- `input`
  - keyboard/controller actions, independent of the renderer
- `render`
  - terminal and SDL renderers behind `IRenderer`
- `audio`
  - sound effects and music behind `IAudio`
- `core`
  - shared grid, position, direction, timing, and collision helpers

### 2. World and levels

- `map`
  - static tile layout and map loading/saving
  - tile types: solid wall, fragile coral, open water, spawn, exit
- `maze`
  - maze generation and editing tools
  - validates that required fish and the exit remain reachable
- `level`
  - level rules, objectives, spawn tables, and progression

### 3. Gameplay entities

- `player` (the dolphin)
  - grid movement, facing, lives, score, animation
  - collects fish and power-ups; places bubble charges
- `rescue`
  - collectible stranded fish and optional bonus creatures
  - tracks the remaining rescue objective
- `enemy`
  - keeper/crab-style patrols with distinct behaviours
  - states: patrol, chase, investigate, stunned, returning
- `bubble`
  - charge placement, fuse, propagation, burst lifetime
  - interaction with players, enemies, fragile coral, and pickups
- `pickup`
  - optional abilities such as an extra charge, longer burst, speed boost, or
    temporary invulnerability

### 4. Game rules and events

- `Game` / `ATFATFGame`
  - owns the active level and orchestrates update order
- `Event`
  - typed gameplay events: fish rescued, bubble detonated, wall broken,
    enemy stunned, player hit, level completed
- `collision`
  - one authoritative set of tile/entity interaction rules
- `debug`
  - maze snapshots and replay/history support for validating generation and
    debugging difficult levels

## Suggested data ownership

```text
Engine
└── ATFATFGame
    ├── Level (Map + objectives)
    ├── Player
    ├── RescueManager
    ├── EnemyManager
    ├── BubbleManager
    └── EventQueue
```

Entities should ask the map whether a tile is traversable; they should not
modify map data directly. Bubble effects are resolved by `BubbleManager` and
reported as events, which keeps entity interactions predictable and testable.

## Minimum viable version

- One hand-authored level with walls, fragile coral, fish, an exit, and one
  enemy type.
- Four-direction grid movement and collision.
- A single active bubble charge with a fuse and cross-shaped burst.
- Bubble bursts break coral and stun enemies, but hurt the player.
- Win when all fish are rescued and the player reaches the exit.
- SDL rendering first; terminal rendering remains useful for development and
  gameplay tests.

## Later additions

- Multiple enemies with distinct patrol/chase rules.
- Procedurally generated, validated levels.
- Chain reactions between bubble charges.
- More rescue objectives, items, hazards, score targets, and local co-op.
- Better animations, sound, menus, and a level editor workflow.

## Design principles

- Keep rendering, input, audio, map data, and gameplay rules independent.
- Prefer events and small manager interfaces over direct cross-entity access.
- Keep all tile effects deterministic so a level can be replayed and tested.
- Add mechanics only when they create an interesting navigation or timing
  choice.
