#include "moves.h"

// init slider piece's attack tables
void init_sliders_attacks(bool bishop){
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
    {
        // init bishop & rook masks
        bishop_masks[square] = bischopMoves[square];
        rook_masks[square] = rookMoves[square];

        // init current mask
        U64 attack_mask = bishop ? bishop_masks[square] : rook_masks[square];

        // init relevant occupancy bit count
        int relevant_bits_count = count_bits(attack_mask);

        // init occupancy indicies
        int occupancy_indicies = (1 << relevant_bits_count);

        // loop over occupancy indicies
        for (int index = 0; index < occupancy_indicies; index++)
        {
            // bishop
            if (bishop)
            {
                // init current occupancy variation
                U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);

                // init magic index
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bischopRelevantBits[square]);

                // init bishop attacks
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            }

                // rook
            else
            {
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
U64 get_bishop_attacks(int square, U64 occupancy)
{
    // get bishop attacks assuming current board occupancy
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bischopRelevantBits[square];

    // return bishop attacks
    return bishop_attacks[square][occupancy];
}

// get rook attacks
U64 get_rook_attacks(int square, U64 occupancy)
{
    // get bishop attacks assuming current board occupancy
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rookRelevantBits[square];

    // return rook attacks
    return rook_attacks[square][occupancy];
}