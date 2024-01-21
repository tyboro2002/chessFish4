#include "tests.h"

#define REMOVE_CASTELS()    bord.whiteKingsideCastle = 0; \
                            bord.blackKingsideCastle = 0; \
                            bord.whiteQueensideCastle = 0; \
                            bord.blackQueensideCastle = 0; \
using namespace std;

// Function to get the index of the least significant 1-bit
static inline int get_ls1b_index_tests(U64 bitboard){
    return bitboard ? __builtin_ctzll(bitboard) : -1;
}

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
        std::ostringstream formattedString;
        formattedString << "check white King on square: " << i;
        testResultTrue(bitmap_white_king(i,&bord) == kingMoves[i],formattedString.str());
    }

    // black castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BKING, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check black King on square: " << i;
        testResultTrue(bitmap_black_king(i,&bord) == kingMoves[i],formattedString.str());
    }

    // white queenside castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, WROOK, A1);
        addPiece(&bord, WKING, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check white King queenside castle on square: " << i;
        testResultTrue(bitmap_white_king(i,&bord) == kingMovesWhiteQueenSide[i],formattedString.str());
    }

    // black queenside castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BROOK, A8);
        addPiece(&bord, BKING, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check black King queenside castle on square: " << i;
        testResultTrue(bitmap_black_king(i,&bord) == kingMovesBlackQueenSide[i],formattedString.str());
    }

    // white kingside castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, WROOK, H1);
        addPiece(&bord, WKING, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check white King kingside castle on square: " << i;
        testResultTrue(bitmap_white_king(i,&bord) == kingMovesWhiteKingSide[i],formattedString.str());
    }

    // black kingside castle
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BROOK, H8);
        addPiece(&bord, BKING, (Square)i);
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
        std::ostringstream formattedString;
        formattedString << "check white knight on square: " << i;
        testResultTrue(bitmap_white_knight(i,&bord) == knightMoves[i],formattedString.str());
    }
    // test black knight
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BKNIGHT, (Square)i);
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
        std::ostringstream formattedString;
        formattedString << "check white rook on square: " << i;
        testResultTrue(bitmap_white_rook(i,&bord) == rookMoves[i],formattedString.str());
    }
    // test black rook
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BROOK, (Square)i);
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
        std::ostringstream formattedString;
        formattedString << "check white bischop on square: " << i;
        testResultTrue(bitmap_white_bishop(i,&bord) == bischopMoves[i],formattedString.str());
    }
    //test black bischop
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BBISCHOP, (Square)i);
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
        std::ostringstream formattedString;
        formattedString << "check white queen on square: " << i;
        testResultTrue(bitmap_white_queen(i,&bord) == queenMoves[i],formattedString.str());
    }
    //test black queen
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BQUEEN, (Square)i);
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
        std::ostringstream formattedString;
        formattedString << "check white pawn all empty on square: " << i;
        testResultTrue(bitmap_white_pawn(i, &bord) == whitePawnMovesAllEmpty[i], formattedString.str());
    }
    //test black pawn
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        addPiece(&bord, BPAWN, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check black pawn all empty on square: " << i;
        testResultTrue(bitmap_black_pawn(i,&bord) == blackPawnMovesAllEmpty[i],formattedString.str());
    }
    //test white pawn on a board with all black pawns
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, BPAWN, (Square)j);
        addPiece(&bord, WPAWN, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check white pawn on a board with all black pawns on square: " << i;
        testResultTrue(bitmap_white_pawn(i, &bord) == whitePawnAttacks[i], formattedString.str());
    }
    //test black pawn on a board with all white pawns
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, WPAWN, (Square)j);
        addPiece(&bord, BPAWN, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check black pawn on a board with all white pawns on square: " << i;
        testResultTrue(bitmap_black_pawn(i,&bord) == blackPawnAttacks[i],formattedString.str());
    }

    //test white pawn on a board with all white pawns
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, WPAWN, (Square)j);
        addPiece(&bord, WPAWN, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check white pawn on a board with all white pawns on square: " << i;
        testResultTrue(bitmap_white_pawn(i, &bord) == 0ULL, formattedString.str());
    }
    //test black pawn on a board with all black pawns
    for (int i = 0; i < 64; i++) {
        setupEmpty(&bord);
        for (int j = 0; j<64;j++) addPiece(&bord, BPAWN, (Square)j);
        addPiece(&bord, BPAWN, (Square)i);
        std::ostringstream formattedString;
        formattedString << "check black pawn on a board with all black pawns on square: " << i;
        testResultTrue(bitmap_black_pawn(i,&bord) == 0ULL,formattedString.str());
    }
    //TODO add en passent tests
}

void TestRunner::GenerateActions() {
    ActionList testActionList;
    testActionList.addMove(G1,H3);
    testActionList.addMove(G1,F3);
    testActionList.addMove(B1,C3);
    testActionList.addMove(B1,A3);
    testActionList.addMove(H2,H3);
    testActionList.addMove(H2,H4);
    testActionList.addMove(G2,G3);
    testActionList.addMove(G2,G4);
    testActionList.addMove(F2,F3);
    testActionList.addMove(F2,F4);
    testActionList.addMove(E2,E3);
    testActionList.addMove(E2,E4);
    testActionList.addMove(D2,D3);
    testActionList.addMove(D2,D4);
    testActionList.addMove(C2,C3);
    testActionList.addMove(C2,C4);
    testActionList.addMove(B2,B3);
    testActionList.addMove(B2,B4);
    testActionList.addMove(A2,A4);
    testActionList.addMove(A2,A3);
    Board bord;
    setup(&bord);
    ActionList actionList;
    getAllMoves(&bord,&actionList);
    testResultTrue(actionList.count == 20,"correct amount of actions from starting position as white");
    testResultTrue(areActionListsEqual(actionList,testActionList),"actions from starting position as white are the correct actions");

    testActionList.count = 0;
    actionList.count = 0;
    testActionList.addMove(E1,G1);
    testActionList.addMove(E1,F2);
    testActionList.addMove(A2, A3);
    testActionList.addMove(B2, B3);
    testActionList.addMove(C2, C3);
    testActionList.addMove(G2, G3);
    testActionList.addMove(H2, H3);
    testActionList.addMove(A2, A4);
    testActionList.addMove(B2, B4);
    testActionList.addMove(G2, G4);
    testActionList.addMove(H2, H4);
    testActionList.addMove(D7, C8, Promote_Queen);
    testActionList.addMove(D7, C8, Promote_Rook);
    testActionList.addMove(D7, C8, Promote_Bishop);
    testActionList.addMove(D7, C8, Promote_Knight);
    testActionList.addMove(B1, D2);
    testActionList.addMove(B1, A3);
    testActionList.addMove(B1, C3);
    testActionList.addMove(E2, G1);
    testActionList.addMove(E2, C3);
    testActionList.addMove(E2, G3);
    testActionList.addMove(E2, D4);
    testActionList.addMove(E2, F4);
    testActionList.addMove(C1, D2);
    testActionList.addMove(C1, E3);
    testActionList.addMove(C1, F4);
    testActionList.addMove(C1, G5);
    testActionList.addMove(C1, H6);
    testActionList.addMove(C4, B3);
    testActionList.addMove(C4, D3);
    testActionList.addMove(C4, B5);
    testActionList.addMove(C4, D5);
    testActionList.addMove(C4, A6);
    testActionList.addMove(C4, E6);
    testActionList.addMove(C4, F7);
    testActionList.addMove(H1, F1);
    testActionList.addMove(H1, G1);
    testActionList.addMove(D1, D2);
    testActionList.addMove(D1, D3);
    testActionList.addMove(D1, D4);
    testActionList.addMove(D1, D5);
    testActionList.addMove(D1, D6);
    testActionList.addMove(E1, F1);
    testActionList.addMove(E1, D2);
    setupEmpty(&bord);
    readInFen(&bord,"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 ");
    getAllMoves(&bord,&actionList);
    testResultTrue(actionList.count == 44,"correct amount of actions from rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 position as white");
    testResultTrue(areActionListsEqual(actionList,testActionList),"actions from rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 position as white are the correct actions");

}

void TestRunner::testGeneralPerftResultst() {
    Board bord;
    setup(&bord);
    bool printPercent = true;

    testResultTrue(generalPerft(&bord,0,printPercent,0,0ULL) == 1, "perft depth 0, fen: startpos");
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 20, "perft depth 1, fen: startpos");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 400, "perft depth 2, fen: startpos");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 8'902, "perft depth 3, fen: startpos");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 197'281, "perft depth 4, fen: startpos");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 4'865'609, "perft depth 5, fen: startpos");
    testResultTrue(generalPerft(&bord,6,printPercent,0,0ULL) == 119'060'324, "perft depth 6, fen: startpos");
    //testResultTrue(generalPerft(&bord,7,printPercent,0,0ULL) == 3'195'901'860, "perft depth 7, fen: startpos");
    //testResultTrue(generalPerft(&bord,8,printPercent,0,0ULL) == 84'998'978'956, "perft depth 8, fen: startpos");
    //testResultTrue(generalPerft(&bord,9,printPercent,0,0ULL) == 2'439'530'234'167, "perft depth 9, fen: startpos");


    char* fen;

    setupEmpty(&bord);
    REMOVE_CASTELS()
    fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0";
    readInFen(&bord, fen);
    printFancyBoard(&bord);
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 48, "perft depth 1, fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 2039, "perft depth 2, fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 97'862, "perft depth 3, fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 4'085'603, "perft depth 4, fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 193'690'690, "perft depth 5, fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    testResultTrue(generalPerft(&bord,6,printPercent,0,0ULL) == 8'031'647'685, "perft depth 6, fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");

    setupEmpty(&bord);
    REMOVE_CASTELS()
    fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0";
    readInFen(&bord, fen);
    printFancyBoard(&bord);
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 14, "perft depth 1, fen: 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 191, "perft depth 2, fen: 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 2'812, "perft depth 3, fen: 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 43'238, "perft depth 4, fen: 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 674'624, "perft depth 5, fen: 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
    testResultTrue(generalPerft(&bord,6,printPercent,0,0ULL) == 11'030'083, "perft depth 6, fen: 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
    testResultTrue(generalPerft(&bord,7,printPercent,0,0ULL) == 178'633'661, "perft depth 7, fen: 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
    testResultTrue(generalPerft(&bord,8,printPercent,0,0ULL) == 3'009'794'393, "perft depth 8, fen: 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");

    setupEmpty(&bord);
    REMOVE_CASTELS()
    fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    readInFen(&bord, fen);
    printFancyBoard(&bord);
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 6, "perft depth 1, fen: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 264, "perft depth 2, fen: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 9'467, "perft depth 3, fen: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 422'333, "perft depth 4, fen: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 15'833'292, "perft depth 5, fen: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    testResultTrue(generalPerft(&bord,6,printPercent,0,0ULL) == 706'045'033, "perft depth 6, fen: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

    setupEmpty(&bord);
    REMOVE_CASTELS()
    fen = "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ";
    readInFen(&bord, fen);
    printFancyBoard(&bord);
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 6, "perft depth 1, fen: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 264, "perft depth 2, fen: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 9'467, "perft depth 3, fen: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 422'333, "perft depth 4, fen: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 15'833'292, "perft depth 5, fen: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");
    testResultTrue(generalPerft(&bord,6,printPercent,0,0ULL) == 706'045'033, "perft depth 6, fen: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");

    setupEmpty(&bord);
    REMOVE_CASTELS()
    fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ";
    readInFen(&bord, fen);
    printFancyBoard(&bord);
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 44, "perft depth 1, fen: rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 1'486, "perft depth 2, fen: rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 62'379, "perft depth 3, fen: rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 2'103'487, "perft depth 4, fen: rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 89'941'194, "perft depth 5, fen: rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");

    setupEmpty(&bord);
    REMOVE_CASTELS()
    fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ";
    readInFen(&bord, fen);
    printFancyBoard(&bord);
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 46, "perft depth 1, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 2'079, "perft depth 2, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 89'890, "perft depth 3, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 3'894'594, "perft depth 4, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 164'075'551, "perft depth 5, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    testResultTrue(generalPerft(&bord,6,printPercent,0,0ULL) == 6'923'051'137, "perft depth 6, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    //testResultTrue(generalPerft(&bord,7,printPercent,0,0ULL) == 287'188'994'746, "perft depth 7, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    //testResultTrue(generalPerft(&bord,8,printPercent,0,0ULL) == 11'923'589'843'526, "perft depth 8, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    //testResultTrue(generalPerft(&bord,9,printPercent,0,0ULL) == 490'154'852'788'714, "perft depth 9, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");




    /*
    //cout << detailedPerft(&bord,9) << endl;


    Action action;
    action.src = D2;
    action.dst = D4;
    movePiece(&bord,&action);

    //printBoard(&bord);
    cout << detailedPerft(&bord,8) << endl;


    action.src = G8;
    action.dst = F6;
    movePiece(&bord,&action);

    //printBoard(&bord);
    //cout << detailedPerft(&bord,7) << endl;


    action.src = D1;
    action.dst = D3;
    movePiece(&bord,&action);

    //printBoard(&bord);
    //cout << detailedPerft(&bord,6) << endl;


    action.src = F6;
    action.dst = E4;
    movePiece(&bord,&action);

    //printBoard(&bord);
    //cout << detailedPerft(&bord,5) << endl;


    action.src = C1;
    action.dst = E3;
    movePiece(&bord,&action);

    //printBoard(&bord);
    //cout << detailedPerft(&bord,4) << endl;


    action.src = E4;
    action.dst = D2;
    movePiece(&bord,&action);

    //printBoard(&bord);
    //cout << detailedPerft(&bord,3) << endl;


    action.src = B1;
    action.dst = C3;
    movePiece(&bord,&action);

    //printBoard(&bord);
    //cout << detailedPerft(&bord,2) << endl;


    action.src = H7;
    action.dst = H5;
    movePiece(&bord,&action);

    printBoard(&bord);
    printBitBoard(calculateDanger(&bord,B1),"B1 danger squares");
    printBitBoard(calculateDanger(&bord,C1),"C1 danger squares");
    printBitBoard(calculateDanger(&bord,D1),"D1 danger squares");
    printBitBoard(~c1_mask, "all danger");
    cout << ((get_ls1b_index_tests(bord.white & bord.king) == E1) ? "white king is at E1" : "white king is not at E1") << endl;
    cout << detailedPerft(&bord,1) << endl;


    action.src = G3;
    action.dst = H4;
    movePiece(&bord,&action);
    */

     /*
    bord.whiteToPlay ^= 1;
    printBoard(&bord);
    printBitBoard(calculateKingDanger(&bord),"king danger");
    printBitBoard(bord.white & bord.king, "index");
    printBitBoard(bitmap_black_queen(H4,&bord),"bitmap at h4");
    printBitBoard(bitmap_white_queen(H4,&bord),"bitmap at h4");
    bord.whiteToPlay ^= 1;
     */


}


int TestRunner::runAutomatedTestCases() {
    /*
    bool temp = printPassed;
    printPassed = false;
    // Run the tests
    kingMovesGenerator();
    knightMovesGenerator();
    rookMovesGenerator();
    bischopMovesGenerator();
    queenMovesGenerator();
    pawnMovesGenerator();
    printPassed = temp;
    */

    GenerateActions();


    //testGeneralPerftResultst();

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

bool TestRunner::areActionListsEqual(const ActionList& list1, const ActionList& list2) {
    // Check if the counts are the same
    if (list1.count != list2.count){
        if (printFaults) cout << "the lists dont have equal sizes, list1: " << list1.count << " list2: " << list2.count << endl;
        return false;
    }

    // Check if both sets of moves contain the same elements
    if(std::is_permutation(list1.moves, list1.moves + list1.count, list2.moves)) return true;
    else{
        if (printFaults){cout << "moves in list1 but not in list2: " << endl;
            for (int i = 0; i<list1.count; i++) {
                Action actie = list1.moves[i];
                bool moveFound = false;
                for (int j = 0; j < list2.count; j++) {
                    if (actie == list2.moves[j]) {
                        moveFound = true;
                    }
                }
                if (!moveFound) {
                    printAction(&actie);
                }
                moveFound = false;
            }
            cout << "moves in list2 but not in list1: " << endl;
            for (int i = 0; i<list2.count; i++) {
                Action actie = list2.moves[i];
                bool moveFound = false;
                for (int j = 0; j < list1.count; j++) {
                    if (actie == list1.moves[j]) {
                        moveFound = true;
                    }
                }
                if (!moveFound) {
                    printAction(&actie);
                }
                moveFound = false;
            }
        }
        return false;
    }
}

U64 TestRunner::generalPerft(Board *bord, int depth, bool printPercentage = false, int startAt = 0, U64 initMoves = 0ULL) {
    std::unordered_map<std::string, U64> wanted;

    if(depth == 9) {
        // Add move-value pairs to the map
        wanted["A2A3"] = 74950758099ULL;
        wanted["B2B3"] = 96577095997ULL;
        wanted["C2C3"] = 108697368719ULL;
        wanted["D2D3"] = 176976245463ULL;
        wanted["E2E3"] = 259522947791ULL;
        wanted["F2F3"] = 68094899093ULL;
        wanted["G2G3"] = 99646370024ULL;
        wanted["H2H3"] = 74778417365ULL;
        wanted["A2A4"] = 101265301849ULL;
        wanted["B2B4"] = 97442160946ULL;
        wanted["C2C4"] = 120549219832ULL;
        wanted["D2D4"] = 227220482342ULL;
        wanted["E2E4"] = 263561543780ULL;
        wanted["F2F4"] = 84792070664ULL;
        wanted["G2G4"] = 92281289941ULL;
        wanted["H2H4"] = 102853440161ULL;
        wanted["B1A3"] = 85849641909ULL;
        wanted["B1C3"] = 109418317145ULL;
        wanted["G1F3"] = 108393009416ULL;
        wanted["G1H3"] = 86659653631ULL;
    }else if (depth == 8){
        wanted["A2A3"] = 2863411653ULL;
        wanted["B2B3"] = 3579299617ULL;
        wanted["C2C3"] = 3806229124ULL;
        wanted["D2D3"] = 6093248619ULL;
        wanted["E2E3"] = 8039390919ULL;
        wanted["F2F3"] = 2728615868ULL;
        wanted["G2G3"] = 3641432923ULL;
        wanted["H2H3"] = 2860408680ULL;
        wanted["A2A4"] = 3676309619ULL;
        wanted["B2B4"] = 3569067629ULL;
        wanted["C2C4"] = 4199667616ULL;
        wanted["D2D4"] = 7184581950ULL;
        wanted["E2E4"] = 8102108221ULL;
        wanted["F2F4"] = 3199039406ULL;
        wanted["G2G4"] = 3466204702ULL;
        wanted["H2H4"] = 3711123115ULL;
        wanted["B1A3"] = 3193522577ULL;
        wanted["B1C3"] = 3926684340ULL;
        wanted["G1F3"] = 3937354096ULL;
        wanted["G1H3"] = 3221278282ULL;
    }

    if(depth == 0) return 1;
    /* generate a list of all legal moves from this position */
    ActionList actionList;
    getLegalMoves(bord,&actionList);

    /* if we are at depth 1 the count of moves is a correct perft for the lower level (stop case) */
    //if(depth == 1) return actionList.count;

    U64 moves = initMoves;
    U64 totalMoves = actionList.count;

    for (int i = startAt; i<actionList.count; i++){
        Board copy;
        copyBoard(bord,&copy);
        movePiece(&copy,&(actionList.moves[i]));
        U64 currMoves = perftHelper(&copy, depth-1);
        moves += currMoves;

        if (printPercentage) {
            float percentage = (i + 1) * 100.0 / totalMoves;
            if (depth == 9 || depth == 8) {
                U64 wantedAmount = wanted[squareToString(actionList.moves[i].src) +
                                      squareToString(actionList.moves[i].dst)];
                std::cout << "Percentage done: " << percentage << "% finding: " << moves << " moves, currMoves: " << currMoves << " after: " << i << " moves are evaluated ( last move: " << squareToString(actionList.moves[i].src) << squareToString(actionList.moves[i].dst) << " ) wanted: " << wantedAmount << ", difference of: " << currMoves-wantedAmount << std::endl;
            }else{
                std::cout << "Percentage done: " << percentage << "% finding: " << moves << " moves, currMoves: " << currMoves << " after: " << i << " moves are evaluated ( last move: " << squareToString(actionList.moves[i].src) << squareToString(actionList.moves[i].dst) << " )" << std::endl;
            }
        }
    }
    return moves;
}

U64 TestRunner::detailedPerft(Board *bord, int depth) {
    if(depth == 0) return 1;
    /* generate a list of all legal moves from this position */
    ActionList actionList;
    getLegalMoves(bord,&actionList);

    /* if we are at depth 1 the count of moves is a correct perft for the lower level (stop case) */
    //if(depth == 1) return actionList.count;

    U64 moves = 0ULL;
    for (int i = 0; i<actionList.count; i++){
        Board copy;
        copyBoard(bord,&copy);
        movePiece(&copy,&(actionList.moves[i]));
        U64 movesHere = perftHelper(&copy, depth-1);
        std::cout << "move: " << squareToString(actionList.moves[i].src) <<  squareToString(actionList.moves[i].dst) << " found: " << movesHere << " moves" << std::endl;
        moves += movesHere;
    }
    std::cout << "total: " << moves << std::endl;
    return moves;
}

U64 TestRunner::perftHelper(Board *bord, int depth) {
    if(depth == 0) return 1;
    /* generate a list of all legal moves from this position */
    ActionList actionList;
    getLegalMoves(bord,&actionList);

    /* if we are at depth 1 the count of moves is a correct perft for the lower level (stop case) */
    if(depth == 1) return actionList.count;

    U64 moves = 0ULL;
    for (int i = 0; i<actionList.count; i++){
        Board copy;
        copyBoard(bord,&copy);
        movePiece(&copy,&(actionList.moves[i]));
        moves += generalPerft(&copy, depth-1);
    }
    return moves;
}

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

    std::cout << std::endl;
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
    //readInFen(&bord, &fen);
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
    readInFen(&bord, "r1bq2r1/b4pk1/p1pp1p2/1p2pPQ1/1P2P1PB/3P4/1PP3P1/R3K2R b - - 0 1");
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
    //readInFen(&bord, &fen);
    if (!inCheck(&bord)) return false;

    // not in check tests
    fen = "R1B1KBR1/PPbP1N1P/6P1/3p4/6p1/8/ppp1pp1p/rn2kb1r w KQq - 0 1";
    //readInFen(&bord, &fen);
    if (inCheck(&bord)) return false;


    // black
    // in check tests
    fen = "rnbqkbnr/pppp1ppp/8/8/8/4Q3/PPPPPPPP/RNB1KBNR b KQkq - 0 1";
    //readInFen(&bord, &fen);
    if (!inCheck(&bord)) return false;

    fen = "4k2R/8/4K3/8/8/8/8/8 b - - 0 1";
    //readInFen(&bord, &fen);
    if (!inCheck(&bord)) return false;

    fen = "r1b1kbr1/ppBp1n1p/6p1/3P4/6P1/4Q3/PPP1PP1P/RN2KB1R b KQq - 0 1";
    //readInFen(&bord, &fen);
    if (!inCheck(&bord)) return false;

    // not in check tests
    fen = "r1b1kbr1/ppBp1n1p/6p1/3P4/6P1/8/PPP1PP1P/RN2KB1R b KQq - 0 1";
    //readInFen(&bord, &fen);
    if (inCheck(&bord)) return false;

    fen = "r1b1kbr1/ppBp1n1p/Q5pQ/Q6Q/Q6Q/Q6Q/P3P3/RN2KB1R b - - 0 1";
    //readInFen(&bord, &fen);
    if (inCheck(&bord)) return false;

    fen = "r1bqkbr1/pprp1n1p/Q2PN1pQ/Q3R2B/Q6Q/Q6Q/P3P3/1N2K2R b - - 0 1";
    //readInFen(&bord, &fen);
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
    //readInFen(&bord, &fen);

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
    //readInFen(&bord, &fen);
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
    //readInFen(&bord, &fen);
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