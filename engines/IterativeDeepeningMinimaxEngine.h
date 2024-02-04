#pragma once
#include "ChessEngine.h"

class IterativeDeepeningMinimaxEngine : public ChessEngine{
public:
    explicit IterativeDeepeningMinimaxEngine(int timeGiven) : time(timeGiven == 60 ? 1 : timeGiven){}

    void initialize() override {
        std::cout << "MiniMax Chess Engine with iterative deepening initialized.\n";
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
    int time = 0;
    TranspositionTable transpositionTable;
    void minimax_root(Board* bord, int depth, bool maximize, Action* moveOut, ActionList* moveList);
    double minimax(Board* bord, double alpha, double beta, int depth, bool maximizing_player, bool whitePlays);
};
