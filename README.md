# Conway's Game of Life — Terminal Renderer

A C++ implementation of Conway's Game of Life with live ANSI terminal rendering, generation tracking, and multiple seed patterns.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

---

## What Is Conway's Game of Life?

Conway's Game of Life is a **cellular automaton** devised by mathematician John Horton Conway in 1970. It is a zero-player game — meaning its evolution is determined entirely by its initial state, with no further input.

The universe is a 2D grid of cells, each either **alive** or **dead**. Each generation applies four simple rules simultaneously to every cell:

| Rule | Condition | Outcome |
|------|-----------|---------|
| Underpopulation | Live cell with fewer than 2 live neighbours | Dies |
| Survival        | Live cell with 2 or 3 live neighbours       | Survives |
| Overpopulation  | Live cell with more than 3 live neighbours  | Dies |
| Reproduction    | Dead cell with exactly 3 live neighbours    | Becomes alive |

From these four rules, remarkably complex and beautiful patterns emerge.

---

## Features

- Live ANSI color terminal rendering (green cells on dark background)
- Toroidal (wrapping) grid — edges connect to the opposite side
- Generation counter and live cell count displayed each frame
- Three seed modes: random, glider, and pulsar
- Configurable grid size, speed, and density via compile-time constants

---

## Build and Run

**Requirements:** C++17 compiler (GCC, Clang, or MSVC), a terminal with ANSI support

```bash
# Clone the repo
git clone https://github.com/YOUR_USERNAME/YOUR_REPO.git
cd YOUR_REPO

# Compile
g++ -O2 -std=c++17 game_of_life.cpp -o game_of_life

# Run with random seed (default)
./game_of_life

# Run with a glider
./game_of_life glider

# Run with a pulsar + random background
./game_of_life pulsar
```

> Press **Ctrl+C** to exit at any time.

---

## Seed Modes

| Mode | Description |
|------|-------------|
| `random` | 30% cell density, randomly distributed (default) |
| `glider`  | Single glider — travels diagonally across the grid |
| `pulsar`  | Period-3 oscillator placed at centre + random background |

---

## Configuration

You can tweak these constants at the top of `game_of_life.cpp`:

```cpp
constexpr int  WIDTH     = 60;    // Grid width
constexpr int  HEIGHT    = 30;    // Grid height
constexpr int  MAX_GENS  = 300;   // Max generations before stopping
constexpr int  DELAY_MS  = 80;    // Milliseconds between frames
```

---

## How It Works

```
Seed grid → Compute neighbour counts → Apply rules → Render → Repeat
```

- **Grid**: A 2D vector of 0s (dead) and 1s (alive)
- **Toroidal wrapping**: Neighbour lookup uses modulo arithmetic so the grid wraps at the edges
- **Double buffering**: Each generation reads from the current grid and writes to a new one — cells don't affect each other mid-step
- **Rendering**: Each frame is built into a single string and flushed at once to avoid flicker

---

## File Structure

```
.
+-- game_of_life.cpp   # Full simulation and renderer
+-- README.md          # This file
```

---

## Concepts Demonstrated

- **Cellular automata** and emergent complexity
- **2D grid simulation** with boundary wrapping
- **Double-buffered updates** for correct simultaneous rule application
- **ANSI escape codes** for terminal colors and cursor control
- **Command-line argument parsing**
- **std::this_thread::sleep_for** for frame-rate control

---

## Ideas to Extend This

- Add an interactive mode to draw cells with the keyboard or mouse
- Load patterns from `.rle` (Run Length Encoded) files from LifeWiki
- Track and display population history as a mini graph
- Detect stable states and still lifes automatically
- Port to SDL2 or SFML for a graphical window renderer

---

## License

MIT License — free to use, modify, and distribute.
