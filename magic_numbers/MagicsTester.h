#pragma once
#include <cstring>
#include <cstdio>

#include "MagicNumberGenerator.h"
#include "../moves.h"

class MagicsTester {
    MagicNumberGenerator magicNumberGenerator;
public:
    void init_magic_numbers();
    U64 find_magic_number(int square, int relevant_bits, bool bischop);
    void resetMagicSeed();
private:
    //bool check_magic_number(int square, U64 magic_number, bool bishop);
};
