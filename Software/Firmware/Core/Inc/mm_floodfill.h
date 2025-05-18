/*
 * mm_floodfill.h
 *
 *  File for the implementation of the floodfill maze-solving algorithm
 */

#ifndef INC_MM_FLOODFILL_H_
#define INC_MM_FLOODFILL_H_

#include "main.h"

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
    struct Coord pos;
    enum Direction dir;
};

struct CellList {
    int size;
    struct Cell* cells;
};

struct Maze {
    struct Coord mouse_pos;
    enum Direction mouse_dir;
    int distances[16][16];
    int cellWalls[16][16];
    bool exploredCells[16][16];
    struct Coord goalPos[4];
};

bool Off_Maze(int mouse_pos_x, int mouse_pos_y);
struct CellList* Set_Neighbor_Cells(struct Maze* maze, struct Coord* pos);
uint8_t Scan_Walls(struct Maze* maze);
void Update_Mouse_Pos(struct Coord *pos, enum Direction dir);
void Set_Goal_Cell(struct Maze* maze, int num_of_goals);
void Floodfill(struct Maze* maze);
enum Direction Best_Cell(struct Maze* maze, struct Coord mouse_pos);
void Maze_Init(struct Maze* maze);

void Search_Mode(struct Maze* maze);
void Race_Mode(struct Maze* maze);

#endif /* INC_MM_FLOODFILL_H_ */
