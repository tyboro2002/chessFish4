#include "MagicsTester.h"

U64 rook_magic_numbers[64] = {
        4611864723980305410ULL,
        281477157749761ULL,
        1189231811004465153ULL,
        20829462082625578ULL,
        153966256128161ULL,
        2310909593963889186ULL,
        142111894967329ULL,
        9226187337452552226ULL,
        4426018394624ULL,
        18296363112760576ULL,
        17771626943364595840ULL,
        1191202651262486656ULL,
        580981954854781056ULL,
        327777748020887936ULL,
        270849297951490112ULL,
        617063537021296768ULL,
        4611989486392442897ULL,
        498663151632386ULL,
        360294567326548096ULL,
        1407684125720592ULL,
        27311869102424192ULL,
        2332864813211877408ULL,
        35253095776256ULL,
        90072308246413321ULL,
        9571249827088516ULL,
        18016666319325185ULL,
        3377708318852224ULL,
        4614237985401997312ULL,
        180289429875732480ULL,
        8667318289090748416ULL,
        2305993367437189121ULL,
        153333495920329760ULL,
        748123173486740ULL,
        36040084194068515ULL,
        4648559279031451654ULL,
        13854910843678097536ULL,
        11294466909934080ULL,
        1445690669057446144ULL,
        72199443927007520ULL,
        9295500018964332579ULL,
        4611723401910845700ULL,
        576465152515250178ULL,
        378443656010402304ULL,
        4740179895118764034ULL,
        9896158302208ULL,
        2311050846575677440ULL,
        14992483434398556160ULL,
        145685293318144ULL,
        140810503086720ULL,
        18295882076390144ULL,
        1266723302943232ULL,
        577164474113328130ULL,
        9223512843071031296ULL,
        5838354106795705856ULL,
        577023840861880833ULL,
        337910744974631040ULL,
        180145380959199360ULL,
        14195346661697390848ULL,
        72066407310950656ULL,
        5224184363909939330ULL,
        5260221958582192276ULL,
        2449978573690372224ULL,
        18031991232675840ULL,
        5512406218953752592ULL
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
