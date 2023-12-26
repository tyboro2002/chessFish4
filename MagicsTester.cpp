#include "MagicsTester.h"

U64 rook_magic_numbers[64] = {
        0x4409008012ULL,
        0x8000000800a400ULL,
        0x9002041009000030ULL,
        0x10480100226009a8ULL,
        0x801a0105000424ULL,
        0x5000500208000008ULL,
        0x4441004002904008ULL,
        0x40801910d4004ULL,
        0x40002148012c000ULL,
        0x1410000000715000ULL,
        0x1915500080810013ULL,
        0x10001004000518ULL,
        0x8000020400881001ULL,
        0x800604200081161ULL,
        0xc0018c1000262001ULL,
        0x82c3002408000005ULL,
        0x4050008210040800ULL,
        0x1001040003ULL,
        0x2010020000000002ULL,
        0x810a228058600ULL,
        0x280000a0000ULL,
        0x1010088000040040ULL,
        0x4908002040042210ULL,
        0x5020008400082300ULL,
        0x100e400a8048002ULL,
        0x808210004088418ULL,
        0x800000005310000ULL,
        0x1180400041012202ULL,
        0x10001000300080ULL,
        0xa00880900000eaULL,
        0x8043008480200818ULL,
        0x80012018004004ULL,
        0x1800200000200004ULL,
        0x800044c12000020ULL,
        0x1445008c10040001ULL,
        0x7000000009018ULL,
        0x1520040030002800ULL,
        0x1100008020014ULL,
        0x1001064a2250000ULL,
        0x2000820300180020ULL,
        0x8b00000824002080ULL,
        0x200044000e020008ULL,
        0x1400063040000ULL,
        0x400000003100082ULL,
        0x880208001804ULL,
        0x504010a040410720ULL,
        0xa0800302128a0ULL,
        0x620020000348940ULL,
        0x8080108000000108ULL,
        0xa442200000080ULL,
        0x20004403000010ULL,
        0x80300000020830ULL,
        0x2001800040001402ULL,
        0x240200288000022ULL,
        0x400c802211420200ULL,
        0x4000001000d2000ULL,
        0x8024012001002108ULL,
        0xa09540042500000ULL,
        0x1080c804200ULL,
        0x10088143a0022000ULL,
        0x4408500080104806ULL,
        0x2014020042200700ULL,
        0x1100004024821800ULL,
        0xa08830040080400ULL,
};

U64 bishop_magic_numbers[64] = {
        0x231002a00084088ULL,
        0x8400802000004ULL,
        0x44501200012088c4ULL,
        0x2008028800020000ULL,
        0x2c08401405280000ULL,
        0x2000084044001401ULL,
        0x8048000026000ULL,
        0x246110002400008ULL,
        0x800420008018010ULL,
        0x40a40004400009ULL,
        0xa880088c00104ULL,
        0x68800000810c008ULL,
        0x901200010220040ULL,
        0x2042a2000030a080ULL,
        0x8300108200000ULL,
        0x5010040002558250ULL,
        0x501a000400000402ULL,
        0xc8100001000000ULL,
        0x580040000010100ULL,
        0x4004010048ULL,
        0x30000400040a0000ULL,
        0x9084040000000ULL,
        0x8080402200800239ULL,
        0x8a0002a190600110ULL,
        0x1042004421004200ULL,
        0x9001250008210008ULL,
        0x85a480900000120ULL,
        0x6004000100001004ULL,
        0x8110888004010ULL,
        0x800020010400200ULL,
        0x1881080240228b44ULL,
        0x4004001080834ULL,
        0x8009005282000428ULL,
        0x1000028000480020ULL,
        0x400060a400a0412ULL,
        0x162100108220002ULL,
        0x241102000008124ULL,
        0x4401014040008100ULL,
        0x880200102070ULL,
        0x802013810442b200ULL,
        0x220300804080420ULL,
        0xa40020019001000ULL,
        0x400009201000210ULL,
        0x8100010800102200ULL,
        0x48c0001020044ULL,
        0x422008180c1020ULL,
        0x8004000302200012ULL,
        0x82000240403950ULL,
        0x20011410048ULL,
        0x901200524402a00ULL,
        0x1a02002820002020ULL,
        0xd002108011480004ULL,
        0x80900000ULL,
        0x510000894120410ULL,
        0x9480800100080200ULL,
        0x201020660000502ULL,
        0x2018a80200021000ULL,
        0x2000100042400ULL,
        0x2a2340004809080ULL,
        0x200100280521010ULL,
        0x4812001860831201ULL,
        0x6580400009003001ULL,
        0x8080024009001042ULL,
        0x4000010004104144ULL,
};

U64 reverse_bitboard(U64 bb) {
    U64 result = 0ULL;
    for (int i = 0; i < 64; i++) {
        if (bb & (1ULL << i)) {
            result |= (1ULL << (63 - i));
        }
    }
    return result;
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
    for (int index = 0; index < occupancy_indicies; index++)
    {
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
        //printf("0x%llxULL,\n",find_magic_number(square, rookRelevantBits[square], false));
        rook_magic_numbers[square] = find_magic_number(square, rookRelevantBits[square], false);

    printf("\n\n\n");
    // loop over 64 board squares
    for (int square = 0; square < 64; square++)
        // init bishop magic numbers
        //printf("0x%llxULL,\n",find_magic_number(square, bischopRelevantBits[square], true));
        bishop_magic_numbers[square] = find_magic_number(square, bischopRelevantBits[square], true);
}