#ifndef CHESSFISH4_MAGICNUMBERGENERATOR_H
#define CHESSFISH4_MAGICNUMBERGENERATOR_H

typedef unsigned long long U64;

class MagicNumberGenerator {
    unsigned int state = 1804289383;
public:
    U64 generate_magic_number_canidate();
private:
    unsigned int get_random_U32_number();
    U64 get_random_U64_number();
};
#endif //CHESSFISH4_MAGICNUMBERGENERATOR_H