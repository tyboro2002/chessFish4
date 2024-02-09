#pragma once
#include "ChessEngine.h"

class MiniMaxEngine : public ChessEngine {
public:
    MiniMaxEngine(int depthGiven) : depth(depthGiven == 0 ? 1: depthGiven){}

    void initialize() override {
        std::cout << "MiniMax Chess Engine initialized.\n";
    }

    Action getPreferredAction(Board* bord) override {
        ActionList actionList;
        getLegalMoves(bord,&actionList);
        Action move;
        minimax_root(bord, false, &move, &actionList); //maximize means if its white ?
        return move;
        //movePiece(bord,&move);
        //printAction(&move);
        //std::cout << "MiniMax move made.\n";
    }

private:
    int depth = 0;
    TranspositionTable transpositionTable = TranspositionTable(TRANSPOSITION_TABLE_SIZE_MINIMAX);
    void minimax_root(Board* bord, bool maximize, Action* moveOut, ActionList* moveList);
    double minimax(Board* bord, double alpha, double beta, int depth, bool maximizing_player, bool whitePlays);
};

