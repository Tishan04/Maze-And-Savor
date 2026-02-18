// types.h
#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

//CONSTANTS
#define NO_OF_FLOORS 3
#define FLOOR_WIDTH 10
#define FLOOR_LENGTH 25
#define NO_OF_PLAYERS 3
#define START_POINTS 100
#define A_START_WIDTH 6
#define A_START_LENGTH 12
#define B_START_WIDTH 9
#define B_START_LENGTH 8
#define C_START_WIDTH 9
#define C_START_LENGTH 16
#define A_FIRST_NEXT_W 5 //SINCE PLAYER MOVE NORTH NEXT LENGTH IS SAME SO NO NEED TO DEFINE IT
#define B_FIRST_NEXT_L 7 //SINCE PLAYER MOVES EAST NEXT WIDTH DOESNT CHANGE
#define C_FIRST_NEXT_L 17 //SINCE PLAYER MOVES WEST NEXT WIDTH DOESNT CHANGE
#define STAIRS_CHANGE_INTERVAL 50

//ENUMERATIONS
typedef enum
{
    NORTH,
    EAST,
    SOUTH,
    WEST
}Direction;

typedef enum
{
    STAIR,
    POLE,
    WALL,
    STARTING_AREA,
    BRIDGE,
    BAWANA,
    INVALID_CELL,
    EMPTY,
    BAWANA_WALL,
    POLE_END
}CellType;

typedef enum
{ 
    NO,
    TWO_X,
    THREE_X,
    PLUS_1,
    PLUS_2,
    PLUS_3,
    PLUS_4,
    PLUS_5,
    SUB_1,
    SUB_2,
    SUB_3,
    SUB_4,
}Points;

typedef enum 
{
    NONE,
    FOOD_POISON,
    DISORIENTED,
    TRIGERED,
    HAPPY
}Special;

//STRUCTURES
typedef struct Cell
{
    int floor;
    int width;
    int length;
}Cell;

typedef struct Player
{
    char name;
    int direction;
    int floor;
    int width;
    int length;
    int turn;   //TURN IS ONLY ACTIVE WHEN THE PLAYER IS IN THE PLAYABLE ZONE
    int round;  //ROUND INCREASES WHEN ALL THREE HAVE COMPLETED THROWING DICE AT A CERTAIN INSTANCE
    int next_width;
    int next_length;
    int movement_points;
    int special_ability;
    bool is_player_active;
    bool is_under_ability;
    int count_under_special_ability;
}Player;

typedef struct Stair
{
    int start_floor;
    int start_width;
    int start_length;
    int end_floor;
    int end_width;
    int end_length;
    int direction;
}Stair;

/* FOR STAIRS BIDIRECTIONAL THE STAIR DIRECRION IS 0
UNIDIRECTIONAL UP 1
UNIDIRECTIONAL DOWN 2
*/

typedef struct Pole
{
    int start_floor;
    int end_floor;
    int width;
    int length;
}Pole;

typedef struct Wall
{
    int floor;
    int start_width;
    int start_length;
    int end_width;
    int end_length;
}Wall;

//FUNCTION PROTOTYPES
void initialize_floors();
void initialize_players ();
int roll_movement_dice();
int roll_direction_dice();
int read_seed_file();
void read_stairs_file();
void change_stair_direction (struct Stair *stairs, int number_of_stairs);
void read_poles_file();
void read_walls_file();
bool is_flag_location_valid(int floor, int width, int length);
bool is_flag_accessible (int floor, int width, int length);
void read_flag_file();
int count_cells();
void assign_to_NO();
void assign_points(int number_of_valid_cells);
Direction get_direction_from_dice(int movement_dice, Direction prev);
void execute_pole(struct Player *player);
void execute_stair(struct Player *player, struct Stair stairs[]);
const char *dir_to_str(Direction d);
void starting_area(struct Player *player);
void found_flag(struct Player *player, int dice);
void define_bawana();
void bawana_functionality(struct Player *player);
bool can_complete_move(struct Player *player, int dice, int direction_turn, struct Stair *stairs);
int update_movement_points(struct Player *player, int current_points, Points movement_type);
void player_capture_check (struct Player *player, struct Player players[], int NUMBER_OF_PLAYERS);
void move_to_cell(struct Player *player, int dice);
void move_players(struct Player players[], int NUMBER_OF_PLAYERS);
int play();

#endif 