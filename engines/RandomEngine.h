#pragma once
#include "ChessEngine.h"
#include <iostream>
#include <cstdlib>

class RandomChessEngine : public ChessEngine {
public:
    void initialize() override {
        std::srand(std::time(0));
        std::cout << "Simple Random Chess Engine initialized.\n";
    }

    Action getPreferredAction(Board* bord) override {
        // Generate a random move (for simplicity, just print a message)
        ActionList actionList;
        getLegalMoves(bord,&actionList);
        int selectedIndex = getRandomNumber(actionList.count);
        return actionList.moves[selectedIndex];
        //movePiece(bord,&actionList.moves[selectedIndex]);
        //std::cout << "Random move made.\n";
    }

private:
};
