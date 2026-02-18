#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"

//INITIALIZING THE FLOORS
CellType floor1[FLOOR_WIDTH][FLOOR_LENGTH];
CellType floor2[FLOOR_WIDTH][FLOOR_LENGTH];
CellType floor3[FLOOR_WIDTH][FLOOR_LENGTH];

//SETTING THE BOUNDARIES
void initialize_floors()
{
    //GROUND FLOOR
    for (int w = 0; w < FLOOR_WIDTH; w++)
    {
        for (int l = 0; l < FLOOR_LENGTH; l++)
        {
            //STARTING AREA
            if ((l >= 7 && l <= 16) && (w >= 6 && w <= 9))
            {
                floor1[w][l] = STARTING_AREA;
            }
            //BAWANA AREA
            else if ((l > 20 && l <= 24) && (w > 6 && w <= 9))
            {
                floor1[w][l] = BAWANA;
            }
            //BAWANA WALL CELLS
            else if ((l == 20 && w >= 6) || (w == 6 && l > 19))
            {
                floor1[w][l] = BAWANA_WALL;
            }
            else
            {
                floor1[w][l] = EMPTY;
            }
        }
    }

    //1ST FLOOR
    for (int w = 0; w < FLOOR_WIDTH; w++)
    {
        for (int l = 0; l < FLOOR_LENGTH; l++)
        {
            if ((l >= 8 && l <= 16) && (w >= 0 && w <= 5))
            {
                floor2[w][l] = INVALID_CELL;
            }
            else
            {
                floor2[w][l] = EMPTY;
            }
        }
    }

    //2ND FLOOR
    for (int w = 0; w < FLOOR_WIDTH; w++)
    {
        for (int l = 0; l < FLOOR_LENGTH; l++)
        {
            if (((l >= 0 && l <= 7) && (w >= 0 && w <= 9)) ||
            ((l >= 17 && l <= 24) && (w >= 0 && w <= 9)))
            {
                floor3[w][l] = INVALID_CELL;
            }
            else
            {
                floor3[w][l] = EMPTY;
            }
        }
    }
}

//CREATING AN ARRAY OF PLAYER STRUCTS
struct Player players[3];

//RULE 01 - INITIALIZING STARTING POSITIONS OF PLAYERS
//FUNCTION TO INITIALIZE THE VALUES TO THE PLAYERS
void initialize_players ()
{
    players[0].name = 'A';
    players[0].direction = NORTH;
    players[0].floor = 0;
    players[0].width = A_START_WIDTH;
    players[0].length = A_START_LENGTH;
    players[0].turn = 0;
    players[0].next_width = A_FIRST_NEXT_W;
    players[0].next_length = A_START_LENGTH;
    players[0].movement_points = START_POINTS;
    players[0].special_ability = 0;
    players[0].is_player_active = false;
    players[0].is_under_ability = false;
    players[0].count_under_special_ability = 0;

    players[1].name = 'B';
    players[1].direction = WEST;
    players[1].floor = 0;
    players[1].width = B_START_WIDTH;
    players[1].length = B_START_LENGTH;
    players[1].turn = 0;
    players[1].next_width = B_START_WIDTH;
    players[1].next_length = B_FIRST_NEXT_L;
    players[1].movement_points = START_POINTS;
    players[1].special_ability = 0;
    players[1].is_player_active = false;
    players[1].is_under_ability = false;
    players[1].count_under_special_ability = 0;

    players[2].name = 'C';
    players[2].direction = EAST;
    players[2].floor = 0;
    players[2].width = C_START_WIDTH;
    players[2].length = C_START_LENGTH;
    players[2].turn = 0;
    players[2].next_width = C_START_WIDTH;
    players[2].next_length = C_FIRST_NEXT_L;
    players[2].movement_points = START_POINTS;
    players[2].special_ability = 0;
    players[2].is_player_active = false;
    players[2].is_under_ability = false;
    players[2].count_under_special_ability = 0;
}

//RULE 02 - ROLLING TWO DICES
//ROLLING THE TWO DICES
int roll_movement_dice() {
    return (rand() % 6) + 1;
}

int roll_direction_dice() {
    return (rand() % 6) + 1;
}

bool game_over = false;

//FUNCTION TO READ THE SEED FILE
int read_seed_file()
{
    unsigned int seed;
    FILE *file_seed = fopen("seed.txt", "r");

    if (!file_seed)
    {
        perror("Unable to open seed.txt file");
        exit(1);
    }

    fscanf(file_seed, "%u", &seed);
    fclose(file_seed);

    return seed;
}

//ARRAYS TO STORE THE NUMBER OF STAIRS EACH CELL HAVE
int stair_count_floor1[FLOOR_WIDTH][FLOOR_LENGTH] = {0};
int stair_count_floor2[FLOOR_WIDTH][FLOOR_LENGTH] = {0};
int stair_count_floor3[FLOOR_WIDTH][FLOOR_LENGTH] = {0};

struct Stair *stairs = NULL;
int number_of_stairs = 0;
int global_turn_count = 0;

//FUNCTION TO READ THE STAIRS FILE
void read_stairs_file()
{
    FILE *file_stairs = fopen("stairs.txt", "r");

    if (!file_stairs)
    {
        perror("Error opening the file 'stairs.txt'");
        exit(1);
    }

    int stair_capacity = 10;
    int stair_count = 0;
    stairs = malloc(stair_capacity * sizeof(struct Stair));
    if(!stairs)
    {
        perror("Memory allocation to stairs failed");
        exit(1);
    }

    while(1)
    {
        int start_floor, start_width, start_length;
        int end_floor, end_width, end_length;

        int result = fscanf(file_stairs, "[%d, %d, %d, %d, %d, %d]\n",
        &start_floor, &start_width, &start_length,
        &end_floor, &end_width, &end_length);

        if (result == EOF)
        {
            break;
        }
        if (result != 6)
        {
            char badline[256];
            fgets(badline, sizeof(badline), file_stairs);

            FILE *log_file = fopen("log.txt", "a");
            fprintf(log_file, "A stair doesn't have enough data to implement it. Line: %s", badline);
            fclose(log_file);
            continue;
        }

        if (start_width < 0 || start_width >= FLOOR_WIDTH || start_length < 0 || start_length >= FLOOR_LENGTH ||
            end_width < 0 || end_width >= FLOOR_WIDTH || end_length < 0 || end_length >= FLOOR_LENGTH)
        {
            FILE *log_file = fopen ("log.txt", "a");
            fprintf(log_file, "%d, %d, %d, %d, %d, %d stair has invalid cordinates.\n", start_floor, start_width, start_length, end_floor, end_width, end_length);
            fclose (log_file);
            continue;
        }

        if (stair_count >= stair_capacity)
        {
            stair_capacity *= 2;
            stairs = realloc(stairs, stair_capacity * sizeof(struct Stair));
            if(!stairs)
            {
                perror("Memory allocation to stairs failed");
                exit(1);
            }
        }

        if (start_floor >= end_floor)
        {
            FILE *log_file = fopen ("log.txt", "a");
            fprintf (log_file, "%d, %d, %d, %d, %d, %d stair is not in the correct format.\n", start_floor, start_width, start_length, end_floor, end_width, end_length);
            fclose (log_file);
            continue;
        }

        stairs[stair_count].start_floor = start_floor;
        stairs[stair_count].start_width = start_width;
        stairs[stair_count].start_length = start_length;
        stairs[stair_count].end_floor = end_floor;
        stairs[stair_count].end_width = end_width;
        stairs[stair_count].end_length = end_length;
        stairs[stair_count].direction = 0;        

        if (start_floor == 0)
        {
            floor1[start_width][start_length] = STAIR;
            stair_count_floor1[start_width][start_length]++;
            if (stair_count_floor1[start_width][start_length] > 2)
            {
                printf("Error: More than 2 stairs assigned to one cell on Floor 0!\n");
                exit(1);
            }
        }
        else if (start_floor == 1)
        {
            floor2[start_width][start_length] = STAIR;
            stair_count_floor2[start_width][start_length]++;
            if (stair_count_floor2[start_width][start_length] > 2)
            {
                printf("Error: More than 2 stairs assigned to one cell on Floor 1!\n");
                exit(1);
            }
        }
        if (end_floor == 1)
        {
            floor2[end_width][end_length] = STAIR;
            stair_count_floor2[end_width][end_length]++;

            if (stair_count_floor2[end_width][end_length] > 2)
            {
                printf("Error: More than 2 stairs assigned to one cell on Floor 1!\n");
                exit(1);
            }
        }
        else if (end_floor == 2)
        {
            floor3[end_width][end_length] = STAIR;
            stair_count_floor3[end_width][end_length]++;

            if (stair_count_floor3[end_width][end_length] > 2)
            {
                printf("Error: More than 2 stairs assigned to one cell on Floor 2!\n");
                exit(1);
            }
        }
        
        //BLOCKING THE INTERSECTION CELL IN THE 1ST FLOOR WHEN THE STAIR SPAN FOR ALL THREE FLOORS
        if (start_floor == 0 && end_floor == 2)
        {
            floor2[start_width][start_length] = INVALID_CELL;
        }
        
        stair_count ++;
    }

    fclose(file_stairs);
    number_of_stairs = stair_count;
}

//RULE 06 - RANDOMLY CHANGE THE DIRECTION OF THE STAIRS EVERY 5 TURNS
void change_stair_direction (struct Stair *stairs, int number_of_stairs)
{
    for (int i = 0; i < number_of_stairs; i++)
    {
        int r = rand() % 3;
        if (r == 0)
        {
            stairs[i].direction = 0;
        }
        else if (r == 1)
        {
            stairs[i].direction = 1;
        }
        else 
        {
            stairs[i].direction = 2;
        }
    }
}

struct Pole *poles = NULL;
int number_of_poles = 0;

//FUNCTION TO READ THE POLE FILE AND ASSIGN THE CELLS
void read_poles_file()
{
    FILE *file_poles = fopen("poles.txt", "r");
    if (!file_poles)
    {
        perror("Error opening the file 'poles.txt'");
        exit(1);
    }

    int pole_capacity = 10;
    int pole_count = 0;
    poles = malloc(pole_capacity * sizeof(struct Pole));
    if (!poles)
    {
        perror("Memory allocation to poles failed");
        exit(1);
    }

    while (1)
    {
        int start_floor, end_floor, width, length;
        int t = fscanf(file_poles, "[%d, %d, %d, %d]\n", &start_floor, &end_floor, &width, &length);

        if (t == EOF)
        {
            break;
        }
        if (t != 4)
        {
            char badline[256];
            fgets(badline, sizeof(badline), file_poles);

            FILE *log_file = fopen("log.txt", "a");
            fprintf(log_file, "A pole doesn't have enough data to implement it. Line: %s", badline);
            fclose(log_file);
            continue;
        }

        if (width < 0 || width >= FLOOR_WIDTH || length < 0 || length >= FLOOR_LENGTH)
        {
            FILE *log_file = fopen ("log.txt", "a");
            fprintf (log_file, "%d, %d, %d, %d pole has invalid cordinates.\n", start_floor, end_floor, width, length);
            fclose (log_file);
        }

        if (start_floor >= end_floor)
        {
            FILE *log_file = fopen ("log.txt", "a");
            fprintf (log_file, "%d, %d, %d, %d pole is not in the correct format.\n", start_floor, end_floor, width, length);
            fclose (log_file);
            continue;
        }

        if (pole_count >= pole_capacity)
        {
            pole_capacity *= 2;
            poles = realloc(poles, pole_capacity * sizeof(struct Pole));
            if (!poles)
            {
                perror("Memory allocation to poles failed");
                exit(1);
            }
        }

        poles[pole_count].start_floor = start_floor;
        poles[pole_count].end_floor = end_floor;
        poles[pole_count].width = width;
        poles[pole_count].length = length;

        if (start_floor == 0 && end_floor == 2)
        {
            if (floor2[width][length] == INVALID_CELL || floor2[width][length] == WALL)
            {
                floor3[width][length] = POLE;
                floor1[width][length] = POLE_END;
            }
            else
            {
                floor1[width][length] = POLE_END;
                floor2[width][length] = POLE;
                floor3[width][length] = POLE;
            }
        }
        else if (start_floor == 0 && end_floor == 1)
        {
            floor2[width][length] = POLE;
            floor1[width][length] = POLE_END;
        }
        else if (start_floor == 1)
        {
            floor3[width][length] = POLE;
            floor2[width][length] = POLE_END;
        }

        pole_count++;
    }

    fclose(file_poles);
    number_of_poles = pole_count;
}

struct Wall *walls = NULL;
int number_of_walls;

//FUNCTION TO READ THE WALLS FILE
void read_walls_file()
{
    FILE *file_walls = fopen("walls.txt", "r");

    if (!file_walls)
    {
        perror("Error opening the file 'walls.txt'");
        exit(1);
    }

    int wall_capacity = 10;
    int wall_count = 0;
    walls = malloc(wall_capacity * sizeof(struct Wall));
    if(!walls)
    {
        perror("Memory allocation to walls failed");
        exit(1);
    }

    while (1)
    {
        int wf, sw, sl, ew, el;
        int read = fscanf(file_walls, "[%d, %d, %d, %d, %d]\n", &wf, &sw, &sl, &ew, &el);

        if (read == EOF)
        {
            break;
        }

        if (read != 5) 
        {
            char badline[256];
            fgets(badline, sizeof(badline), file_walls);

            FILE *log_file = fopen("log.txt", "a");
            fprintf(log_file, "A wall doesn't have enough data to implement it. Line: %s", badline);
            fclose(log_file);
            continue;
        }

        if (wf < 0 || wf > 2 ||
            sw < 0 || sw >= FLOOR_WIDTH || sl < 0 || sl >= FLOOR_LENGTH ||
            ew < 0 || ew >= FLOOR_WIDTH || el < 0 || el >= FLOOR_LENGTH)
        {
            FILE *log_file = fopen ("log.txt", "a");
            fprintf (log_file, "%d, %d, %d, %d, %d wall has invalid cordinates.\n", wf, sw, sl, ew, el);
            fclose (log_file);
            continue;
        }

        if (wall_count >= wall_capacity)
        {
            wall_capacity *= 2;
            walls = realloc(walls, wall_capacity * sizeof(struct Wall));
            if (!walls) 
            { 
                perror("Memory allocation to walls failed"); 
                exit(1);
            }
        }

        walls[wall_count].floor = wf;
        walls[wall_count].start_width = sw;
        walls[wall_count].start_length = sl;
        walls[wall_count].end_width = ew;
        walls[wall_count].end_length = el;

        if (sw == ew)
        {
            int minl = 0;
            int maxl = 0;
            if (sl < el)
            {
                minl = sl;
                maxl = el;
            }
            else
            {
                minl = el;
                maxl = sl;
            }
            for (int l = minl; l <= maxl; l++)
            {
                if (wf == 0) 
                {
                    floor1[sw][l] = WALL;
                }
                else if (wf == 1) 
                {
                    floor2[sw][l] = WALL;
                }
                else if (wf == 2) 
                {
                    floor3[sw][l] = WALL;
                }
            }
        }
        else if (sl == el)
        {
            int minw = 0;
            int maxw = 0;
            if (sw < ew)
            {
                minw = sw;
                maxw = ew;
            }
            else
            {
                minw = ew;
                maxw = sw;
            }
            for (int w = minw; w <= maxw; w++)
            {
                if (wf == 0) 
                {
                    floor1[w][sl] = WALL;
                }
                else if (wf == 1) 
                {
                    floor2[w][sl] = WALL;
                }
                else if (wf == 2) 
                {
                    floor3[w][sl] = WALL;
                }
            }
        }
        else
        {
            printf("Skipping wall: not aligned [%d, %d, %d, %d, %d]\n", wf, sw, sl, ew, el);
        }

        wall_count++;
    }

    number_of_walls = wall_count;
    fclose(file_walls);
}

int flag[3];

//CHECK IF THE FLAG IS PLACED IN A VALID POSITION
bool is_flag_location_valid(int floor, int width, int length)
{
    if (floor < 0 || floor > 2 || width < 0 || width >= FLOOR_WIDTH || length < 0 || length >= FLOOR_LENGTH)
    {
        return false;
    }

    if (floor == 0)
    {
        if ((floor1[width][length] == WALL))
        {
            printf ("Flag is placed on a Wall. Cannot start the game.\n");
            return false;
        }
        else if ((floor1[width][length] == STARTING_AREA))
        {
            printf ("Flag is placed in the Starting area. Cannot start the game.\n");
            return false;
        }
        else if ((floor1[width][length] == BAWANA))
        {
            printf ("Flag is placed in the Bawana area. Cannot start the game.\n");
            return false;
        }
        else if((floor1[width][length] == INVALID_CELL))
        {
            printf ("Flag is placed in an Invalid area. Cannot start the game.\n");
            return false;
        }
        else if ((floor1[width][length] == BAWANA_WALL))
        {
            printf ("Flag is placed in the Bawana wall. Cannot start the game.\n");
            return false;
        }
    }
    else if (floor == 1)
    {
        if ((floor2[width][length] == WALL))
        {
            printf ("Flag is placed on a Wall. Cannot start the game.\n");
            return false;
        }
        else if ((floor2[width][length] == INVALID_CELL))
        {
            printf ("Flag is placed in an Invalid area. Cannot start the game.\n");
            return false;
        }
    }
    else if (floor == 2)
    {
        if ((floor3[width][length] == WALL))
        {
            printf ("Flag is placed on a Wall. Cannot start the game.\n");
            return false;
        }
        else if ((floor3[width][length] == INVALID_CELL))
        {
            printf ("Flag is placed in an Invalid area. Cannot start the game.\n");
            return false;
        }
    }
    
    return true;
}

//FUNCTION TO CHECK WHETHER THE FLAG IS PLACED IN AN ACCESSIBLE POSITION
bool is_flag_accessible (int floor, int width, int length)
{
    int west_l = length - 1;
    int east_l = length + 1;
    int north_w = width - 1;
    int south_w = width + 1;

    if (floor == 0)
    {
        if ((floor1[width][west_l] == WALL || floor1[width][west_l] == STARTING_AREA || floor1[width][west_l] == INVALID_CELL) && 
        (floor1[width][east_l] == WALL || floor1[width][east_l] == STARTING_AREA || floor1[width][east_l] == BAWANA_WALL || floor1[width][west_l] == INVALID_CELL) &&
        (floor1[north_w][length] == INVALID_CELL || floor1[north_w][length] == WALL) &&
        (floor1[south_w][length] == INVALID_CELL || floor1[south_w][length] == BAWANA_WALL || floor1[south_w][length] == WALL || floor1[south_w][length] == STARTING_AREA) &&
        (floor1[width][length] != STAIR) && (floor1[width][length] != POLE_END))
        {
            printf ("The flag is not placed in an inaccessible cell in the ground floor. Cannot start game.\n");
            return false;
        }
    }
    else if (floor == 1)
    {
        if ((floor2[width][west_l] == WALL || floor2[width][west_l] == INVALID_CELL) && 
        (floor2[width][east_l] == WALL || floor2[width][west_l] == INVALID_CELL) &&
        (floor2[north_w][length] == INVALID_CELL || floor2[north_w][length] == WALL) &&
        (floor2[south_w][length] == INVALID_CELL || floor2[south_w][length] == WALL) &&
        (floor2[width][length] != STAIR) && (floor2[width][length] != POLE_END))
        {
            printf ("The flag is not placed in an inacessible cell in the first floor. Cannot start game.\n");
            return false;
        }
    }
    else
    {
        if ((floor3[width][west_l] == WALL || floor3[width][west_l] == INVALID_CELL) && 
        (floor3[width][east_l] == WALL || floor3[width][west_l] == INVALID_CELL) &&
        (floor3[north_w][length] == INVALID_CELL || floor3[north_w][length] == WALL) &&
        (floor3[south_w][length] == INVALID_CELL || floor3[south_w][length] == WALL) &&
        (floor3[width][length] != STAIR))
        {
            printf ("The flag is not placed in an inaccessible cell in the second floor. Cannot start game.\n");
            return false;
        }
    }
    return true;
}

//FUNCTION TO READ THE DETAILS IN THE FLAG FILE
void read_flag_file()
{
    FILE *file_flag = fopen("flag.txt", "r");

    if (!file_flag)
    {
        perror("Unable to read the flag.txt file\n");
        exit (1);
    }

    if (fscanf(file_flag, "[%d, %d, %d]", 
        &flag[0], 
        &flag[1], 
        &flag[2]) != 3)
    {
        fprintf(stderr, "Error: invalid flag format in flag.txt\n");
        fclose(file_flag);
        exit(1);
    }

    if(!is_flag_location_valid(flag[0], flag[1], flag[2]))
    {
        fclose(file_flag);
        exit(1);
    }

    if (!is_flag_accessible(flag[0], flag[1], flag[2]))
    {
        fclose(file_flag);
        exit(1);
    }
    
    fclose(file_flag);
}

//ARRAYS THAT HOLDS THE MOVEMENT POINT TYPE
Points movement_hold[NO_OF_FLOORS][FLOOR_WIDTH][FLOOR_LENGTH];

//COUNT THE TOTAL NUMBER OF VALID CELLS (TO ASSIGN MOVEMENT POINTS)
int count_cells()
{
    CellType cell;
    int number_of_valid_cells = 0;

    for (int f = 0; f < NO_OF_FLOORS; f++)
    {
        for (int w = 0; w < FLOOR_WIDTH; w++)
        {
            for (int l = 0; l < FLOOR_LENGTH; l++)
            {
                if (f == 0) 
                {
                    cell = floor1[w][l];
                }
                else if (f == 1) 
                {
                    cell = floor2[w][l];
                }
                else 
                {
                    cell = floor3[w][l];
                }

                if (cell != WALL && cell != STARTING_AREA && cell != INVALID_CELL && 
                    cell != BAWANA && cell != BAWANA_WALL && cell != STAIR && cell != POLE)
                {
                    number_of_valid_cells++;
                }
            }
        }
    }

    return number_of_valid_cells;
}

//RULE 09 - DECLARATION OF ARRAY TO HOLD MOVEMENT POINTS
void assign_to_NO()
{
    for (int f = 0; f < NO_OF_FLOORS; f++)
    {
        for (int w = 0; w < FLOOR_WIDTH; w++)
        {
            for (int l = 0; l < FLOOR_LENGTH; l++)
            {
                movement_hold[f][w][l] = NO;
            }
        }
    }
}

//RULE 10 - ASSIGNING CONSUMABLE POINTS TO CELLS IN THE GIVEN PERCENTAGE OF VALID CELLS
//ASSIGNING POINTS
void assign_points(int number_of_valid_cells)
{
    int subtract_count = (number_of_valid_cells * 35) / 100;
    int small_bonus_count = (number_of_valid_cells * 25) / 100;
    int big_bonus_count = (number_of_valid_cells * 10) / 100;
    int multiplier_count = (number_of_valid_cells * 5) / 100;

    assign_to_NO();

    int z = 0;
    int retries = 0;
    while (z < subtract_count && retries < 1000)
    {
        z++;
        int f = rand() % NO_OF_FLOORS;
        int w = rand() % FLOOR_WIDTH;
        int l = rand() % FLOOR_LENGTH;

        CellType cell;
        if (f == 0) 
        {
            cell = floor1[w][l];
        }
        else if (f == 1) 
        {
            cell = floor2[w][l];
        }
        else 
        {
            cell = floor3[w][l];
        }

        if (cell == WALL || cell == STARTING_AREA || cell == BAWANA_WALL || cell == STAIR || cell == POLE ||
            cell == BAWANA || cell == INVALID_CELL || movement_hold[f][w][l] != NO)
        {
            continue;
        }

        int r = rand() % 4;
        if (r == 0) 
        {
            movement_hold[f][w][l] = SUB_1;
        }
        else if (r == 1) 
        {
            movement_hold[f][w][l] = SUB_2;
        }
        else if (r == 2) 
        {
            movement_hold[f][w][l] = SUB_3;
        }
        else 
        {
            movement_hold[f][w][l] = SUB_4;
        }
        retries++;
    }

    z = 0;
    retries = 0;
    while (z < small_bonus_count && retries < 1000)
    {
        z++;
        int f = rand() % NO_OF_FLOORS;
        int w = rand() % FLOOR_WIDTH;
        int l = rand() % FLOOR_LENGTH;

        CellType cell;
        if (f == 0) 
        {
            cell = floor1[w][l];
        }
        else if (f == 1) 
        {
            cell = floor2[w][l];
        }
        else 
        {
            cell = floor3[w][l];
        }

        if (cell == WALL || cell == STARTING_AREA || cell == BAWANA_WALL || 
            cell == BAWANA || cell == INVALID_CELL || movement_hold[f][w][l] != NO)
        {
            continue;
        }

        int r = rand() % 2;
        movement_hold[f][w][l] = (r == 0) ? PLUS_1 : PLUS_2;

        retries++;
    }

    z = 0;
    retries = 0;
    while (z < big_bonus_count && retries < 1000)
    {
        z++;
        int f = rand() % NO_OF_FLOORS;
        int w = rand() % FLOOR_WIDTH;
        int l = rand() % FLOOR_LENGTH;

        CellType cell;
        if (f == 0) 
        {
            cell = floor1[w][l];
        }
        else if (f == 1) 
        {
            cell = floor2[w][l];
        }
        else 
        {
            cell = floor3[w][l];
        }

        if (cell == WALL || cell == STARTING_AREA || cell == BAWANA_WALL || 
            cell == BAWANA || cell == INVALID_CELL || movement_hold[f][w][l] != NO)
        {
            continue;
        }

        int r = rand() % 3;
        if (r == 0) 
        {
            movement_hold[f][w][l] = PLUS_3;
        }
        else if (r == 1) 
        {
            movement_hold[f][w][l] = PLUS_4;
        }
        else 
        {
            movement_hold[f][w][l] = PLUS_5;
        }

        retries++;
    }

    z = 0;
    retries = 0;
    while (z < multiplier_count && retries < 1000)
    {
        z++;
        int f = rand() % NO_OF_FLOORS;
        int w = rand() % FLOOR_WIDTH;
        int l = rand() % FLOOR_LENGTH;

        CellType cell;
        if (f == 0) 
        {
            cell = floor1[w][l];
        }
        else if (f == 1) 
        {
            cell = floor2[w][l];
        }
        else 
        {
            cell = floor3[w][l];
        }

        if (cell == WALL || cell == STARTING_AREA || cell == BAWANA_WALL || 
            cell == BAWANA || cell == INVALID_CELL || movement_hold[f][w][l] != NO)
        {
            continue;
        }

        int r = rand() % 2;
        if (r == 0)
        {
            movement_hold[f][w][l] = TWO_X;
        }
        else
        {
            movement_hold[f][w][l] = THREE_X;
        }

        retries++;
    }
}

//FUNCTION TO GET THE DIRECTION FROM THE DIRECTION DICE
Direction get_direction_from_dice(int movement_dice, Direction prev)
{
    switch(movement_dice)
    {
        case 1: return prev;
        case 2: return NORTH;
        case 3: return EAST;
        case 4: return SOUTH;
        case 5: return WEST;
        case 6: return prev;
        default: return prev;
    }
}

//RULE 04 - TAKING STAIRS AND POLES IN THE MIDDLE OF MOVEMENTS. LOGIC IMPLEMENTED IN THE MOVING FUNCTION
//FUNCTION TO EXECUTE THE FUNCTIONALITY OF A POLE
void execute_pole(struct Player *player)
{
    for (int p = 0; p < number_of_poles; p++)
    {
        if (player -> width == poles[p].width && player -> length == poles[p].length && 
        player -> floor == poles[p].end_floor)
        {
            player -> floor = poles[p].start_floor;
            printf("%c takes a pole and ends in floor %d, width %d, length %d.\n", player -> name, player -> floor, player -> width, player -> length);
            break;
        }
    }
}

//FUNCTION TO ECXECUTE THE FUNCTIONALITY OF A STAIR
void execute_stair(struct Player *player, struct Stair stairs[])
{
    for (int s = 0; s < number_of_stairs; s++)
    {
        if (stairs[s].direction == 0)
        {
            if (stairs[s].start_width == player -> width && stairs[s].start_length == player -> length && stairs[s].start_floor == player -> floor)
            {
                player -> floor = stairs[s].end_floor;
                player -> width = stairs[s].end_width;
                player -> length = stairs[s].end_length;
                printf ("%c takes a stair and end in floor %d, width %d, length %d.\n", player -> name, player -> floor, player -> width, player -> length);
                break;
            }
            else if (stairs[s].end_width == player -> width && stairs[s].end_length == player -> length && stairs[s].end_floor == player -> floor)
            {
                player -> floor = stairs[s].start_floor;
                player -> width = stairs[s].start_width;
                player -> length = stairs[s].start_length;
                printf ("%c takes a stair and end in floor %d, width %d, length %d.\n", player -> name, player -> floor, player -> width, player -> length);
                break;
            }
            else 
            {
                continue;
            }
        }
        else if (stairs[s].direction == 1)
        {
            if (player -> floor == stairs[s].start_floor && stairs[s].start_width == player -> width && stairs[s].start_length == player -> length)
            {
                player -> floor = stairs[s].end_floor;
                player -> width = stairs[s].end_width;
                player -> length = stairs[s].end_length;
                printf ("%c takes a stair and end in floor %d, width %d, length %d.\n", player -> name, player -> floor, player -> width, player -> length);
                break;
            }
            else
            {
                continue;
            }
        }
        else if (stairs[s].direction == 2)
        {
            if (player -> floor == stairs[s].end_floor && stairs[s].end_width == player -> width && stairs[s].end_length == player -> length)
            {
                player -> floor = stairs[s].start_floor;
                player -> width = stairs[s].start_width;
                player -> length = stairs[s].start_length;                
                printf ("%c takes a stair and end in floor %d, width %d, length %d.\n", player -> name, player -> floor, player -> width, player -> length);
                break;
            }
            else
            {
                continue;
            }
        }
    }
}

const char *dir_to_str(Direction d) 
{
    switch (d) {
        case NORTH: return "North";
        case EAST:  return "East";
        case SOUTH: return "South";
        case WEST:  return "West";
        default:    return "Unknown";
    }
}

void starting_area(struct Player *player)
{
    player -> is_player_active = false;
    if (player -> name == 'A')
    {    
        player -> width = A_START_WIDTH;
        player -> length = A_START_LENGTH;
    }
    else if (player -> name == 'B')
    {    
        player -> width = B_START_WIDTH;
        player -> length = B_START_LENGTH;
    }
    else if (player -> name == 'C')
    {    
        player -> width = C_START_WIDTH;
        player -> length = C_START_LENGTH;
    }
    player -> floor = 0;
}

//RULE 03 - FINDING THE FLAG 
void found_flag(struct Player *player, int dice)
{
    printf ("%c rolls a %d on the movement dice and %d on the direction dice, changes direction to %s and moves %d cells and is now at floor %d, width %d, length %d.\n", player -> name, dice, player -> direction, dir_to_str(player -> direction), dice, player -> floor, player -> width, player -> length);
    char name = player -> name;
    printf("The Player %c wins the game. Congratulations Player %c.\n", name, name);
    printf("Number of turns the winning player played is %d.\n", player -> turn);
    printf("Player have %d movement points left.\n", player -> movement_points);
    game_over = true;
}

//RULE 07 - BAWANA DECLARATION
Special bawana_cell_type[FLOOR_WIDTH][FLOOR_LENGTH];

//RULE 08 - ASSINGING ABILITIES TO BAWANA CELLS
//DEFINING THE BAWANA CELLS
void define_bawana()
{
    int poison_c = 0;
    int disoriented = 0;
    int trigered = 0;
    int happy = 0;

    Special bawana_cell;
    int max_w = 9;
    int min_w = 7;
    int max_l = 24;
    int min_l = 21;

    for (poison_c; poison_c < 2;)
    {
        int w = min_w + rand() % (max_w - min_w + 1);
        int l = min_l + rand() % (max_l - min_l + 1); 

        bawana_cell = bawana_cell_type[w][l];
        if (bawana_cell == NONE)
        {
            bawana_cell_type[w][l] = FOOD_POISON;
        }
        else
        {
            continue;
        }
        poison_c++;
    }
    for (disoriented; disoriented < 2;)
    {
        int w = min_w + rand() % (max_w - min_w + 1);
        int l = min_l + rand() % (max_l - min_l + 1); 

        bawana_cell = bawana_cell_type[w][l];
        if (bawana_cell == NONE)
        {
            bawana_cell_type[w][l] = DISORIENTED;
        }
        else
        {
            continue;
        }
        disoriented++;
    }
    for (trigered; trigered < 2;)
    {
        int w = min_w + rand() % (max_w - min_w + 1);
        int l = min_l + rand() % (max_l - min_l + 1); 
        bawana_cell = bawana_cell_type[w][l];
        if (bawana_cell == NONE)
        {
            bawana_cell_type[w][l] = TRIGERED;
        }
        else
        {
            continue;
        }
        trigered++;
    }
    for (happy; happy < 2;)
    {
        int w = min_w + rand() % (max_w - min_w + 1);
        int l = min_l + rand() % (max_l - min_l + 1); 

        bawana_cell = bawana_cell_type[w][l];
        if (bawana_cell == NONE)
        {
            bawana_cell_type[w][l] = HAPPY;
        }
        else
        {
            continue;
        }
        happy++;
    }
}

//FUNCTION TO EXECUTE THE FUNCTIONALITY OF THE BAWANA
void bawana_functionality(struct Player *player)
{
    int max_w = 9;
    int min_w = 7;
    int max_l = 24;
    int min_l = 21;

    //GENERATING RANDOM WIDTH AND LENGTH TO SPAWN THE PLAYER IN BAWANA
    int w = min_w + rand() % (max_w - min_w + 1);
    int l = min_l + rand() % (max_l - min_l + 1);
    player -> width = w;
    player -> length = l;
    player -> floor = 0;

    Special special_cell;
    special_cell = bawana_cell_type[w][l];

    if (special_cell == NONE)
    {
        int max_p = 100;
        int min_p = 10;

        //GENERATING RANDOM MOVEMENT POINTS IF THE PLAYER LANDED IN A NORMAL CELL
        int movement_p = min_p + rand() % (max_p - min_p + 1);

        player -> count_under_special_ability = 0;
        player -> movement_points = player -> movement_points + movement_p;
        player -> floor = 0;
        player -> width = 9;
        player -> length = 19;
        player -> direction = 0;
        printf("%c got placed in a normal cell in Bawana.\n", player -> name);
    }
    else if (special_cell == FOOD_POISON)
    {
        player -> is_under_ability = true;
        player -> count_under_special_ability = 0;
        player -> special_ability = 1;
        printf("%c is placed in Bawana and got food poisoned. And stays in floor %d, width %d, length %d for the next three turns.\n", player -> name, 0, w, l);
    }
    else if (special_cell == DISORIENTED)
    {
        player -> movement_points = player -> movement_points + 50;
        player -> width = 9;
        player -> length = 19;
        player -> direction = 0;
        player -> special_ability = 2;
        player -> is_under_ability = true;
        player -> count_under_special_ability = 0;
        printf("%c is placed in Bawana and got disoriented.\n", player -> name);
    }
    else if (special_cell == TRIGERED)
    {
        player -> movement_points += 50;
        player -> width = 9; 
        player -> length = 19;
        player -> direction = 0;
        player -> special_ability = 3;
        player -> is_under_ability = true;
        player -> count_under_special_ability = 0;
        printf ("%c is placed in Bawana and got triggered.\n", player -> name);
    }
    else if (special_cell == HAPPY)
    {
        player -> movement_points += 200;
        player -> width = 9; 
        player -> length = 19;
        player -> direction = 0;
        player -> special_ability = 0;
        player -> count_under_special_ability = 0;
        printf ("%c is placed in Bawana and is suprisingly happy.\n", player -> name);
    }

    if (player -> movement_points > 250) 
    {
        player -> movement_points = 250;
    }
}

//CHECKS IF THE MOVEMENT IS POSSIBLE
bool can_complete_move(struct Player *player, int dice, int direction_turn, struct Stair *stairs)
{
    int simulate_floor = player->floor;
    int simulate_width = player->width;
    int simulate_length = player->length;
    Direction dir = player->direction;

    int dw = 0, dl = 0;
    switch (dir)
    {
        case NORTH: dw = -1; dl = 0; break;
        case EAST:  dw = 0;  dl = 1; break;
        case SOUTH: dw = 1;  dl = 0; break;
        case WEST:  dw = 0;  dl = -1; break;
        default:    break;
    }

    for (int step = 0; step < dice; step++)
    {
        int next_width  = simulate_width + dw;
        int next_length = simulate_length + dl;

        if (next_width < 0 || next_width >= FLOOR_WIDTH ||
            next_length < 0 || next_length >= FLOOR_LENGTH)
        {
            if (direction_turn != 0)
            {
                printf("%c rolls a %d on the movement dice and cannot move in the %s direction. "
                       "Player remains at floor %d, width %d, length %d.\n",
                       player -> name, dice, dir_to_str(player -> direction),
                       player -> floor, player -> width, player -> length);
            }
            return false;
        }

        CellType cell;
        if (simulate_floor == 0)
        {
            cell = floor1[next_width][next_length];
        }
        else if (simulate_floor == 1)
        {
            cell = floor2[next_width][next_length];
        }
        else
        {
            cell = floor3[next_width][next_length];
        }

        if (cell == WALL || cell == INVALID_CELL || cell == BAWANA_WALL || cell == STARTING_AREA)
        {
            if (direction_turn != 0)
            {
                printf("%c rolls a %d on the movement dice and cannot move in the %s direction. "
                       "Player remains at floor %d, width %d, length %d.\n",
                       player -> name, dice, dir_to_str(player -> direction),
                       player -> floor, player -> width, player -> length);
            }
            return false;
        }

        simulate_width  = next_width;
        simulate_length = next_length;

        int safety = 0;
        while ((cell == POLE || cell == STAIR) && safety < 20)
        {
            bool found = false;

            if (cell == POLE)
            {
                for (int p = 0; p < number_of_poles; p++)
                {
                    if (poles[p].width == simulate_width && poles[p].length == simulate_length && poles[p].end_floor == simulate_floor) 
                    {
                        simulate_floor = poles[p].start_floor;
                        found = true;
                        break;
                    }
                }
            }
            else if (cell == STAIR)
            {
                for (int s = 0; s < number_of_stairs; s++)
                {
                    if (stairs[s].direction == 0)
                    {
                        if (stairs[s].start_floor == simulate_floor &&
                            stairs[s].start_width == simulate_width &&
                            stairs[s].start_length == simulate_length)
                        {
                            simulate_floor  = stairs[s].end_floor;
                            simulate_width  = stairs[s].end_width;
                            simulate_length = stairs[s].end_length;
                            found = true;
                            break;
                        }
                        else if (stairs[s].end_floor == simulate_floor &&
                                 stairs[s].end_width == simulate_width &&
                                 stairs[s].end_length == simulate_length)
                        {
                            simulate_floor  = stairs[s].start_floor;
                            simulate_width  = stairs[s].start_width;
                            simulate_length = stairs[s].start_length;
                            found = true;
                            break;
                        }
                    }
                    else if (stairs[s].direction == 1)
                    {
                        if (stairs[s].start_floor == simulate_floor &&
                            stairs[s].start_width == simulate_width &&
                            stairs[s].start_length == simulate_length)
                        {
                            simulate_floor  = stairs[s].end_floor;
                            simulate_width  = stairs[s].end_width;
                            simulate_length = stairs[s].end_length;
                            found = true;
                            break;
                        }
                    }
                    else if (stairs[s].direction == 2)
                    {
                        if (stairs[s].end_floor == simulate_floor &&
                            stairs[s].end_width == simulate_width &&
                            stairs[s].end_length == simulate_length)
                        {
                            simulate_floor  = stairs[s].start_floor;
                            simulate_width  = stairs[s].start_width;
                            simulate_length = stairs[s].start_length;
                            found = true;
                            break;
                        }
                    }
                }
            }
            if (!found) 
            {
                return false;
            }

            if (simulate_floor == 0)
            {
                cell = floor1[simulate_width][simulate_length];
            }
            else if (simulate_floor == 1)
            {
                cell = floor2[simulate_width][simulate_length];
            }
            else
            {
                cell = floor3[simulate_width][simulate_length];
            }

            safety++;
        }
    }

    return true;
}

//FUNCTION TO UPDATE THE MOVEMENT POINTS
int update_movement_points(struct Player *player, int current_points, Points movement_type)
{
    switch (movement_type)
    {
        case NO:
            return player -> movement_points = current_points;
        case TWO_X:
            return player -> movement_points = current_points * 2;
        case THREE_X:
            return player -> movement_points = current_points * 3;
        case PLUS_1:
            return player -> movement_points = current_points + 1;
        case PLUS_2:
            return player -> movement_points = current_points + 2;
        case PLUS_3:
            return player->movement_points = current_points + 3;
        case PLUS_4:
            return player->movement_points = current_points + 4;
        case PLUS_5:
            return player->movement_points = current_points + 5;
        case SUB_1:
            return player->movement_points = current_points - 1;
        case SUB_2:
            return player->movement_points = current_points - 2;
        case SUB_3:
            return player->movement_points = current_points - 3;
        case SUB_4:
            return player->movement_points = current_points - 4;
        default:
            return player->movement_points = current_points;
    }
}

//RULE 05 - CAPTURE THE PLAYER IN THE LANDING CELL AND SEND HIM TO THE STARTING AREA
void player_capture_check (struct Player *player, struct Player players[], int NUMBER_OF_PLAYERS)
{
    for (int i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        if (player -> name == players[i].name)
        {
            continue;
        }

        if (player -> width == players[i].width && player -> length == players[i].length && player -> floor == players[i].floor)
        {
            printf("%c is captured by %c and is sent to the starting area.\n", players[i].name, player -> name);
            starting_area(&players[i]);
        }
    }
}

//FUNCTION TO INITIATE THE MOVEMENTS OF THE PLAYER
void move_to_cell(struct Player *player, int dice)
{
    int current_width = player -> width;
    int current_length = player -> length;
    int current_floor = player -> floor;
    int current_points = player -> movement_points;
    int ability = player -> special_ability;
    Direction dir = player -> direction;

    int prev_floor = player -> floor;
    int prev_width = player -> width;
    int prev_length = player -> length;

    if (player -> floor == flag[0] && player -> width == flag[1] && player -> length == flag[2])
    {
        found_flag(player, dice);
        return;
    }

    int dw, dl;

    switch(dir)
    {
        case NORTH:
            dw = -1; dl = 0;
            break;
        case EAST:
            dw = 0; dl = 1;
            break;
        case SOUTH:
            dw = 1; dl = 0;
            break;
        case WEST:
            dw = 0; dl = -1;
            break;
        default:
            dw = 0; dl = 0;
            break;
    }

    int next_width, next_length;
    for (int step = 0; step < dice; step++)
    {
        current_width += dw;
        current_length += dl;

        player -> width = current_width;
        player -> length = current_length;

        if (player -> floor == flag[0] && player -> width == flag[1] && player -> length == flag[2])
        {
            found_flag(player, dice);
            return;
        }  

        CellType current_cell;
        Points movement_type;

        if (current_floor == 0)
        {
            current_cell = floor1[player -> width][player -> length];
            movement_type = movement_hold[0][player -> width][player -> length];
        }
        else if (current_floor == 1)
        {
            current_cell = floor2[player -> width][player -> length];
            movement_type = movement_hold[1][player -> width][player -> length];
        }
        else
        {
            current_cell = floor3[player -> width][player -> length];
            movement_type = movement_hold[2][player -> width][player -> length];
        }
        
        if (current_cell == POLE)
        {
            execute_pole(player);
        }
        else if (current_cell == STAIR)
        {
            execute_stair(player, stairs);
        }
        current_floor = player -> floor;

        current_points = update_movement_points(player, current_points, movement_type);
        if (current_points >= 250)
        {
            current_points = 250;
        }

        player -> movement_points = current_points;
        int temp_points = current_points;

        //RULE 11 - IF MOVEMENT POINTS GO NEGATIVE OR = 00. PLAYER TAKEN TO BAWANA
        if (current_points <= 0)
        {
            printf ("%c is taken to bawana due to not enough movement points. Current movement points is %d.\n", player -> name, player -> movement_points);
            bawana_functionality(player);
            break;
        }
        
        if (current_cell == BAWANA)
        {
            bawana_functionality(player);
            break;
        }

        int stair_chain_count = 0;
        int iterate_count = 0;
        while (1)
        {
            if (player -> floor == 0)
            {
                current_cell = floor1[player -> width][player -> length];
            }
            else if (player -> floor == 1)
            {
                current_cell = floor2[player -> width][player -> length];
            }
            else
            {
                current_cell = floor3[player -> width][player -> length];
            }

            if (current_cell != POLE && current_cell != STAIR)
            {
                break;
            }

            if (current_cell == POLE)
            {
                execute_pole(player);
            }
            else if (current_cell == STAIR)
            {
                if (stair_chain_count >= 0)
                {
                    break;
                }

                execute_stair(player, stairs);
                stair_chain_count++;

                if (player -> floor == prev_floor && player -> width == prev_width && player -> length == prev_length)
                {
                    break;
                }
            }

            if (player -> floor == 0)
            {
                movement_type = movement_hold[0][player -> width][player -> length];
            }
            else if (player -> floor == 1)
            {
                movement_type = movement_hold[1][player -> width][player -> length];
            }
            else
            {
                movement_type = movement_hold[2][player -> width][player -> length];
            }

            player -> movement_points = update_movement_points(player, player -> movement_points, movement_type);
            current_points = player -> movement_points;

            if (current_points <= 0)
            {
                printf ("%c is taken to bawana due to not enough movement points. Current movement points is %d.\n", player -> name, player -> movement_points);
                bawana_functionality(player);
                break;
            }
            
            iterate_count++;
            if (iterate_count >= 20)
            {
                player -> movement_points = temp_points;
                starting_area(player);
                break;
            }
        }

        if (current_width < 0 || current_width >= FLOOR_WIDTH || current_floor >= NO_OF_FLOORS || current_length < 0 || current_length >= FLOOR_LENGTH)
        {
            player -> width = prev_width;
            player -> length = prev_length;
            player -> floor = prev_floor;

            return;
        }

        current_width = player -> width;
        current_length = player -> length;
        current_floor = player -> floor;
        
        if (player -> floor == flag[0] && player -> width == flag[1] && player -> length == flag[2])
        {
            found_flag(player, dice);
            return;
        }
    }
}

void move_players(struct Player players[], int NUMBER_OF_PLAYERS)
{
    if (global_turn_count % STAIRS_CHANGE_INTERVAL == 0 && global_turn_count > 0)
    {
        change_stair_direction(stairs, number_of_stairs);
        printf ("The stair direction has been changed randomly.\n");
    }

    for (int i = 0; i < NUMBER_OF_PLAYERS; i++)
    {
        if (game_over)
        {
            return;
        }

        int dice = roll_movement_dice();
        if (!players[i].is_player_active)
        {
            if (dice == 6)
            {
                players[i].is_player_active = true;
                printf("%c is at the starting area and rolls 6 on the movement dice and is placed in the relevant first cell of the player in the maze.\n", players[i].name);
                if (players[i].name == 'A')
                {
                    players[i].width = A_FIRST_NEXT_W;
                    players[i].length = A_START_LENGTH;
                    
                    if (players[i].floor == flag[0] && players[i].width == flag[1] && players[i].length == flag[2])
                    {
                        found_flag(&players[i], dice);
                        return;
                    }
                }
                else if (players[i].name == 'B')
                {
                    players[i].width = B_START_WIDTH;
                    players[i].length = B_FIRST_NEXT_L;

                    if (players[i].floor == flag[0] && players[i].width == flag[1] && players[i].length == flag[2])
                    {
                        found_flag(&players[i], dice);
                        return;
                    }
                }
                else if (players[i].name == 'C')
                {
                    players[i].width = C_START_WIDTH;
                    players[i].length = C_FIRST_NEXT_L;

                    if (players[i].floor == flag[0] && players[i].width == flag[1] && players[i].length == flag[2])
                    {
                        found_flag(&players[i], dice);
                        return;
                    }
                }
                players[i].turn++;
                continue;
            }
            else
            {
                printf("%c is at the starting area and rolls %d on the movement dice cannot enter the maze.\n", players[i].name, dice);
                continue;
            }
        }

        if (players[i].is_under_ability)
        {
            if (players[i].special_ability == 1)
            {
                players[i].count_under_special_ability++;
                if (players[i].count_under_special_ability < 4)
                {
                    printf("%c turn is skipped due to food poisoning. Player remains at floor %d, width %d, length %d.\n", players[i].name, players[i].floor, players[i].width, players[i].length);
                    continue;
                }
                else
                {
                    players[i].special_ability = 0;
                    players[i].is_under_ability = false;
                    printf ("%c is placed in Bawana after food poisoning becomes fine.\n", players[i].name);
                    bawana_functionality(&players[i]);
                    continue;
                }
            }        
            else if (players[i].special_ability == 2)
            {
                players[i].count_under_special_ability++;
                dice = rand() % 6 + 1;
                int n = rand() % 4 + 1;
                if (n == 1)
                {
                    players[i].direction = NORTH;
                }
                else if (n == 2)
                {
                    players[i].direction = EAST;
                }
                else if (n == 3)
                {
                    players[i].direction = SOUTH;
                }
                else
                {
                    players[i].direction = WEST;
                }

                if (players[i].count_under_special_ability == 4)
                {
                    players[i].special_ability = 0;
                    players[i].is_under_ability = false;
                }
            }
            else if (players[i].special_ability == 3)
            {
                players[i].count_under_special_ability++;
                dice = dice * 2;
                if (players[i].count_under_special_ability == 4)
                {
                    players[i].special_ability = 0;
                    players[i].is_under_ability = false;
                }
            }
        }

        int turn_value_for_direction = players[i].turn % 4;
        if (players[i].turn > 0 && turn_value_for_direction == 0){
            int direction_dice = roll_direction_dice();
            if (!(direction_dice == 1 || direction_dice == 6))
            {
                players[i].direction = get_direction_from_dice(direction_dice, players[i].direction);   
            }
        }

        //RULE 12 - REDUCING 2 MOVEMENT POINTS IF THE PLAYER IS BLOCKED
        if(!can_complete_move(&players[i], dice, turn_value_for_direction, stairs))
        {
            players[i].turn++;
            players[i].movement_points = players[i].movement_points - 2;
            //printf ("%c rolls a %d on the movement dice and %d on the direction dice, changes direction to %s and moves %d cells and is now at floor %d, width %d, length %d.\n", players[i].name, dice, players[i].direction, dir_to_str(players[i].direction), dice, players[i].floor, players[i].width, players[i].length);
            if (players[i].movement_points <= 0)
            {
                printf ("%c is taken to bawana due to not enough movement points Current movement points is %d.\n", players[i].name, players[i].movement_points);
                bawana_functionality(&players[i]);
            }
            continue;
        }     

        move_to_cell(&players[i], dice);
        players[i].turn++;
        if (game_over)
        {
            return;
        }

        if(turn_value_for_direction == 0 && players[i].special_ability == 3)
        {
            printf ("%c rolls a %d on the movement dice and %d on the direction dice, changes direction to %s and moves %d cells and is now at floor %d, width %d, length %d.\n", players[i].name, dice/2, players[i].direction, dir_to_str(players[i].direction), dice, players[i].floor, players[i].width, players[i].length);
        }
        else
        {
            printf ("%c rolls a %d on the movement dice and %d on the direction dice, changes direction to %s and moves %d cells and is now at floor %d, width %d, length %d.\n", players[i].name, dice, players[i].direction, dir_to_str(players[i].direction), dice, players[i].floor, players[i].width, players[i].length);
        }
        player_capture_check(&players[i], players, NUMBER_OF_PLAYERS);
    }
    global_turn_count++;
}

int play()
{
    initialize_floors();
    read_stairs_file();
    read_poles_file();
    read_walls_file();
    read_flag_file();

    define_bawana();
    unsigned int seed = read_seed_file();
    srand(seed);

    assign_points(count_cells());
    initialize_players();

    int rounds = 0;
    const int MAX_ROUNDS = 100000;
    while (!game_over && rounds < MAX_ROUNDS)
    {
        move_players(players, NO_OF_PLAYERS);
        rounds++;
    }

    if (rounds == MAX_ROUNDS)
    {
        printf ("WARNING: Maximum number of rounds reached.\n");
        printf ("Player A position %d %d %d %d %d.\n", players[1].floor, players[1].width, players[1].length, players[1].movement_points, players[1].turn);
    }

    free(walls);
    free(stairs);
    free(poles);
    return 0;
}