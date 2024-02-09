#pragma once
#include "ChessEngine.h"
#include <chrono>

class IterativeDeepeningMinimaxEngine : public ChessEngine{
public:
    explicit IterativeDeepeningMinimaxEngine(int timeGiven) : time(timeGiven == 60 ? 1 : timeGiven){}

    void initialize() override {
        std::cout << "MiniMax Chess Engine with iterative deepening initialized.\n";
    }

    Action getPreferredAction(Board* bord) override {
        auto startTime = std::chrono::high_resolution_clock::now();
        ActionList actionList;
        getLegalMoves(bord,&actionList);
        orderMoves(bord, &actionList);
        Action move;
        int depth = 0;
        int remainingTimeSeconds = time; // Set the initial remaining time

        //std::cout << "before" << std::endl;
        //printActionListNumberd(&actionList);
        while (true) {
            // keep doing minimax each time 1 deeper but with the best move moved to the front
            depth += 1;
            minimax_root(bord, depth, false, &move, &actionList,remainingTimeSeconds); //maximize means if it's white ?
            //std::cout << "currently at depth: " << depth << std::endl;
            auto endTime = std::chrono::high_resolution_clock::now();
            // Calculate the elapsed time in seconds
            auto durationSeconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

            actionList.moveToFront(move); // move the best move to the front of the array to make it better for the pruning

            //std::cout << "best move found: ";
            //printAction(&move);

            //std::cout << "after depth: " << depth << std::endl;
            //printActionListNumberd(&actionList);

            remainingTimeSeconds = time - durationSeconds;

            if (durationSeconds >= time) {
                std::cout << "Time limit exceeded by: " << durationSeconds-time << " with depth: " << depth << std::endl;
                break;
            }
        }
        return move;
        //movePiece(bord,&move);
        //printAction(&move);
        //std::cout << "MiniMax move made.\n";
    }

private:
    int time = 0;
    TranspositionTable transpositionTable = TranspositionTable(TRANSPOSITION_TABLE_SIZE_ITERATIVE_DEEPENING);
    void minimax_root(Board* bord, int depth, bool maximize, Action* moveOut, ActionList* moveList, int timeRemaining);
    double minimax(Board* bord, double alpha, double beta, int depth, bool maximizing_player, bool whitePlays, bool doNullPruning);
};
