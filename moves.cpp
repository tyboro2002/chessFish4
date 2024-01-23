#include "moves.h"

// Definitions for bishop_attacks and rook_attacks
U64 bishop_attacks[64][BISHOP_ATTACKS];
U64 rook_attacks[64][ROOK_ATTACKS];

#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? bitboard ^= (1ULL << square) : 0)


U64 set_occupancy(int index, int bits_in_mask, U64 attacks_mask){
    U64 occupancy = 0ULL;
    for (int count = 0; count < bits_in_mask; count++){
        int square = get_ls1b_index(attacks_mask);

        pop_bit(attacks_mask,square);

        if(index & (1 << count)) occupancy |= (1ULL << square);
    }
    return occupancy;
}

// generate bishop attacks on the fly
U64 bishop_attacks_on_the_fly(int square, U64 block){
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

bool testMagicNumber(int square,bool bischop){
    //printf("testing magic number: %llu for square: %d: for piece: %s\n",bischop ? bishop_magic_numbers[square] : rook_magic_numbers[square], square, bischop ? "bischop" : "rook");
    U64 mask = bischop ? bischopMovesONE_OFF[square] : rookMovesONE_OFF[square];
    int bits_in_mask = count_bits(mask);
    U64 amount_of_tests = 1 << bits_in_mask;
    //printf("bits in mask: %d giving %llu occupancys\n", bits_in_mask, amount_of_tests);
    for (int i = 0; i<amount_of_tests;i++){
        U64 occ_test = set_occupancy(i,bits_in_mask,mask);
        if(bischop){
            U64 fly = bishop_attacks_on_the_fly(square,occ_test);
            U64 magic = get_bishop_attacks(square, occ_test); // 1152930310397399040 square 5

            //std::cout << std::bitset<64>(occ_test) << " " << i << std::endl;
            //std::cout << std::bitset<64>(fly) << std::endl;
            //std::cout << std::bitset<64>(magic) << std::endl << std::endl;

            //printBitBoard(fly, "fly");
            //printBitBoard(magic, "magic");
            if(fly != magic){
                return false;
            }
        }else{
            U64 fly = rook_attacks_on_the_fly(square,occ_test);
            U64 magic = get_rook_attacks(square, occ_test);
            if(fly != magic){
                std::cout << std::bitset<64>(occ_test) << " " << i  << " failed, needed: " << amount_of_tests << " (magic number:  " << rook_magic_numbers[square] << " )" << std::endl;
                std::cout << std::bitset<64>(fly) << std::endl;
                std::cout << std::bitset<64>(magic) << std::endl << std::endl;
                return false;
            }
        }
    }
    return true;
}

void init_all_sliders_attacks(bool testWorking, const char* outputFile, const char* outputFileCondensed){
    init_sliders_attacks(true);
    init_sliders_attacks(false);
    MagicsTester magicsTester;
    std::ofstream output(outputFile);
    if (!output.is_open()) {
        std::cerr << "Error opening output file\n";
        exit(EXIT_FAILURE);
    }
    // Output to the file
    output << "started generating" << std::endl;

    std::ofstream outputCondensed(outputFileCondensed);
    if (!outputCondensed.is_open()) {
        std::cerr << "Error opening outputCondensed file\n";
        exit(EXIT_FAILURE);
    }
    // Output to the file
    outputCondensed << "started generating" << std::endl;
    if(testWorking){
        // test working bischops
        U64 tries = 0;
        int i=0;
        output << "bischop: " << std::endl;
        outputCondensed << "bischop: " << std::endl;
        while(i<64){
            init_sliders_attacks(true);
            if(testMagicNumber(i,true)){
                //fprintf(output,"found magic number: %llu for square: %d: for piece: %s\n",bischop ? bishop_magic_numbers[i] : rook_magic_numbers[i], i, bischop ? "bischop" : "rook");
                output << "found magic number: " << bishop_magic_numbers[i] << "ULL for square: " << i << " for piece: bishop after: " << tries << " tries" << std::endl;
                outputCondensed << bishop_magic_numbers[i] << "ULL," << std::endl;
                i++;
                tries = 0;
            }else{
                magicsTester.resetMagicSeed();
                bishop_magic_numbers[i] = magicsTester.find_magic_number(i, bischopRelevantBits[i], true);
                tries++;
                std::cout << tries << " for square: " << i << std::endl;
            }
        }
        init_sliders_attacks(true);
        // Print the elements of the array
        for (int j = 0; j < 64; j++) {
            std::cout << "bishop_magic_numbers[" << j << "] = " << bishop_magic_numbers[j] << " works " << (testMagicNumber(j,true) ? "" : "not") << std::endl;
        }

        // test working rooks
        i=0;
        output << "rook: " << std::endl;
        outputCondensed << "rook: " << std::endl;
        while(i<64){
            init_sliders_attacks(false);
            if(testMagicNumber(i, false)){
                output << "found magic number: " << rook_magic_numbers[i] << " for square: " << i << " for piece: rook after: " << tries << " tries" << std::endl;
                outputCondensed << rook_magic_numbers[i] << "ULL," << std::endl;
                i++;
                tries = 0;
            }else{
                //magicsTester.resetMagicSeed();
                rook_magic_numbers[i] = magicsTester.find_magic_number(i, rookRelevantBits[i], false);
                tries++;
                std::cout << tries << " for square: " << i << std::endl;
            }
        }
        init_sliders_attacks(false);
        // Print the elements of the array
        for (int j = 0; j < 64; j++) {
            std::cout << "rook_magic_numbers[" << j << "] = " << rook_magic_numbers[j] << " works " << (testMagicNumber(j,false) ? "" : "not") << std::endl;
        }
    }
}

// init slider piece's attack tables
void init_sliders_attacks(bool bishop){
    // loop over 64 board squares
    for (int square = 0; square < 64; square++){
        // init current mask
        U64 attack_mask = bishop ? bischopMovesONE_OFF[square] : rookMovesONE_OFF[square];

        // init relevant occupancy bit count
        int relevant_bits_count = count_bits(attack_mask);

        // init occupancy indicies
        int occupancy_indicies = (1 << relevant_bits_count);

        // loop over occupancy indicies
        for (int index = 0; index < occupancy_indicies; index++){
            // bishop
            // init current occupancy variation
            U64 occupancy = set_occupancy(index, relevant_bits_count, attack_mask);
            if (bishop){
                // init magic index
                U64 magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bischopRelevantBits[square]);
                if(magic_index >= BISHOP_ATTACKS) {printf("ohla %llu is te veel bishop for occupancy: %llu at index: %d for square: %d\n",magic_index, occupancy, index, square); exit(EXIT_FAILURE);}
                // init bishop attacks
                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            }else{ // rook
                // init magic index
                U64 magic_index = (occupancy * rook_magic_numbers[square]) >> (64-rookRelevantBits[square]);
                if(magic_index >= ROOK_ATTACKS) {printf("ohla %llu is te veel rook for occupancy: %llu at index: %d for square: %d magic number: %llu\n",magic_index, occupancy, index, square,rook_magic_numbers[square]); exit(EXIT_FAILURE);}
                // init bishop attacks
                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
            }
        }
    }
}