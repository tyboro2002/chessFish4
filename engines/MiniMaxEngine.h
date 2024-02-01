#pragma once
#include "ChessEngine.h"

const int MATE_SCORE = 1000000000;
const int MATE_THRESHOLD = 999000000;

class MiniMaxEngine : public ChessEngine {
public:
    MiniMaxEngine(int depthGiven) : depth(depthGiven == 0 ? 1: depthGiven){}

    void initialize() override {
        std::cout << "MiniMax Chess Engine initialized.\n";
    }

    void makeMove(Board* bord) override {
        // Generate a random move (for simplicity, just print a message)
        ActionList actionList;
        getLegalMoves(bord,&actionList);
        Action move;
        minimax_root(bord, depth, false, &move, &actionList); //maximize means if its white ?
        movePiece(bord,&move);
        //printAction(&move);
        //std::cout << "MiniMax move made.\n";
    }

private:
    int depth = 0;
    void minimax_root(Board* bord, int depth, bool maximize, Action* moveOut, ActionList* moveList);
    double minimax(Board* bord, double alpha, double beta, int depth, bool maximizing_player, bool whitePlays);
};

