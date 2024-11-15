#include <iostream>
#include <string>

#include "API.h"

void log(const std::string& text) 
{
    std::cerr << text << std::endl;
}

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

enum DirectionBitmask {
    NORTH_MASK = 0b1000,
    EAST_MASK  = 0b0100,
    SOUTH_MASK = 0b0010,
    WEST_MASK  = 0b0001
};

struct Coord {
    int x;
    int y;
};

struct Cell {
    Coord pos;
    Direction dir;
    bool blocked;
};

struct CellList {
    int size;
    Cell* cells;
};

struct Maze {
    Coord mouse_pos;
    Direction mouse_dir;

    int distances[16][16];
    int cellWalls[16][16];

    Coord goalPos[4];
};

char dir_chars[4] = {'n', 'e', 's', 'w'};
int dir_mask[4] = {0b1000, 0b0100, 0b0010, 0b0001};

bool offMaze(int mouse_pos_x, int mouse_pos_y)
{
    if (mouse_pos_x < 0 || mouse_pos_x > 15 || mouse_pos_y < 0 || mouse_pos_y > 15)
    {
        return false; // False means a cell is off of the maze
    }
    return true;
}

void updateSimulator(Maze maze) // Redraws the simulator based off of current distance and wall values
{
    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            API::setText(x, y, std::to_string(maze.distances[y][x])); // Update simulator distances

            if (maze.cellWalls[y][x] & NORTH_MASK) // Update simulator walls
            {
                API::setWall(x, y, 'n');
            }
            if (maze.cellWalls[y][x] & EAST_MASK)
            {
                API::setWall(x, y, 'e');
            }
            if (maze.cellWalls[y][x] & SOUTH_MASK)
            {
                API::setWall(x, y, 's');
            }
            if (maze.cellWalls[y][x] & WEST_MASK)
            {
                API::setWall(x, y, 'w');
            }
        }
    }
}

CellList* getNeighborCells(Maze* maze) {}

void scanWalls(Maze* maze) {}

void updateMousePos(Coord *pos, Direction dir) {
    switch (dir) {
        case NORTH:
            pos->y++;
            break;
        case SOUTH:
            pos->y--;
            break;
        case WEST:
            pos->x--;
            break;
        case EAST:
            pos->x++;
            break;
        default:
            break;
    }
}

void setGoalCell(Maze* maze, int num_of_goals) {}

void Floodfill(Maze* maze) {}

Cell bestCell(Maze* maze) {}

int main(int argc, char* argv[]) {}