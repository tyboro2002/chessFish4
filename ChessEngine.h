#pragma once
#include "game.h"

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
    int pawnScore = countSetBits(colorMask & bord->rook);
    int knightScore = countSetBits(colorMask & bord->knight) * 3;
    int bishopScore = countSetBits(colorMask & bord->bishop) * 3;
    int rookScore = countSetBits(colorMask & bord->rook) * 5;
    int queenScore = countSetBits(colorMask & bord->queen) * 10;
    return pawnScore + knightScore + bishopScore+ rookScore + queenScore;
}