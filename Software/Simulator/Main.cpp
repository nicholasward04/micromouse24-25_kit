#include <iostream>
#include <string>
#include <stdint.h>

#include "API.h"

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
    int explored[16][16];
    int distances[16][16];
    int cellWalls[16][16];
    Coord goalPos[4];
};

typedef enum {
    FORWARD_ONE_CELL = 0,
    TURN_RIGHT = 1,
    TURN_LEFT = 2
} motion_type_t;

typedef enum {
    SEARCH = 0,
    RACE = 1
} mode_type_t;

const uint8_t MAX_COST = 255;
char dir_chars[4] = {'n', 'e', 's', 'w'};
DirectionBitmask mask_array[4] = {NORTH_MASK, EAST_MASK, SOUTH_MASK, WEST_MASK};

bool offMaze(int mouse_pos_x, int mouse_pos_y) {
    if (mouse_pos_x < 0 || mouse_pos_x > 15 || mouse_pos_y < 0 || mouse_pos_y > 15) {
        return false; // False means a cell is off of the maze
    }
    return true;
}

void updateSimulator(Maze maze) {                                    // Redraws the simulator based off of current distance and wall values
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++) {
            if (maze.explored[y][x]) {
                API::setColor(x, y, 'g');
            }
            API::setText(x, y, std::to_string(maze.distances[y][x])); // Update simulator distances

            if (maze.cellWalls[y][x] & NORTH_MASK) {                  // Update simulator walls
                API::setWall(x, y, 'n');
            }
            if (maze.cellWalls[y][x] & EAST_MASK) {
                API::setWall(x, y, 'e');
            }
            if (maze.cellWalls[y][x] & SOUTH_MASK) {
                API::setWall(x, y, 's');
            }
            if (maze.cellWalls[y][x] & WEST_MASK) {
                API::setWall(x, y, 'w');
            }
        }
    }
}

CellList* getNeighborCells(Maze* maze, Coord* pos) {
    CellList* cell_list = (CellList*)malloc(sizeof(CellList));

    bool north_cell = false, east_cell = false, south_cell = false, west_cell = false;
    uint8_t x_coord = pos->x, y_coord = pos->y;
    uint8_t num_cells = 0;

    // If a cell is adjacent to the cell represented by pos, exists in the 16x16 maze, and is not blocked by a wall, add it to the cell list
    if (offMaze(x_coord, y_coord+1) && !(maze->cellWalls[y_coord][x_coord] & NORTH_MASK)) { north_cell = true; num_cells++; } // NORTH
    if (offMaze(x_coord+1, y_coord) && !(maze->cellWalls[y_coord][x_coord] & EAST_MASK)) { east_cell = true; num_cells++; }   // EAST
    if (offMaze(x_coord, y_coord-1) && !(maze->cellWalls[y_coord][x_coord] & SOUTH_MASK)) { south_cell = true; num_cells++; } // SOUTH
    if (offMaze(x_coord-1, y_coord) && !(maze->cellWalls[y_coord][x_coord] & WEST_MASK)) { west_cell = true; num_cells++; }   // WEST

    cell_list->size = num_cells;
    cell_list->cells = (Cell*)malloc(num_cells*sizeof(Cell)); 

    uint8_t i = 0; // Create new cells with appropriate coordinate and direction and add to the cell list if it is not blocked or off the maze
    if (north_cell) {
        Cell new_cell = {{x_coord, y_coord+1}, NORTH}; // NORTH
        cell_list->cells[i] = new_cell; i++;
    }
    if (east_cell) {
        Cell new_cell = {{x_coord+1, y_coord}, EAST};  // EAST
        cell_list->cells[i] = new_cell; i++;
    }
    if (south_cell) {
        Cell new_cell = {{x_coord, y_coord-1}, SOUTH}; // SOUTH
        cell_list->cells[i] = new_cell; i++;
    }
    if (west_cell) {
        Cell new_cell = {{x_coord-1, y_coord}, WEST}; // WEST
        cell_list->cells[i] = new_cell; i++;
    }
    return cell_list;
}

uint8_t scanWalls(Maze* maze) { // Checks wall information based on mouse's current position and updates maze walls. Returns an integer 0-3 depending on # of walls spotted
    Direction cur_dir = maze->mouse_dir;
    Coord cur_pos = maze->mouse_pos;
    uint8_t walls_changed = 0;

    if (API::wallFront()) {
        maze->cellWalls[cur_pos.y][cur_pos.x] |= mask_array[cur_dir];
        walls_changed += 1;
        switch (cur_dir) { // Update adjacent walls with relevant wall information
            case (NORTH):
                if (offMaze(cur_pos.x, cur_pos.y + 1)) { maze->cellWalls[maze->mouse_pos.y + 1][maze->mouse_pos.x] |= SOUTH_MASK; } break;
            case (EAST):
                if (offMaze(cur_pos.x + 1, cur_pos.y)) { maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x + 1] |= WEST_MASK; }  break;
            case (SOUTH):
                if (offMaze(cur_pos.x, cur_pos.y - 1)) { maze->cellWalls[maze->mouse_pos.y - 1][maze->mouse_pos.x] |= NORTH_MASK; } break;
            case (WEST):
                if (offMaze(cur_pos.x - 1, cur_pos.y)) { maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x - 1] |= EAST_MASK; }  break;
        }
    }
    if (API::wallLeft()) {
        maze->cellWalls[cur_pos.y][cur_pos.x] |= mask_array[(cur_dir + 3) % 4];
        walls_changed += 1;
        switch (cur_dir) {
            case (NORTH):
                if (offMaze(cur_pos.x - 1, cur_pos.y)) { maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x - 1] |= EAST_MASK; }  break;
            case (EAST):
                if (offMaze(cur_pos.x, cur_pos.y + 1)) { maze->cellWalls[maze->mouse_pos.y + 1][maze->mouse_pos.x] |= SOUTH_MASK; } break;
            case (SOUTH):
                if (offMaze(cur_pos.x + 1, cur_pos.y)) { maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x + 1] |= WEST_MASK; }  break;
            case (WEST):
                if (offMaze(cur_pos.x, cur_pos.y - 1)) { maze->cellWalls[maze->mouse_pos.y - 1][maze->mouse_pos.x] |= NORTH_MASK; } break;
        }
    }
    if (API::wallRight()) {
        maze->cellWalls[cur_pos.y][cur_pos.x] |= mask_array[(cur_dir + 1) % 4];
        walls_changed += 1;
        switch (cur_dir) { 
            case (NORTH):
                if (offMaze(cur_pos.x + 1, cur_pos.y)) { maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x + 1] |= WEST_MASK; }  break;
            case (EAST):
                if (offMaze(cur_pos.x, cur_pos.y - 1)) { maze->cellWalls[maze->mouse_pos.y - 1][maze->mouse_pos.x] |= NORTH_MASK; } break;
            case (SOUTH):
                if (offMaze(cur_pos.x - 1, cur_pos.y)) { maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x - 1] |= EAST_MASK; }  break;
            case (WEST):
                if (offMaze(cur_pos.x, cur_pos.y + 1)) { maze->cellWalls[maze->mouse_pos.y + 1][maze->mouse_pos.x] |= SOUTH_MASK; } break;
        }
    }
    return walls_changed;
}

void updateMousePos(Coord *pos, Direction dir, Maze* maze)
{
    maze->explored[pos->y][pos->x] = 1;

    if      (dir == NORTH) { pos->y++; }
    else if (dir == SOUTH) { pos->y--; }
    else if (dir == WEST)  { pos->x--; }
    else if (dir == EAST)  { pos->x++; }

    maze->explored[pos->y][pos->x] = 1;
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

void floodfill(Maze* maze, mode_type_t mode) {
    for (uint8_t y=0; y<16; y++) { for (uint8_t x=0; x<16; x++) { maze->distances[y][x] = MAX_COST; } } // Initialize all maze costs/distances to the maximum = 255

    uint8_t goal_count = 4;
    if (maze->goalPos[0].x == 0) { goal_count = 1; }                                                    // Check if goal is maze center or start cell
    
    Coord queue[MAX_COST];                                                                              // Initialize queue
    uint8_t head = 0, tail = 0;
    for (uint8_t cell = 0; cell < goal_count; cell++) {
        maze->distances[maze->goalPos[cell].y][maze->goalPos[cell].x] = 0;                              // Set goal cells to cost/distance minimum = 0
        queue[tail] = maze->goalPos[cell]; tail++;                                                      // Add goal cells to queue, increment tail
    }

    Coord curr_pos;
    while (head != tail) {
        curr_pos = queue[head];
        head++;
        uint8_t new_distance = maze->distances[curr_pos.y][curr_pos.x] + 1;                             // Calculate cost for adjacent cells

        CellList* neighbors = getNeighborCells(maze, &curr_pos);
        Cell* replacement_array = (Cell*)malloc(sizeof(Cell) * 4);

        if (mode == RACE) {
            std::cerr << "entered" << std::endl;
            uint8_t replacement_index = 0;
            uint8_t loop_size = neighbors->size;
            for (uint8_t neighbor = 0; neighbor < loop_size; neighbor++) {
                if (!maze->explored[neighbors->cells[neighbor].pos.y][neighbors->cells[neighbor].pos.x]) {
                    neighbors->size--;
                }
                else {
                    replacement_array[replacement_index] = neighbors->cells[neighbor];
                    replacement_index++;
                }
            }
            neighbors->cells = replacement_array;
        }

        for (uint8_t neighbor = 0; neighbor < neighbors->size; neighbor++) {                            // For each neighbor cell, check if its cost/distance is > new distance -- if so, update its value
            if (maze->distances[neighbors->cells[neighbor].pos.y][neighbors->cells[neighbor].pos.x] > new_distance) {
                maze->distances[neighbors->cells[neighbor].pos.y][neighbors->cells[neighbor].pos.x] = new_distance;
                queue[tail] = neighbors->cells[neighbor].pos;
                tail++;
            }
        }
        free(replacement_array);
        if (mode == SEARCH) {
            free(neighbors->cells);
        }
        free(neighbors);
    }
}

Direction bestCell(Maze* maze, Coord mouse_pos, mode_type_t mode) {
    CellList* neighbors = getNeighborCells(maze, &mouse_pos);
    Cell* replacement_array = (Cell*)malloc(sizeof(Cell) * 4);

    if (mode == RACE) {
        uint8_t replacement_index = 0;
        uint8_t loop_size = neighbors->size;
        for (uint8_t neighbor = 0; neighbor < loop_size; neighbor++) {
            if (!maze->explored[neighbors->cells[neighbor].pos.y][neighbors->cells[neighbor].pos.x]) {
                neighbors->size--;
            }
            else {
                replacement_array[replacement_index] = neighbors->cells[neighbor];
                replacement_index++;
            }
        }
        neighbors->cells = replacement_array;
    }

    uint8_t best_cell_index = 0;
    uint8_t lowest_cost = maze->distances[mouse_pos.y][mouse_pos.x];

    for (uint8_t neighbor = 0; neighbor < neighbors->size; neighbor++) {
        if ((maze->distances[neighbors->cells[neighbor].pos.y][neighbors->cells[neighbor].pos.x] < lowest_cost) ||  // For each neighbor cell, check if its cost is the lowest seen
           ((maze->distances[neighbors->cells[neighbor].pos.y][neighbors->cells[neighbor].pos.x] == lowest_cost) && // Or check cost == lowest and the dir of the cell matches that of the mouse (prioritize forward)
           (maze->mouse_dir == neighbors->cells[neighbor].dir))) {
                best_cell_index = neighbor;
                lowest_cost = maze->distances[neighbors->cells[neighbor].pos.y][neighbors->cells[neighbor].pos.x];  // Update best cell index and lowest cost seen
           }
    }
    Direction ret_dir = neighbors->cells[best_cell_index].dir;
    free(replacement_array);
    if (mode == SEARCH) {
        free(neighbors->cells);
    }
    free(neighbors);

    return ret_dir;                                                                                                 // Return direction of lowest cost cell
}

void mazeInit(Maze* maze) {
    for (uint8_t y = 0; y < 16; y++) { for (uint8_t x = 0; x < 16; x++) { maze->cellWalls[y][x] = 0;
                                                                          maze->explored[y][x] = 0; } }             // Initialize all wall/explored values to 0
    maze->mouse_dir = NORTH;                                                                                        // Mouse starting direction/pos always NORTH/{0,0}
    maze->mouse_pos = {0,0};
}

void raceMode(Maze maze) {
    // Make copy of maze and run floodfill on only explored cells
    // Plan motion by running best cell until reaching center of maze (don't actually move mouse)
    // Store desired movements into a queue
    // After center is reached, reset mouse position/direction in the maze
    // Combine movements into larger profiles (eg. 1 cell forward + 1 cell forward = 2 cells forward) - not possible for sim
    // Store combined profiles into a queue
    // Until the queue is empty, execute the motion at the front of queue
    // Once reached center, don't return to start

    floodfill(&maze, RACE);
    updateSimulator(maze);    

    std::cerr << "Completed race floodfill" << std::endl;

    uint16_t prelim_motions_length = 0;
    motion_type_t prelim_motion_queue[256];
    
    setGoalCell(&maze, 4);                                                                                        
    while (true) {
        Direction best_dir = bestCell(&maze, maze.mouse_pos, RACE);                                                      

        if (best_dir == (Direction)((maze.mouse_dir + 1) % 4)) {       // Right turn
            prelim_motion_queue[prelim_motions_length] = TURN_RIGHT;
            maze.mouse_dir = (Direction)((maze.mouse_dir + 1) % 4);
        }
        else if (best_dir == (Direction)((maze.mouse_dir + 3) % 4)) {  // Left turn
            prelim_motion_queue[prelim_motions_length] = TURN_LEFT;
            maze.mouse_dir = (Direction)((maze.mouse_dir + 3) % 4);
        }
        else {
            prelim_motion_queue[prelim_motions_length] = FORWARD_ONE_CELL;
        }

        prelim_motions_length++;
        updateMousePos(&maze.mouse_pos, maze.mouse_dir, &maze);

        if (maze.distances[maze.mouse_pos.y][maze.mouse_pos.x] == 0) {    // Check if mouse has reached the goal position, if so break
            break;
        }
    }

    std::cerr << "Completed motion planning" << std::endl;

    for (uint16_t curr_motion = 0; curr_motion < prelim_motions_length; curr_motion++) {
        std::cerr << prelim_motion_queue[curr_motion] << std::endl;
        switch (prelim_motion_queue[curr_motion]) {
            case FORWARD_ONE_CELL:
                std::cerr << "Making movement: FORWARD" << std::endl;
                API::moveForward();
                break;
            case TURN_RIGHT:
            std::cerr << "Making movement: RIGHT" << std::endl;
                API::turnRight();
                API::moveForward();
                break;
            case TURN_LEFT:
            std::cerr << "Making movement: LEFT" << std::endl;
                API::turnLeft();
                API::moveForward();
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    Maze maze;
    mazeInit(&maze);                                                                                                // Intialize maze
    uint8_t goal_swap = 0;
    uint8_t run_count = 0;

    setGoalCell(&maze, 4);                                                                                          // Set initial goal cells -- center 4 cells
    while (true) {
        uint8_t walls_changed = scanWalls(&maze);                                                                   // Update wall information
        if ((walls_changed > 0) || goal_swap) { floodfill(&maze, SEARCH); goal_swap = 0; }                          // Update distance information if walls or goal have been updated
        updateSimulator(maze);

        if (run_count >= 2) {
            std::cerr << "Switching to race mode" << std::endl;
            updateSimulator(maze);
            break;
        }

        Direction best_dir = bestCell(&maze, maze.mouse_pos, SEARCH);                                               // Get best direction to move to

        if (best_dir == (Direction)((maze.mouse_dir + 1) % 4)) {                                                    // Right turn
            API::turnRight();
            maze.mouse_dir = (Direction)((maze.mouse_dir + 1) % 4);
        }
        else if (best_dir == (Direction)((maze.mouse_dir + 3) % 4)) {                                               // Left turn
            API::turnLeft();
            maze.mouse_dir = (Direction)((maze.mouse_dir + 3) % 4);
        }
        else if (best_dir == (Direction)((maze.mouse_dir + 2) % 4)) {                                               // Turn around
            API::turnLeft();
            API::turnLeft();
            maze.mouse_dir = (Direction)((maze.mouse_dir + 2) % 4);
        }

        API::moveForward();
        updateMousePos(&maze.mouse_pos, maze.mouse_dir, &maze);

        if (maze.distances[maze.mouse_pos.y][maze.mouse_pos.x] == 0) {                                              // Check if mouse has reached the goal position
            if (maze.goalPos[0].x == 0) { 
                setGoalCell(&maze, 4);                                                                              // Mouse has reached {0,0} goal position
                run_count++; 
                API::turnLeft();
                API::turnLeft();
                maze.mouse_dir = (Direction)((maze.mouse_dir + 2) % 4);
            }                                 
            else { 
                setGoalCell(&maze, 1);
                goal_swap = 1;
            
                maze.cellWalls[maze.mouse_pos.y][maze.mouse_pos.x] = 0b1111;
                if (maze.mouse_dir == NORTH) {
                    maze.cellWalls[maze.mouse_pos.y][maze.mouse_pos.x] &= 0b1101;
                    maze.cellWalls[maze.mouse_pos.y+1][maze.mouse_pos.x] |= SOUTH_MASK;
                    maze.cellWalls[maze.mouse_pos.y][maze.mouse_pos.x+1] |= WEST_MASK;
                    maze.cellWalls[maze.mouse_pos.y][maze.mouse_pos.x-1] |= EAST_MASK;
                }
                else if (maze.mouse_dir == SOUTH) {
                    maze.cellWalls[maze.mouse_pos.y][maze.mouse_pos.x] &= 0b0111;
                    maze.cellWalls[maze.mouse_pos.y-1][maze.mouse_pos.x] |= NORTH_MASK;
                    maze.cellWalls[maze.mouse_pos.y][maze.mouse_pos.x+1] |= WEST_MASK;
                    maze.cellWalls[maze.mouse_pos.y][maze.mouse_pos.x-1] |= EAST_MASK;
                }
                else if (maze.mouse_dir == EAST) {
                    maze.cellWalls[maze.mouse_pos.y][maze.mouse_pos.x] &= 0b1110;
                    maze.cellWalls[maze.mouse_pos.y+1][maze.mouse_pos.x] |= SOUTH_MASK;
                    maze.cellWalls[maze.mouse_pos.y][maze.mouse_pos.x+1] |= WEST_MASK;
                    maze.cellWalls[maze.mouse_pos.y-1][maze.mouse_pos.x] |= NORTH_MASK;
                }
                else if (maze.mouse_dir == WEST) {
                    maze.cellWalls[maze.mouse_pos.y][maze.mouse_pos.x] &= 0b1011;
                    maze.cellWalls[maze.mouse_pos.y+1][maze.mouse_pos.x] |= SOUTH_MASK;
                    maze.cellWalls[maze.mouse_pos.y][maze.mouse_pos.x-1] |= EAST_MASK;
                    maze.cellWalls[maze.mouse_pos.y-1][maze.mouse_pos.x] |= NORTH_MASK;
                }
            }                                                           // Mouse has reached center of maze goal position
        }
    }
    raceMode(maze);
}