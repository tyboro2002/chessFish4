#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include "moves.h"

class TimerRunner {
    /*
     * 1 second
     * 1 millisecond (1/1000 second)
     * 1 microsecond (1/1000 millisecond)
     * 1 nanosecond (1/1000 microsecond)
     * 1 picosecond (1/1000 nanosecond)
     * 1 femtosecond (1/1000 picosecond)
     * 1 attosecond (1/1000 femtosecond)
     * 1 zeptosecond (1/1000 attosecond)
     * 1 yoctosecond (1/1000 zeptosecond)
     */
    int totalTests = 0;
    U64 totalTime = 0;
    std::vector<std::string> runnedTests;
    bool printRunned = true;
    U64 itterations = 1000;
    U64 num_runs = 10000;
    U64 dummy_var = 0;
public:
    TimerRunner(U64 itterationsArg = 1000, U64 num_runs = 10000, bool printRunnedArg = true) : printRunned(printRunnedArg), itterations(itterationsArg) {}
    int runAutomatedTimerCases();
private:
    U64 time_knight_moves();
    U64 time_king_moves();
    U64 time_queen_moves();
    U64 time_rook_moves();
    U64 time_bischop_moves();
    U64 time_pawn_moves();

    U64 time_all_attackers();

    U64 time_rook_moves_on_the_fly();
    U64 time_bischop_moves_on_the_fly();
    U64 time_queen_moves_on_the_fly();

    U64 time_rook_moves_magic();
    U64 time_bischop_moves_magic();
    U64 time_queen_moves_magic();

    U64 timeFunction(std::function<U64()> func, U64 num_runs, const char* description);
};

void time_code();
void time_bitwise_code();
void time_minimax_code();
void time_mate_test_code();
void time_parralel_evaluation();