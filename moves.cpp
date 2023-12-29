#include <cstdint>
#include "moves.h"

// Definitions for bishop_attacks and rook_attacks
U64 bishop_attacks[64][512];
U64 rook_attacks[64][4096];

#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)

int count_bits(U64 bitboard){
    int count = 0;
    while(bitboard){
        count++;
        bitboard &= bitboard-1;
    }
    return count;
}

// Function to get the index of the least significant 1-bit
static inline int get_ls1b_index(U64 bitboard){
    return bitboard ? __builtin_ctzll(bitboard) : -1;
}

U64 set_occupancy(int index, int bits_in_mask, U64 attacks_mask){
    U64 occupancy = 0ULL;
    for (int count = 0; count < bits_in_mask; count++){
        int square = get_ls1b_index(attacks_mask);

        pop_bit(attacks_mask,square);

        if(index & (1 << count)){
            occupancy |= (1ULL << square);
        }
    }
    return occupancy;
}

// generate bishop attacks on the fly
U64 bishop_attacks_on_the_fly(int square, U64 block){
    square = 63-square;
    // result attacks bitboard
    U64 attacks = 0ULL;

    // init ranks & files
    int r, f;

    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;

    // generate bishop atacks
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++){
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }

    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++){
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }

    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--){
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }

    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--){
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }

    // return attack map
    return attacks;
}

// generate rook attacks on the fly
U64 rook_attacks_on_the_fly(int square, U64 block){
    square = 63-square;
    // result attacks bitboard
    U64 attacks = 0ULL;

    // init ranks & files
    int r, f;

    // init target rank & files
    int tr = square / 8;
    int tf = square % 8;

    // generate rook attacks
    for (r = tr + 1; r <= 7; r++){
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }

    for (r = tr - 1; r >= 0; r--){
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }

    for (f = tf + 1; f <= 7; f++){
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }

    for (f = tf - 1; f >= 0; f--){
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }

    // return attack map
    return attacks;
}

// init slider piece's attack tables
void init_sliders_attacks(bool bishop){
    // loop over 64 board squares
    for (int square = 0; square < 64; square++){
        // init current mask
        U64 attack_mask = bishop ? bischopMoves[square] : rookMoves[square];

        // init relevant occupancy bit count
        int relevant_bits_count = count_bits(attack_mask);

        // init occupancy indicies
        int occupancy_indicies = (1 << relevant_bits_count);

        // loop over occupancy indicies
        for (int index = 0; index < occupancy_indicies; index++){
            // bishop
            if (bishop){
                // init current occupancy variation
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);

                // init magic index
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bischopRelevantBits[square]);

                // init bishop attacks
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            }
            // rook
            else{
                // init current occupancy variation
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);

                // init magic index
                int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - rookRelevantBits[square]);

                // init bishop attacks
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);

            }
        }
    }
}

// get bishop attacks
U64 get_bishop_attacks(int square, U64 occupancy){
    // get bishop attacks assuming current board occupancy
    occupancy &= bischopMoves[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bischopRelevantBits[square];

    // return bishop attacks
    return bishop_attacks[square][occupancy];
}

// get rook attacks
U64 get_rook_attacks(int square, U64 occupancy)
{
    // get bishop attacks assuming current board occupancy
    occupancy &= rookMoves[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rookRelevantBits[square];

    // return rook attacks
    return rook_attacks[square][occupancy];
}