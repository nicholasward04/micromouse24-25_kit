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
DirectionBitmask mask_array[4] = {NORTH_MASK, EAST_MASK, SOUTH_MASK, WEST_MASK};

bool offMaze(int mouse_pos_x, int mouse_pos_y) {
    if (mouse_pos_x < 0 || mouse_pos_x > 15 || mouse_pos_y < 0 || mouse_pos_y > 15) {
        return false; // False means a cell is off of the maze
    }
    return true;
}

void updateSimulator(Maze maze) { // Redraws the simulator based off of current distance and wall values
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            API::setText(x, y, std::to_string(maze.distances[x][y])); // Update simulator distances

            if (maze.cellWalls[x][y] & NORTH_MASK) { // Update simulator walls
                API::setWall(y, x, 'n');
            }
            if (maze.cellWalls[x][y] & EAST_MASK) {
                API::setWall(y, x, 'e');
            }
            if (maze.cellWalls[x][y] & SOUTH_MASK) {
                API::setWall(y, x, 's');
            }
            if (maze.cellWalls[x][y] & WEST_MASK) {
                API::setWall(y, x, 'w');
            }
        }
    }
}

CellList* getNeighborCells(Maze* maze, Cell* cell) {
    
}

void scanWallsAdjacent(Maze* maze, Coord cur_pos, Direction cur_dir) {
    switch (cur_dir) {
        case NORTH:
            if (offMaze(cur_pos.x, cur_pos.y + 1)) { maze->cellWalls[cur_pos.x][cur_pos.y + 1] = maze->cellWalls[cur_pos.x][cur_pos.y + 1] || mask_array[(cur_dir + 2) % 4]; }
            break;
        case EAST:
            if (offMaze(cur_pos.x + 1, cur_pos.y)) { maze->cellWalls[cur_pos.x + 1][cur_pos.y] = maze->cellWalls[cur_pos.x + 1][cur_pos.y] || mask_array[(cur_dir + 2) % 4]; }
            break;
        case SOUTH:
            if (offMaze(cur_pos.x, cur_pos.y - 1)) { maze->cellWalls[cur_pos.x][cur_pos.y - 1] = maze->cellWalls[cur_pos.x][cur_pos.y - 1] || mask_array[(cur_dir + 2) % 4]; }
            break;
        case WEST:
            if (offMaze(cur_pos.x - 1, cur_pos.y)) { maze->cellWalls[cur_pos.x - 1][cur_pos.y] = maze->cellWalls[cur_pos.x - 1][cur_pos.y] || mask_array[(cur_dir + 2) % 4]; }
            break;
    }
}

void scanWalls(Maze* maze) {
    Direction cur_dir = maze->mouse_dir;
    Coord cur_pos = {maze->mouse_pos.x, maze->mouse_pos.y};

    if (API::wallFront()) {
        maze->cellWalls[cur_pos.x][cur_pos.y] = maze->cellWalls[cur_pos.x][cur_pos.y] || mask_array[cur_dir];
        scanWallsAdjacent(maze, cur_pos, cur_dir);
    }
    if (API::wallLeft()) {
        maze->cellWalls[cur_pos.x][cur_pos.y] = maze->cellWalls[cur_pos.x][cur_pos.y] || mask_array[(cur_dir + 3) % 4];
        scanWallsAdjacent(maze, cur_pos, (Direction)((cur_dir + 3) % 4));
    }
    if (API::wallRight()) {
        maze->cellWalls[cur_pos.x][cur_pos.y] = maze->cellWalls[cur_pos.x][cur_pos.y] || mask_array[(cur_dir + 1) % 4];
        scanWallsAdjacent(maze, cur_pos, (Direction)((cur_dir + 1) % 4));
    }
}

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

void setGoalCell(Maze* maze, int num_of_goals) {
    switch (num_of_goals) {
        case 1:
            maze->goalPos[0] = {0, 0};
            break;
        case 4:
            maze->goalPos[0] = {7, 7}; maze->goalPos[1] = {7, 8}; maze->goalPos[2] = {8, 7}; maze->goalPos[3] = {8, 8};
            break;
    }
}

void Floodfill(Maze* maze) {}

Direction bestCell(Coord mouse_pos) {}

int main(int argc, char* argv[]) {}