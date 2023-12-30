#include "MagicsTester.h"

U64 rook_magic_numbers[64] = {
        0x4000010062184408ULL,
        0x3000029002460001ULL,
        0x2000840001004802ULL,
        0x1000240068000231ULL,
        0x2100018400481ULL,
        0x100088412002ULL,
        0x814001049421ULL,
        0x201002041840014ULL,
        0x10000800200ULL,
        0x1800400200010101ULL,
        0x240020521100ULL,
        0x2000824040048348ULL,
        0x8001000802100100ULL,
        0x3300088004080ULL,
        0x8001004010042100ULL,
        0x409214090400ULL,
        0x800080040108000ULL,
        0x200040500020010ULL,
        0x211003680100ULL,
        0x200824024000ULL,
        0x3002810810001ULL,
        0x2020041042002000ULL,
        0x8000208040010004ULL,
        0xc002208c4008ULL,
        0x20060800024ULL,
        0x110200801440ULL,
        0x400088c412000020ULL,
        0x80000415331800a1ULL,
        0x1483103000010ULL,
        0x8190e3800104ULL,
        0xa002882005200ULL,
        0x410066000005ULL,
        0x6000141040020000ULL,
        0x14002684000e0082ULL,
        0x508048200020002ULL,
        0xa00044a00010421ULL,
        0x11a20004a008ULL,
        0x221100040240ULL,
        0x4024408200200044ULL,
        0x102083000008eULL,
        0x4000004100860181ULL,
        0x21004400080ULL,
        0x5000080110200000ULL,
        0x2000404100000809ULL,
        0xc001010408200025ULL,
        0x420021021004ULL,
        0x40c000000100ULL,
        0x2020453920420ULL,
        0xc02000441008410ULL,
        0x2002924018400ULL,
        0x210400201800ULL,
        0x101200040801020cULL,
        0x2002089800091ULL,
        0x201002000800ULL,
        0x8000224242004208ULL,
        0x460020004010ULL,
        0x200022302008050ULL,
        0x8400040908108121ULL,
        0x2000200a4014010ULL,
        0x200020008051100ULL,
        0x8500081005000800ULL,
        0x200081022020000ULL,
        0x8500104104200000ULL,
        0x418000a084114400ULL
};

U64 bishop_magic_numbers[64] = {
        0xc0814802104880ULL,
        0x100417008400ULL,
        0x2000021024880200ULL,
        0x4c15010210260080ULL,
        0x8000000042b000ULL,
        0x1e00060021044040ULL,
        0x8d20009084102000ULL,
        0x2000904806188080ULL,
        0x8382104430000ULL,
        0x24210801000900ULL,
        0x100410050910ULL,
        0x10020000a4ULL,
        0x4d8b0002c4014080ULL,
        0x8000885c00a00000ULL,
        0x2838430000000ULL,
        0x180aa20820400081ULL,
        0x4094010242100010ULL,
        0x20030142084002ULL,
        0x105200800400001ULL,
        0x400080100400400ULL,
        0x42000090a180024ULL,
        0x1400420024048000ULL,
        0x1008880800029280ULL,
        0x8120220404002ULL,
        0x40821040c0024910ULL,
        0x142020400404000ULL,
        0x8580020080616ULL,
        0x22002602030000ULL,
        0xd000118081380205ULL,
        0x8000109400020304ULL,
        0x81401040202220cULL,
        0xa080484080048ULL,
        0x501000c405080ULL,
        0x101000a001240ULL,
        0x10008001080041ULL,
        0x220000200004ULL,
        0x419050400020ULL,
        0x20c844800200ULL,
        0x9008040808610402ULL,
        0x10420011020000ULL,
        0x80208210b311850ULL,
        0x4603400002301ULL,
        0x6042008090aaULL,
        0x2144402000040ULL,
        0x2080688000110ULL,
        0x1010084104205520ULL,
        0x10812000802000ULL,
        0x110000660088000ULL,
        0x8260209a14080ULL,
        0x8221001105ULL,
        0x1201c0006000ULL,
        0x2000010c00804001ULL,
        0x4042a000000ULL,
        0x4008072880200144ULL,
        0x2000044868000001ULL,
        0x80608404084200ULL,
        0x100b08a08201c01ULL,
        0x440228020200c0ULL,
        0x914820201000001ULL,
        0xa44042201150042ULL,
        0x2040c0008498002ULL,
        0x4040410228000ULL,
        0x405102242020091ULL,
        0x4014080208060530ULL
};

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
        //if (count_bits((attack_mask * magic_number) & 0xFF00000000000000) < 6) continue;

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
            if(magic_index < (bishop ? 512 : 4096)){
                if (used_attacks[magic_index] == 0ULL) used_attacks[magic_index] = attacks[index]; // init used attacks
                else if (used_attacks[magic_index] != attacks[index]) fail = true; // magic index doesn't work
            }
        }

        // if magic number works
        if (!fail) return magic_number; // return it
    }

    // if magic number doesn't work
    printf("  Magic number fails!\n");
    return 0ULL;
}



// init magic numbers
void MagicsTester::init_magic_numbers(){
    printf("rook\n");
    // loop over 64 board squares
    for (int square = 0; square < 64; square++) {
        // init rook magic numbers
        printf("0x%llxULL,\n",find_magic_number(square, rookRelevantBits[square], false));
        rook_magic_numbers[square] = find_magic_number(square, rookRelevantBits[square], false);
    }

    printf("\n\nbischop\n");
    // loop over 64 board squares
    for (int square = 0; square < 64; square++) {
        // init bishop magic numbers
        printf("0x%llxULL,\n",find_magic_number(square, bischopRelevantBits[square], true));
        bishop_magic_numbers[square] = find_magic_number(square, bischopRelevantBits[square], true);
    }
}