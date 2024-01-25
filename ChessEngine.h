#pragma once
#include "game.h"

class ChessEngine {
public:
    virtual void initialize() = 0;
    virtual void makeMove(Board* bord) = 0;
    virtual ~ChessEngine() = default;
};
