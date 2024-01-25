#include "MagicNumberGenerator.h"

MagicNumberGenerator::MagicNumberGenerator() {
    // Use a random_device to seed the random number generator
    std::random_device rd;

    // Use the Mersenne Twister engine for randomness
    std::mt19937 gen(rd());

    // Define a uniform distribution for integers in a certain range
    std::uniform_int_distribution<unsigned int> distribution(1, INT32_MAX);
    state = distribution(gen);
}

unsigned int MagicNumberGenerator::get_random_U32_number(){
    unsigned int number = state;

    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    state = number;

    return number;
}

U64 MagicNumberGenerator::get_random_U64_number(){
    U64 n1, n2, n3, n4;

    n1 = (U64)(get_random_U32_number() & 0xFFFF);
    n2 = (U64)(get_random_U32_number() & 0xFFFF);
    n3 = (U64)(get_random_U32_number() & 0xFFFF);
    n4 = (U64)(get_random_U32_number() & 0xFFFF);

    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

U64 MagicNumberGenerator::generate_magic_number_canidate(){
    return get_random_U64_number() & get_random_U64_number() & get_random_U64_number();
}

unsigned int MagicNumberGenerator::getState() const {
    return state;
}

void MagicNumberGenerator::resetState() {

    // Use a random_device to seed the random number generator
    std::random_device rd;

    // Use the Mersenne Twister engine for randomness
    std::mt19937 gen(rd());

    // Define a uniform distribution for integers in a certain range
    std::uniform_int_distribution<unsigned int> distribution(1, INT32_MAX);

    state = distribution(gen);
}
