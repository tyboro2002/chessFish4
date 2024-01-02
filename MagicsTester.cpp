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
        3412955027014208ULL,
        8933607770624ULL,
        4685995447202349186ULL,
        144115192639394304ULL,
        9381213528093919238ULL,
        144348289943244824ULL,
        108651548648932352ULL,
        12106521327377195557ULL,
        1134842033078820ULL,
        19175491412033664ULL,
        44057908871936ULL,
        1153203048336722016ULL,
        2317102010498024448ULL,
        6359082819893854208ULL,
        362847695667200ULL,
        2597455491759079424ULL,
        74380866438042112ULL,
        9414776133582464000ULL,
        18018830918877312ULL,
        39584868082688ULL,
        2305843147258730496ULL,
        2305856204494211072ULL,
        1128133323916288ULL,
        4661998306461824ULL,
        1196827133116480ULL,
        1223765031911552ULL,
        9223662310208963072ULL,
        2251988794360064ULL,
        13198443940096ULL,
        563508434436128ULL,
        8072711136744834048ULL,
        9226063744475340800ULL,
        79714895561728ULL,
        1972524464243712ULL,
        563508433912064ULL,
        145135543263232ULL,
        141291673485314ULL,
        74771756753920ULL,
        2306973328676490240ULL,
        9187528029504000ULL,
        585758772987757568ULL,
        1127145456534656ULL,
        281483835114496ULL,
        4612813741558333442ULL,
        145169928176129ULL,
        1125934268942416ULL,
        1196277375304704ULL,
        9243642908156101632ULL,
        6919822609988456448ULL,
        2306412590664124416ULL,
        13835130657443414288ULL,
        1173599814786ULL,
        4415234769408ULL,
        36046393504235520ULL,
        2256249534152740ULL,
        36688573805757456ULL,
        4620992662836613120ULL,
        39407237890310146ULL,
        9293081941655552ULL,
        1154612622218231832ULL,
        291612484723277824ULL,
        4504776484061196ULL,
        433053688438128642ULL,
        4613423251128058114ULL
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
    //printf("%u\n",magicNumberGenerator.getState());
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

void MagicsTester::resetMagicSeed() {
    magicNumberGenerator.resetState();
}
