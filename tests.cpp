#include "tests.h"

using namespace std;

// Utility function to check and report test results
void TestRunner::testResultTrue(bool condition, const std::string& testName) {
    totalTests++;
    if (condition) {
        if (printPassed) std::cout << testName << " - Passed" << std::endl;
        passedTests++;
    }
    else {
        if (printPassed) std::cout << testName << " - Failed" << std::endl;
        failedTests.push_back(testName);
    }
}


// Utility function to check and report test results
void TestRunner::testResultFalse(bool condition, const std::string& testName) {
    totalTests++;
    if (!condition) {
        if (printPassed) std::cout << testName << " - Passed" << std::endl;
        passedTests++;
    }
    else {
        if (printPassed) std::cout << testName << " - Failed" << std::endl;
        failedTests.push_back(testName);
    }
}

void TestRunner::kingMovesGenerator() {
    Board bord{};
    // white no castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, WKING, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_white_king(i, &bord),"");
        //cout << std::bitset<64>(bitmap_white_king(i,&bord)) << endl;
        //cout << bitmap_white_king(i,&bord) << endl;
        std::ostringstream formattedString;
        formattedString << "check white King on square: " << i;
        testResultTrue(bitmap_white_king(i,&bord) == kingMoves[i],formattedString.str());
    }

    // black castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BKING, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_black_king(i, &bord),"");
        //cout << std::bitset<64>(bitmap_black_king(i,&bord)) << endl;
        //cout << bitmap_black_king(i,&bord) << endl;
        std::ostringstream formattedString;
        formattedString << "check black King on square: " << i;
        testResultTrue(bitmap_black_king(i,&bord) == kingMoves[i],formattedString.str());
    }

    // white queenside castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, WROOK, A1);
        addPiece(&bord, WKING, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_white_king(i, &bord),"");
        //cout << std::bitset<64>(bitmap_white_king(i,&bord)) << endl;
        //cout << std::bitset<64>(kingMovesWhiteQueenSide[i]) << endl;
        std::ostringstream formattedString;
        formattedString << "check white King queenside castle on square: " << i;
        testResultTrue(bitmap_white_king(i,&bord) == kingMovesWhiteQueenSide[i],formattedString.str());
    }

    // black queenside castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BROOK, A8);
        addPiece(&bord, BKING, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_black_king(i, &bord),"");
        //cout << std::bitset<64>(bitmap_black_king(i,&bord)) << endl;
        //cout << bitmap_black_king(i,&bord) << endl;
        std::ostringstream formattedString;
        formattedString << "check black King queenside castle on square: " << i;
        testResultTrue(bitmap_black_king(i,&bord) == kingMovesBlackQueenSide[i],formattedString.str());
    }

    // white kingside castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, WROOK, H1);
        addPiece(&bord, WKING, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_white_king(i, &bord),"");
        //cout << std::bitset<64>(bitmap_white_king(i,&bord)) << endl;
        //cout << bitmap_white_king(i,&bord) << endl;
        std::ostringstream formattedString;
        formattedString << "check white King kingside castle on square: " << i;
        testResultTrue(bitmap_white_king(i,&bord) == kingMovesWhiteKingSide[i],formattedString.str());
    }

    // black kingside castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BROOK, H8);
        addPiece(&bord, BKING, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_black_king(i, &bord),"");
        //cout << std::bitset<64>(bitmap_black_king(i,&bord)) << endl;
        //cout << bitmap_black_king(i,&bord) << endl;
        std::ostringstream formattedString;
        formattedString << "check black King kingside castle on square: " << i;
        testResultTrue(bitmap_black_king(i,&bord) == kingMovesBlackKingSide[i],formattedString.str());
    }

    // white both castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, WROOK, H1);
        addPiece(&bord, WROOK, A1);
        addPiece(&bord, WKING, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_white_king(i, &bord),"");
        //cout << std::bitset<64>(bitmap_white_king(i,&bord)) << endl;
        //cout << bitmap_white_king(i,&bord) << endl;
        std::ostringstream formattedString;
        formattedString << "check white King both castle on square: " << i;
        testResultTrue(bitmap_white_king(i,&bord) == kingMovesWhiteBothSide[i],formattedString.str());
    }

    // black both castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BROOK, H8);
        addPiece(&bord, BROOK, A8);
        addPiece(&bord, BKING, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_black_king(i, &bord),"");
        //cout << std::bitset<64>(bitmap_black_king(i,&bord)) << endl;
        //cout << bitmap_black_king(i,&bord) << endl;
        std::ostringstream formattedString;
        formattedString << "check black King both castle on square: " << i;
        testResultTrue(bitmap_black_king(i,&bord) == kingMovesBlackBothSide[i],formattedString.str());
    }

    // white both castle blocked
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, WROOK, H1);
        addPiece(&bord, WROOK, A1);
        addPiece(&bord, WKNIGHT, B1);
        addPiece(&bord, WKNIGHT, G1);
        addPiece(&bord, WKING, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_white_king(i, &bord),"");
        //cout << std::bitset<64>(bitmap_white_king(i,&bord)) << endl;
        //cout << std::bitset<64>(kingMoves[i]) << endl;
        std::ostringstream formattedString;
        formattedString << "check white King both blocked castle on square: " << i;
        testResultTrue(bitmap_white_king(i,&bord) == (kingMoves[i] & (~b1_mask) & (~a1_mask) & (~g1_mask) & (~h1_mask)),formattedString.str());
    }

    // black both castle blocked
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BROOK, H8);
        addPiece(&bord, BROOK, A8);
        addPiece(&bord, BKNIGHT, B8);
        addPiece(&bord, BKNIGHT, G8);
        addPiece(&bord, BKING, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_black_king(i, &bord),"");
        //cout << std::bitset<64>(bitmap_black_king(i,&bord)) << endl;
        //cout << bitmap_black_king(i,&bord) << endl;
        std::ostringstream formattedString;
        formattedString << "check black King both blocked castle on square: " << i;
        testResultTrue(bitmap_black_king(i,&bord) == (kingMoves[i] & (~b8_mask) & (~a8_mask) & (~g8_mask) & (~h8_mask)),formattedString.str());
    }

    //TODO add king in check tests
}

void TestRunner::knightMovesGenerator() {
    Board bord;
    // test white knight
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, WKNIGHT, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_white_knight(i, &bord), "");
        //cout << std::bitset<64>(bitmap_white_knight(i, &bord)) << endl;
        std::ostringstream formattedString;
        formattedString << "check white knight on square: " << i;
        testResultTrue(bitmap_white_knight(i,&bord) == knightMoves[i],formattedString.str());
    }
    // test black knight
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BKNIGHT, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_black_knight(i, &bord), "");
        //cout << std::bitset<64>(bitmap_black_knight(i, &bord)) << endl;
        std::ostringstream formattedString;
        formattedString << "check black knight on square: " << i;
        testResultTrue(bitmap_black_knight(i,&bord) == knightMoves[i],formattedString.str());
    }
    // test white knight nowhere to move
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, WPAWN, (Square)j);
        addPiece(&bord, WKNIGHT, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check white knight nowhere to move on square: " << i;
        testResultTrue(bitmap_white_knight(i,&bord) == 0ULL,formattedString.str());
    }
    // test black knight nowhere to move
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, BPAWN, (Square)j);
        addPiece(&bord, BKNIGHT, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check black knight nowhere to move on square: " << i;
        testResultTrue(bitmap_black_knight(i,&bord) == 0ULL,formattedString.str());
    }
}

void TestRunner::rookMovesGenerator() {
    Board bord;
    // test white rook
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, WROOK, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_white_rook(i, &bord), "");
        //cout << std::bitset<64>(bitmap_white_rook(i, &bord)) << endl;
        std::ostringstream formattedString;
        formattedString << "check white rook on square: " << i;
        testResultTrue(bitmap_white_rook(i,&bord) == rookMoves[i],formattedString.str());
    }
    // test black rook
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BROOK, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_black_rook(i, &bord), "");
        //cout << std::bitset<64>(bitmap_black_rook(i, &bord)) << endl;
        std::ostringstream formattedString;
        formattedString << "check black rook on square: " << i;
        testResultTrue(bitmap_black_rook(i,&bord) == rookMoves[i],formattedString.str());
    }
    // test white rook nowhere to move
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, WPAWN, (Square)j);
        addPiece(&bord, WROOK, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check white rook nowhere to move on square: " << i;
        testResultTrue(bitmap_white_rook(i,&bord) == 0ULL,formattedString.str());
    }
    // test black rook nowhere to move
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, BPAWN, (Square)j);
        addPiece(&bord, BROOK, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check black rook nowhere to move on square: " << i;
        testResultTrue(bitmap_black_rook(i,&bord) == 0ULL,formattedString.str());
    }

    for (int i=0; i<64; i++){
        U64 mask = rookMoves[i];
        setupEmpty(&bord);
        int bits_in_mask = count_bits(mask);
        U64 amount_of_tests = 1 << bits_in_mask;
        for (int test = 0; test<amount_of_tests;test++) {
            U64 occ_test = set_occupancy(test, bits_in_mask, mask);
            bord.black = occ_test;
            U64 fly = rook_attacks_on_the_fly(i, occ_test);
            U64 testMap = bitmap_white_rook(i, &bord);
            std::ostringstream formattedString;
            formattedString << "check rook on square: " << i << " with bitmap: 0b" << std::bitset<64>(occ_test);
            testResultTrue(fly==testMap,formattedString.str());
        }
    }
}

void TestRunner::bischopMovesGenerator() {
    Board bord;
    // test white bischop
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, WBISCHOP, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_white_bishop(i, &bord), "");
        //cout << std::bitset<64>(bitmap_white_bishop(i, &bord)) << endl;
        std::ostringstream formattedString;
        formattedString << "check white bischop on square: " << i;
        testResultTrue(bitmap_white_bishop(i,&bord) == bischopMoves[i],formattedString.str());
    }
    //test black bischop
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BBISCHOP, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_black_bishop(i, &bord), "");
        //cout << std::bitset<64>(bitmap_black_bishop(i, &bord)) << endl;
        std::ostringstream formattedString;
        formattedString << "check black bischop on square: " << i;
        testResultTrue(bitmap_black_bishop(i,&bord) == bischopMoves[i],formattedString.str());
    }
    // test white bischop nowhere to move
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, WPAWN, (Square)j);
        addPiece(&bord, WBISCHOP, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check white bischop nowhere to move on square: " << i;
        testResultTrue(bitmap_white_bishop(i,&bord) == 0ULL,formattedString.str());
    }
    //test black bischop nowhere to move
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, BPAWN, (Square)j);
        addPiece(&bord, BBISCHOP, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check black bischop nowhere to move on square: " << i;
        testResultTrue(bitmap_black_bishop(i,&bord) == 0ULL,formattedString.str());
    }

    for (int i=0; i<64; i++){
        U64 mask = bischopMoves[i];
        setupEmpty(&bord);
        int bits_in_mask = count_bits(mask);
        U64 amount_of_tests = 1 << bits_in_mask;
        for (int test = 0; test<amount_of_tests;test++) {
            U64 occ_test = set_occupancy(test, bits_in_mask, mask);
            bord.black = occ_test;
            U64 fly = bishop_attacks_on_the_fly(i, occ_test);
            U64 testMap = bitmap_white_bishop(i, &bord);
            std::ostringstream formattedString;
            formattedString << "check bishop on square: " << i << " with bitmap: 0b" << std::bitset<64>(occ_test);
            testResultTrue(fly==testMap,formattedString.str());
        }
    }
}

void TestRunner::queenMovesGenerator() {
    Board bord;
    //test white queen
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, WQUEEN, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_white_queen(i, &bord), "");
        //cout << std::bitset<64>(bitmap_white_queen(i, &bord)) << endl;
        std::ostringstream formattedString;
        formattedString << "check white queen on square: " << i;
        testResultTrue(bitmap_white_queen(i,&bord) == queenMoves[i],formattedString.str());
    }
    //test black queen
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BQUEEN, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_black_queen(i, &bord), "");
        //cout << std::bitset<64>(bitmap_black_queen(i, &bord)) << endl;
        std::ostringstream formattedString;
        formattedString << "check black queen on square: " << i;
        testResultTrue(bitmap_black_queen(i,&bord) == queenMoves[i],formattedString.str());
    }
    //test white queen nowhere to move
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, WPAWN, (Square)j);
        addPiece(&bord, WQUEEN, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check white queen nowhere to move on square: " << i;
        testResultTrue(bitmap_white_queen(i,&bord) == 0ULL,formattedString.str());
    }
    //test black queen nowhere to move
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, BPAWN, (Square)j);
        addPiece(&bord, BQUEEN, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check black queen nowhere to move on square: " << i;
        testResultTrue(bitmap_black_queen(i,&bord) == 0ULL,formattedString.str());
    }

    for (int i=0; i<64; i++){
        //U64 mask = queenMoves[i];
        U64 mask = rookMovesONE_OFF[i] | bischopMovesONE_OFF[i];
        setupEmpty(&bord);
        int bits_in_mask = count_bits(mask);
        U64 amount_of_tests = 1 << bits_in_mask;
        for (int test = 0; test<amount_of_tests;test++) {
            U64 occ_test = set_occupancy(test, bits_in_mask, mask);
            bord.black = occ_test;
            U64 fly = bishop_attacks_on_the_fly(i, occ_test)| rook_attacks_on_the_fly(i,occ_test);
            U64 testMap = bitmap_white_queen(i, &bord);
            std::ostringstream formattedString;
            formattedString << "check queen on square: " << i << " with bitmap: 0b" << std::bitset<64>(occ_test);
            testResultTrue(fly==testMap,formattedString.str());
        }
    }
}

void TestRunner::pawnMovesGenerator() {
    Board bord;
    //test white pawn
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, WPAWN, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_white_pawn(i, &bord), "");
        //cout << "0b" << std::bitset<64>(bitmap_white_pawn(i, &bord))  << "," << endl;
        std::ostringstream formattedString;
        formattedString << "check white pawn all empty on square: " << i;
        testResultTrue(bitmap_white_pawn(i, &bord) == whitePawnMovesAllEmpty[i], formattedString.str());
    }
    //test black pawn
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BPAWN, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_black_pawn(i, &bord), "");
        //cout << "0b" << std::bitset<64>(bitmap_black_pawn(i, &bord))  << "," << endl;
        std::ostringstream formattedString;
        formattedString << "check black pawn all empty on square: " << i;
        testResultTrue(bitmap_black_pawn(i,&bord) == blackPawnMovesAllEmpty[i],formattedString.str());
    }
    //test white pawn on a board with all black pawns
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, BPAWN, (Square)j);
        addPiece(&bord, WPAWN, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_white_pawn(i, &bord), "");
        //cout << "0b" << std::bitset<64>(bitmap_white_pawn(i, &bord))  << "," << endl;
        //cout << "0b" << std::bitset<64>(whitePawnAttacks[i])  << "," << endl;
        std::ostringstream formattedString;
        formattedString << "check white pawn on a board with all black pawns on square: " << i;
        testResultTrue(bitmap_white_pawn(i, &bord) == whitePawnAttacks[i], formattedString.str());
    }
    //test black pawn on a board with all white pawns
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, WPAWN, (Square)j);
        addPiece(&bord, BPAWN, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_black_pawn(i, &bord), "");
        //cout << "0b" << std::bitset<64>(bitmap_black_pawn(i, &bord))  << "," << endl;
        std::ostringstream formattedString;
        formattedString << "check black pawn on a board with all white pawns on square: " << i;
        testResultTrue(bitmap_black_pawn(i,&bord) == blackPawnAttacks[i],formattedString.str());
    }

    //test white pawn on a board with all white pawns
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, WPAWN, (Square)j);
        addPiece(&bord, WPAWN, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_white_pawn(i, &bord), "");
        //cout << "0b" << std::bitset<64>(bitmap_white_pawn(i, &bord))  << "," << endl;
        //cout << "0b" << std::bitset<64>(whitePawnAttacks[i])  << "," << endl;
        std::ostringstream formattedString;
        formattedString << "check white pawn on a board with all white pawns on square: " << i;
        testResultTrue(bitmap_white_pawn(i, &bord) == 0ULL, formattedString.str());
    }
    //test black pawn on a board with all black pawns
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, BPAWN, (Square)j);
        addPiece(&bord, BPAWN, (Square)i);
        //printBoard(&bord);
        //printBitBoard(bitmap_black_pawn(i, &bord), "");
        //cout << "0b" << std::bitset<64>(bitmap_black_pawn(i, &bord))  << "," << endl;
        std::ostringstream formattedString;
        formattedString << "check black pawn on a board with all black pawns on square: " << i;
        testResultTrue(bitmap_black_pawn(i,&bord) == 0ULL,formattedString.str());
    }
    //TODO add en passent tests
}

int TestRunner::runAutomatedTestCases() {
    // Run the tests
    kingMovesGenerator();
    knightMovesGenerator();
    rookMovesGenerator();
    bischopMovesGenerator();
    queenMovesGenerator();
    pawnMovesGenerator();

    // Print a summary
    std::cout << std::endl << std::endl << "Summary: " << passedTests << " out of " << totalTests << " tests passed." << std::endl;

    // Print the list of failed tests
    if (!failedTests.empty()) {
        std::cout << std::endl << std::endl << "Failed Tests:" << std::endl;
        for (const std::string& testName : failedTests) {
            std::cout << "  " << testName << std::endl;
        }
    }
    return 0;
}

/*
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
            U64 magic = get_bishop_attacks(square, occ_test);

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
 */
/**************************
 * old tests
 * below
 */


void king_danger_squares_test() {
    Board bord;
    Move move;
    MOVELIST moveList;
    // Clear move list
    moveList.count = 0;   // set each field for each move

    setup(&bord);
    setupEmpty(&bord);
    printBoard(&bord);
    addPiece(&bord, BKING, E7);
    addPiece(&bord, BROOK, E3);
    addPiece(&bord, WKING, E1);
    addPiece(&bord, WROOK, E4);
    addPiece(&bord, WROOK, H7);
    printBoard(&bord);
    printBitBoard(all_black_attacks(&bord), "black moves");
    printBitBoard(bitmap_black_king(E7, &bord), "black king moves");
    printBitBoard(black_checking_pieces(&bord), "black checking pieces");

    black_king_moves(E7, &moveList, &bord);

    cout << endl;
}

void path_test() {
    printBitBoard(squaresBetweenBitmap(A8, H1), "between A8 H1");
    printBitBoard(squaresBetweenBitmap(C7, C2), "between C7 C2");
    printBitBoard(squaresBetweenBitmap(H2, D6), "between H2 D6");
    //printBitBoard(squaresBetweenBitmap(H3, D6), "between H3 D6");
    printBitBoard(squaresBetweenBitmap(H3, H3), "between H3 H3");
}

void checking_test() {
    Board bord;
    Move move;
    MOVELIST moveList;
    // Clear move list
    moveList.count = 0;   // set each field for each move

    setup(&bord);
    setupEmpty(&bord);
    printBoard(&bord);
    addPiece(&bord, BKING, E7);
    addPiece(&bord, BROOK, E3);
    addPiece(&bord, WKING, E1);
    addPiece(&bord, WROOK, E4);
    addPiece(&bord, WROOK, H7);
    addPiece(&bord, WKNIGHT, G6);
    printBoard(&bord);
    printBitBoard(black_checking_bitmap(&bord), "black checking");

    black_king_moves(E7, &moveList, &bord);

    cout << endl;
}

void move_test() {
    Board bord;
    Move move;
    MOVELIST moveList;
    PositionTracker positionTracker;
    // Clear move list
    moveList.count = 0;   // set each field for each move

    setup(&bord);
    printBoard(&bord);

    move.src = F8;
    move.dst = B4;
    move.special = NOT_SPECIAL;
    makeMove(&bord, &move, &positionTracker);
    printBoard(&bord);
}

void move_test_halfmove() {
    Board bord;
    Move move;
    MOVELIST moveList;
    PositionTracker positionTracker;
    // Clear move list
    moveList.count = 0;   // set each field for each move

    setup(&bord);
    printBoard(&bord);

    for (int i = 0; i < 10; i++) {
        GenLegalMoveList(&moveList, &bord, &positionTracker);
        makeRandomMove(&bord, &moveList, &positionTracker);
        printBoard(&bord);
        cout << bord.halfmoveClock << endl;
    }
}

void move_test_check() {
    Board bord;
    Move move;
    MOVELIST moveList;
    // Clear move list
    moveList.count = 0;   // set each field for each move

    setupEmpty(&bord);
    addPiece(&bord, WKING, A4);
    addPiece(&bord, BROOK, F4);
    addPiece(&bord, BKING, D6);
    addPiece(&bord, WKNIGHT, D3);
    printBoard(&bord);
    printBitBoard(all_white_attacks(&bord), "all white attacks");
}

void randomMoveTest() {
    Board bord;
    Move move;
    MOVELIST moveList;
    PositionTracker positionTracker;
    // Clear move list
    moveList.count = 0;   // set each field for each move
    setup(&bord);
    for (int i = 0; i < 7; i++) {
        makeRandomMove(&bord, &moveList, &positionTracker);
    }
}

void fen_test() {
    Board bord;
    setupEmpty(&bord);
    std::string fen = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
    readInFen(&bord, &fen);
}

void legalMoveTest() {
    Board bord;
    Move move;
    MOVELIST moveList;
    // Clear move list
    moveList.count = 0;   // set each field for each move
    PositionTracker positionTracker;

    /*
    setupEmpty(&bord);
    std::string fen = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";
    readInFen(&bord, &fen);

    //GenLegalMoveList(&moveList, &bord);
    //cout << (moveList.count == 22) << endl;

    setupEmpty(&bord);
    fen = "rnb1kbnr/pp1p1ppp/8/2p1Q3/2q1P3/8/PPPP1PPP/RNB1KB1R b KQkq - 1 2";
    readInFen(&bord, &fen);

    GenLegalMoveList(&moveList, &bord);

    */
    setupEmpty(&bord);
    std::string fen = "r1bq2r1/b4pk1/p1pp1p2/1p2pPQ1/1P2P1PB/3P4/1PP3P1/R3K2R b - - 0 1";
    readInFen(&bord, &fen);
    GenLegalMoveList(&moveList, &bord, &positionTracker);
    cout << endl;
}

/*
* further tests are automatic tests
*/

bool inCheckTest() {
    Board bord;
    Move move;
    setupEmpty(&bord);
    std::string fen;
    // white
    // in check tests
    fen = "RNBQKBNR/PPPP1PPP/8/8/8/4q3/pppppppp/rnb1kbnr w KQkq - 0 1";
    readInFen(&bord, &fen);
    if (!inCheck(&bord)) return false;

    // not in check tests
    fen = "R1B1KBR1/PPbP1N1P/6P1/3p4/6p1/8/ppp1pp1p/rn2kb1r w KQq - 0 1";
    readInFen(&bord, &fen);
    if (inCheck(&bord)) return false;


    // black
    // in check tests
    fen = "rnbqkbnr/pppp1ppp/8/8/8/4Q3/PPPPPPPP/RNB1KBNR b KQkq - 0 1";
    readInFen(&bord, &fen);
    if (!inCheck(&bord)) return false;

    fen = "4k2R/8/4K3/8/8/8/8/8 b - - 0 1";
    readInFen(&bord, &fen);
    if (!inCheck(&bord)) return false;

    fen = "r1b1kbr1/ppBp1n1p/6p1/3P4/6P1/4Q3/PPP1PP1P/RN2KB1R b KQq - 0 1";
    readInFen(&bord, &fen);
    if (!inCheck(&bord)) return false;

    // not in check tests
    fen = "r1b1kbr1/ppBp1n1p/6p1/3P4/6P1/8/PPP1PP1P/RN2KB1R b KQq - 0 1";
    readInFen(&bord, &fen);
    if (inCheck(&bord)) return false;

    fen = "r1b1kbr1/ppBp1n1p/Q5pQ/Q6Q/Q6Q/Q6Q/P3P3/RN2KB1R b - - 0 1";
    readInFen(&bord, &fen);
    if (inCheck(&bord)) return false;

    fen = "r1bqkbr1/pprp1n1p/Q2PN1pQ/Q3R2B/Q6Q/Q6Q/P3P3/1N2K2R b - - 0 1";
    readInFen(&bord, &fen);
    if (inCheck(&bord)) return false;

    return true;
}


bool mateInOneTest() {
    Board bord;
    Move move;
    Move moveOut;
    TranspositionTable transpositionTable;
    PositionTracker positionTracker;
    setupEmpty(&bord);
    std::string fen = "4kb1r/p2ppppp/8/8/8/8/P1PPPPPP/RQ2KB1R w - - 0 1";
    readInFen(&bord, &fen);

    MOVELIST moveList;
    // Clear move list
    moveList.count = 0;   // set each field for each move
    GenLegalMoveList(&moveList, &bord, &positionTracker);

    minimax_root(&bord, 1, true, &moveOut, &moveList, &transpositionTable, &positionTracker);
    cout << "the minimax engine selected: " << moveToString(&moveOut) << " out of " << moveList.count << " moves and it was located at position: " << findMoveIndex(&moveList, &moveOut) << endl;
    move.src = B1;
    move.dst = B8;
    move.capture = -52;
    move.special = NOT_SPECIAL;
    makeMove(&bord, &moveOut, &positionTracker);
    //printBoard(&bord);
    return move == moveOut;
}

bool mateInTwoTest() {
    Board bord;
    Move move;
    Move moveOut;
    TranspositionTable transpositionTable;
    PositionTracker positionTracker;
    setupEmpty(&bord);
    std::string fen = "r1bq2r1/b4pk1/p1pp1p2/1p2pP2/1P2P1PB/3P4/1PP3P1/R1Q1K2R w - - 0 1";
    readInFen(&bord, &fen);
    //printBoard(&bord);
    int depth = 3; // set the depth for the minimax
    MOVELIST moveList;
    // Clear move list

    //white move
    moveList.count = 0;   // set each field for each move
    GenLegalMoveList(&moveList, &bord, &positionTracker);
    minimax_root(&bord, depth, true, &moveOut, &moveList, &transpositionTable, &positionTracker);
    cout << "the minimax engine selected: " << moveToString(&moveOut) << " out of " << moveList.count << " moves and it was located at position: " << findMoveIndex(&moveList, &moveOut) << endl;
    move.src = C1;
    move.dst = H6;
    move.capture = -52;
    move.special = NOT_SPECIAL;
    makeMove(&bord, &moveOut, &positionTracker);
    //printBoard(&bord);
    if (!(move == moveOut)) {
        return false;
    }

    //black move
    moveList.count = 0;   // set each field for each move
    GenLegalMoveList(&moveList, &bord,&positionTracker);
    minimax_root(&bord, depth, true, &moveOut, &moveList, &transpositionTable, &positionTracker);
    cout << "the minimax engine selected: " << moveToString(&moveOut) << " out of " << moveList.count << " moves and it was located at position: " << findMoveIndex(&moveList, &moveOut) << endl;
    move.src = G7;
    move.dst = H6;
    move.capture = H6;
    move.special = NOT_SPECIAL;
    makeMove(&bord, &moveOut, &positionTracker);
    //printBoard(&bord);
    if (!(move == moveOut)) {
        return false;
    }


    //white move
    moveList.count = 0;   // set each field for each move
    GenLegalMoveList(&moveList, &bord, &positionTracker);
    minimax_root(&bord, depth, true, &moveOut, &moveList, &transpositionTable, &positionTracker);
    cout << "the minimax engine selected: " << moveToString(&moveOut) << " out of " << moveList.count << " moves and it was located at position: " << findMoveIndex(&moveList, &moveOut) << endl;
    move.src = H4;
    move.dst = F6;
    move.capture = F6;
    move.special = NOT_SPECIAL;
    makeMove(&bord, &moveOut,&positionTracker);
    //printBoard(&bord);
    if (!(move == moveOut)) {
        return false;
    }
    return move == moveOut;
}

bool mateInThreeTest() {
    Board bord;
    Move move;
    Move moveOut;
    TranspositionTable transpositionTable;
    PositionTracker positionTracker;
    setupEmpty(&bord);
    std::string fen = "r3k2r/ppp2Npp/1b5n/4p2b/2B1P2q/BQP2P2/P5PP/RN5K w kq - 1 0";
    readInFen(&bord, &fen);
    //printBoard(&bord);
    int depth = 5; // set the depth for the minimax
    MOVELIST moveList;
    // Clear move list

    //white move
    moveList.count = 0;   // set each field for each move
    GenLegalMoveList(&moveList, &bord, &positionTracker);
    minimax_root(&bord, depth, true, &moveOut, &moveList, &transpositionTable, &positionTracker);
    cout << "the minimax engine selected: " << moveToString(&moveOut) << " out of " << moveList.count << " moves and it was located at position: " << findMoveIndex(&moveList, &moveOut) << endl;
    move.src = C4;
    move.dst = B5;
    move.capture = -52;
    move.special = NOT_SPECIAL;
    makeMove(&bord, &moveOut,&positionTracker);
    //printBoard(&bord);
    if (!(move == moveOut)) {
        return false;
    }

    //black move
    moveList.count = 0;   // set each field for each move
    GenLegalMoveList(&moveList, &bord, &positionTracker);
    minimax_root(&bord, depth, true, &moveOut, &moveList, &transpositionTable, &positionTracker);
    cout << "the minimax engine selected: " << moveToString(&moveOut) << " out of " << moveList.count << " moves and it was located at position: " << findMoveIndex(&moveList, &moveOut) << endl;
    move.src = C7;
    move.dst = C6;
    move.capture = -52;
    move.special = NOT_SPECIAL;
    makeMove(&bord, &moveOut, &positionTracker);
    //printBoard(&bord);
    if (!(move == moveOut)) {
        return false;
    }


    //white move
    moveList.count = 0;   // set each field for each move
    GenLegalMoveList(&moveList, &bord, &positionTracker);
    minimax_root(&bord, depth, true, &moveOut, &moveList, &transpositionTable, &positionTracker);
    cout << "the minimax engine selected: " << moveToString(&moveOut) << " out of " << moveList.count << " moves and it was located at position: " << findMoveIndex(&moveList, &moveOut) << endl;
    move.src = B3;
    move.dst = E6;
    move.capture = -52;
    move.special = NOT_SPECIAL;
    makeMove(&bord, &moveOut, &positionTracker);
    //printBoard(&bord);
    if (!(move == moveOut)) {
        return false;
    }

    //black move
    moveList.count = 0;   // set each field for each move
    GenLegalMoveList(&moveList, &bord, &positionTracker);
    minimax_root(&bord, depth, true, &moveOut, &moveList, &transpositionTable, &positionTracker);
    cout << "the minimax engine selected: " << moveToString(&moveOut) << " out of " << moveList.count << " moves and it was located at position: " << findMoveIndex(&moveList, &moveOut) << endl;
    move.src = H4;
    move.dst = E7;
    move.capture = -52;
    move.special = NOT_SPECIAL;
    makeMove(&bord, &moveOut, &positionTracker);
    //printBoard(&bord);
    if (!(move == moveOut)) {
        return false;
    }

    //white move
    moveList.count = 0;   // set each field for each move
    GenLegalMoveList(&moveList, &bord, &positionTracker);
    minimax_root(&bord, depth, true, &moveOut, &moveList, &transpositionTable, &positionTracker);
    cout << "the minimax engine selected: " << moveToString(&moveOut) << " out of " << moveList.count << " moves and it was located at position: " << findMoveIndex(&moveList, &moveOut) << endl;
    move.src = E6;
    move.dst = E7;
    move.capture = E7;
    move.special = NOT_SPECIAL;
    makeMove(&bord, &moveOut, &positionTracker);
    //printBoard(&bord);
    if (!(move == moveOut)) {
        return false;
    }
    return move == moveOut;
}

bool runAutomatedTests() {
    // run inCheck test
    cout << "in Check test" << endl;
    if (inCheckTest()) {
        cout << "SUCCEED" << endl;
    }
    else {
        cout << "FAILED" << endl;
        return false;
    }
    // run mate in one test
    cout << "mate in one test" << endl;
    if (mateInOneTest()) {
        cout << "SUCCEED" << endl;
    }
    else {
        cout << "FAILED" << endl;
        return false;
    }
    // run mate in two test
    cout << "mate in two test" << endl;
    if (mateInTwoTest()) {
        cout << "SUCCEED" << endl;
    }
    else {
        cout << "FAILED" << endl;
        return false;
    }
    // run mate in three test
    cout << "mate in three test" << endl;
    if (mateInThreeTest()) {
        cout << "SUCCEED" << endl;
    }
    else {
        cout << "FAILED" << endl;
        return false;
    }
    return true;
}

bool runAutomatedTestsSilent() {
    if (!inCheckTest()) {
        cout << "check test FAILED" << endl;
        return false;
    }
    if (!mateInOneTest()) {
        cout << "mate in one test FAILED" << endl;
        return false;
    }
    if (!mateInTwoTest()){
        cout << "mate in two test FAILED" << endl;
        return false;
    }
    if (!mateInThreeTest()){
        cout << "mate in three test FAILED" << endl;
        return false;
    }
    return true;
}