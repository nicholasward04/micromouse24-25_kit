#include <iostream>
#include <string>

#include "API.h"

void log(const std::string& text) {
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

const uint8_t MAX_COST = 255;
char dir_chars[4] = {'n', 'e', 's', 'w'};
DirectionBitmask mask_array[4] = {NORTH_MASK, EAST_MASK, SOUTH_MASK, WEST_MASK};

bool offMaze(int mouse_pos_x, int mouse_pos_y) {
    if (mouse_pos_x < 0 || mouse_pos_x > 15 || mouse_pos_y < 0 || mouse_pos_y > 15) {
        return false; // False means a cell is off of the maze
    }
    return true;
}

void updateSimulator(Maze maze) {                                     // Redraws the simulator based off of current distance and wall values
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            API::setText(x, y, std::to_string(maze.distances[x][y])); // Update simulator distances

            if (maze.cellWalls[x][y] & NORTH_MASK) {                  // Update simulator walls
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

CellList* getNeighborCells(Maze* maze, Coord* pos) {
    
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

uint8_t scanWalls(Maze* maze) { // Checks wall information based on mouse's current position and updates maze walls. Returns an integer 0-3 depending on # of walls spotted
    Direction cur_dir = maze->mouse_dir;
    Coord cur_pos = {maze->mouse_pos.x, maze->mouse_pos.y};
    uint8_t walls_changed = 0;

    if (API::wallFront()) {
        maze->cellWalls[cur_pos.x][cur_pos.y] = maze->cellWalls[cur_pos.x][cur_pos.y] || mask_array[cur_dir];
        scanWallsAdjacent(maze, cur_pos, cur_dir); walls_changed += 1;
    }
    if (API::wallLeft()) {
        maze->cellWalls[cur_pos.x][cur_pos.y] = maze->cellWalls[cur_pos.x][cur_pos.y] || mask_array[(cur_dir + 3) % 4];
        scanWallsAdjacent(maze, cur_pos, (Direction)((cur_dir + 3) % 4)); walls_changed += 1;
    }
    if (API::wallRight()) {
        maze->cellWalls[cur_pos.x][cur_pos.y] = maze->cellWalls[cur_pos.x][cur_pos.y] || mask_array[(cur_dir + 1) % 4];
        scanWallsAdjacent(maze, cur_pos, (Direction)((cur_dir + 1) % 4)); walls_changed += 1;
    }
    return walls_changed;
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

void Floodfill(Maze* maze) {
    for (uint8_t x=0; x<16; x++) { for (uint8_t y=0; y<16; y++) { maze->distances[x][y] = MAX_COST; } } // Initialize all maze costs/distances to the maximum = 255

    uint8_t goal_count = 4;
    if (maze->goalPos[0].x == 0) { goal_count = 1; }                                                    // Check if goal is maze center or start cell
    
    Coord queue[MAX_COST];                                                                              // Initialize queue
    uint8_t head = 0, tail = 0;
    for (uint8_t cell = 0; cell < goal_count; cell++) {
        maze->distances[maze->goalPos[cell].x][maze->goalPos[cell].y] = 0;                              // Set goal cells to cost/distance minimum = 0
        queue[tail] = maze->goalPos[cell]; tail++;                                                      // Add goal cells to queue, increment tail
    }

    Coord curr_pos;
    while (head != tail) {
        curr_pos = queue[head];
        head++;
        uint8_t new_distance = maze->distances[curr_pos.x][curr_pos.y] + 1;                             // Calculate cost for adjacent cells

        CellList* neighbors = getNeighborCells(maze, &curr_pos);

        for (uint8_t neighbor = 0; neighbor < neighbors->size; neighbor++) {                            // For each neighbor cell, check if its cost/distance is > new distance -- if so, update its value
            if (maze->distances[neighbors->cells[neighbor].pos.x][neighbors->cells[neighbor].pos.y] > new_distance) {
                maze->distances[neighbors->cells[neighbor].pos.x][neighbors->cells[neighbor].pos.y] = new_distance;
                queue[tail] = neighbors->cells[neighbor].pos; tail++;
            }
        }
    }
}

Direction bestCell(Coord mouse_pos) {}

int main(int argc, char* argv[]) {}