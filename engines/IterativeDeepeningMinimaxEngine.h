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
            //TODO check if i need to give through the color the engine is playing
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

inline void IterativeDeepeningMinimaxEngine::minimax_root(Board *bord, int depth, bool maximize, Action *moveOut, ActionList *moveList, int timeRemaining) {
    auto startTime = std::chrono::high_resolution_clock::now();

    //transpositionTable.printInfoShort();
    //transpositionTable.printTranspositionTable();

    // Lookup the position in the Transposition Table
    TranspositionTableEntry* ttEntry = transpositionTable.lookup(bord);
    if (ttEntry != nullptr && ttEntry->depth >= depth) {
        // Return the stored evaluation score if depth is sufficient
        moveOut->src = (ttEntry->bestMove).src;
        moveOut->dst = (ttEntry->bestMove).dst;
        moveOut->special = (ttEntry->bestMove).special;
        return;
    }

    double best_move = maximize ? -INFINITY : INFINITY;
    int size = moveList->count;
    Action best_move_found = moveList->moves[0];

    for (int i = 0; i < size; i++) {
        Action move = moveList->moves[i];
        Board boardCopy{};
        copyBoard(bord, &boardCopy);
        movePiece(&boardCopy,&move);
        //printAction(&move);
        //printFancyBoard(&boardCopy);

        if (isCheckmate(&boardCopy)) {
            moveOut->src = move.src;
            moveOut->dst = move.dst;
            moveOut->special = move.special;
            return;
        }

        double value = 0.0;
        if (!isDraw(&boardCopy)) value = minimax(&boardCopy, -INFINITY, INFINITY, depth - 1, !maximize, boardCopy.whiteToPlay, true);
        //std::cout << "resulted in: " << value << std::endl;
        if ((maximize && value > best_move) || (!maximize && value < best_move)) {
            best_move = value;
            best_move_found = move;
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        // Calculate the elapsed time in seconds
        auto durationSeconds = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

        if (durationSeconds >= timeRemaining) {
            moveOut->src = best_move_found.src;
            moveOut->dst = best_move_found.dst;
            moveOut->special = best_move_found.special;
            //std::cout << "ooh its time i quit" << std::endl;
            //printAction(&best_move_found);
            return;
        }
    }

    transpositionTable.store(bord, best_move, depth, best_move_found);
    moveOut->src = best_move_found.src;
    moveOut->dst = best_move_found.dst;
    moveOut->special = best_move_found.special;
}

inline double IterativeDeepeningMinimaxEngine::minimax(Board *bord, double alpha, double beta, int currentDepth, bool maximizing_player,bool whitePlays, bool doNullPruning) { //TODO are these minimax functions posible to be inlined ?
    if (currentDepth == 0) return evaluateBoard(bord);

    // Lookup the position in the Transposition Table
    TranspositionTableEntry* ttEntry = transpositionTable.lookup(bord);
    if (ttEntry != nullptr && ttEntry->depth >= currentDepth) {
        // Return the stored evaluation score if depth is sufficient
        return ttEntry->score;
    }

    // Null move pruning (if we don't do a move and the opponent isn't able to make the position not initiate a beta cutoff we can prune this branch)
    if (doNullPruning && currentDepth >= Rplus1 && !isCheck(bord) && bigPiece(bord)) {
        Board boardCopy{};
        copyBoard(bord, &boardCopy);  // Create a copy of the board
        makeNullMove(&boardCopy);     // Make null move on the copy
        double nullMoveScore = -minimax(&boardCopy, -beta, -beta + 1, currentDepth - R, !maximizing_player, !whitePlays, false);

        if (nullMoveScore >= beta)
            return nullMoveScore;  // Null move cutoff
    }

    if (maximizing_player) {
        float best_move = -INFINITY;
        ActionList moveList{};
        moveList.count = 0;
        getLegalMoves(bord,&moveList);
        orderMoves(bord, &moveList);

        for (int i = 0; i < moveList.count; i++) {
            Board boardCopy{};
            copyBoard(bord, &boardCopy);
            movePiece(&boardCopy,&moveList.moves[i]);

            //printFancyBoard(&boardCopy);
            if (isCheckmate(&boardCopy)) return INFINITY;

            double curr_move = -INFINITY;
            if (!isDraw(&boardCopy)) curr_move = minimax(&boardCopy, alpha, beta, currentDepth - 1, false,
                                                         whitePlays, true);

            // Each ply after a checkmate is slower, so they get ranked slightly less
            // We want the fastest mate!
            if (curr_move > MATE_THRESHOLD) curr_move -= 1;
            else if (curr_move < -MATE_THRESHOLD) curr_move += 1;

            best_move = (best_move > curr_move) ? best_move : curr_move;
            alpha = (alpha > best_move) ? alpha : best_move;
            if (beta <= alpha){
                transpositionTable.store(bord, best_move, currentDepth, moveList.moves[i]);
                return best_move;
            }
        }
        return best_move;
    }
    else {
        double best_move = INFINITY;
        ActionList moveList;
        moveList.count = 0;
        getLegalMoves(bord,&moveList);
        orderMoves(bord, &moveList);

        for (int i = 0; i < moveList.count; i++) {
            Board boardCopy{};
            copyBoard(bord, &boardCopy);
            movePiece(&boardCopy,&moveList.moves[i]);

            if (isCheckmate(&boardCopy)) return -INFINITY;

            double curr_move = INFINITY;
            if (!isDraw(&boardCopy)) curr_move = minimax(&boardCopy, alpha, beta, currentDepth - 1,
                                                         true, !whitePlays, true);

            // Each ply after a checkmate is slower, so they get ranked slightly less
            // We want the fastest mate!
            if (curr_move > MATE_THRESHOLD) curr_move -= 1;
            else if (curr_move < -MATE_THRESHOLD) curr_move += 1;

            best_move = (best_move < curr_move) ? best_move : curr_move;
            beta = (beta < best_move) ? beta : best_move;
            if (beta <= alpha){
                transpositionTable.store(bord, best_move, currentDepth, moveList.moves[i]);
                return best_move;
            }
        }
        return best_move;
    }
}