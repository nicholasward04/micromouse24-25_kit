/*
 * mm_floodfill.c
 *
 *  File for the implementation of the floodfill maze-solving algorithm
 */

#include "mm_floodfill.h"
#include "mm_systick.h"
#include "mm_vision.h"
#include "mm_supplemental.h"
#include "mm_profiles.h"
#include "mm_motors.h"

//#define SMOOTH_TURNS

extern mouse_state_t mouse_state;
extern struct Maze maze;

extern profile_t forward_profile;
extern profile_t rotational_profile;

extern bool armed;
extern bool motor_controller_enabled;
extern bool steering_adjustment;
bool disable_adc = false;

extern bool wall_front;
bool just_reached_goal = false;

const uint16_t MAX_COST = 256;
char dir_chars[4] = {'n', 'e', 's', 'w'};
enum DirectionBitmask mask_array[4] = {NORTH_MASK, EAST_MASK, SOUTH_MASK, WEST_MASK};

prev_action_t prev_action = ABOUT_FACE;

bool Off_Maze(int mouse_pos_x, int mouse_pos_y) {
    if (mouse_pos_x < 0 || mouse_pos_x > 15 || mouse_pos_y < 0 || mouse_pos_y > 15) {
        return false; // False means a cell is off of the maze
    }
    return true;
}

const float WALL_THICKNESS = 12;
const float MOUSE_BACK_TO_CENTER_MM = 32.467; // Distance from back of mouse PCB to centerpoint of motors

const float WALL_BIAS_MM = 0;
const float CELL_TO_CELL_MM = 168; // 172 for 500 mm/s  168 for 600
const float WALL_TO_WALL_MM = 165;
const float BACK_ON_WALL_TO_CELL_MM = CELL_TO_CELL_MM - (WALL_THICKNESS / 2) - MOUSE_BACK_TO_CENTER_MM;
const float CELL_TO_CENTER_MM = (WALL_TO_WALL_MM / 2) - (WALL_THICKNESS / 2) - WALL_BIAS_MM;
const float CENTER_TO_WALL_REVERSE_MM = -1 * ((WALL_TO_WALL_MM / 2) + WALL_THICKNESS); // Include some extra distance to guarantee pushed against wall
const float SMOOTH_TURN_RADIUS_MM = WALL_TO_WALL_MM / 2; // May need to decrease for sharper turn
const float SMOOTH_TURN_FWD_MM = M_PI * SMOOTH_TURN_RADIUS_MM / 6;  // div by 2

const float LEFT_TURN_DEG = -93;
const float RIGHT_TURN_DEG = 92;
const float ABOUT_TURN_DEG = 185;

const float SEARCH_SPEED_FWD_MAX = 600;
const float SEARCH_SPEED_ROT_MAX = 600;
const float SEARCH_SPEED_ROT_SMOOTH_MAX = 500;
const float SEARCH_SPEED_FWD_SMOOTH_MAX = 350;
const float SEARCH_ACCELERATION = 2500;


const float RACE_SPEED_FWD_MAX = 700;
const float RACE_SPEED_ROT_MAX = 500;
const float RACE_SPEED_ROT_SMOOTH_MAX = 450;
const float RACE_SPEED_FWD_SMOOTH_MAX = 350;
const float RACE_ACCELERATION = 2500;

param_t fwd_placeholder_1 = { 0 };
param_t fwd_placeholder_2 = { 0 };
param_t rot_placeholder_1 = { 0 };

struct CellList* Get_Neighbor_Cells(struct Maze* maze, struct Coord* pos) {
	struct CellList* cell_list = (struct CellList*)malloc(sizeof(struct CellList));

    bool north_cell = false, east_cell = false, south_cell = false, west_cell = false;
    uint8_t x_coord = pos->x, y_coord = pos->y;
    uint8_t num_cells = 0;

    // If a cell is adjacent to the cell represented by pos, exists in the 16x16 maze, and is not blocked by a wall, add it to the cell list
    if (Off_Maze(x_coord, y_coord+1) && !(maze->cellWalls[y_coord][x_coord] & NORTH_MASK)) { north_cell = true; num_cells++; } // NORTH
    if (Off_Maze(x_coord+1, y_coord) && !(maze->cellWalls[y_coord][x_coord] & EAST_MASK)) { east_cell = true; num_cells++; }   // EAST
    if (Off_Maze(x_coord, y_coord-1) && !(maze->cellWalls[y_coord][x_coord] & SOUTH_MASK)) { south_cell = true; num_cells++; } // SOUTH
    if (Off_Maze(x_coord-1, y_coord) && !(maze->cellWalls[y_coord][x_coord] & WEST_MASK)) { west_cell = true; num_cells++; }   // WEST

    cell_list->size = num_cells;
    cell_list->cells = (struct Cell*)malloc(num_cells*sizeof(struct Cell));

    uint8_t i = 0; // Create new cells with appropriate coordinate and direction and add to the cell list if it is not blocked or off the maze
    if (north_cell) {
    	struct Cell new_cell = {{x_coord, y_coord+1}, NORTH}; // NORTH
        cell_list->cells[i] = new_cell; i++;
    }
    if (east_cell) {
    	struct Cell new_cell = {{x_coord+1, y_coord}, EAST};  // EAST
        cell_list->cells[i] = new_cell; i++;
    }
    if (south_cell) {
    	struct Cell new_cell = {{x_coord, y_coord-1}, SOUTH}; // SOUTH
        cell_list->cells[i] = new_cell; i++;
    }
    if (west_cell) {
    	struct Cell new_cell = {{x_coord-1, y_coord}, WEST}; // WEST
        cell_list->cells[i] = new_cell; i++;
    }
    return cell_list;
}

uint8_t Scan_Walls(struct Maze* maze) { // Checks wall information based on mouse's current position and updates maze walls. Returns an integer 0-3 depending on # of walls spotted
    enum Direction cur_dir = maze->mouse_dir;
    struct Coord cur_pos = maze->mouse_pos;
    uint8_t walls_changed = 0;

    Poll_Sensors(&mouse_state);
    if (Wall_Front()) {
        maze->cellWalls[cur_pos.y][cur_pos.x] |= mask_array[cur_dir];
        walls_changed += 1;
        switch (cur_dir) { // Update adjacent walls with relevant wall information
            case (NORTH):
                if (Off_Maze(cur_pos.x, cur_pos.y + 1)) { maze->cellWalls[maze->mouse_pos.y + 1][maze->mouse_pos.x] |= SOUTH_MASK; } break;
            case (EAST):
                if (Off_Maze(cur_pos.x + 1, cur_pos.y)) { maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x + 1] |= WEST_MASK; }  break;
            case (SOUTH):
                if (Off_Maze(cur_pos.x, cur_pos.y - 1)) { maze->cellWalls[maze->mouse_pos.y - 1][maze->mouse_pos.x] |= NORTH_MASK; } break;
            case (WEST):
                if (Off_Maze(cur_pos.x - 1, cur_pos.y)) { maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x - 1] |= EAST_MASK; }  break;
        }
    }
    if (Wall_Right()) {
        maze->cellWalls[cur_pos.y][cur_pos.x] |= mask_array[(cur_dir + 3) % 4];
        walls_changed += 1;
        switch (cur_dir) {
            case (NORTH):
                if (Off_Maze(cur_pos.x - 1, cur_pos.y)) { maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x - 1] |= EAST_MASK; }  break;
            case (EAST):
                if (Off_Maze(cur_pos.x, cur_pos.y + 1)) { maze->cellWalls[maze->mouse_pos.y + 1][maze->mouse_pos.x] |= SOUTH_MASK; } break;
            case (SOUTH):
                if (Off_Maze(cur_pos.x + 1, cur_pos.y)) { maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x + 1] |= WEST_MASK; }  break;
            case (WEST):
                if (Off_Maze(cur_pos.x, cur_pos.y - 1)) { maze->cellWalls[maze->mouse_pos.y - 1][maze->mouse_pos.x] |= NORTH_MASK; } break;
        }
    }
    if (Wall_Left()) {
        maze->cellWalls[cur_pos.y][cur_pos.x] |= mask_array[(cur_dir + 1) % 4];
        walls_changed += 1;
        switch (cur_dir) {
            case (NORTH):
                if (Off_Maze(cur_pos.x + 1, cur_pos.y)) { maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x + 1] |= WEST_MASK; }  break;
            case (EAST):
                if (Off_Maze(cur_pos.x, cur_pos.y - 1)) { maze->cellWalls[maze->mouse_pos.y - 1][maze->mouse_pos.x] |= NORTH_MASK; } break;
            case (SOUTH):
                if (Off_Maze(cur_pos.x - 1, cur_pos.y)) { maze->cellWalls[maze->mouse_pos.y][maze->mouse_pos.x - 1] |= EAST_MASK; }  break;
            case (WEST):
                if (Off_Maze(cur_pos.x, cur_pos.y + 1)) { maze->cellWalls[maze->mouse_pos.y + 1][maze->mouse_pos.x] |= SOUTH_MASK; } break;
        }
    }
    return walls_changed;
}

void Update_Mouse_Pos(struct Coord *pos, enum Direction dir, struct Maze* maze)
{
	maze->exploredCells[pos->y][pos->x] = true;

    if      (dir == NORTH) { pos->y++; }
    else if (dir == SOUTH) { pos->y--; }
    else if (dir == WEST)  { pos->x--; }
    else if (dir == EAST)  { pos->x++; }

    maze->exploredCells[pos->y][pos->x] = true;

    mouse_state.current_cell = maze->cellWalls[pos->y][pos->x];
	mouse_state.mouse_position[0] = (uint8_t)pos->x;
	mouse_state.mouse_position[1] = (uint8_t)pos->y;
	mouse_state.mouse_direction = dir;
}

void Set_Goal_Cell(struct Maze* maze, int num_of_goals) {
    if (num_of_goals == 1) {
		maze->goalPos[0] = (struct Coord){0, 0};
    }
    else if (num_of_goals == 4) {
    	maze->goalPos[0] = (struct Coord){7, 7};
		maze->goalPos[1] = (struct Coord){7, 8};
		maze->goalPos[2] = (struct Coord){8, 7};
		maze->goalPos[3] = (struct Coord){8, 8};
    }
}

void Floodfill(struct Maze* maze, enum mode_type_t mode) {
    for (uint8_t y=0; y<16; y++) { for (uint8_t x=0; x<16; x++) { maze->distances[y][x] = MAX_COST; } } // Initialize all maze costs/distances to the maximum = 255

    uint8_t goal_count = 4;
    if (maze->goalPos[0].x == 0) { goal_count = 1; }                                                    // Check if goal is maze center or start cell

    struct Coord queue[MAX_COST];                                                                       // Initialize queue
    uint16_t head = 0, tail = 0;
    for (uint8_t cell = 0; cell < goal_count; cell++) {
        maze->distances[maze->goalPos[cell].y][maze->goalPos[cell].x] = 0;                              // Set goal cells to cost/distance minimum = 0
        queue[tail] = maze->goalPos[cell]; tail++;                                                      // Add goal cells to queue, increment tail
    }

    struct Coord curr_pos;
    while (head != tail) {
        curr_pos = queue[head];
        head++;
        uint8_t new_distance = maze->distances[curr_pos.y][curr_pos.x] + 1;                             // Calculate cost for adjacent cells

        struct CellList* neighbors = Get_Neighbor_Cells(maze, &curr_pos);
        struct Cell* replacement_array = (struct Cell*)malloc(sizeof(struct Cell) * 4);

        if (mode == RACE) {
            uint8_t replacement_index = 0;
            uint8_t loop_size = neighbors->size;
            for (uint8_t neighbor = 0; neighbor < loop_size; neighbor++) {
                if (!maze->exploredCells[neighbors->cells[neighbor].pos.y][neighbors->cells[neighbor].pos.x]) {
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
                queue[tail] = neighbors->cells[neighbor].pos; tail++;
            }
        }
        free(replacement_array);
        if (mode == SEARCH) {
            free(neighbors->cells);
        }
        free(neighbors);
    }
}

enum Direction Best_Cell(struct Maze* maze, struct Coord mouse_pos, enum mode_type_t mode) {
	struct CellList* neighbors = Get_Neighbor_Cells(maze, &mouse_pos);
    struct Cell* replacement_array = (struct Cell*)malloc(sizeof(struct Cell) * 4);

    if (mode == RACE) {
        uint8_t replacement_index = 0;
        uint8_t loop_size = neighbors->size;
        for (uint8_t neighbor = 0; neighbor < loop_size; neighbor++) {
            if (!maze->exploredCells[neighbors->cells[neighbor].pos.y][neighbors->cells[neighbor].pos.x]) {
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
    enum Direction ret_dir = neighbors->cells[best_cell_index].dir;
    free(replacement_array);
    if (mode == SEARCH) {
        free(neighbors->cells);
    }
    free(neighbors);

    return ret_dir;                                                                                                 // Return direction of lowest cost cell
}

void Maze_Init(struct Maze* maze) {
    for (uint8_t y = 0; y < 16; y++) { for (uint8_t x = 0; x < 16; x++) { maze->cellWalls[y][x] = 0;
    																	  maze->exploredCells[y][x] = 0; } }        // Initialize all wall/explored values to 0
    maze->mouse_dir = NORTH;                                                                                        // Mouse starting direction/pos always NORTH/{0,0}
    maze->mouse_pos = (struct Coord){0,0};
}

void Search_Mode(struct Maze* maze) {
	disable_adc = true;
	Scan_Walls(maze);
	disable_adc = false;
	Floodfill(maze, SEARCHING);

	enum Direction best_dir = Best_Cell(maze, maze->mouse_pos, SEARCHING);

	if (best_dir == (enum Direction)((maze->mouse_dir + 3) % 4)) { // Right Turn
		#ifndef SMOOTH_TURNS
			rot_placeholder_1 = Parameter_Packer(RIGHT_TURN_DEG, SEARCH_SPEED_ROT_MAX, 0, SEARCH_ACCELERATION, false);
			fwd_placeholder_1 = Parameter_Packer(CELL_TO_CENTER_MM, SEARCH_SPEED_FWD_MAX, 0, SEARCH_ACCELERATION, true);
			Turn_Container(fwd_placeholder_1, rot_placeholder_1, &forward_profile, &rotational_profile);
		#else
			rot_placeholder_1 = Parameter_Packer(RIGHT_TURN_DEG, SEARCH_SPEED_ROT_SMOOTH_MAX, 0, SEARCH_ACCELERATION, false);
			fwd_placeholder_1 = Parameter_Packer(SMOOTH_TURN_FWD_MM, SEARCH_SPEED_FWD_SMOOTH_MAX, SEARCH_SPEED_FWD_SMOOTH_MAX, SEARCH_ACCELERATION, true);
			Smooth_Turn_Container(fwd_placeholder_1, rot_placeholder_1, &forward_profile, &rotational_profile);
		#endif
			maze->mouse_dir = (enum Direction)((maze->mouse_dir + 3) % 4);
			prev_action = RIGHT_TURN;
		}
	else if (best_dir == (enum Direction)((maze->mouse_dir + 1) % 4)) { // Left Turn
		#ifndef SMOOTH_TURNS
			rot_placeholder_1 = Parameter_Packer(LEFT_TURN_DEG, SEARCH_SPEED_ROT_MAX, 0, SEARCH_ACCELERATION, false);
			fwd_placeholder_1 = Parameter_Packer(CELL_TO_CENTER_MM, SEARCH_SPEED_FWD_MAX, 0, SEARCH_ACCELERATION, true);
			Turn_Container(fwd_placeholder_1, rot_placeholder_1, &forward_profile, &rotational_profile);
		#else
			rot_placeholder_1 = Parameter_Packer(LEFT_TURN_DEG, SEARCH_SPEED_ROT_SMOOTH_MAX, 0, SEARCH_ACCELERATION, false);
			fwd_placeholder_1 = Parameter_Packer(SMOOTH_TURN_FWD_MM, SEARCH_SPEED_FWD_SMOOTH_MAX, SEARCH_SPEED_FWD_SMOOTH_MAX, SEARCH_ACCELERATION, true);
			Smooth_Turn_Container(fwd_placeholder_1, rot_placeholder_1, &forward_profile, &rotational_profile);
		#endif
			maze->mouse_dir = (enum Direction)((maze->mouse_dir + 1) % 4);
			prev_action = LEFT_TURN;
		}
	else if (best_dir == (enum Direction)((maze->mouse_dir + 2) % 4)) { // About turn
		rot_placeholder_1 = Parameter_Packer(ABOUT_TURN_DEG, SEARCH_SPEED_ROT_MAX, 0, SEARCH_ACCELERATION, false);
		if (wall_front && !just_reached_goal) { // Back up into wall to realign, continue from there
			fwd_placeholder_1 = Parameter_Packer(CELL_TO_CENTER_MM, SEARCH_SPEED_FWD_MAX, 0, SEARCH_ACCELERATION, true);
			fwd_placeholder_2 = Parameter_Packer(CENTER_TO_WALL_REVERSE_MM, SEARCH_SPEED_FWD_MAX, 0, SEARCH_ACCELERATION, true);
			About_Face_Container(fwd_placeholder_1, fwd_placeholder_2, rot_placeholder_1, &forward_profile, &rotational_profile, true);
			maze->mouse_dir = (enum Direction)((maze->mouse_dir + 2) % 4);
			prev_action = ABOUT_FACE;
		}
		else { // If no wall, simply turn 180 degrees
			About_Face_Container(fwd_placeholder_1, fwd_placeholder_2, rot_placeholder_1, &forward_profile, &rotational_profile, false);
			maze->mouse_dir = (enum Direction)((maze->mouse_dir + 2) % 4);
			prev_action = NONE;
			just_reached_goal = false;
		}
	}

	if (prev_action == FORWARD_DRIVE) { // Normal forward movement
		fwd_placeholder_1 = Parameter_Packer(CELL_TO_CELL_MM, SEARCH_SPEED_FWD_MAX, SEARCH_SPEED_FWD_MAX, SEARCH_ACCELERATION, true);
		Profile_Container(fwd_placeholder_1, &forward_profile);
	}
	else if (prev_action == ABOUT_FACE){ // Distance to travel is less than after a forward movement or turn
		fwd_placeholder_1 = Parameter_Packer(BACK_ON_WALL_TO_CELL_MM, SEARCH_SPEED_FWD_MAX, SEARCH_SPEED_FWD_MAX, SEARCH_ACCELERATION, true);
		Profile_Container(fwd_placeholder_1, &forward_profile);
	}
	else { // On a turn don't make any additional movement

	}

	prev_action = FORWARD_DRIVE;

	Update_Mouse_Pos(&maze->mouse_pos, maze->mouse_dir, maze);

	// Check if mouse is in goal, if so change goal back to start location
	if (maze->distances[maze->mouse_pos.y][maze->mouse_pos.x] == 0) {
		if (!((maze->goalPos[0].x == 0) && (maze->goalPos[0].y == 0))) {
			Set_Goal_Cell(maze, 1); // Change goal cell back to origin

			// Save maze to flash memory
//			Save_Maze_To_Flash(maze);
			Clear_Profile(&forward_profile);
			Clear_Profile(&rotational_profile);
		}
		else {
			// Mouse has returned to start position, realign with wall and wait for user input
			rot_placeholder_1 = Parameter_Packer(ABOUT_TURN_DEG, SEARCH_SPEED_ROT_MAX, 0, SEARCH_ACCELERATION, false);
			fwd_placeholder_1 = Parameter_Packer(CELL_TO_CENTER_MM, SEARCH_SPEED_FWD_MAX, 0, SEARCH_ACCELERATION, true);
			fwd_placeholder_2 = Parameter_Packer(CENTER_TO_WALL_REVERSE_MM, SEARCH_SPEED_FWD_MAX, 0, SEARCH_ACCELERATION, true);
			About_Face_Container(fwd_placeholder_1, fwd_placeholder_2, rot_placeholder_1, &forward_profile, &rotational_profile, true);

			maze->mouse_dir = NORTH;
			prev_action = ABOUT_FACE;
			Set_Goal_Cell(maze, 4); // Change goal cell back to center of maze

			Clear_Profile(&forward_profile);
			Clear_Profile(&rotational_profile);

			armed = false;
			steering_adjustment = false;
			motor_controller_enabled = false;
		}
	}

}

void Race_Mode(struct Maze maze) {
    Floodfill(&maze, RACE);

    uint16_t prelim_motions_length = 0;
    enum motion_type_t prelim_motion_queue[256];

    Set_Goal_Cell(&maze, 4);
    while (true) {
        enum Direction best_dir = Best_Cell(&maze, maze.mouse_pos, RACE);

        if (best_dir == (enum Direction)((maze.mouse_dir + 3) % 4)) {       // Right turn
            prelim_motion_queue[prelim_motions_length] = TURN_RIGHT;
            maze.mouse_dir = (enum Direction)((maze.mouse_dir + 3) % 4);
        }
        else if (best_dir == (enum Direction)((maze.mouse_dir + 1) % 4)) {  // Left turn
            prelim_motion_queue[prelim_motions_length] = TURN_LEFT;
            maze.mouse_dir = (enum Direction)((maze.mouse_dir + 1) % 4);
        }
        else {
            prelim_motion_queue[prelim_motions_length] = FORWARD_ONE_CELL;
        }

        prelim_motions_length++;
        Update_Mouse_Pos(&maze.mouse_pos, maze.mouse_dir, &maze);

        if (maze.distances[maze.mouse_pos.y][maze.mouse_pos.x] == 0) {    // Check if mouse has reached the goal position, if so break
            break;
        }
    }

    uint16_t total_params = 0;
	param_t param_list[512] = { 0 };
	bool back_to_wall = true;
    // Combine indivudal motions into larger profiles
    for (uint16_t curr_motion = 0; curr_motion < prelim_motions_length; curr_motion++) {
        switch (prelim_motion_queue[curr_motion]) {
            case FORWARD_ONE_CELL:
            	param_list[total_params].acceleration = RACE_ACCELERATION;
				param_list[total_params].max_speed = RACE_SPEED_FWD_MAX;
            	if (back_to_wall) { // For start cell in maze
            		param_list[total_params].distance += BACK_ON_WALL_TO_CELL_MM;
            		back_to_wall = false;
            	}
            	else {
            		param_list[total_params].distance += CELL_TO_CELL_MM;
            	}
                break;
            case TURN_RIGHT:
				#ifndef SMOOTH_TURNS
            		// No smooth turns
					param_list[total_params].end_speed = 0;
					param_list[total_params].distance += CELL_TO_CENTER_MM;

					total_params++;

					// Add rotational parameters for turn
					param_list[total_params] = Parameter_Packer(RIGHT_TURN_DEG, RACE_SPEED_ROT_MAX, 0, RACE_ACCELERATION, false);
					total_params++;

					// Start new forward profile for intial movement
					param_list[total_params].distance += CELL_TO_CENTER_MM;
				#else
            		// Smooth turns
            		param_list[total_params].end_speed = RACE_SPEED_FWD_SMOOTH_MAX;

            		total_params++;

            		// Add rotational parameters then forward parameters for smooth turn
            		param_list[total_params] = Parameter_Packer(RIGHT_TURN_DEG, RACE_SPEED_ROT_SMOOTH_MAX, 0, RACE_ACCELERATION, false);
            		total_params++;
            		param_list[total_params] = Parameter_Packer(SMOOTH_TURN_FWD_MM, RACE_SPEED_FWD_SMOOTH_MAX, RACE_SPEED_FWD_SMOOTH_MAX, RACE_ACCELERATION, false);
            		total_params++;
				#endif
                break;
            case TURN_LEFT:
				#ifndef SMOOTH_TURNS
            		// No smooth turns
					param_list[total_params].end_speed = 0;
					param_list[total_params].distance += CELL_TO_CENTER_MM;

					total_params++;

					// Add rotational parameters for turn
					param_list[total_params] = Parameter_Packer(LEFT_TURN_DEG, RACE_SPEED_ROT_MAX, 0, RACE_ACCELERATION, false);
					total_params++;

					// Start new forward profile for intial movement
					param_list[total_params].distance += CELL_TO_CENTER_MM;
				#else
            		// Smooth turns
            		param_list[total_params].end_speed = RACE_SPEED_FWD_SMOOTH_MAX;

            		total_params++;

            		// Add rotational parameters then forward parameters for smooth turn
            		param_list[total_params] = Parameter_Packer(LEFT_TURN_DEG, RACE_SPEED_ROT_SMOOTH_MAX, 0, RACE_ACCELERATION, false);
            		total_params++;
            		param_list[total_params] = Parameter_Packer(SMOOTH_TURN_FWD_MM, RACE_SPEED_FWD_SMOOTH_MAX, RACE_SPEED_FWD_SMOOTH_MAX, RACE_ACCELERATION, false);
            		total_params++;
				#endif
                break;
        }
    }

    // Execute motions according to parameter list
    uint16_t param = 0;
    while (param < (total_params + 1)) {
    	if (param_list[param].forward) { // Forward
    		Profile_Container(param_list[param], &forward_profile);
    	}
    	else { // Rotational
			#ifndef SMOOTH_TURNS
    			Profile_Container(param_list[param], &rotational_profile);
			#else // Use 2 parameters from list on smooth turns
    			Smooth_Turn_Container(param_list[param], param_list[param+1], &forward_profile, &rotational_profile);
    			param++;
			#endif
    	}
    	param++;
    }

    armed = false;
    motor_controller_enabled = false;

    Complete_Stop();
}
