#pragma once
#include "../game.h"
#include <map>

enum Engines { // to add an engine add it in this list and in the list in the constructor in GUIEngineGame.h
    HUMAN,
    MINIMAX,
    RANDOM,
    MONTE_CARLO,
    NUM_ENGINES
};

struct SimulationResults{
    U64 whiteWins = 0;
    U64 blackWins = 0;
    U64 draws = 0;
public:
    void update(EndGameResult endGameResult){
        switch (endGameResult) {
            case WHITE_WINS_Checkmate:
            case WHITE_WINS_Resignation:
            case WHITE_WINS_Timeout:
                whiteWins++;
                break;
            case BLACK_WINS_Checkmate:
            case BLACK_WINS_Resignation:
            case BLACK_WINS_Timeout:
                blackWins++;
                break;
            default:
                draws++;
        }
    }

    void reset(){
        whiteWins = 0;
        blackWins = 0;
        draws = 0;
    }

    [[nodiscard]] U64 getWhiteWins() const{
        return whiteWins;
    }

    [[nodiscard]] U64 getBlackWins() const{
        return blackWins;
    }

    [[nodiscard]] U64 getDraws() const{
        return draws;
    }
};

const std::map<Pieces, int> piece_score_dic = {
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

class ChessEngine {
public:
    virtual void initialize() = 0;
    virtual void makeMove(Board* bord) = 0;
    virtual ~ChessEngine() = default;
};

inline int evaluateBoard(Board* bord){
    U64 colorMask;
    if(bord->whiteToPlay) colorMask = bord->white;
    else colorMask = bord->black;
    int pawnScore = countSetBits(colorMask & bord->pawn);
    int knightScore = countSetBits(colorMask & bord->knight) * 3;
    int bishopScore = countSetBits(colorMask & bord->bishop) * 3;
    int rookScore = countSetBits(colorMask & bord->rook) * 5;
    int queenScore = countSetBits(colorMask & bord->queen) * 10;
    return pawnScore + knightScore + bishopScore+ rookScore + queenScore;
}

inline int evaluate_capture(Board* bord, Action* mv) {
    /*
    *   Given a capturing move, weight the trade being made.
    */

    Pieces _to = pieceAt(mv->dst,bord);
    Pieces _from = pieceAt(mv->src, bord);

    if (bord->enPassentValid && (bord->enPassantTarget == mv->dst)) {
        return 100;// piece_score_dic.find(WPAWN)->second;
    }
    return piece_score_dic.find(_to)->second - piece_score_dic.find(_from)->second;
}

inline int evaluate_piece(Pieces piece, int square, bool endgame) {
    switch (piece){
        case WPAWN:
            return pawnEvalWhite[square];
        case BPAWN:
            return pawnEvalBlack[square];
        case WKNIGHT:
            return knightEval[square];
        case BKNIGHT:
            return knightEval[square];
        case WBISCHOP:
            return bishopEvalWhite[square];
        case BBISCHOP:
            return bishopEvalBlack[square];
        case WROOK:
            return rookEvalWhite[square];
        case BROOK:
            return rookEvalBlack[square];
        case WQUEEN:
            return queenEval[square];
        case BQUEEN:
            return queenEval[square];
        case WKING:
            if (endgame) {
                return kingEvalEndGameWhite[square];
            }
            else {
                return kingEvalWhite[square];
            }
        case BKING:
            if (endgame) {
                return kingEvalEndGameBlack[square];
            }
            else {
                return kingEvalBlack[square];
            }
        default:
            return 0;
    }
}

inline double move_value(Board* bord, Action* mv, bool endgame) {
    /*
    * How good is a move?
    * A promotion is great.
    * A weaker piece taking a stronger piece is good.
    * A stronger piece taking a weaker piece is bad.
    * Also consider the position change via piece-square table.
    */

    if (mv->special != Non_Exceptional) {
        if (bord->whiteToPlay) return INFINITY;
        else return -INFINITY;
    }

    Pieces _piece = pieceAt(mv->src, bord);
    int _from_value = evaluate_piece(_piece, mv -> src, endgame);
    int _to_value = evaluate_piece(_piece, mv -> dst, endgame);
    int position_change = _to_value - _from_value;

    int capture_value = 0.0;

    if (pieceAt(mv->dst, bord) != NOPIECE) {
        capture_value = evaluate_capture(bord, mv);
    }

    int current_move_value = capture_value + position_change;
    if (!bord->whiteToPlay) current_move_value = -current_move_value;
    return current_move_value;

}

inline void reverseArray(Action* arr, const std::size_t size) {
    for (std::size_t i = 0; i < size / 2; i++) {
        std::swap(arr[i], arr[size - 1 - i]);
    }
}

inline void orderMoves(Board* bord, ActionList* moveList) {
    bool end_game = false;//check_end_game(bord);

    int size = moveList->count;
    // Define a lambda function to compare moves by their values
    auto compareMoves = [&](Action a, Action b) {
        int value1 = move_value(bord, &a, end_game);
        int value2 = move_value(bord, &b, end_game);
        return value1 > value2;
    };

    // Sort the moves array using the comparator function
    std::stable_sort(moveList->moves, moveList->moves + size, compareMoves);

    if (!bord->whiteToPlay) reverseArray(moveList->moves, size);
}