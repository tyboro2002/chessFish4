#ifndef CHESSFISH4_MAGICNUMBERGENERATOR_H
#define CHESSFISH4_MAGICNUMBERGENERATOR_H
#include <random>

typedef unsigned long long U64;

class MagicNumberGenerator {
    unsigned int state = 1804289383;
public:
    MagicNumberGenerator();
    U64 generate_magic_number_canidate();
    U64 get_random_U64_number();
    unsigned int getState();
    void resetState();
private:
    unsigned int get_random_U32_number();
};
#endif //CHESSFISH4_MAGICNUMBERGENERATOR_H
