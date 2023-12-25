#include <iostream>
#include "game.h"
#include "engine.h"
#include "tests.h"
#include "timing.h"
#include <limits>
#include "chessFish4Game.h"

using namespace std;

#define RUN_TESTS // Define this macro to run tests
//#define RUN_GAME  // Uncomment this line to run the game

int main() {
#ifdef RUN_TESTS
    TestRunner testRunner = TestRunner(false);
    testRunner.runAutomatedTestCases();
    //king_danger_squares_test();
    //path_test();
    //checking_test();
    //time_code();
    //move_test();
    //move_test_check();
    //time_bitwise_code();
    //fen_test();
    //legalMoveTest();
    //randomMoveTest();
    //move_test_halfmove();

    //time_minimax_code();
    //time_mate_test_code();
    //runAutomatedTests();

    //time_parralel_evaluation();

    //knightMovesGenerator();

    //randomTest();
#endif

#ifdef RUN_GAME
    runGame();
#endif

    return 0;
}
