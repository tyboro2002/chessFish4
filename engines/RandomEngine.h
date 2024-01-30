// SimpleRandomChessEngine.h
#pragma once
#include "ChessEngine.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

class RandomChessEngine : public ChessEngine {
public:
    void initialize() override {
        std::srand(std::time(0));
        std::cout << "Simple Random Chess Engine initialized.\n";
    }

    void makeMove(Board* bord) override {
        // Generate a random move (for simplicity, just print a message)
        ActionList actionList;
        getLegalMoves(bord,&actionList);
        int selectedIndex = getRandomNumber(actionList.count);
        movePiece(bord,&actionList.moves[selectedIndex]);
        //std::cout << "Random move made.\n";
    }

private:
    inline int getRandomNumber(int n) {
        // Seed the random number generator
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        // Generate a random number between 0 and n-1
        return std::rand() % n;
    }
};
