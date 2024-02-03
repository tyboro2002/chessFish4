#include "timing.h"
#include "../tests/tests.h"
#include <chrono>
#include <complex>

using namespace std;

// Function to time and calculate mean and variation
U64 TimerRunner::timeFunction(const std::function<U64()>& func, U64 num_runsRequested, const char* description) {
    totalTests++;
    std::cout << "timing: " << description << std::endl;
    std::vector<U64> run_times;
    U64 total_time = 0;

    for (int run = 0; run < num_runsRequested; ++run) {
        U64 current_time = func();
        run_times.push_back(current_time);
        total_time += current_time;
    }

    // Calculate the mean
    U64 mean_time = total_time / num_runsRequested;

    // Calculate the variation
    U64 sum_squared_diff = 0;
    for (U64 run_time : run_times) {
        U64 diff = run_time - mean_time;
        sum_squared_diff += diff * diff;
    }
    double variation = std::sqrt(sum_squared_diff / (num_runsRequested - 1));

    std::cout << "Mean time: " << mean_time << " nanoseconds" << endl;
    std::cout << "per call time: " << mean_time*1'000'000/itterations << " femtosecondes  " << endl;
    std::cout << "Variation: " << variation << " nanoseconds" << endl << endl;
    return mean_time;
}

int TimerRunner::runAutomatedTimerCases() {
    totalTime += timeFunction([this] { return time_perft(1); },num_runs, "perft depth 1");
    totalTime += timeFunction([this] { return time_perft(2); },num_runs, "perft depth 2");
    totalTime += timeFunction([this] { return time_perft(3); },num_runs, "perft depth 3");
    //totalTime += timeFunction([this] { return time_perft(4); },num_runs, "perft depth 4"); // takes a lot too much time
    //totalTime += timeFunction([this] { return time_perft(5); },num_runs, "perft depth 5");
    //totalTime += timeFunction([this] { return time_perft(6); },num_runs, "perft depth 6");

    totalTime += timeFunction([this] { return time_perft_in_check(1); },num_runs, "perft depth 1 in check");
    totalTime += timeFunction([this] { return time_perft_in_check(2); },num_runs, "perft depth 2 in check");
    totalTime += timeFunction([this] { return time_perft_in_check(3); },num_runs, "perft depth 3 in check");
    //totalTime += timeFunction([this] { return time_perft_in_check(4); },num_runs, "perft depth 4 in check"); // takes a lot too much time
    //totalTime += timeFunction([this] { return time_perft_in_check(5); },num_runs, "perft depth 5 in check");
    //totalTime += timeFunction([this] { return time_perft_in_check(6); },num_runs, "perft depth 6 in check");

    //totalTime += timeFunction([this] { return time_all_attackers(); },num_runs, "all attackers function");

    itterations = 1'000'000; //temporary

    totalTime += timeFunction([this] { return time_knight_moves(); },num_runs, "knight function");
    totalTime += timeFunction([this] { return time_king_moves(); },num_runs, "king function");
    totalTime += timeFunction([this] { return time_queen_moves(); },num_runs, "queen function");
    totalTime += timeFunction([this] { return time_rook_moves(); },num_runs, "rook function");
    totalTime += timeFunction([this] { return time_bishop_moves(); }, num_runs, "bishop function");
    totalTime += timeFunction([this] { return time_pawn_moves(); },num_runs, "pawn function");

    /*
    totalTime += timeFunction([this] { return time_bischop_moves_on_the_fly(); },num_runs, "bishop moves on the fly function");
    totalTime += timeFunction([this] { return time_rook_moves_on_the_fly(); },num_runs, "rook moves on the fly function");
    totalTime += timeFunction([this] { return time_queen_moves_on_the_fly(); },num_runs, "queen moves on the fly function");
    */
    /*
    totalTime += timeFunction([this] { return time_bischop_moves_magic(); },num_runs, "bishop moves magic number function");
    totalTime += timeFunction([this] { return time_rook_moves_magic(); },num_runs, "rook moves magic number function");
    totalTime += timeFunction([this] { return time_queen_moves_magic(); },num_runs, "queen moves magic number function");
    */
    std::cout << "all test cases runned in: " << totalTime << " nanoseconds" << std::endl;
    std::cout << "mean amount of time per function: " << totalTime/totalTests << " nanoseconds" << std::endl;
    std::cout << "mean amount of time per bitmap: " << ((totalTime/totalTests)*1'000'000/itterations) << " femtosecondes" << std::endl;

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

U64 TimerRunner::time_knight_moves() {
    Board bord{};
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
    Board bord{};
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
    Board bord{};
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
    Board bord{};
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

U64 TimerRunner::time_bishop_moves() {
    Board bord{};
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
    Board bord{};
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
    Board bord{};
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
    Board bord{};
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

U64 TimerRunner::time_queen_moves_on_the_fly() {
    Board bord{};
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = bishop_attacks_on_the_fly(i&63,bord.white|bord.black) | rook_attacks_on_the_fly(i&63,bord.white|bord.black);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_rook_moves_magic() {
    Board bord{};
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
    Board bord{};
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

U64 TimerRunner::time_queen_moves_magic() {
    Board bord{};
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = get_queen_attacks(i&63,bord.white|bord.black);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_all_attackers() {
    Board bord{};
    setup(&bord);
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        dummy_var = all_attacks(&bord);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_perft(int depth) const {
    Board bord{};
    setup(&bord);
    TestRunner testRunner;
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        testRunner.generalPerft(&bord,depth,false,0,0ULL);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}

U64 TimerRunner::time_perft_in_check(int depth) const {
    Board bord{};
    setupEmpty(&bord);
    readInFen(&bord,"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    TestRunner testRunner;
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0;i<itterations;i++) {
        testRunner.generalPerft(&bord,depth,false,0,0ULL);
    }
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    return duration;
}
