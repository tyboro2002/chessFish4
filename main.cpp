#include <iostream>
#include "game.h"
#include "engine.h"
#include "tests.h"
#include "timing.h"
#include <limits>
#include "chessFish4Game.h"
#include "GUIEngineGame.h"
#include "MagicsTester.h"

using namespace std;

//#define RUN_TESTS // Define this macro to run tests
//#define RUN_GAME  // Uncomment this line to run the game
//#define DRAW // Define this macro to run draw
//#define MAGIC // Define this macro to run the initialisation of the magic numbers
#define TIME // Define this macro to time the code

int main() {
    //for (int i = 63; i>= 0; i--){
    //    std::bitset<64> binaryRepresentation(blackPawnAttacks[i]);
    //    std::cout << "0b" << binaryRepresentation << ",\n";
    //}
#ifdef MAGIC
    MagicsTester magicsTester;
    magicsTester.init_magic_numbers();
#endif

    init_all_sliders_attacks(false, "../foundMagicNumbers.txt", "../foundMagicNumbersCondensed.txt");

#ifdef RUN_TESTS
    TestRunner testRunner = TestRunner(false);
    testRunner.runAutomatedTestCases();
#endif

#ifdef TIME
    TimerRunner timerRunner = TimerRunner(1'000'000,1'000,true);
    timerRunner.runAutomatedTimerCases();
#endif

#ifdef DRAW
    //init_sliders_attacks(true);
    //init_sliders_attacks(false);

    ChessFishVisualiserUI ui;
    if (ui.Construct(1920*SCREEN_SIZE, 1080*SCREEN_SIZE, 1, 1, true))
        ui.Start();
#endif

#ifdef MAGIC_NUMBER_GENERATION
    MagicsTester magicsTester;
    magicsTester.init_magic_numbers();
#endif

#ifdef RUN_GAME
    //init_sliders_attacks(true);
    //init_sliders_attacks(false);
    runGame();
#endif
    return 0;
}


/*
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
 */