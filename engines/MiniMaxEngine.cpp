#include "MiniMaxEngine.h"

void MiniMaxEngine::minimax_root(Board *bord, int currentDepth, bool maximize, Action *moveOut, ActionList *moveList) {
    //TODO try iterative deepening (maybe in other engine)
    //std::cout << "root" << std::endl;
    //printFancyBoard(bord);
    transpositionTable.printInfoShort();
    //transpositionTable.printTranspositionTable();

    // Lookup the position in the Transposition Table
    TranspositionTableEntry* ttEntry = transpositionTable.lookup(bord);
    if (ttEntry != nullptr && ttEntry->depth >= depth) {
        // Return the stored evaluation score if depth is sufficient
        moveOut->src = (ttEntry->bestMove).src;
        moveOut->dst = (ttEntry->bestMove).dst;
        moveOut->special = (ttEntry->bestMove).special;
        printAction(&ttEntry->bestMove);
        std::cout << "used transposition table" << std::endl;
        return;
    }

    double best_move = maximize ? -INFINITY : INFINITY;
    orderMoves(bord, moveList);
    int size = moveList->count;
    Action best_move_found = moveList->moves[0];

    for (int i = 0; i < size; i++) {
        Action move = moveList->moves[i];
        Board boardCopy{};
        copyBoard(bord, &boardCopy);
        movePiece(&boardCopy,&move);
        //printAction(&move);
        //printFancyBoard(&boardCopy);

        if (isChekmate(&boardCopy)) {
            moveOut->src = move.src;
            moveOut->dst = move.dst;
            moveOut->special = move.special;
            return;
        }

        double value = 0.0;
        if (!isDraw(bord)) value = minimax(&boardCopy, -INFINITY, INFINITY, currentDepth - 1, !maximize, bord->whiteToPlay);
        //std::cout << "resulted in: " << value << std::endl;
        if ((maximize && value > best_move) || (!maximize && value < best_move)) {
            best_move = value;
            best_move_found = move;
        }
    }

    transpositionTable.store(bord, best_move, depth, best_move_found);
    moveOut->src = best_move_found.src;
    moveOut->dst = best_move_found.dst;
    moveOut->special = best_move_found.special;
}

double MiniMaxEngine::minimax(Board* bord, double alpha, double beta, int currentDepth, bool maximizing_player, bool whitePlays) {
    if (currentDepth == 0) return evaluateBoard(bord);

    // Lookup the position in the Transposition Table
    TranspositionTableEntry* ttEntry = transpositionTable.lookup(bord);
    if (ttEntry != nullptr && ttEntry->depth >= depth) {
        // Return the stored evaluation score if depth is sufficient
        return ttEntry->score;
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
            if (isChekmate(&boardCopy)) return INFINITY;

            double curr_move = -INFINITY;
            if (!isDraw(&boardCopy)) curr_move = minimax(&boardCopy, alpha, beta, currentDepth - 1, false,
                                                   whitePlays);

            // Each ply after a checkmate is slower, so they get ranked slightly less
            // We want the fastest mate!
            //if (curr_move > MATE_THRESHOLD) curr_move -= 1;
            //else if (curr_move < -MATE_THRESHOLD) curr_move += 1;

            best_move = (best_move > curr_move) ? best_move : curr_move;
            alpha = (alpha > best_move) ? alpha : best_move;
            if (beta <= alpha){
                transpositionTable.store(bord, best_move, depth, moveList.moves[i]);
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

            if (isChekmate(&boardCopy)) return -INFINITY;

            double curr_move = INFINITY;
            if (!isDraw(&boardCopy)) curr_move = minimax(&boardCopy, alpha, beta, currentDepth - 1,
                                true, !whitePlays);

            // Each ply after a checkmate is slower, so they get ranked slightly less
            // We want the fastest mate!
            //if (curr_move > MATE_THRESHOLD) curr_move -= 1;
            //else if (curr_move < -MATE_THRESHOLD) curr_move += 1;

            best_move = (best_move < curr_move) ? best_move : curr_move;
            beta = (beta < best_move) ? beta : best_move;
            if (beta <= alpha){
                transpositionTable.store(bord, best_move, depth, moveList.moves[i]);
                return best_move;
            }
        }
        return best_move;
    }
}
