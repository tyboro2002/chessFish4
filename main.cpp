#include "game.h"
#include "tests/tests.h"
#include "timing/timing.h"
#include "visuals/GUIEngineGame.h"
#include "magic_numbers/MagicsTester.h"
#include "UCI/UCISupport.h"

using namespace std;

#define RUN_TESTS // Define this macro to run tests
//#define RUN_GAME  // Uncomment this line to run the game
#define DRAW // Define this macro to run draw
//#define MAGIC // Define this macro to run the initialisation of the magic numbers
//#define TIME // Define this macro to time the code
#define UCI // Define this macro to start the engine in UCI mode (if this is on all other macros are neglected)

int main() {
#ifdef UCI
    UCISupport uciSupport;
    uciSupport.runUCI(); // run the UCI mode (exits the program if a quit command is sent)
#endif

#ifdef MAGIC
    MagicsTester magicsTester;
    magicsTester.init_magic_numbers();
#endif

    init_all_sliders_attacks(false, "../magic_numbers/foundMagicNumbers.txt", "../magic_numbers/foundMagicNumbersCondensed.txt");

#ifdef RUN_TESTS
    TestRunner testRunner = TestRunner(true,true);
    testRunner.runAutomatedTestCases();
#endif

#ifdef TIME
    TimerRunner timerRunner = TimerRunner(100,1'000,true); // 1'000'000, 1'000
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