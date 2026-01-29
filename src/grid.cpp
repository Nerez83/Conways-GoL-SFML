#include "../include/grid.hpp"

Grid::Grid(uint init_width_cells, uint init_height_cells)
    : graphGrid(sf::VertexArray(sf::PrimitiveType::Triangles, init_width_cells * init_height_cells * 6)), cellArray(new Cell[init_width_cells * init_height_cells]), width_cells(init_width_cells), height_cells(init_height_cells)
{
    idx_t idx = 0;
    for (uint x = 0; x < init_width_cells; x++)
    {
        for (uint y = 0; y < init_height_cells; y++)
        {
            idx = x + y * init_width_cells;
            cellArray[idx] = 0;
            idx *= 6;

            // first triangle
            graphGrid[idx].position = {float(x), float(y)};
            graphGrid[idx + 1].position = {float(x + 1), float(y)};
            graphGrid[idx + 2].position = {float(x + 1), float(y + 1)};

            // second triangle
            graphGrid[idx + 3].position = {float(x + 1), float(y + 1)};
            graphGrid[idx + 4].position = {float(x), float(y + 1)};
            graphGrid[idx + 5].position = {float(x), float(y)};

            for (int i = 0; i < 6; i++)
                graphGrid[idx + i].color = dead_color;
        }
    }
}
void Grid::nextFrame()
{
    calculateBorders();
    calculateNeighbors();
    updateCells();
}
void Grid::nextFrameSkip(uint n)
{
    for (uint i = 0; i < n; i++)
    {
        calculateBorders();
        calculateNeighbors();
        updateCellsSkip();
    }
    nextFrame();
}
void Grid::setCell(idx_t idx)
{
    bit::set(cellArray[idx], 7);
    for (int i = 0; i < 6; i++)
        graphGrid[idx * 6 + i].color = alive_color;
}
void Grid::clearCell(idx_t idx)
{
    bit::clear(cellArray[idx], 7);
    for (int i = 0; i < 6; i++)
        graphGrid[idx * 6 + i].color = dead_color;
}
void Grid::clearGrid()
{
    for (idx_t i = 0; i < width_cells * height_cells; i++)
        clearCell(i);
}
void Grid::randGrid()
{
    RandBool rand_bool = RandBool();
    for (idx_t i = 0; i < width_cells * height_cells; i++)
        rand_bool.next() ? setCell(i) : clearCell(i);
}

void Grid::calculateBorders()
{
    Cell *oldNum = nullptr;

    // corners:
    // #.
    // ..
    oldNum = &cellArray[getIndex(0, 0)];
    *oldNum = (isAlive(*oldNum) * 0x80) + isAlive(1, 1) + isAlive(0, 1);

    // .#
    // ..
    oldNum = &cellArray[getIndex(width_cells - 1, 0)];
    *oldNum = (isAlive(*oldNum) * 0x80) + isAlive(width_cells - 2, 0) + isAlive(width_cells - 2, 1) + isAlive(width_cells - 1, 1);

    // ..
    // .#
    oldNum = &cellArray[getIndex(width_cells - 1, height_cells - 1)];
    *oldNum = (isAlive(*oldNum) * 0x80) + isAlive(width_cells - 1, height_cells - 2) + isAlive(width_cells - 2, height_cells - 2) + isAlive(width_cells - 2, height_cells - 1);

    // ..
    // #.
    oldNum = &cellArray[getIndex(0, height_cells - 1)];
    *oldNum = (isAlive(*oldNum) * 0x80) + isAlive(0, height_cells - 2) + isAlive(1, height_cells - 2) + isAlive(1, height_cells - 1);

    // sides:
    // horizontal
    for (uint j = 1; j < width_cells - 1; j++)
    {
        // up
        oldNum = &cellArray[getIndex(j, 0)];
        *oldNum = (isAlive(*oldNum) * 0x80) + isAlive(j - 1, 0) + isAlive(j + 1, 0) + isAlive(j - 1, 1) + isAlive(j, 1) + isAlive(j + 1, 1);

        // down
        oldNum = &cellArray[getIndex(j, height_cells - 1)];
        *oldNum = (isAlive(*oldNum) * 0x80) + isAlive(j - 1, height_cells - 1) + isAlive(j + 1, height_cells - 1) + isAlive(j - 1, height_cells - 2) + isAlive(j, height_cells - 2) + isAlive(j + 1, height_cells - 2);
    }

    // vertical
    for (uint i = 1; i < height_cells - 1; i++)
    {
        // left
        oldNum = &cellArray[getIndex(0, i)];
        *oldNum = (isAlive(*oldNum) * 0x80) + isAlive(0, i - 1) + isAlive(0, i + 1) + isAlive(1, i - 1) + isAlive(1, i) + isAlive(1, i + 1);

        // right
        oldNum = &cellArray[getIndex(width_cells - 1, i)];
        *oldNum = (isAlive(*oldNum) * 0x80) + isAlive(width_cells - 1, i - 1) + isAlive(width_cells - 1, i + 1) + isAlive(width_cells - 2, i - 1) + isAlive(width_cells - 2, i) + isAlive(width_cells - 2, i + 1);
    }
}

void Grid::calculateNeighbors()
{
    Cell *oldNum = nullptr;

    for (uint x = 1; x < width_cells - 1; x++)
    {
        for (uint y = 1; y < height_cells - 1; y++)
        {
            oldNum = &cellArray[getIndex(x, y)];
            *oldNum = (isAlive(*oldNum) * 0x80) + isAlive(x - 1, y - 1) + isAlive(x, y - 1) + isAlive(x + 1, y - 1) + isAlive(x - 1, y) + isAlive(x + 1, y) + isAlive(x - 1, y + 1) + isAlive(x, y + 1) + isAlive(x + 1, y + 1);
        }
    }
}
void Grid::updateCells()
{
    Cell currNeigbors = 0;
    for (idx_t i = 0; i < width_cells * height_cells; i++)
    {
        currNeigbors = cellArray[i];
        bit::clear(currNeigbors, 7); // clears the msb to get just the number of neighbors

        if (isAlive(i) && (currNeigbors < 2 || currNeigbors > 3))
        {
            clearCell(i);
        }
        if (!isAlive(i) && (currNeigbors == 3))
        {
            setCell(i);
        }
    }
}
void Grid::toggleCell(sf::Vector2u coor)
{
    isAlive(getIndex(coor.x, coor.y)) ? clearCell(getIndex(coor.x, coor.y)) : setCell(getIndex(coor.x, coor.y));
}
void Grid::updateCellsSkip()
{
    Cell currNeigbors = 0;
    for (idx_t i = 0; i < width_cells * height_cells; i++)
    {
        currNeigbors = cellArray[i];
        bit::clear(currNeigbors, 7); // clears the msb to get just the number of neighbors

        if (isAlive(i) && (currNeigbors < 2 || currNeigbors > 3))
        {
            bit::clear(cellArray[i], 7);
        }
        if (!isAlive(i) && (currNeigbors == 3))
        {
            bit::set(cellArray[i], 7);
        }
    }
}
