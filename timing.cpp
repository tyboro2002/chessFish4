#include "timing.h"
#include "tests.h"
#include "game.h"
#include "engine.h"
#include <chrono>
#include <complex>

using namespace std;

void time_code() {
    Board bord;
    Move move;
    MOVELIST moveList;
    // Clear move list
    moveList.count = 0;   // set each field for each move

    setup(&bord);
    //printBoard(&bord);
    //timing code
    // Get the starting timestamp
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000000; ++i) {
        //bitmap_white_pawn(bord) | bitmap_black_pawn(bord) | bitmap_black_king(bord) | bitmap_white_king(bord);
        //all_white_attacks(&bord, 1) | all_black_attacks(&bord, 1);
        //bitmap_white_knight(E5, &bord);
    }

    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    cout << "first aproach needed:" << endl;
    std::cout << "Time taken: " << duration << " microseconds" << std::endl;


    moveList.count = 0;   // set each field for each move

    setup(&bord);
    //printBoard(&bord);
    //timing code
    // Get the starting timestamp
    startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000000; ++i) {
        //bitmap_white_pawn(bord) | bitmap_black_pawn(bord) | bitmap_black_king(bord) | bitmap_white_king(bord);
        //all_white_attacks(&bord, 1) | all_black_attacks(&bord, 1);
        //bitmap_white_knight_speed_test(E5, &bord);
    }

    // Get the ending timestamp
    endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    cout << "second aproach needed:" << endl;
    std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}

void time_parralel_evaluation() {
    Board bord;
    MOVELIST moveList;
    setup(&bord);
    PositionTracker positionTracker;
    GenLegalMoveList(&moveList, &bord, &positionTracker);
    auto startTime = std::chrono::high_resolution_clock::now();

    //for (int i = 0; i < 1000000; ++i) orderMoves(&bord,&moveList);

    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    printMoveList(&moveList);
    cout << "first aproach needed:" << endl;
    std::cout << "Time taken: " << duration << " microseconds" << std::endl;

    GenLegalMoveList(&moveList, &bord, &positionTracker);
    startTime = std::chrono::high_resolution_clock::now();

    //for (int i = 0; i < 1000; ++i) orderMovesThreaded(&bord, &moveList);

    // Get the ending timestamp
    endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    printMoveList(&moveList);
    cout << "second aproach needed:" << endl;
    std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}

void time_bitwise_code() {
    //timing code
    // Get the starting timestamp
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000000; ++i) {
        countSetBits(3);
    }

    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}


void time_minimax_code() {
    Board bord;
    Move move;
    MOVELIST moveList;
    TranspositionTable transpositionTable;
    PositionTracker positionTracker;
    // Clear move list
    moveList.count = 0;   // set each field for each move
    setup(&bord);

    //timing code
    // Get the starting timestamp
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1; ++i){
        minimax_root(&bord, 1000, false, &move, &moveList, &transpositionTable, &positionTracker);
    }

    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}

void time_mate_test_code() {
    //timing code
    // Get the starting timestamp
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1; ++i) {
        runAutomatedTestsSilent();
    }

    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}

// Function to time and calculate mean and variation
U64 TimerRunner::timeFunction(std::function<U64()> func, U64 num_runs, const char* description) {
    std::cout << "timing: " << description << std::endl;
    std::vector<U64> run_times;
    U64 total_time = 0;

    for (int run = 0; run < num_runs; ++run) {
        U64 current_time = func();
        run_times.push_back(current_time);
        total_time += current_time;
    }

    // Calculate the mean
    U64 mean_time = total_time / num_runs;

    // Calculate the variation
    U64 sum_squared_diff = 0;
    for (U64 run_time : run_times) {
        U64 diff = run_time - mean_time;
        sum_squared_diff += diff * diff;
    }
    double variation = std::sqrt(sum_squared_diff / (num_runs-1));

    std::cout << "Mean time: " << mean_time << " nanoseconds" << endl;
    std::cout << "per call time: " << mean_time*1'000'000/itterations << " femtosecond  " << endl;
    std::cout << "Variation: " << variation << " nanoseconds" << endl << endl;
    return mean_time;
}

int TimerRunner::runAutomatedTimerCases() {
    totalTime += timeFunction([this] { return time_knight_moves(); },num_runs, "knight function");
    totalTime += timeFunction([this] { return time_king_moves(); },num_runs, "king function");
    totalTime += timeFunction([this] { return time_queen_moves(); },num_runs, "queen function");
    totalTime += timeFunction([this] { return time_rook_moves(); },num_runs, "rook function");
    totalTime += timeFunction([this] { return time_bischop_moves(); },num_runs, "bischop function");
    totalTime += timeFunction([this] { return time_pawn_moves(); },num_runs, "pawn function");
    totalTime += timeFunction([this] { return time_bischop_moves_on_the_fly(); },num_runs, "bischop moves on the fly function");
    totalTime += timeFunction([this] { return time_rook_moves_on_the_fly(); },num_runs, "rook moves on the fly function");
    totalTime += timeFunction([this] { return time_bischop_moves_magic(); },num_runs, "bischop moves magic number function");
    totalTime += timeFunction([this] { return time_rook_moves_magic(); },num_runs, "rook moves magic number function");
    std::cout << "all test cases runned in: " << totalTime << " nanoseconds" << std::endl;

    auto resolution = std::chrono::high_resolution_clock::period::num /
                      static_cast<double>(std::chrono::high_resolution_clock::period::den);

    std::cout << "Resolution of high_resolution_clock: " << resolution << " seconds per tick" << std::endl;
    // Convert seconds per tick to nanoseconds per tick
    double nanosecondsPerTick = resolution * 1000.0 * 1000.0 * 1000.0;

    std::cout << "Nanoseconds per tick: " << nanosecondsPerTick << std::endl;
    // Use dummy in a way that has a side effect
    if(printRunned) std::cout << "make compiler not optimise the functions: " << dummy_var << std::endl;
    return 0;
}

/*
 * below are all move timing functions
 */

// TODO below update timers
U64 TimerRunner::time_knight_moves() {
    Board bord;
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = bitmap_white_knight(i&63,&bord);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_king_moves() {
    Board bord;
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = bitmap_black_king(i&63,&bord);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_queen_moves() {
    Board bord;
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = bitmap_white_queen(i&63, &bord);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_rook_moves() {
    Board bord;
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = bitmap_white_rook(i&63,&bord);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_bischop_moves() {
    Board bord;
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = bitmap_white_bishop(i&63,&bord);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_pawn_moves() {
    Board bord;
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = bitmap_white_pawn(i&63,&bord);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_rook_moves_on_the_fly() {
    Board bord;
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = rook_attacks_on_the_fly(i&63,bord.white|bord.black);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_bischop_moves_on_the_fly() {
    Board bord;
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = bishop_attacks_on_the_fly(i&63,bord.white|bord.black);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_rook_moves_magic() {
    Board bord;
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = get_rook_attacks(i&63,bord.white|bord.black);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_bischop_moves_magic() {
    Board bord;
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = get_bishop_attacks(i&63,bord.white|bord.black);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}