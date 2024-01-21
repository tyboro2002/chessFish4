#include "MagicsTester.h"

U64 rook_magic_numbers[64] = {
        1261007982636851332ULL,
        306245393155360832ULL,
        9835879183195310208ULL,
        324276767638422656ULL,
        2738192980078757920ULL,
        504405430303457416ULL,
        144116845933756676ULL,
        612489828495401344ULL,
        11835600558763294729ULL,
        2308165214815670272ULL,
        1315332776668438528ULL,
        140771856748544ULL,
        9270800862496688128ULL,
        2306405967779073160ULL,
        2325546291960086532ULL,
        20407075397976832ULL,
        36064531423707136ULL,
        70643890528260ULL,
        45108564343685152ULL,
        18155686022219778ULL,
        144680337389134024ULL,
        704237264699904ULL,
        144040318468608ULL,
        5332299342279149636ULL,
        11547230056610799936ULL,
        564075239170180ULL,
        252485326151360513ULL,
        4785216339578880ULL,
        2020446078822252548ULL,
        149181763426979876ULL,
        844463584968708ULL,
        5506148089988ULL,
        72127982386282529ULL,
        1170938721697013761ULL,
        282162188267714ULL,
        282643224600584ULL,
        2256200016070656ULL,
        145804055149545488ULL,
        3659192430756352ULL,
        4614927360725268ULL,
        7043630231676026886ULL,
        583858953998614528ULL,
        1481693090684084225ULL,
        1729399849499000960ULL,
        4522291459325956ULL,
        43347164935618568ULL,
        92403036387016705ULL,
        176201109995524ULL,
        144255944358887552ULL,
        148618925381255232ULL,
        594633755910603264ULL,
        3386908398911744ULL,
        2256200008204928ULL,
        220817136409510016ULL,
        306403108665689088ULL,
        4399128716800ULL,
        2287055052816641ULL,
        9227101584625803403ULL,
        5080095566466124035ULL,
        2964695933781249ULL,
        757167824929296430ULL,
        366198979094118401ULL,
        5203346422293136386ULL,
        71468830687362ULL
};

U64 bishop_magic_numbers[64] = {
        3412955027014208ULL,
        867312368475504648ULL,
        659218866550870034ULL,
        2315989304939118764ULL,
        4612260032216563728ULL,
        14432349073107331584ULL,
        2306970704686352448ULL,
        12106521327377195557ULL,
        1157460426381934656ULL,
        2346410916666540576ULL,
        576751581723230740ULL,
        2476065456720ULL,
        90252330187882496ULL,
        71537012048136ULL,
        4611758629182390308ULL,
        4611726292472172548ULL,
        2343103434546481152ULL,
        1176072891000164616ULL,
        20266216174391632ULL,
        155383277578944512ULL,
        1125902064820232ULL,
        612771060279939072ULL,
        1153484457278251008ULL,
        4785084888584192ULL,
        4616194121732727808ULL,
        2310918939542357032ULL,
        1128099198541888ULL,
        18018796589746192ULL,
        154529830950453248ULL,
        9226188986190921984ULL,
        15763698245444125ULL,
        55170080329237512ULL,
        2324147715304329252ULL,
        2273034132296715ULL,
        18150532852737ULL,
        1152923705847316608ULL,
        306245326699102240ULL,
        151022337569980672ULL,
        2595200470683362692ULL,
        1234622373966128648ULL,
        581779626936448ULL,
        37155830839248915ULL,
        294672538826752ULL,
        4719793317670553600ULL,
        1171010704568881168ULL,
        1170940853082923136ULL,
        4612257768886539272ULL,
        117657094349652224ULL,
        4653150662557696ULL,
        2882341153780006912ULL,
        1112531339264ULL,
        577621845256437760ULL,
        1747541928432631808ULL,
        1171289963107721472ULL,
        866102918632439840ULL,
        288799940422436868ULL,
        4620992662836613120ULL,
        9009411373797381ULL,
        4620693359426012160ULL,
        1306045060203234304ULL,
        1732795416151458304ULL,
        42854600113913984ULL,
        4611721786931872768ULL,
        4613423251128058114ULL
};


U64 MagicsTester::find_magic_number(int square, int relevant_bits, bool bishop) {
    // init occupancies
    U64 occupancies[ROOK_ATTACKS];

    // init attack tables
    U64 attacks[ROOK_ATTACKS];

    // init used attacks
    U64 used_attacks[ROOK_ATTACKS];

    // init attack mask for a current piece
    U64 attack_mask = bishop ? bischopMovesONE_OFF[square] : rookMovesONE_OFF[square];

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
            if(magic_index < (bishop ? BISHOP_ATTACKS : ROOK_ATTACKS)){
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
