#pragma once

/* path to engines for extra engines */
#define COMMANDS_PATH "../externalEngines/testCommands.txt"
#define OUTPUT_PATH "../externalEngines/output.txt"

/* size of transpositionTable (sort of cache size) */
#define TRANSPOSITION_TABLE_SIZE_DEFAULT (100 * 1024 * 1024)
#define TRANSPOSITION_TABLE_SIZE_MINIMAX (1024 * 1024 * 1024)
#define TRANSPOSITION_TABLE_SIZE_ITERATIVE_DEEPENING (200 * 1024 * 1024)

/* speed of simulations */
#define LOOP_FRAMES 1

/* mate constants */
const int MATE_SCORE = 1000000000; //TODO look why no define
const int MATE_THRESHOLD = 999000000; //TODO look why no define

/* visual configurations */
#define CHESS_SIZE 8
#define BITMAPS 64

#define TEXT_SIZE 2

#define CELL_SIZE 100
#define TOP_LEFT_X_FIELD 100
#define TOP_LEFT_y_FIELD 100
#define INTENSITY 150

#define CASTELING_RIGHTS_X (ScreenWidth() / 2 - 10)
#define CASTELING_RIGHTS_Y (ScreenHeight() / 2 + 60)
#define CASTELING_RIGHTS_D_TEXT_RIGHTS 30
#define CASTELING_RIGHTS_DIST_RIGHTS 100

#define WHITE_ENGINE_SELECTION_X 1000
#define WHITE_ENGINE_SELECTION_Y 100

#define BLACK_ENGINE_SELECTION_X 1300
#define BLACK_ENGINE_SELECTION_Y 100

#define END_GAME_X (ScreenWidth() / 2)
#define END_GAME_Y (ScreenHeight() / 2 - 200)
#define END_GAME_SIZE 3
#define END_GAME_TOLERANCE 100

#define ENGINE_TOLERANCE 100 //the amount of pixels we can click away from the center of the engine to rotate it

#define SIMULATION_RESULT_X 100
#define SIMULATION_RESULT_Y (ScreenHeight() - 150)

#define START_SIMULATION_X 100
#define START_SIMULATION_Y (ScreenHeight() - 90)
#define START_SIMULATION_WIDTH 80
#define START_SIMULATION_HEIGHT 30

#define RESET_SIMULATION_X 280
#define RESET_SIMULATION_Y (ScreenHeight() - 90)
#define RESET_SIMULATION_WIDTH 80
#define RESET_SIMULATION_HEIGHT 30

#define SIMULATION_BUTTON_INC_X 105
#define INC_SIMULATION_WIDTH 50
#define INC_SIMULATION_HEIGHT 30


#define SCREEN_SIZE 1 // the program will scale all sprites down this amount making the screen appear this amount larger
