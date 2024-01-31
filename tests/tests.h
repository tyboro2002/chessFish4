// test.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iostream>
#include <bitset>
#include <sstream> // for std::ostringstream
#include <algorithm>  // for std::is_permutation
#include <thread> // to parallelize perft

#include "../game.h"
#include "../old_code/engine.h"
#include "../moves.h"
#include "testMoves.h"

using namespace std;

class TestRunner {
    int passedTests = 0;
    int totalTests = 0;
    std::vector<std::string> failedTests;
    bool printPassed = true;
    bool printFaults = false;
public:
    explicit TestRunner(bool printPassedArg = true, bool printFaultsArg = false) : printPassed(printPassedArg),printFaults(printFaultsArg) {}
    int runAutomatedTestCases();
    U64 generalPerft(Board* bord, int depth, bool printPercentage, int startAt, U64 initMoves);
    U64 detailedPerft(Board *bord, int depth);
private:
    void testResultTrue(bool condition, const std::string& testName); // Utility function to check and report test results
    void testResultFalse(bool condition, const std::string& testName); // Utility function to check and report test results

    [[nodiscard]] bool areActionListsEqual(const ActionList& list1, const ActionList& list2) const;

    /* test cases */
    void kingMovesGenerator();
    void knightMovesGenerator();
    void rookMovesGenerator();
    void bischopMovesGenerator();
    void queenMovesGenerator();
    void pawnMovesGenerator();

    void GenerateActions();
    void testDrawFunction();
    void testCheckMateFunction();
    void testIsEndedFunction();

    void testGeneralPerftResultst();

    U64 perftHelper(Board* bord, int depth);
};

void king_danger_squares_test();

void path_test();
void checking_test();
void move_test();
void move_test_check();
void randomMoveTest();
void fen_test();
void legalMoveTest();
void move_test_halfmove();

/*
* automatic tests
*/
bool runAutomatedTests();
bool runAutomatedTestsSilent();
bool inCheckTest();
bool mateInOneTest();
bool mateInTwoTest();
bool mateInThreeTest();