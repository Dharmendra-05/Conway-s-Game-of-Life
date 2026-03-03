/**
 * ============================================================
 *  Conway's Game of Life — Terminal Renderer
 * ============================================================
 *  Classic cellular automaton with:
 *   - Random or preset pattern seeding
 *   - Live terminal rendering with ANSI colors
 *   - Generation counter & live cell stats
 *   - Wrapping toroidal grid (edges connect)
 *
 *  Rules:
 *   1. Live cell with <2 neighbours  → dies (underpopulation)
 *   2. Live cell with 2-3 neighbours → survives
 *   3. Live cell with >3 neighbours  → dies (overpopulation)
 *   4. Dead cell with =3 neighbours  → becomes alive (reproduction)
 *
 *  Compile: g++ -O2 -std=c++17 game_of_life.cpp -o game_of_life
 *  Run:     ./game_of_life
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#ifdef _WIN32
  #include <windows.h>
  #define SLEEP_MS(ms) Sleep(ms)
#else
  #include <thread>
  #include <chrono>
  #define SLEEP_MS(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#endif
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <thread>

// ─── Config ───────────────────────────────────────────────────
constexpr int  WIDTH       = 60;
constexpr int  HEIGHT      = 30;
constexpr int  MAX_GENS    = 300;
constexpr int  DELAY_MS    = 80;
constexpr char ALIVE       = '#';
constexpr char DEAD        = '.';

// ANSI color codes
const std::string GREEN  = "\033[32m";
const std::string CYAN   = "\033[36m";
const std::string YELLOW = "\033[33m";
const std::string RESET  = "\033[0m";
const std::string CLEAR  = "\033[2J\033[H";
const std::string BOLD   = "\033[1m";

// ─── Grid type ────────────────────────────────────────────────
using Grid = std::vector<std::vector<int>>;

Grid make_grid() {
    return Grid(HEIGHT, std::vector<int>(WIDTH, 0));
}

// ─── Seeding ──────────────────────────────────────────────────

void seed_random(Grid& g, double density = 0.30) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int r = 0; r < HEIGHT; ++r)
        for (int c = 0; c < WIDTH; ++c)
            g[r][c] = (std::rand() / double(RAND_MAX) < density) ? 1 : 0;
}

// Place a pattern at (row, col) from a list of {dr, dc} offsets
void place_pattern(Grid& g, int row, int col,
                   const std::vector<std::pair<int,int>>& cells) {
    for (size_t i = 0; i < cells.size(); ++i) {
        int r = (row + cells[i].first  + HEIGHT) % HEIGHT;
        int c = (col + cells[i].second + WIDTH)  % WIDTH;
        g[r][c] = 1;
    }
}

void seed_glider(Grid& g) {
    // Famous glider pattern (moves diagonally)
    place_pattern(g, 5, 5, {{0,1},{1,2},{2,0},{2,1},{2,2}});
}

void seed_pulsar(Grid& g) {
    // Pulsar — period-3 oscillator
    seed_random(g, 0.20);
    // Add a glider gun cluster for visual interest
    std::vector<std::pair<int,int>> pulsar = {
        {0,2},{0,3},{0,4},{0,8},{0,9},{0,10},
        {2,0},{2,5},{2,7},{2,12},
        {3,0},{3,5},{3,7},{3,12},
        {4,0},{4,5},{4,7},{4,12},
        {5,2},{5,3},{5,4},{5,8},{5,9},{5,10},
        {7,2},{7,3},{7,4},{7,8},{7,9},{7,10},
        {8,0},{8,5},{8,7},{8,12},
        {9,0},{9,5},{9,7},{9,12},
        {10,0},{10,5},{10,7},{10,12},
        {12,2},{12,3},{12,4},{12,8},{12,9},{12,10}
    };
    place_pattern(g, HEIGHT/2 - 6, WIDTH/2 - 6, pulsar);
}

// ─── Simulation ───────────────────────────────────────────────

int count_neighbours(const Grid& g, int r, int c) {
    int count = 0;
    for (int dr = -1; dr <= 1; ++dr)
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = (r + dr + HEIGHT) % HEIGHT;  // toroidal wrap
            int nc = (c + dc + WIDTH)  % WIDTH;
            count += g[nr][nc];
        }
    return count;
}

Grid next_generation(const Grid& cur) {
    Grid next = make_grid();
    for (int r = 0; r < HEIGHT; ++r) {
        for (int c = 0; c < WIDTH; ++c) {
            int n = count_neighbours(cur, r, c);
            if (cur[r][c] == 1)
                next[r][c] = (n == 2 || n == 3) ? 1 : 0;
            else
                next[r][c] = (n == 3) ? 1 : 0;
        }
    }
    return next;
}

int count_alive(const Grid& g) {
    int total = 0;
    for (const auto& row : g)
        for (int cell : row)
            total += cell;
    return total;
}

// ─── Rendering ────────────────────────────────────────────────

void render(const Grid& g, int gen, int alive) {
    std::string frame;
    frame.reserve((WIDTH + 3) * (HEIGHT + 5));

    frame += CLEAR;
    frame += BOLD + CYAN;
    frame += "  Conway's Game of Life  |  ";
    frame += YELLOW + "Gen: " + std::to_string(gen);
    frame += CYAN  + "  |  ";
    frame += GREEN + "Alive: " + std::to_string(alive);
    frame += RESET + "\n";

    // Top border
    frame += CYAN + "  +" + std::string(WIDTH, '-') + "+\n" + RESET;

    for (int r = 0; r < HEIGHT; ++r) {
        frame += CYAN + "  |" + RESET;
        for (int c = 0; c < WIDTH; ++c) {
            if (g[r][c])
                frame += GREEN + ALIVE;
            else
                frame += RESET + DEAD;
        }
        frame += CYAN + "|" + RESET + "\n";
    }

    // Bottom border
    frame += CYAN + "  +" + std::string(WIDTH, '-') + "+\n" + RESET;
    frame += "  Press Ctrl+C to quit\n";

    std::cout << frame << std::flush;
}

// ─── Main ─────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    // Choose seed mode: "./game_of_life glider" or "./game_of_life pulsar"
    std::string mode = (argc > 1) ? std::string(argv[1]) : "random";

    Grid grid = make_grid();

    if (mode == "glider") {
        seed_glider(grid);
        std::cout << "Seeding: Glider\n";
    } else if (mode == "pulsar") {
        seed_pulsar(grid);
        std::cout << "Seeding: Pulsar + random background\n";
    } else {
        seed_random(grid, 0.30);
        std::cout << "Seeding: Random (30% density)\n";
    }

    SLEEP_MS(500);

    for (int gen = 0; gen <= MAX_GENS; ++gen) {
        int alive = count_alive(grid);

        render(grid, gen, alive);

        if (alive == 0) {
            std::cout << YELLOW << "\n  All cells dead. Simulation ended at gen "
                      << gen << ".\n" << RESET;
            break;
        }

        grid = next_generation(grid);
        SLEEP_MS(DELAY_MS);
    }

    std::cout << "\n  Simulation complete.\n";
    return 0;
}