#pragma once

#include <SFML/Graphics.hpp>
#include "bit_manip.hpp"
#include "rand_bool.hpp"

typedef uint8_t Cell;

typedef unsigned int uint;
typedef uint_fast32_t idx_t;

class Grid
{
private:
    sf::VertexArray graphGrid;
    Cell *cellArray = nullptr;

    uint width_cells;
    uint height_cells;

    sf::Color alive_color = sf::Color::White;
    sf::Color dead_color = sf::Color::Black;

    idx_t getIndex(uint x, uint y) { return y * width_cells + x; }

    bool isAlive(idx_t idx) { return bit::read(cellArray[idx], 7); }
    bool isAlive(uint x, uint y) { return bit::read(cellArray[getIndex(x, y)], 7); }
    bool isAlive(Cell &num) { return bit::read(num, 7); }
    void setCell(idx_t idx);
    void clearCell(idx_t idx);

    void calculateBorders();
    void calculateNeighbors();
    void updateCells();
    void updateCellsSkip();

public:
    Grid() {}
    Grid(uint init_width_cells, uint init_height_cells);
    ~Grid() { delete[] cellArray; }
    void nextFrame();
    void nextFrameSkip(uint n); // calculates n frames ahead and calculates and displays the n+1 frame (nextFrameSkip(0) is the same as nextFrame())
    void clearGrid();
    void randGrid();
    sf::VertexArray &getGrid() { return graphGrid; };
    void toggleCell(sf::Vector2u coor);
};

