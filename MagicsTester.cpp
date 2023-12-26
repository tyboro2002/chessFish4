#include "MagicsTester.h"

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
    if(bitboard){
        return count_bits((bitboard & ~bitboard)-1);
    }else{
        return -1;
    }
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
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }

    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }

    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }

    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
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
    for (r = tr + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }

    for (r = tr - 1; r >= 0; r--)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }

    for (f = tf + 1; f <= 7; f++)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }

    for (f = tf - 1; f >= 0; f--)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }

    // return attack map
    return attacks;
}

U64 MagicsTester::find_magic_number(int square, int relevant_bits, bool bischop){
    // init occupancies
    U64 occupancies[4096];

    // init attack tables
    U64 attacks[4096];

    // init used attacks
    U64 used_attacks[4096];

    // init attack mask for a current piece
    U64 attack_mask = bischop ? bischopMovesTester[square] : rookMovesTester[square];

    // init occupancy indicies
    int occupancy_indicies = 1 << relevant_bits;

    // loop over occupancy indicies
    for (int index = 0; index < occupancy_indicies; index++){
        // init occupancies
        // init occupancies
        occupancies[index] = set_occupancy(index, relevant_bits, attack_mask);

        // init attacks
        attacks[index] = bischop ? bishop_attacks_on_the_fly(square, occupancies[index]) :
                         rook_attacks_on_the_fly(square, occupancies[index]);
    }
// test magic numbers loop
    for (int random_count = 0; random_count < 100000000; random_count++)
    {
        // generate magic number candidate
        U64 magic_number = magicNumberGenerator.generate_magic_number_canidate();

        // skip inappropriate magic numbers
        if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;

        // init used attacks
        memset(used_attacks, 0ULL, sizeof(used_attacks));

        // init index & fail flag
        int index, fail;

        // test magic index loop
        for (index = 0, fail = 0; !fail && index < occupancy_indicies; index++)
        {
            // init magic index
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevant_bits));

            // if magic index works
            if (used_attacks[magic_index] == 0ULL)
                // init used attacks
                used_attacks[magic_index] = attacks[index];

                // otherwise
            else if (used_attacks[magic_index] != attacks[index])
                // magic index doesn't work
                fail = 1;
        }

        // if magic number works
        if (!fail)
            // return it
            return magic_number;
    }

    // if magic number doesn't work
    printf("  Magic number fails!\n");
    return 0ULL;
}

// init magic numbers
void MagicsTester::init_magic_numbers()
{
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
        // init rook magic numbers
        printf("0x%llxULL,\n",find_magic_number(square, rookRelevantBits[square], false));
        //rook_magic_numbers[square] = find_magic_number(square, rookRelevantBits[square], false);

    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
        // init bishop magic numbers
        //printf("0x%llxULL,\n",find_magic_number(square, bischopRelevantBits[square], true);
        bishop_magic_numbers[square] = find_magic_number(square, bischopRelevantBits[square], true);
}
