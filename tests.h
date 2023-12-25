// test.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <iostream>

class TestRunner {
    int passedTests = 0;
    int totalTests = 0;
    std::vector<std::string> failedTests;
public:
    int runAutomatedTestCases();
private:
    void testResultTrue(bool condition, const std::string& testName); // Utility function to check and report test results
    void testResultFalse(bool condition, const std::string& testName); // Utility function to check and report test results

    /* test cases */
    void kingMovesGenerator();
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

void knightMovesGenerator();

void randomTest();