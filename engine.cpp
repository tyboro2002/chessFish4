
#include "engine.h"
#include <random>
#include <map>
#include <chrono>
#include <array>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

#define white_plays(bord) (bord->whiteToPlay)
#define en_passent_target(bord) (((1ULL<<63) >> (bord->enPassantTarget)) & (bord->enPassentValid ? UINT64_MAX : 0))

constexpr const char engine0[] = "the human player";
constexpr const char engine1[] = "the random engine";
constexpr const char engine2[] = "the minimax engine";
constexpr const char engine3[] = "the minimax optimized engine";
constexpr const char engine4[] = "the minimax optimized engine with itterative deepening";
constexpr std::array<const char*, 5> engines = { engine0, engine1, engine2, engine3 , engine4 };


const map<Pieces, int> piece_score_dic = {
        //white
        {WPAWN, 100},
        {WROOK, 500},
        {WKNIGHT, 300},
        {WBISCHOP, 300},
        {WQUEEN, 900},
        {WKING, 0},
        //black
        {BPAWN, 100},
        {BROOK, 500},
        {BKNIGHT, 300},
        {BBISCHOP, 300},
        {BQUEEN, 900},
        {BKING, 0},
        //empty
        {NOPIECE,0}
};

const int MATE_SCORE = 1000000000;
const int MATE_THRESHOLD = 999000000;

const int pawnEvalWhite[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, -20, -20, 10, 10, 5,
        5, -5, -10, 0, 0, -10, -5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, 5, 10, 25, 25, 10, 5, 5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 50, 50, 50, 50, 50, 50,
        0, 0, 0, 0, 0, 0, 0, 0
};

const int pawnEvalBlack[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5, 5, 10, 25, 25, 10, 5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, -5, -10, 0, 0, -10, -5, 5,
        5, 10, 10, -20, -20, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0 };

const int knightEval[64] = {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0, 0, 0, 0, -20, -40,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30,
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50
};

const int bishopEvalWhite[64] = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -20, -10, -10, -10, -10, -10, -10, -20
};

const int bishopEvalBlack[64] = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20
};

const int rookEvalWhite[64] = {
        0, 0, 0, 5, 5, 0, 0, 0,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        5, 10, 10, 10, 10, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
};

const int rookEvalBlack[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, 10, 10, 10, 10, 5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        0, 0, 0, 5, 5, 0, 0, 0
};

const int queenEval[64] = {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5, 0, 5, 5, 5, 5, 0, -5,
        0, 0, 5, 5, 5, 5, 0, -5,
        -10, 5, 5, 5, 5, 5, 0, -10,
        -10, 0, 5, 0, 0, 0, 0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20
};

const int kingEvalWhite[64] = {
        20, 30, 10, 0, 0, 10, 30, 20,
        20, 20, 0, 0, 0, 0, 20, 20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, -30, -30, -40, -40, -30, -30, -20,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30
};
const int kingEvalBlack[64] = {
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, 20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, 20, 0, 0, 0, 0, 20, 20,
        20, 30, 10, 0, 0, 10, 30, 20
};

const int kingEvalEndGameWhite[64] = {
        50, -30, -30, -30, -30, -30, -30, -50,
        -30, -30, 0, 0, 0, 0, -30, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -20, -10, 0, 0, -10, -20, -30,
        -50, -40, -30, -20, -20, -30, -40, -50
};

const int kingEvalEndGameBlack[64] = {
        -50, -40, -30, -20, -20, -30, -40, -50,
        -30, -20, -10, 0, 0, -10, -20, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -30, 0, 0, 0, 0, -30, -30, -50,
        -30, -30, -30, -30, -30, -30, 50
};

/*
* helper functions
*/
int generateRandomNumber(int upperBound) {
    upperBound = upperBound - 1; //remove upper bound from posible outcomes
    std::random_device rd; // Seed the random number generator
    std::mt19937 gen(rd()); // Mersenne Twister engine
    std::uniform_int_distribution<> distribution(0, upperBound);
    return distribution(gen);
}

bool isMoveInVector(const std::vector<Move>& vector, const Move& move) {
    return std::find(vector.begin(), vector.end(), move) != vector.end();
}

/*
* functions for the actual engine
*/

bool check_end_game(Board* bord) {
    /*
    *  Are we in the end game ?
    *  Per Michniewski :
    *  -Both sides have no queens or
    *  -Every side which has a queen has additionally no other pieces or one minorpiece maximum.
    */
    int queens = 0;
    int minors = 0;
    U64 queensBitmap = bord->queen;
    U64 minorsBitmap = (bord->bishop | bord->knight);
    queens = countSetBits(queensBitmap);
    minors = countSetBits(minorsBitmap);
    return queens == 0 || (queens == 2 && minors <= 1);
}

int evaluateWhite(Board* bord) {
    int white_material = 0;
    U64 wrook = bord->white & bord->rook;
    U64 wknight = bord->white & bord->knight;
    U64 wbishop = bord->white & bord->bishop;
    U64 wqueen = bord->white & bord->queen;
    U64 wpawn = bord->white & bord->pawn;
    while (wrook) {
        int bitIndex = getFirst1BitSquare(wrook); // Get the index of the least significant set bit
        white_material += 500;// piece_score_dic.find(WROOK)->second; //TODO hardcode
        wrook &= (wrook - 1); // Clear the least significant set bit
    }
    while (wknight) {
        int bitIndex = getFirst1BitSquare(wknight); // Get the index of the least significant set bit
        white_material += 300;// piece_score_dic.find(WKNIGHT)->second; //TODO hardcode
        wknight &= (wknight - 1); // Clear the least significant set bit
    }
    while (wbishop) {
        int bitIndex = getFirst1BitSquare(wbishop); // Get the index of the least significant set bit
        white_material += 300;// piece_score_dic.find(WBISCHOP)->second; //TODO hardcode
        wbishop &= (wbishop - 1); // Clear the least significant set bit
    }
    while (wqueen) {
        int bitIndex = getFirst1BitSquare(wqueen); // Get the index of the least significant set bit
        white_material += 900;// piece_score_dic.find(WQUEEN)->second; //TODO hardcode
        wqueen &= (wqueen - 1); // Clear the least significant set bit
    }
    while (wpawn) {
        int bitIndex = getFirst1BitSquare(wpawn); // Get the index of the least significant set bit
        white_material += 100; // piece_score_dic.find(WPAWN)->second; //TODO hardcode
        wpawn &= (wpawn - 1); // Clear the least significant set bit
    }
    return white_material;
}

int evaluateBlack(Board* bord) {
    int black_material = 0;
    U64 brook = bord->black & bord->rook;
    U64 bknight = bord->black & bord->knight;
    U64 bbishop = bord->black & bord->bishop;
    U64 bqueen = bord->black & bord->queen;
    U64 bpawn = bord->black & bord->pawn;
    while (brook) {
        int bitIndex = getFirst1BitSquare(brook); // Get the index of the least significant set bit
        black_material += 500;// piece_score_dic.find(BROOK)->second; //TODO hardcode
        brook &= (brook - 1); // Clear the least significant set bit
    }
    while (bknight) {
        int bitIndex = getFirst1BitSquare(bknight); // Get the index of the least significant set bit
        black_material += 300;// piece_score_dic.find(BKNIGHT)->second; //TODO hardcode
        bknight &= (bknight - 1); // Clear the least significant set bit
    }
    while (bbishop) {
        int bitIndex = getFirst1BitSquare(bbishop); // Get the index of the least significant set bit
        black_material += 300;// piece_score_dic.find(BBISCHOP)->second; //TODO hardcode
        bbishop &= (bbishop - 1); // Clear the least significant set bit
    }
    while (bqueen) {
        int bitIndex = getFirst1BitSquare(bqueen); // Get the index of the least significant set bit
        black_material += 900;// piece_score_dic.find(BQUEEN)->second; //TODO hardcode
        bqueen &= (bqueen - 1); // Clear the least significant set bit
    }
    while (bpawn) {
        int bitIndex = getFirst1BitSquare(bpawn); // Get the index of the least significant set bit
        black_material += 100; // piece_score_dic.find(BPAWN)->second; //TODO hardcode
        bpawn &= (bpawn - 1); // Clear the least significant set bit
    }
    return black_material;
}

int evaluate(Board* bord) {
    if (white_plays(bord)) {
        return evaluateWhite(bord);
    }
    else {
        return evaluateBlack(bord);
    }
}

std::tuple<int, int> evaluateBothSides(Board* bord) {
    return std::make_tuple(evaluateWhite(bord), evaluateBlack(bord));
}

int evaluate_capture(Board* bord, Move* mv) {
    /*
    *   Given a capturing move, weight the trade being made.
    */

    Pieces _to = pieceAt(mv->dst,bord);
    Pieces _from = pieceAt(mv->src, bord);
    int val = 0;
    if (getFirst1BitSquare(en_passent_target(bord)) == mv->dst) {
        return 100;// piece_score_dic.find(WPAWN)->second; //TODO hardcode value
    }
    return piece_score_dic.find(_to)->second - piece_score_dic.find(_from)->second;
}

int evaluate_piece(Pieces piece, int square, bool endgame) {
    switch (piece)
    {
        case WPAWN:
            return pawnEvalWhite[square];
            break;
        case BPAWN:
            return pawnEvalBlack[square];
            break;
        case WKNIGHT:
            return knightEval[square];
            break;
        case BKNIGHT:
            return knightEval[square];
            break;
        case WBISCHOP:
            return bishopEvalWhite[square];
            break;
        case BBISCHOP:
            return bishopEvalBlack[square];
            break;
        case WROOK:
            return rookEvalWhite[square];
            break;
        case BROOK:
            return rookEvalBlack[square];
            break;
        case WQUEEN:
            return queenEval[square];
            break;
        case BQUEEN:
            return queenEval[square];
            break;
        case WKING:
            if (endgame) {
                return kingEvalEndGameWhite[square];
            }
            else {
                return kingEvalWhite[square];
            }
            break;
        case BKING:
            if (endgame) {
                return kingEvalEndGameBlack[square];
            }
            else {
                return kingEvalBlack[square];
            }
            break;
        default:
            return 0;
            break;
    }
}

double move_value(Board* bord, Move* mv, bool endgame) {
    /*
    * How good is a move?
    * A promotion is great.
    * A weaker piece taking a stronger piece is good.
    * A stronger piece taking a weaker piece is bad.
    * Also consider the position change via piece-square table.
    */

    if (mv->special == SPECIAL_PROMOTION_QUEEN || mv->special == SPECIAL_PROMOTION_ROOK || mv->special == SPECIAL_PROMOTION_BISHOP || mv->special == SPECIAL_PROMOTION_KNIGHT) {
        if (white_plays(bord)) {
            return INFINITY;
        }else {
            return -INFINITY;
        }
    }

    Pieces _piece = pieceAt(mv->src, bord);

    int _from_value = evaluate_piece(_piece, mv -> src, endgame);
    int _to_value = evaluate_piece(_piece, mv -> dst, endgame);
    int position_change = _to_value - _from_value;
    //int position_change = evaluate_piece(_piece, mv->dst, endgame) - evaluate_piece(_piece, mv->src, endgame);
    int capture_value = 0.0;
    if (mv->capture != (-52)) {
        capture_value = evaluate_capture(bord, mv);
    }
    int current_move_value = capture_value + position_change;
    if (!white_plays(bord)) {
        current_move_value = -current_move_value;
    }
    //cout << current_move_value << endl;
    //TODO check if a check if oponent has move is faster and better
    return current_move_value;
}

/*
Move* reverseArray(Move* arr, const std::size_t size) { //TODO check
    Move* reversed = new Move[size];
    for (std::size_t i = 0; i < size; i++) {
        reversed[i] = arr[size - 1 - i];
    }
    return reversed;
}
*/
void reverseArray(Move* arr, const std::size_t size) {
    for (std::size_t i = 0; i < size / 2; i++) {
        std::swap(arr[i], arr[size - 1 - i]);
    }
}

void orderMoves(Board* bord, MOVELIST* moveList) {
    bool end_game = check_end_game(bord);

    int size = moveList->count;
    Move* moves = moveList->moves;
    // Define a lambda function to compare moves by their values
    auto compareMoves = [&](Move a, Move b) {
        int value1 = move_value(bord, &a, end_game);
        int value2 = move_value(bord, &b, end_game);
        return value1 > value2;
    };

    // Sort the moves array using the comparator function
    std::stable_sort(moves, moves + size, compareMoves);

    if (!white_plays(bord)) {
        reverseArray(moves, size);
    }
    // Create a new MOVELIST and copy the sorted moves into it
    //MOVELIST orderedMoves;
    //orderedMoves.count = size;
    std::copy(moves, moves + size, moveList->moves);

}

void orderMovesWithTriangularTable(Board* bord, MOVELIST* moveList, std::vector<Move>* triangular_table) {
    bool end_game = check_end_game(bord);

    int size = moveList->count;
    Move* moves = moveList->moves;

    // Remove moves from moveList that are already in the triangular_table vector
    auto newEnd = std::remove_if(moves, moves + size, [&](const Move& move) {
        return std::find(triangular_table->begin(), triangular_table->end(), move) != triangular_table->end();
    });

    // Update the count of moves after removing duplicates
    moveList->count = newEnd - moves;

    // Define a lambda function to compare moves by their values
    auto compareMoves = [&](Move a, Move b) {
        int value1 = move_value(bord, &a, end_game);
        int value2 = move_value(bord, &b, end_game);
        return value1 > value2;
    };

    // Sort the remaining moves array using the comparator function
    std::stable_sort(moves, moves + moveList->count, compareMoves);

    if (!white_plays(bord)) {
        reverseArray(moves, moveList->count);
    }

    // Move moves from the triangular_table vector to the front of moveList
    size_t numTriangularMoves = triangular_table->size();
    std::copy_backward(moves, moves + moveList->count, moves + moveList->count + numTriangularMoves);
    std::copy(triangular_table->begin(), triangular_table->end(), moves);
    moveList->count += numTriangularMoves;
}

void get_orderd_moves(Board* bord, MOVELIST* moveList, PositionTracker* positionTracker) {
    GenLegalMoveList(moveList, bord, positionTracker); //TODO
    orderMoves(bord, moveList);
}

double minimax(Board* bord, double alpha, double beta, int depth, bool maximizing_player, bool whiteVraagteken, int* counter, TranspositionTable* transpositionTable, PositionTracker* positionTracker) {
    //std::string key = generateHashKey(cr);

    // Lookup the position in the Transposition Table
    TranspositionTableEntry* ttEntry = transpositionTable->lookup(bord);
    if (ttEntry != nullptr && ttEntry->depth >= depth) {
        // Return the stored evaluation score if depth is sufficient
        return ttEntry->score;
    }

    TERMINAL eval_final_position;
    //bool legal = cr->Evaluate(eval_final_position); //TODO
    //bool matew = (eval_final_position == TERMINAL_WCHECKMATE);
    //bool mateb = (eval_final_position == TERMINAL_BCHECKMATE);
    //if (matew && !whiteVraagteken || mateb && whiteVraagteken) {
    //    return maximizing_player ? -MATE_SCORE : MATE_SCORE;
    //}

    if (depth == 0) {
        //printBoard(bord);
        //int ev = evaluate(bord);
        ////std::tuple<int, int> material_scores = evaluateBothSides(bord);
        ////cout << std::get<0>(material_scores) << "," << std::get<1>(material_scores) << endl;
        //cout << "score: " << ev << endl;
        return evaluate(bord);
    }

    if (maximizing_player) {
        double best_move = -INFINITY;
        MOVELIST moveList;
        moveList.count = 0; //TODO check if needed
        get_orderd_moves(bord, &moveList, positionTracker);

        int size = moveList.count;
        Move* moves = moveList.moves;
        for (int i = 0; i < size; i++) {
            *counter += 1;
            Move move = moves[i];
            Board boardCopy;
            copyBoard(bord, &boardCopy);
            makeMove(&boardCopy, &move, positionTracker);
            if (!weHaveMoves(&boardCopy)) {
                return INFINITY;
            }
            double curr_move = -INFINITY;
            if (isDraw(bord, positionTracker) == NOT_DRAW) {
                curr_move = minimax(&boardCopy, alpha, beta, depth - 1, !maximizing_player, !whiteVraagteken, counter, transpositionTable, positionTracker);
            }
            positionTracker->removePosition(&boardCopy);
            // Each ply after a checkmate is slower, so they get ranked slightly less
            // We want the fastest mate!
            if (curr_move > MATE_THRESHOLD) {
                curr_move -= 1;
            }
            else if (curr_move < -MATE_THRESHOLD) {
                curr_move += 1;
            }
            best_move = (best_move > curr_move) ? best_move : curr_move;
            alpha = (alpha > best_move) ? alpha : best_move;
            if (beta <= alpha) {
                transpositionTable->store(bord, best_move, depth, move);
                return best_move;
            }
        }
        return best_move;
    }
    else {
        double best_move = INFINITY;
        MOVELIST moveList;
        moveList.count = 0; //TODO check if needed
        get_orderd_moves(bord, &moveList, positionTracker);
        int size = moveList.count;
        Move* moves = moveList.moves;
        for (int i = 0; i < size; i++) {
            *counter += 1;
            Move move = moves[i];
            Board boardCopy;
            copyBoard(bord, &boardCopy);
            makeMove(&boardCopy, &move, positionTracker);
            if (!weHaveMoves(&boardCopy)) {
                return -INFINITY;
            }
            double curr_move = INFINITY;
            if (isDraw(bord, positionTracker) == NOT_DRAW) {
                curr_move = minimax(&boardCopy, alpha, beta, depth - 1, !maximizing_player, !whiteVraagteken, counter, transpositionTable, positionTracker);
            }
            positionTracker->removePosition(&boardCopy);
            // Each ply after a checkmate is slower, so they get ranked slightly less
            // We want the fastest mate!
            if (curr_move > MATE_THRESHOLD) {
                curr_move -= 1;
            }
            else if (curr_move < -MATE_THRESHOLD) {
                curr_move += 1;
            }
            best_move = (best_move < curr_move) ? best_move : curr_move;
            beta = (beta < best_move) ? beta : best_move;
            if (beta <= alpha) {
                transpositionTable->store(bord, best_move, depth, move);
                return best_move;
            }
        }
        return best_move;
    }
}

void minimax_root(Board* bord, int depth, bool maximize, Move* moveOut, MOVELIST* moveList, TranspositionTable* transpositionTable, PositionTracker* positionTracker) {
    //What is the highest value move per our evaluation function?
    //std::string key = generateHashKey(cr);

    // Lookup the position in the Transposition Table
    TranspositionTableEntry* ttEntry = transpositionTable->lookup(bord);
    if (ttEntry != nullptr && ttEntry->depth >= depth) {
        Board boardCopy;
        copyBoard(bord, &boardCopy);
        makeMove(&boardCopy, &(ttEntry->bestMove), positionTracker);
        if (isDraw(&boardCopy, positionTracker) || (findMoveIndex(moveList, &(ttEntry->bestMove))) == -1) {
            positionTracker->removePosition(&boardCopy);
            goto breakBothIfs; // Jump to the labeled statement
        }
        positionTracker->removePosition(&boardCopy);
        // Return the stored evaluation score if depth is sufficient
        moveOut->src = (ttEntry->bestMove).src;
        moveOut->dst = (ttEntry->bestMove).dst;
        moveOut->special = (ttEntry->bestMove).special;
        moveOut->capture = (ttEntry->bestMove).capture;
        cout << "used transposition table" << endl;
        return;
    }
    breakBothIfs:

    double best_move = maximize ? -INFINITY : INFINITY;
    orderMoves(bord, moveList);
    int size = moveList->count;
    Move* moves = moveList->moves;
    Move best_move_found = moves[0];

    bool end_game = check_end_game(bord);
    for (int i = 0; i < size; i++) {
        Move move = moves[i];
        Board boardCopy;
        copyBoard(bord, &boardCopy);
        makeMove(&boardCopy, &move, positionTracker);
        if (!weHaveMoves(&boardCopy)) {
            moveOut->src = move.src;
            moveOut->dst = move.dst;
            moveOut->capture = move.capture;
            moveOut->special = move.special;
            return;
        }
        double value = 0.0;
        int branchCount = 0;
        int* branchCounter = &branchCount;
        if (isDraw(bord, positionTracker) == NOT_DRAW) {
            value = minimax(&boardCopy, -INFINITY, INFINITY, depth - 1, !maximize, !white_plays((&boardCopy)), branchCounter, transpositionTable, positionTracker);
        }
        positionTracker->removePosition(&boardCopy);
        //cout << "move: " << move.NaturalOut(cr) << " " << branchCount << endl;
        if (maximize && value > best_move) {
            best_move = value;
            best_move_found = move;
        }
        else if (!maximize && value < best_move) {
            best_move = value;
            best_move_found = move;
        }
        //cout << "move: " << moveToString(&move) << " has value: " << best_move << endl;
    }
    transpositionTable->store(bord, best_move, depth, best_move_found);
    moveOut->src = best_move_found.src;
    moveOut->dst = best_move_found.dst;
    moveOut->capture = best_move_found.capture;
    moveOut->special = best_move_found.special;
}

double minimaxOptimized(Board* bord, double alpha, double beta, int depth, bool maximizing_player, bool whiteVraagteken, int* counter, TranspositionTable* transpositionTable, PositionTracker* positionTracker) {
    //std::string key = generateHashKey(cr);

    // Lookup the position in the Transposition Table
    TranspositionTableEntry* ttEntry = transpositionTable->lookup(bord);
    if (ttEntry != nullptr && ttEntry->depth >= depth) {
        // Return the stored evaluation score if depth is sufficient
        return ttEntry->score;
    }

    TERMINAL eval_final_position;
    //bool legal = cr->Evaluate(eval_final_position); //TODO
    //bool matew = (eval_final_position == TERMINAL_WCHECKMATE);
    //bool mateb = (eval_final_position == TERMINAL_BCHECKMATE);
    //if (matew && !whiteVraagteken || mateb && whiteVraagteken) {
    //    return maximizing_player ? -MATE_SCORE : MATE_SCORE;
    //}

    if (depth == 0) {
        //printBoard(bord);
        //int ev = evaluate(bord);
        ////std::tuple<int, int> material_scores = evaluateBothSides(bord);
        ////cout << std::get<0>(material_scores) << "," << std::get<1>(material_scores) << endl;
        //cout << "score: " << ev << endl;
        return evaluate(bord);
    }

    if (maximizing_player) {
        double best_move = -INFINITY;
        MOVELIST moveList;
        moveList.count = 0; //TODO check if needed
        get_orderd_moves(bord, &moveList, positionTracker);

        int size = moveList.count;
        Move* moves = moveList.moves;
        for (int i = 0; i < size; i++) {
            *counter += 1;
            Move move = moves[i];
            Board boardCopy;
            copyBoard(bord, &boardCopy);
            makeMove(&boardCopy, &move, positionTracker);
            if (!weHaveMoves(&boardCopy)) {
                return INFINITY;
            }
            double curr_move = -INFINITY;
            if (isDraw(bord, positionTracker) == NOT_DRAW) {
                curr_move = minimaxOptimized(&boardCopy, alpha, beta, depth - 1 + (inCheck(bord) ? 1 : 0), !maximizing_player, !whiteVraagteken, counter, transpositionTable, positionTracker);
            }
            positionTracker->removePosition(&boardCopy);
            // Each ply after a checkmate is slower, so they get ranked slightly less
            // We want the fastest mate!
            if (curr_move > MATE_THRESHOLD) {
                curr_move -= 1;
            }
            else if (curr_move < -MATE_THRESHOLD) {
                curr_move += 1;
            }
            best_move = (best_move > curr_move) ? best_move : curr_move;
            alpha = (alpha > best_move) ? alpha : best_move;
            if (beta <= alpha) {
                transpositionTable->store(bord, best_move, depth, move);
                return best_move;
            }
        }
        return best_move;
    }
    else {
        double best_move = INFINITY;
        MOVELIST moveList;
        moveList.count = 0; //TODO check if needed
        get_orderd_moves(bord, &moveList, positionTracker);
        int size = moveList.count;
        Move* moves = moveList.moves;
        for (int i = 0; i < size; i++) {
            *counter += 1;
            Move move = moves[i];
            Board boardCopy;
            copyBoard(bord, &boardCopy);
            makeMove(&boardCopy, &move, positionTracker);
            if (!weHaveMoves(&boardCopy)) {
                return -INFINITY;
            }
            double curr_move = INFINITY;
            if (isDraw(bord, positionTracker) == NOT_DRAW) {
                curr_move = minimaxOptimized(&boardCopy, alpha, beta, depth - 1 + (inCheck(bord) ? 1 : 0), !maximizing_player, !whiteVraagteken, counter, transpositionTable, positionTracker);
            }
            positionTracker->removePosition(&boardCopy);
            // Each ply after a checkmate is slower, so they get ranked slightly less
            // We want the fastest mate!
            if (curr_move > MATE_THRESHOLD) {
                curr_move -= 1;
            }
            else if (curr_move < -MATE_THRESHOLD) {
                curr_move += 1;
            }
            best_move = (best_move < curr_move) ? best_move : curr_move;
            beta = (beta < best_move) ? beta : best_move;
            if (beta <= alpha) {
                transpositionTable->store(bord, best_move, depth, move);
                return best_move;
            }
        }
        return best_move;
    }
}

void minimax_rootOptimized(Board* bord, int depth, bool maximize, Move* moveOut, MOVELIST* moveList, TranspositionTable* transpositionTable, PositionTracker* positionTracker) {
    //What is the highest value move per our evaluation function?
    //std::string key = generateHashKey(cr);

    // Lookup the position in the Transposition Table
    TranspositionTableEntry* ttEntry = transpositionTable->lookup(bord);
    if (ttEntry != nullptr && ttEntry->depth >= depth) {
        // Return the stored evaluation score if depth is sufficient
        Board boardCopy;
        copyBoard(bord, &boardCopy);
        makeMove(&boardCopy, &(ttEntry->bestMove), positionTracker);
        if (isDraw(&boardCopy, positionTracker) || (findMoveIndex(moveList, &(ttEntry->bestMove))) == -1) {
            positionTracker->removePosition(&boardCopy);
            goto breakBothIfs; // Jump to the labeled statement
        }
        positionTracker->removePosition(&boardCopy);
        moveOut->src = (ttEntry->bestMove).src;
        moveOut->dst = (ttEntry->bestMove).dst;
        moveOut->special = (ttEntry->bestMove).special;
        moveOut->capture = (ttEntry->bestMove).capture;
        cout << "used transposition table" << endl;
        return;
    }
    breakBothIfs:

    double best_move = maximize ? -INFINITY : INFINITY;
    orderMoves(bord, moveList);
    int size = moveList->count;
    Move* moves = moveList->moves;
    Move best_move_found = moves[0];

    bool end_game = check_end_game(bord);
    for (int i = 0; i < size; i++) {
        Move move = moves[i];
        Board boardCopy;
        copyBoard(bord, &boardCopy);
        makeMove(&boardCopy, &move, positionTracker);
        if (!weHaveMoves(&boardCopy)) {
            moveOut->src = move.src;
            moveOut->dst = move.dst;
            moveOut->capture = move.capture;
            moveOut->special = move.special;
            return;
        }
        double value = 0.0;
        int branchCount = 0;
        int* branchCounter = &branchCount;
        if (isDraw(bord, positionTracker) == NOT_DRAW) {
            value = minimaxOptimized(&boardCopy, -INFINITY, INFINITY, depth - 1, !maximize, !white_plays((&boardCopy)), branchCounter, transpositionTable, positionTracker);
        }
        positionTracker->removePosition(&boardCopy);
        //cout << "move: " << move.NaturalOut(cr) << " " << branchCount << endl;
        if (maximize && value > best_move) {
            best_move = value;
            best_move_found = move;
        }
        else if (!maximize && value < best_move) {
            best_move = value;
            best_move_found = move;
        }
        //cout << "move: " << moveToString(&move) << " has value: " << best_move << endl;
    }
    transpositionTable->store(bord, best_move, depth, best_move_found);
    moveOut->src = best_move_found.src;
    moveOut->dst = best_move_found.dst;
    moveOut->capture = best_move_found.capture;
    moveOut->special = best_move_found.special;
}

double minimaxOptimizedItterativeDeepening(Board* bord, double alpha, double beta, int depth, bool maximizing_player, bool whiteVraagteken, int* counter, TranspositionTable* transpositionTable, PositionTracker* positionTracker) {
    //std::string key = generateHashKey(cr);

    // Lookup the position in the Transposition Table
    TranspositionTableEntry* ttEntry = transpositionTable->lookup(bord);
    if (ttEntry != nullptr && ttEntry->depth >= depth) {
        // Return the stored evaluation score if depth is sufficient
        return ttEntry->score;
    }

    TERMINAL eval_final_position;
    //bool legal = cr->Evaluate(eval_final_position); //TODO
    //bool matew = (eval_final_position == TERMINAL_WCHECKMATE);
    //bool mateb = (eval_final_position == TERMINAL_BCHECKMATE);
    //if (matew && !whiteVraagteken || mateb && whiteVraagteken) {
    //    return maximizing_player ? -MATE_SCORE : MATE_SCORE;
    //}

    if (depth == 0) {
        //printBoard(bord);
        //int ev = evaluate(bord);
        ////std::tuple<int, int> material_scores = evaluateBothSides(bord);
        ////cout << std::get<0>(material_scores) << "," << std::get<1>(material_scores) << endl;
        //cout << "score: " << ev << endl;
        return evaluate(bord);
    }

    if (maximizing_player) {
        double best_move = -INFINITY;
        MOVELIST moveList;
        moveList.count = 0; //TODO check if needed
        get_orderd_moves(bord, &moveList, positionTracker);

        int size = moveList.count;
        Move* moves = moveList.moves;
        for (int i = 0; i < size; i++) {
            *counter += 1;
            Move move = moves[i];
            Board boardCopy;
            copyBoard(bord, &boardCopy);
            makeMove(&boardCopy, &move, positionTracker);
            if (!weHaveMoves(&boardCopy)) {
                return INFINITY;
            }
            double curr_move = -INFINITY;
            if (isDraw(bord, positionTracker) == NOT_DRAW) {
                curr_move = minimaxOptimizedItterativeDeepening(&boardCopy, alpha, beta, depth - 1 + (inCheck(bord) ? 1 : 0), !maximizing_player, !whiteVraagteken, counter, transpositionTable, positionTracker);
            }
            positionTracker->removePosition(&boardCopy);
            // Each ply after a checkmate is slower, so they get ranked slightly less
            // We want the fastest mate!
            if (curr_move > MATE_THRESHOLD) {
                curr_move -= 1;
            }
            else if (curr_move < -MATE_THRESHOLD) {
                curr_move += 1;
            }
            best_move = (best_move > curr_move) ? best_move : curr_move;
            alpha = (alpha > best_move) ? alpha : best_move;
            if (beta <= alpha) {
                transpositionTable->store(bord, best_move, depth, move);
                return best_move;
            }
        }
        return best_move;
    }
    else {
        double best_move = INFINITY;
        MOVELIST moveList;
        moveList.count = 0; //TODO check if needed
        get_orderd_moves(bord, &moveList, positionTracker);
        int size = moveList.count;
        Move* moves = moveList.moves;
        for (int i = 0; i < size; i++) {
            *counter += 1;
            Move move = moves[i];
            Board boardCopy;
            copyBoard(bord, &boardCopy);
            makeMove(&boardCopy, &move, positionTracker);
            if (!weHaveMoves(&boardCopy)) {
                return -INFINITY;
            }
            double curr_move = INFINITY;
            if (isDraw(bord, positionTracker) == NOT_DRAW) {
                curr_move = minimaxOptimizedItterativeDeepening(&boardCopy, alpha, beta, depth - 1 + (inCheck(bord) ? 1 : 0), !maximizing_player, !whiteVraagteken, counter, transpositionTable, positionTracker);
            }
            positionTracker->removePosition(&boardCopy);
            // Each ply after a checkmate is slower, so they get ranked slightly less
            // We want the fastest mate!
            if (curr_move > MATE_THRESHOLD) {
                curr_move -= 1;
            }
            else if (curr_move < -MATE_THRESHOLD) {
                curr_move += 1;
            }
            best_move = (best_move < curr_move) ? best_move : curr_move;
            beta = (beta < best_move) ? beta : best_move;
            if (beta <= alpha) {
                transpositionTable->store(bord, best_move, depth, move);
                return best_move;
            }
        }
        return best_move;
    }
}

void minimax_rootOptimizedItterativeDeepening(Board* bord, int depth, bool maximize, Move* moveOut, MOVELIST* moveList, TranspositionTable* transpositionTable, PositionTracker* positionTracker, std::vector<Move>* triangular_table, std::chrono::high_resolution_clock::time_point startTime, size_t maxTime) {
    //What is the highest value move per our evaluation function?
    //std::string key = generateHashKey(cr);

    // Lookup the position in the Transposition Table
    TranspositionTableEntry* ttEntry = transpositionTable->lookup(bord);
    if (ttEntry != nullptr && ttEntry->depth >= depth) {
        // Return the stored evaluation score if depth is sufficient
        Board boardCopy;
        copyBoard(bord, &boardCopy);
        makeMove(&boardCopy, &(ttEntry->bestMove), positionTracker);
        if (isDraw(&boardCopy, positionTracker) || (findMoveIndex(moveList, &(ttEntry->bestMove))) == -1) {
            positionTracker->removePosition(&boardCopy);
            goto breakBothIfs; // Jump to the labeled statement
        }
        positionTracker->removePosition(&boardCopy);
        moveOut->src = (ttEntry->bestMove).src;
        moveOut->dst = (ttEntry->bestMove).dst;
        moveOut->special = (ttEntry->bestMove).special;
        moveOut->capture = (ttEntry->bestMove).capture;
        cout << "used transposition table" << endl;
        return;
    }
    breakBothIfs:

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    double best_move = maximize ? -INFINITY : INFINITY;
    orderMovesWithTriangularTable(bord, moveList, triangular_table);
    int size = moveList->count;
    Move* moves = moveList->moves;
    Move best_move_found = moves[0];

    bool end_game = check_end_game(bord);
    for (int i = 0; i < size; i++) {
        endTime = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        if (duration > maxTime) {
            moveOut->src = best_move_found.src;
            moveOut->dst = best_move_found.dst;
            moveOut->capture = best_move_found.capture;
            moveOut->special = best_move_found.special;
            return;
        }
        Move move = moves[i];
        Board boardCopy;
        copyBoard(bord, &boardCopy);
        makeMove(&boardCopy, &move, positionTracker);
        if (!weHaveMoves(&boardCopy)) {
            moveOut->src = move.src;
            moveOut->dst = move.dst;
            moveOut->capture = move.capture;
            moveOut->special = move.special;
            return;
        }
        double value = 0.0;
        int branchCount = 0;
        int* branchCounter = &branchCount;
        if (isDraw(bord, positionTracker) == NOT_DRAW) {
            value = minimaxOptimizedItterativeDeepening(&boardCopy, -INFINITY, INFINITY, depth - 1, !maximize, !white_plays((&boardCopy)), branchCounter, transpositionTable, positionTracker);
        }
        positionTracker->removePosition(&boardCopy);
        //cout << "move: " << move.NaturalOut(cr) << " " << branchCount << endl;
        if (maximize && value > best_move) {
            best_move = value;
            best_move_found = move;
        }
        else if (!maximize && value < best_move) {
            best_move = value;
            best_move_found = move;
        }
        //cout << "move: " << moveToString(&move) << " has value: " << best_move << endl;
    }
    transpositionTable->store(bord, best_move, depth, best_move_found);
    moveOut->src = best_move_found.src;
    moveOut->dst = best_move_found.dst;
    moveOut->capture = best_move_found.capture;
    moveOut->special = best_move_found.special;
}

void makeMiniMaxMove(Board* bord, MOVELIST* moveList, int depth, bool maximize, TranspositionTable* transpositionTable, PositionTracker* positionTracker) {
    Move moveOut;

    transpositionTable->printInfo();

    auto startTime = std::chrono::high_resolution_clock::now();
    minimax_root(bord, depth, maximize, &moveOut, moveList, transpositionTable, positionTracker);
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    cout << "the minimax engine selected: " << moveToString(&moveOut) << " out of " << moveList->count << " moves and it was located at position: " << findMoveIndex(moveList,&moveOut) << " in: " << duration / 1000 << " milliseconds." << endl;
    makeMove(bord, &moveOut, positionTracker);
}

void makeMiniMaxOptimizedMove(Board* bord, MOVELIST* moveList, int depth, bool maximize, TranspositionTable* transpositionTable, PositionTracker* positionTracker) {
    Move moveOut;

    transpositionTable->printInfo();

    auto startTime = std::chrono::high_resolution_clock::now();
    minimax_rootOptimized(bord, depth, maximize, &moveOut, moveList, transpositionTable, positionTracker);
    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();
    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    cout << "the minimax engine selected: " << moveToString(&moveOut) << " out of " << moveList->count << " moves and it was located at position: " << findMoveIndex(moveList, &moveOut) << " in: " << duration/1000 << " milliseconds." << endl;
    makeMove(bord, &moveOut, positionTracker);
}

void makeMiniMaxOptimizedItterativeDeepeningMove(Board* bord, MOVELIST* moveList, int maxDepth, bool maximize, TranspositionTable* transpositionTable, PositionTracker* positionTracker, size_t maxTime) {
    Move moveOut;
    std::vector<Move> triangular_table;
    transpositionTable->printInfo();
    int depth = 1;
    // initialize types
    auto startTime = std::chrono::high_resolution_clock::now();
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    startTime = std::chrono::high_resolution_clock::now();
    while (duration < maxTime && depth < maxDepth) {
        minimax_rootOptimizedItterativeDeepening(bord, depth, maximize, &moveOut, moveList, transpositionTable, positionTracker, &triangular_table, startTime, maxTime);
        if (!isMoveInVector(triangular_table, moveOut)) {
            triangular_table.push_back(moveOut);
        }
        /*
        // Print all elements in the vector
        cout << endl;
        cout << endl;
        std::cout << "Elements in the vector:" << std::endl;
        for (Move move : triangular_table) {
            std::cout << moveToStringShort(&move) << ", ";
        }
        cout << endl;
        cout << endl;
        */
        // Get the ending timestamp
        endTime = std::chrono::high_resolution_clock::now();
        // Calculate the duration in microseconds (change to other duration units as needed)
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        depth++;
        cout << "depth: " << depth << " found: " << moveToStringShort(&moveOut) << " out of " << moveList->count << " moves and it was located at position: " << findMoveIndex(moveList, &moveOut) << " in: " << duration << " milliseconds." << endl;
    }

    cout << "the minimax engine selected: " << moveToString(&moveOut) << " out of " << moveList->count << " moves and it was located at position: " << findMoveIndex(moveList, &moveOut) << " in: " << duration << " milliseconds." << endl;
    makeMove(bord, &moveOut, positionTracker);
}

void askForMove(Board* bord, Move* move, MOVELIST* moveList) {
    orderMoves(bord, moveList); // TODO remove only for testing ordening
    printMoveList(moveList);
    cout << "Enter the number of the move you want to play" << endl;
    int moveNumber;
    std::cin >> moveNumber;
    move->src = moveList->moves[moveNumber].src;
    move->dst = moveList->moves[moveNumber].dst;
    move->special = moveList->moves[moveNumber].special;
    move->capture = moveList->moves[moveNumber].capture;
}

void makeRandomMove(Board* bord, MOVELIST* moveList, PositionTracker* positionTracker) {
    GenLegalMoveList(moveList, bord, positionTracker);
    if (moveList->count == 0) {
        cout << "there are no legal moves" << endl;
        return;
    }
    /*
    for (int i = 0; i < moveList->count; i++) {
        cout << i << ") " << moveToString(&moveList->moves[i]) << endl;
    }
    */
    int choosen = generateRandomNumber(moveList->count);
    cout << "Randomly selected: " << moveToString(&moveList->moves[choosen]) << " out of " << moveList->count << " moves." << endl;
    makeMove(bord, &moveList->moves[choosen], positionTracker);
    //printBoard(bord);
}

void findBestMove(Board* bord, MOVELIST* moveList, TranspositionTable* transpositionTable, PositionTracker* positionTracker, Move* move) {
    std::vector<Move> triangular_table;
    int maxTime = 1000;
    int maxDepth = 50;
    //transpositionTable->printInfo();
    int depth = 1;
    // initialize types
    auto startTime = std::chrono::high_resolution_clock::now();
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    startTime = std::chrono::high_resolution_clock::now();
    while (duration < maxTime && depth < maxDepth) {
        minimax_rootOptimizedItterativeDeepening(bord, depth, true, move, moveList, transpositionTable, positionTracker, &triangular_table, startTime, maxTime);
        if (!isMoveInVector(triangular_table, *move)) {
            triangular_table.push_back(*move);
        }
        endTime = std::chrono::high_resolution_clock::now();
        // Calculate the duration in microseconds (change to other duration units as needed)
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        depth++;
        //cout << depth << endl;
    }

}

void printEngines() {
    int i = 0;
    for (const char* engine : engines) printf("%d) %s.\n", i++, engine);
}