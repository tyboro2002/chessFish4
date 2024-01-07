// test.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iostream>
#include <bitset>
#include <sstream> // For std::ostringstream
#include <algorithm>  // for std::is_permutation

#include "game.h"
#include "engine.h"
#include "moves.h"
#include "testMoves.h"

class TestRunner {
    int passedTests = 0;
    int totalTests = 0;
    std::vector<std::string> failedTests;
    bool printPassed = true;
public:
    explicit TestRunner(bool printPassedArg = true) : printPassed(printPassedArg) {}
    int runAutomatedTestCases();
private:
    void testResultTrue(bool condition, const std::string& testName); // Utility function to check and report test results
    void testResultFalse(bool condition, const std::string& testName); // Utility function to check and report test results

    bool areActionListsEqual(const ActionList& list1, const ActionList& list2);

    /* test cases */
    void kingMovesGenerator();
    void knightMovesGenerator();
    void rookMovesGenerator();
    void bischopMovesGenerator();
    void queenMovesGenerator();
    void pawnMovesGenerator();

    void GenerateActions();

    void testGeneralPerftResultst();

    U64 generalPerft(Board* bord, int depth);
    U64 detailedPerft(Board *bord, int depth);
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