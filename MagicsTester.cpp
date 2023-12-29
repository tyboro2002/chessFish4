#include "MagicsTester.h"

U64 rook_magic_numbers[64] = {
        0x40040844404084ULL,
        0x2004208a004208ULL,
        0x10190041080202ULL,
        0x108060845042010ULL,
        0x581104180800210ULL,
        0x2112080446200010ULL,
        0x1080820820060210ULL,
        0x3c0808410220200ULL,
        0x4050404440404ULL,
        0x21001420088ULL,
        0x24d0080801082102ULL,
        0x1020a0a020400ULL,
        0x40308200402ULL,
        0x4011002100800ULL,
        0x401484104104005ULL,
        0x801010402020200ULL,
        0x400210c3880100ULL,
        0x404022024108200ULL,
        0x810018200204102ULL,
        0x4002801a02003ULL,
        0x85040820080400ULL,
        0x810102c808880400ULL,
        0xe900410884800ULL,
        0x8002020480840102ULL,
        0x220200865090201ULL,
        0x2010100a02021202ULL,
        0x152048408022401ULL,
        0x20080002081110ULL,
        0x4001001021004000ULL,
        0x800040400a011002ULL,
        0xe4004081011002ULL,
        0x1c004001012080ULL,
        0x8004200962a00220ULL,
        0x8422100208500202ULL,
        0x2000402200300c08ULL,
        0x8646020080080080ULL,
        0x80020a0200100808ULL,
        0x2010004880111000ULL,
        0x623000a080011400ULL,
        0x42008c0340209202ULL,
        0x209188240001000ULL,
        0x400408a884001800ULL,
        0x110400a6080400ULL,
        0x1840060a44020800ULL,
        0x90080104000041ULL,
        0x201011000808101ULL,
        0x1a2208080504f080ULL,
        0x8012020600211212ULL,
        0x500861011240000ULL,
        0x180806108200800ULL,
        0x4000020e01040044ULL,
        0x300000261044000aULL,
        0x802241102020002ULL,
        0x20906061210001ULL,
        0x5a84841004010310ULL,
        0x4010801011c04ULL,
        0xa010109502200ULL,
        0x4a02012000ULL,
        0x500201010098b028ULL,
        0x8040002811040900ULL,
        0x28000010020204ULL,
        0x6000020202d0240ULL,
        0x8918844842082200ULL,
        0x4010011029020020ULL
};

U64 bishop_magic_numbers[64] = {
        54045411736421449ul,
        3467949876944314368ul,
        1170054334914564ul,
        9386668213868429344ul,
        2612653001713909776ul,
        143074487959556ul,
        144405463509778640ul,
        2314868913337286662ul,
        1161933171206914308ul,
        5102013192899726ul,
        2306410683698270464ul,
        2343024120192851072ul,
        75437501641129984ul,
        8942543438848ul,
        576498144559498370ul,
        9313461918075293712ul,
        18041957234770120ul,
        9007906867522064ul,
        18577417326039296ul,
        72198365919854592ul,
        291045211837106176ul,
        4728814801970530308ul,
        70385941350476ul,
        72199439661474050ul,
        1227301284920249602ul,
        4904472908247872790ul,
        149533851001120ul,
        576746625460996128ul,
        4686136700865880064ul,
        92330389433092353ul,
        23648296635597120ul,
        1232552543291392ul,
        2348574017462305ul,
        74311661594870048ul,
        13835098222891107329ul,
        2201179652224ul,
        1152992149307130112ul,
        144397766860079688ul,
        1299994464869172242ul,
        2306124761249416192ul,
        23089753846974488ul,
        7512568235569455104ul,
        18579548627011656ul,
        1126038738339843ul,
        672935777931282ul,
        2323875584167379009ul,
        51793734359057408ul,
        2594777699881387520ul,
        1139147805032960ul,
        4683780038219268372ul,
        1161232523493384ul,
        72706313822208ul,
        653022014721892608ul,
        10408980038721605632ul,
        18070782873796608ul,
        4972541478955057216ul,
        10151825236041728ul,
        8521389665689600ul,
        14992505291550049312ul,
        290271107551376ul,
        2594076822951969037ul,
        270224911207891468ul,
        22817078453010948ul,
        153157608243496480ul
};

// Function to check if magic number produces the same result as on-the-fly function
bool MagicsTester::check_magic_number(int square, U64 magic_number, bool bishop) {
    const int num_tests = 1000; // You can adjust the number of random tests

    for (int i = 0; i < num_tests; ++i) {
        // Generate a random occupancy
        U64 random_occupancy = magicNumberGenerator.get_random_U64_number();

        // Get the result from on-the-fly function
        U64 on_the_fly_result = bishop ? bishop_attacks_on_the_fly(square, random_occupancy) :
                                rook_attacks_on_the_fly(square, random_occupancy);

        U64 magic_number_result;
        // Get the result using the magic number
        if(bishop){
            // get bishop attacks assuming current board occupancy
            random_occupancy &= bischopMoves[square];
            random_occupancy *= magic_number;
            random_occupancy >>= 64 - bischopRelevantBits[square];

            magic_number_result = bishop_attacks[square][random_occupancy];
        }else{
            // get bishop attacks assuming current board occupancy
            random_occupancy &= rookMoves[square];
            random_occupancy *= magic_number;
            random_occupancy >>= 64 - rookRelevantBits[square];

            magic_number_result = rook_attacks[square][random_occupancy];
        }

        // Compare the results
        if (on_the_fly_result != magic_number_result) {
            return false;
        }
    }
    return true;
}

U64 MagicsTester::find_magic_number(int square, int relevant_bits, bool bishop) {
    // init occupancies
    U64 occupancies[4096];

    // init attack tables
    U64 attacks[4096];

    // init used attacks
    U64 used_attacks[4096];

    // init attack mask for a current piece
    U64 attack_mask = bishop ? bischopMoves[square] : rookMoves[square];

    // init occupancy indicies
    int occupancy_indicies = 1 << relevant_bits;

    // loop over occupancy indicies
    for (int index = 0; index < occupancy_indicies; index++){
        // init occupancies
        occupancies[index] = set_occupancy(index, relevant_bits, attack_mask);

        // init attacks
        attacks[index] = bishop ? bishop_attacks_on_the_fly(square, occupancies[index]) :
                         rook_attacks_on_the_fly(square, occupancies[index]);
    }

    // test magic numbers loop
    for (int random_count = 0; random_count < 100000000; random_count++){
        // generate magic number candidate
        U64 magic_number = magicNumberGenerator.generate_magic_number_canidate();

        // skip inappropriate magic numbers
        if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;

        // init used attacks
        memset(used_attacks, 0ULL, sizeof(used_attacks));

        // init index & fail flag
        int index;
        bool fail;

        // test magic index loop
        for (index = 0, fail = false; !fail && index < occupancy_indicies; index++){
            // init magic index
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevant_bits));

            // if magic index works
            if (used_attacks[magic_index] == 0ULL)
                // init used attacks
                used_attacks[magic_index] = attacks[index];

                // otherwise
            else if (used_attacks[magic_index] != attacks[index])
                // magic index doesn't work
                fail = true;
        }

        // if magic number works
        if (!fail) {
            // return it
            //return magic_number;
           //if (check_magic_number(square, magic_number, bishop)) {
                // return it
                return magic_number;
            //}
        }
    }

    // if magic number doesn't work
    printf("  Magic number fails!\n");
    return 0ULL;
}



// init magic numbers
void MagicsTester::init_magic_numbers()
{
    // loop over 64 board squares
    for (int square = 0; square < 64; square++) {
        // init rook magic numbers
        printf("0x%llxULL,\n",find_magic_number(square, rookRelevantBits[square], false));
        rook_magic_numbers[square] = find_magic_number(square, rookRelevantBits[square], false);
    }

    printf("\n\n\n");
    // loop over 64 board squares
    for (int square = 0; square < 64; square++) {
        // init bishop magic numbers
        printf("0x%llxULL,\n",find_magic_number(square, bischopRelevantBits[square], true));
        bishop_magic_numbers[square] = find_magic_number(square, bischopRelevantBits[square], true);
    }
}