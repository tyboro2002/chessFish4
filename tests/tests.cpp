#include <unordered_set>
#include "tests.h"

#define REMOVE_CASTELS()    bord.whiteKingsideCastle = 0; \
                            bord.blackKingsideCastle = 0; \
                            bord.whiteQueensideCastle = 0; \
                            bord.blackQueensideCastle = 0; \
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


/***************************************
 *         move generator tests        *
 ***************************************/

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
    Board bord{};
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
    Board bord{};
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
    Board bord{};
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
    Board bord{};
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
    Board bord{};
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

void TestRunner::testMoveToFront() {
    ActionList testActionList;
    testActionList.addMove(G1,H3);
    testActionList.addMove(G1,F3);
    testActionList.addMove(B1,C3);
    testActionList.addMove(B1,A3);

    // Create a copy of the initial action list for comparison
    ActionList originalActionList = testActionList;

    // Move G1,F3 to the front
    testActionList.moveToFront({G1,F3});

    // Check if count is the same
    testResultTrue(testActionList.count == originalActionList.count, "Move to front - Count check");

    // Check if the moved action is now at the front
    testResultTrue(testActionList.moves[0] == Action{G1,F3}, "Move to front - Action at front check");
    testResultTrue(testActionList.moves[1] == Action{G1,H3}, "Move to front - Action at second check");
    testResultTrue(testActionList.moves[2] == Action{B1,C3}, "Move to front - Action at third check");
    testResultTrue(testActionList.moves[3] == Action{B1,A3}, "Move to front - Action at fourth check");
}

/* action list test */
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
    Board bord{};
    setup(&bord);
    ActionList actionList;
    getAllMoves(&bord,&actionList);

    testResultTrue(actionList.count == testActionList.count,"correct amount of actions from starting position as white");
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
    testResultTrue(actionList.count == testActionList.count,"correct amount of actions from rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 position as white");
    testResultTrue(areActionListsEqual(actionList,testActionList),"actions from rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8 position as white are the correct actions");
}

void TestRunner::testGeneralPerftResultst() {
    Board bord{};
    setup(&bord);
    bool printPercent = true;

    testResultTrue(generalPerft(&bord,0,printPercent,0,0ULL) == 1, "perft depth 0, fen: startpos");
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 20, "perft depth 1, fen: startpos");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 400, "perft depth 2, fen: startpos");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 8'902, "perft depth 3, fen: startpos");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 197'281, "perft depth 4, fen: startpos");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 4'865'609, "perft depth 5, fen: startpos");
    testResultTrue(generalPerft(&bord,6,printPercent,0,0ULL) == 119'060'324, "perft depth 6, fen: startpos");
    testResultTrue(generalPerft(&bord,7,printPercent,0,0ULL) == 3'195'901'860, "perft depth 7, fen: startpos");
    //testResultTrue(generalPerft(&bord,8,printPercent,0,0ULL) == 84'998'978'956, "perft depth 8, fen: startpos");
    //testResultTrue(generalPerft(&bord,9,printPercent,0,0ULL) == 2'439'530'234'167, "perft depth 9, fen: startpos");


    setupEmpty(&bord);
    REMOVE_CASTELS()
    readInFen(&bord, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    //printFancyBoard(&bord);
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 48, "perft depth 1, fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 2039, "perft depth 2, fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 97'862, "perft depth 3, fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 4'085'603, "perft depth 4, fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 193'690'690, "perft depth 5, fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");
    testResultTrue(generalPerft(&bord,6,printPercent,0,0ULL) == 8'031'647'685, "perft depth 6, fen: r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");

    setupEmpty(&bord);
    REMOVE_CASTELS()
    readInFen(&bord, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0");
    //printFancyBoard(&bord);
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
    readInFen(&bord, "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    //printFancyBoard(&bord);
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 6, "perft depth 1, fen: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 264, "perft depth 2, fen: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 9'467, "perft depth 3, fen: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 422'333, "perft depth 4, fen: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 15'833'292, "perft depth 5, fen: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
    testResultTrue(generalPerft(&bord,6,printPercent,0,0ULL) == 706'045'033, "perft depth 6, fen: r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");

    setupEmpty(&bord);
    REMOVE_CASTELS()
    readInFen(&bord, "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");
    //printFancyBoard(&bord);
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 6, "perft depth 1, fen: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 264, "perft depth 2, fen: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 9'467, "perft depth 3, fen: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 422'333, "perft depth 4, fen: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 15'833'292, "perft depth 5, fen: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");
    testResultTrue(generalPerft(&bord,6,printPercent,0,0ULL) == 706'045'033, "perft depth 6, fen: r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1 ");

    setupEmpty(&bord);
    REMOVE_CASTELS()
    readInFen(&bord, "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    //printFancyBoard(&bord);
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 44, "perft depth 1, fen: rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 1'486, "perft depth 2, fen: rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 62'379, "perft depth 3, fen: rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 2'103'487, "perft depth 4, fen: rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 89'941'194, "perft depth 5, fen: rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8  ");

    setupEmpty(&bord);
    REMOVE_CASTELS()
    readInFen(&bord, "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    //printFancyBoard(&bord);
    testResultTrue(generalPerft(&bord,1,printPercent,0,0ULL) == 46, "perft depth 1, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    testResultTrue(generalPerft(&bord,2,printPercent,0,0ULL) == 2'079, "perft depth 2, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    testResultTrue(generalPerft(&bord,3,printPercent,0,0ULL) == 89'890, "perft depth 3, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    testResultTrue(generalPerft(&bord,4,printPercent,0,0ULL) == 3'894'594, "perft depth 4, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    testResultTrue(generalPerft(&bord,5,printPercent,0,0ULL) == 164'075'551, "perft depth 5, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    testResultTrue(generalPerft(&bord,6,printPercent,0,0ULL) == 6'923'051'137, "perft depth 6, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    //testResultTrue(generalPerft(&bord,7,printPercent,0,0ULL) == 287'188'994'746, "perft depth 7, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    //testResultTrue(generalPerft(&bord,8,printPercent,0,0ULL) == 11'923'589'843'526, "perft depth 8, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
    //testResultTrue(generalPerft(&bord,9,printPercent,0,0ULL) == 490'154'852'788'714, "perft depth 9, fen: r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
}


int TestRunner::runAutomatedTestCases() {
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

    GenerateActions();

    testDrawFunction();
    testCheckMateFunction();
    testIsEndedFunction();

    testMoveToFront();

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

bool TestRunner::areActionListsEqual(const ActionList& list1, const ActionList& list2) const {
    // Check if the counts are the same
    if (list1.count != list2.count){
        if (printFaults){
            cout << "the lists dont have equal sizes, list1: " << list1.count << " list2: " << list2.count << endl;
            cout << "list1: " << endl;
            printActionList(&list1);
            cout << "list2: " << endl;
            printActionList(&list2);
        }
        return false;
    }

    // Convert arrays to unordered sets
    std::unordered_set<Action> set1(list1.moves, list1.moves + list1.count);
    std::unordered_set<Action> set2(list2.moves, list2.moves + list2.count);

    // Check if sets are the same
    if (set1 == set2) {
        return true;
    }else{
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
        Board copy{};
        copyBoard(bord,&copy);
        movePiece(&copy,&(actionList.moves[i]));
        U64 currMoves = perftHelper(&copy, depth-1);
        moves += currMoves;

        if (printPercentage) {
            float percentage = (i + 1) * 100.0 / totalMoves; // NOLINT(*-narrowing-conversions)
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
        Board copy{};
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
        Board copy{};
        copyBoard(bord,&copy);
        movePiece(&copy,&(actionList.moves[i]));
        moves += generalPerft(&copy, depth-1);
    }
    return moves;
}

void TestRunner::testDrawFunction() {
    Board bord{};
    setupEmpty(&bord);
    readInFen(&bord,"4k3/8/8/8/8/3q1q2/8/4K3 w - - 0 1");
    testResultTrue(isDraw(&bord),"4k3/8/8/8/8/3q1q2/8/4K3 w - - 0 1 is a draw");

    setupEmpty(&bord);
    readInFen(&bord,"4k3/8/8/8/8/3q1q2/8/4K3 b - - 0 1");
    testResultFalse(isDraw(&bord),"4k3/8/8/8/8/3q1q2/8/4K3 b - - 0 1 is not a draw");

    setupEmpty(&bord);
    readInFen(&bord,"4k3/8/8/8/8/p6p/8/4K3 w - - 100 1");
    testResultTrue(isDraw(&bord),"4k3/8/8/8/8/p6p/8/4K3 w - - 100 1 is a draw");

    setupEmpty(&bord);
    readInFen(&bord,"4k3/8/8/8/8/p6p/8/4K3 b - - 100 1");
    testResultTrue(isDraw(&bord),"4k3/8/8/8/8/p6p/8/4K3 b - - 100 1 is a draw");

    setupEmpty(&bord);
    readInFen(&bord,"4k3/8/8/8/8/3qqq2/8/4K3 w - - 0 1");
    testResultFalse(isDraw(&bord),"4k3/8/8/8/8/3qqq2/8/4K3 w - - 0 1 is not a draw");

    setupEmpty(&bord);
    readInFen(&bord,"4k3/8/8/8/8/3qqq2/8/4K3 b - - 0 1");
    testResultFalse(isDraw(&bord),"4k3/8/8/8/8/3qqq2/8/4K3 b - - 0 1 is not a draw");


    /* ------------------------------------------------------------------------- */
    /* the positions after this point are final positions of games magnus played */
    /* ------------------------------------------------------------------------- */


    {
        setupEmpty(&bord);
        readInFen(&bord,"5K1k/7b/7R/8/8/8/8/8 b - - 39 74");
        testResultTrue(isDraw(&bord),"5K1k/7b/7R/8/8/8/8/8 b - - 39 74 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"k7/P7/PK6/8/8/8/8/8 b - - 0 86");
        testResultTrue(isDraw(&bord),"k7/P7/PK6/8/8/8/8/8 b - - 0 86 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"7k/7P/7K/8/8/8/8/8 b - - 2 76");
        testResultTrue(isDraw(&bord),"7k/7P/7K/8/8/8/8/8 b - - 2 76 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"8/2b5/8/8/8/7k/7p/7K w - - 2 75");
        testResultTrue(isDraw(&bord),"8/2b5/8/8/8/7k/7p/7K w - - 2 75 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/6k1/7p/7K w - - 0 67");
        testResultTrue(isDraw(&bord),"8/8/8/8/8/6k1/7p/7K w - - 0 67 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"k7/1pK5/1P6/8/8/8/8/8 b - - 4 56");
        testResultTrue(isDraw(&bord),"k7/1pK5/1P6/8/8/8/8/8 b - - 4 56 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/6b1/8/6k1/7p/7K w - - 0 63");
        testResultTrue(isDraw(&bord),"8/8/8/6b1/8/6k1/7p/7K w - - 0 63 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"6R1/8/8/5N1k/5K2/8/8/8 b - - 0 64");
        testResultTrue(isDraw(&bord),"6R1/8/8/5N1k/5K2/8/8/8 b - - 0 64 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/6k1/6p1/6K1 w - - 2 78");
        testResultTrue(isDraw(&bord),"8/8/8/8/8/6k1/6p1/6K1 w - - 2 78 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"k7/8/K7/P3B3/8/8/8/8 b - - 2 71");
        testResultTrue(isDraw(&bord),"k7/8/K7/P3B3/8/8/8/8 b - - 2 71 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"6r1/5k1K/7P/8/8/8/8/8 w - - 0 65");
        testResultTrue(isDraw(&bord),"6r1/5k1K/7P/8/8/8/8/8 w - - 0 65 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"K3Q3/6pk/2R3p1/4p1P1/4P2P/8/8/8 b - - 0 60");
        testResultTrue(isDraw(&bord),"K3Q3/6pk/2R3p1/4p1P1/4P2P/8/8/8 b - - 0 60 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/5p2/5p2/P4P1p/1P3P1k/7P/7K b - - 0 47");
        testResultTrue(isDraw(&bord),"8/8/5p2/5p2/P4P1p/1P3P1k/7P/7K b - - 0 47 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"7K/7P/8/8/8/5kr1/8/8 w - - 0 66");
        testResultTrue(isDraw(&bord),"7K/7P/8/8/8/5kr1/8/8 w - - 0 66 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"k7/8/1K6/8/p4B2/P7/8/8 b - - 4 68");
        testResultTrue(isDraw(&bord),"k7/8/1K6/8/p4B2/P7/8/8 b - - 4 68 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"k7/PpK5/1P6/8/8/8/8/8 b - - 2 94");
        testResultTrue(isDraw(&bord),"k7/PpK5/1P6/8/8/8/8/8 b - - 2 94 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"7k/8/4B1KP/8/8/8/8/8 b - - 4 123");
        testResultTrue(isDraw(&bord),"7k/8/4B1KP/8/8/8/8/8 b - - 4 123 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"7k/7P/6K1/8/8/8/8/8 b - - 0 86");
        testResultTrue(isDraw(&bord),"7k/7P/6K1/8/8/8/8/8 b - - 0 86 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"8/1P6/8/8/8/4Q2K/8/7k b - - 0 69");
        testResultTrue(isDraw(&bord),"8/1P6/8/8/8/4Q2K/8/7k b - - 0 69 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/6k1/7p/7K w - - 0 59");
        testResultTrue(isDraw(&bord),"8/8/8/8/8/6k1/7p/7K w - - 0 59 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"7k/7P/7K/p7/P7/8/8/8 b - - 2 58");
        testResultTrue(isDraw(&bord),"7k/7P/7K/p7/P7/8/8/8 b - - 2 58 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"k7/P7/1K6/8/8/8/8/8 b - - 0 77");
        testResultTrue(isDraw(&bord),"k7/P7/1K6/8/8/8/8/8 b - - 0 77 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"R5bk/8/7K/8/8/8/8/8 b - - 14 74");
        testResultTrue(isDraw(&bord),"R5bk/8/7K/8/8/8/8/8 b - - 14 74 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/6k1/6p1/6K1 w - - 2 59");
        testResultTrue(isDraw(&bord),"8/8/8/8/8/6k1/6p1/6K1 w - - 2 59 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/5KR1/5B2/5k2 b - - 0 99");
        testResultTrue(isDraw(&bord),"8/8/8/8/8/5KR1/5B2/5k2 b - - 0 99 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"5K1k/8/6P1/8/7P/3B4/8/8 b - - 0 84");
        testResultTrue(isDraw(&bord),"5K1k/8/6P1/8/7P/3B4/8/8 b - - 0 84 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"5k2/5P2/5K2/8/8/8/8/8 b - - 2 100");
        testResultTrue(isDraw(&bord),"5k2/5P2/5K2/8/8/8/8/8 b - - 2 100 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"2k5/2P5/2K5/8/8/8/4R3/8 b - - 0 83");
        testResultTrue(isDraw(&bord),"2k5/2P5/2K5/8/8/8/4R3/8 b - - 0 83 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"2k5/2P1R3/2K5/8/8/8/8/8 b - - 0 88");
        testResultTrue(isDraw(&bord),"2k5/2P1R3/2K5/8/8/8/8/8 b - - 0 88 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"5k2/5P1p/5K1P/8/8/8/8/8 b - - 2 87");
        testResultTrue(isDraw(&bord),"5k2/5P1p/5K1P/8/8/8/8/8 b - - 2 87 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/7k/7p/7K w - - 2 74");
        testResultTrue(isDraw(&bord),"8/8/8/8/8/7k/7p/7K w - - 2 74 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/8/5K1p/7k b - - 1 67");
        testResultTrue(isDraw(&bord),"8/8/8/8/8/8/5K1p/7k b - - 1 67 is a draw");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/3k4/3p4/3K4 w - - 2 69");
        testResultTrue(isDraw(&bord),"8/8/8/8/8/3k4/3p4/3K4 w - - 2 69 is a draw");

    }
}

void TestRunner::testCheckMateFunction() {
    Board bord{};

    setupEmpty(&bord);
    readInFen(&bord,"4k3/8/8/8/8/3q1q2/8/4K3 w - - 0 1");
    testResultFalse(isCheckmate(&bord), "4k3/8/8/8/8/3q1q2/8/4K3 w - - 0 1 is not a chekMate");

    setupEmpty(&bord);
    readInFen(&bord,"4k3/8/8/8/8/3qqq2/8/4K3 w - - 0 1");
    testResultTrue(isCheckmate(&bord), "4k3/8/8/8/8/3qqq2/8/4K3 w - - 0 1 is a chekMate");

    setupEmpty(&bord);
    readInFen(&bord,"4k3/8/8/8/8/3qqq2/8/4K3 b - - 0 1");
    testResultFalse(isCheckmate(&bord), "4k3/8/8/8/8/3qqq2/8/4K3 b - - 0 1 is not a chekMate");

    setupEmpty(&bord);
    readInFen(&bord,"rnbqkbnr/2pp1Qpp/pp6/4p3/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 0 1");
    testResultTrue(isCheckmate(&bord), "rnbqkbnr/2pp1Qpp/pp6/4p3/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 0 1 is a chekMate");

    setupEmpty(&bord);
    readInFen(&bord,"rnbqkbnr/2pp1Qpp/pp6/4p3/2B1P3/8/PPPP1PPP/RNB1K1NR w KQkq - 0 1");
    testResultFalse(isCheckmate(&bord), "rnbqkbnr/2pp1Qpp/pp6/4p3/2B1P3/8/PPPP1PPP/RNB1K1NR w KQkq - 0 1 is not a chekMate");


    /* ------------------------------------------------------------------------- */
    /* the positions after this point are final positions of games magnus played */
    /* ------------------------------------------------------------------------- */


    {
        setupEmpty(&bord);
        readInFen(&bord,"5k2/5Q2/p2q2B1/4p1PP/P3Pp2/2P2P1K/8/3r4 b - - 2 48");
        testResultTrue(isCheckmate(&bord), "5k2/5Q2/p2q2B1/4p1PP/P3Pp2/2P2P1K/8/3r4 b - - 2 48 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/1p6/7Q/1P6/5Rk1/r7/p4K2/8 b - - 8 50");
        testResultTrue(isCheckmate(&bord), "8/1p6/7Q/1P6/5Rk1/r7/p4K2/8 b - - 8 50 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"3R1k2/4p3/p5Q1/4pp2/8/8/P4PPK/1r6 b - - 0 37");
        testResultTrue(isCheckmate(&bord), "3R1k2/4p3/p5Q1/4pp2/8/8/P4PPK/1r6 b - - 0 37 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"5k1Q/p3qr1p/Pp2B1p1/6N1/1b1P3P/8/1P4P1/6K1 b - - 2 35");
        testResultTrue(isCheckmate(&bord), "5k1Q/p3qr1p/Pp2B1p1/6N1/1b1P3P/8/1P4P1/6K1 b - - 2 35 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"7Q/8/6pk/p4n1p/5Pq1/6P1/PP3R1K/8 b - - 3 47");
        testResultTrue(isCheckmate(&bord), "7Q/8/6pk/p4n1p/5Pq1/6P1/PP3R1K/8 b - - 3 47 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"3rk2r/p1p2pN1/2P4p/4P3/1Q6/2p5/P2nbP1P/2K3R1 b - - 0 28");
        testResultTrue(isCheckmate(&bord), "3rk2r/p1p2pN1/2P4p/4P3/1Q6/2p5/P2nbP1P/2K3R1 b - - 0 28 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"4Q1k1/1pr2ppp/4p1b1/1P1pn3/r2P4/2P5/1P1NBP1P/5RK1 b - - 0 21");
        testResultTrue(isCheckmate(&bord), "4Q1k1/1pr2ppp/4p1b1/1P1pn3/r2P4/2P5/1P1NBP1P/5RK1 b - - 0 21 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"r1b1Qk2/3p3p/2p2b2/p4N2/Pp1p2nq/1B1P2pP/1PPB2P1/4RRK1 b - - 3 24");
        testResultTrue(isCheckmate(&bord), "r1b1Qk2/3p3p/2p2b2/p4N2/Pp1p2nq/1B1P2pP/1PPB2P1/4RRK1 b - - 3 24 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"1r2kb1r/p1pQpppp/7n/4N3/5B2/2N5/PPP2P1P/R3K1R1 b Qk - 0 15");
        testResultTrue(isCheckmate(&bord), "1r2kb1r/p1pQpppp/7n/4N3/5B2/2N5/PPP2P1P/R3K1R1 b Qk - 0 15 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"r1bb1Q1k/ppp3pp/8/4N3/1PP5/Pn6/4PPPP/4KB1R b K - 1 18");
        testResultTrue(isCheckmate(&bord), "r1bb1Q1k/ppp3pp/8/4N3/1PP5/Pn6/4PPPP/4KB1R b K - 1 18 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"5Qk1/p1p4p/4N1pB/8/3n4/2p3P1/P4P1P/5RK1 b - - 0 29");
        testResultTrue(isCheckmate(&bord), "5Qk1/p1p4p/4N1pB/8/3n4/2p3P1/P4P1P/5RK1 b - - 0 29 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"rnb1k3/1pppQR1p/8/p2P4/8/2P5/PP2B1PP/RN2K3 b Q - 0 20");
        testResultTrue(isCheckmate(&bord), "rnb1k3/1pppQR1p/8/p2P4/8/2P5/PP2B1PP/RN2K3 b Q - 0 20 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"5rk1/6RR/6p1/p3p3/8/2P3P1/P3PP1P/6K1 b - - 2 27");
        testResultTrue(isCheckmate(&bord), "5rk1/6RR/6p1/p3p3/8/2P3P1/P3PP1P/6K1 b - - 2 27 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"6r1/5N1p/p5p1/6Qk/1p6/8/PPP3PP/6K1 b - - 11 37");
        testResultTrue(isCheckmate(&bord), "6r1/5N1p/p5p1/6Qk/1p6/8/PPP3PP/6K1 b - - 11 37 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/1p5p/q1kQ4/1N6/2p2P2/6P1/5P1P/1R4K1 b - - 8 37");
        testResultTrue(isCheckmate(&bord), "8/1p5p/q1kQ4/1N6/2p2P2/6P1/5P1P/1R4K1 b - - 8 37 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"6R1/7k/5NR1/3p3p/p2P3P/P4p2/5PP1/6K1 b - - 1 39");
        testResultTrue(isCheckmate(&bord), "6R1/7k/5NR1/3p3p/p2P3P/P4p2/5PP1/6K1 b - - 1 39 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"6r1/3b1KPk/p1p1p3/4Bp2/5P2/1P6/P7/7R b - - 12 53");
        testResultTrue(isCheckmate(&bord), "6r1/3b1KPk/p1p1p3/4Bp2/5P2/1P6/P7/7R b - - 12 53 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/1Q6/8/kR6/p2pB1p1/P2K4/1P3P2/8 b - - 0 42");
        testResultTrue(isCheckmate(&bord), "8/1Q6/8/kR6/p2pB1p1/P2K4/1P3P2/8 b - - 0 42 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"7Q/8/6pk/8/2P4P/8/6B1/6K1 b - - 4 53");
        testResultTrue(isCheckmate(&bord), "7Q/8/6pk/8/2P4P/8/6B1/6K1 b - - 4 53 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/6R1/5pk1/5N2/2b2KP1/r7/8 b - - 5 46");
        testResultTrue(isCheckmate(&bord), "8/8/6R1/5pk1/5N2/2b2KP1/r7/8 b - - 5 46 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"5Q2/7p/5Np1/5p2/6kP/3P2P1/P1r2PK1/4q3 b - - 2 33");
        testResultTrue(isCheckmate(&bord), "5Q2/7p/5Np1/5p2/6kP/3P2P1/P1r2PK1/4q3 b - - 2 33 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/5ppk/4p2p/3pPb2/5P2/1R3qP1/5nQP/5rBK w - - 2 46");
        testResultTrue(isCheckmate(&bord), "8/5ppk/4p2p/3pPb2/5P2/1R3qP1/5nQP/5rBK w - - 2 46 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/6k1/p3Q1p1/3pP1P1/P6p/2P4P/5rK1/5r2 w - - 4 49");
        testResultTrue(isCheckmate(&bord), "8/6k1/p3Q1p1/3pP1P1/P6p/2P4P/5rK1/5r2 w - - 4 49 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/5pkp/4b1p1/p7/7P/RPR2BP1/1r6/4r1K1 w - - 6 33");
        testResultTrue(isCheckmate(&bord), "8/5pkp/4b1p1/p7/7P/RPR2BP1/1r6/4r1K1 w - - 6 33 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/5k2/p1p1pp2/5r1p/PpnPK3/1NP1r2P/1P6/1R6 w - - 0 41");
        testResultTrue(isCheckmate(&bord), "8/5k2/p1p1pp2/5r1p/PpnPK3/1NP1r2P/1P6/1R6 w - - 0 41 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/5ppk/7p/8/3N3P/P2PP1P1/2QK1P2/1r2q3 w - - 9 35");
        testResultTrue(isCheckmate(&bord), "8/5ppk/7p/8/3N3P/P2PP1P1/2QK1P2/1r2q3 w - - 9 35 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"6k1/6p1/8/p3P3/6P1/P4P1p/7P/3r3K w - - 0 49");
        testResultTrue(isCheckmate(&bord), "6k1/6p1/8/p3P3/6P1/P4P1p/7P/3r3K w - - 0 49 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/1p4k1/4RR2/6Kp/6qP/6P1/8 w - - 14 58");
        testResultTrue(isCheckmate(&bord), "8/8/1p4k1/4RR2/6Kp/6qP/6P1/8 w - - 14 58 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"3Q1Q2/p1R2ppk/4p2p/3p3P/3Pn3/1P6/P4q2/5K2 w - - 0 40");
        testResultTrue(isCheckmate(&bord), "3Q1Q2/p1R2ppk/4p2p/3p3P/3Pn3/1P6/P4q2/5K2 w - - 0 40 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"4k3/5p1p/p3p3/2p5/1p1Pqb2/2P1N2Q/PP1B1nP1/6RK w - - 1 30");
        testResultTrue(isCheckmate(&bord), "4k3/5p1p/p3p3/2p5/1p1Pqb2/2P1N2Q/PP1B1nP1/6RK w - - 1 30 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"k1Q2b2/pp6/1qp2p2/3P3p/2p2B1P/2P5/PP4r1/1K1R4 b - - 1 34");
        testResultTrue(isCheckmate(&bord), "k1Q2b2/pp6/1qp2p2/3P3p/2p2B1P/2P5/PP4r1/1K1R4 b - - 1 34 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"1k6/pp2pp1p/1b6/1P5P/4PPb1/5K2/2RNB3/6r1 w - - 5 40");
        testResultTrue(isCheckmate(&bord), "1k6/pp2pp1p/1b6/1P5P/4PPb1/5K2/2RNB3/6r1 w - - 5 40 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/kp6/p1p5/3p3p/8/5pNP/6q1/6K1 w - - 2 63");
        testResultTrue(isCheckmate(&bord), "8/kp6/p1p5/3p3p/8/5pNP/6q1/6K1 w - - 2 63 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"1k6/pp5p/2p5/2P1pp2/Q1b5/P1N1P2P/3r2r1/R5KR w - - 0 26");
        testResultTrue(isCheckmate(&bord), "1k6/pp5p/2p5/2P1pp2/Q1b5/P1N1P2P/3r2r1/R5KR w - - 0 26 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/5pk1/1R4p1/p2q4/3P2Pp/3QP2P/5P1K/7r w - - 3 40");
        testResultTrue(isCheckmate(&bord), "8/5pk1/1R4p1/p2q4/3P2Pp/3QP2P/5P1K/7r w - - 3 40 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/5Q2/1k2pBp1/p2pP1P1/Kq1P2B1/8/P7/8 w - - 4 52");
        testResultTrue(isCheckmate(&bord), "8/5Q2/1k2pBp1/p2pP1P1/Kq1P2B1/8/P7/8 w - - 4 52 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"5Qk1/5p1p/6N1/8/q2P4/2P3P1/5PKP/8 b - - 0 47");
        testResultTrue(isCheckmate(&bord), "5Qk1/5p1p/6N1/8/q2P4/2P3P1/5PKP/8 b - - 0 47 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"5r2/pp1Q3R/2pk1b2/2q1p3/2PnK3/1P2P3/P7/3R4 b - - 4 32");
        testResultTrue(isCheckmate(&bord), "5r2/pp1Q3R/2pk1b2/2q1p3/2PnK3/1P2P3/P7/3R4 b - - 4 32 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/1q5p/p6k/5PPb/2PB3Q/3p3P/4P3/6K1 b - - 0 41");
        testResultTrue(isCheckmate(&bord), "8/1q5p/p6k/5PPb/2PB3Q/3p3P/4P3/6K1 b - - 0 41 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"2rq1r2/6Qk/3p2pp/1p3b1N/1PB2P2/3P3P/6P1/4R1K1 b - - 6 37");
        testResultTrue(isCheckmate(&bord), "2rq1r2/6Qk/3p2pp/1p3b1N/1PB2P2/3P3P/6P1/4R1K1 b - - 6 37 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"5R2/p7/1b6/5p2/4Rk1P/3r1P2/6K1/8 b - - 3 49");
        testResultTrue(isCheckmate(&bord), "5R2/p7/1b6/5p2/4Rk1P/3r1P2/6K1/8 b - - 3 49 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"6R1/5p1p/5R1k/pp6/5PP1/1Pn2K2/P6r/8 b - - 0 40");
        testResultTrue(isCheckmate(&bord), "6R1/5p1p/5R1k/pp6/5PP1/1Pn2K2/P6r/8 b - - 0 40 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"3r2k1/1p4pp/5p2/p2p1P1Q/3P2RN/3bq3/6PP/2R2K2 w - - 2 27");
        testResultTrue(isCheckmate(&bord), "3r2k1/1p4pp/5p2/p2p1P1Q/3P2RN/3bq3/6PP/2R2K2 w - - 2 27 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"1k6/1P3p2/3R2p1/5rKp/6nP/6P1/8/8 w - - 14 49");
        testResultTrue(isCheckmate(&bord), "1k6/1P3p2/3R2p1/5rKp/6nP/6P1/8/8 w - - 14 49 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"3k4/3b1Q2/2qK4/4P3/1PB5/p1P5/8/8 w - - 4 60");
        testResultTrue(isCheckmate(&bord), "3k4/3b1Q2/2qK4/4P3/1PB5/p1P5/8/8 w - - 4 60 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"r3bk1Q/1p2bp2/6R1/p1nBP3/8/8/Pq3PPP/3R2K1 b - - 0 26");
        testResultTrue(isCheckmate(&bord), "r3bk1Q/1p2bp2/6R1/p1nBP3/8/8/Pq3PPP/3R2K1 b - - 0 26 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/5K2/6Qk/8/8/5P2/8/8 b - - 2 76");
        testResultTrue(isCheckmate(&bord), "8/5K2/6Qk/8/8/5P2/8/8 b - - 2 76 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"4k1R1/7R/8/5P2/5q2/8/5P2/5BK1 b - - 8 58");
        testResultTrue(isCheckmate(&bord), "4k1R1/7R/8/5P2/5q2/8/5P2/5BK1 b - - 8 58 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/5R2/2bN1kpp/Pp1B4/n3PP2/P2P2P1/5K1P/2r5 b - - 0 39");
        testResultTrue(isCheckmate(&bord), "8/5R2/2bN1kpp/Pp1B4/n3PP2/P2P2P1/5K1P/2r5 b - - 0 39 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"r4k2/pp1n1Q2/2p1p3/3q3B/3P4/P6P/1PP2P2/2KR2r1 b - - 0 23");
        testResultTrue(isCheckmate(&bord), "test r4k2/pp1n1Q2/2p1p3/3q3B/3P4/P6P/1PP2P2/2KR2r1 b - - 0 23 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"k7/P7/1K6/8/4B3/8/8/8 b - - 2 72");
        testResultTrue(isCheckmate(&bord), "k7/P7/1K6/8/4B3/8/8/8 b - - 2 72 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/6pp/4P2k/6QP/5rPK/R7 b - - 1 57");
        testResultTrue(isCheckmate(&bord), "8/8/8/6pp/4P2k/6QP/5rPK/R7 b - - 1 57 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"4r3/ppprqkQp/8/5P2/4b2n/PP1p4/1B1P2PP/4RRK1 b - - 4 27");
        testResultTrue(isCheckmate(&bord), "4r3/ppprqkQp/8/5P2/4b2n/PP1p4/1B1P2PP/4RRK1 b - - 4 27 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"2n1Rk2/pp1r1ppp/3q4/8/8/QP1P1PPP/P4PK1/4R3 b - - 0 29");
        testResultTrue(isCheckmate(&bord), "2n1Rk2/pp1r1ppp/3q4/8/8/QP1P1PPP/P4PK1/4R3 b - - 0 29 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"4q3/8/kQ1P4/P7/4pP1P/8/1P3PK1/8 b - - 10 69");
        testResultTrue(isCheckmate(&bord), "4q3/8/kQ1P4/P7/4pP1P/8/1P3PK1/8 b - - 10 69 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/7r/2R3Q1/1p1p1pk1/4q3/6P1/1N3P2/6K1 b - - 0 42");
        testResultTrue(isCheckmate(&bord), "8/7r/2R3Q1/1p1p1pk1/4q3/6P1/1N3P2/6K1 b - - 0 42 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"5k1Q/2q2p2/4b3/5N2/2Pp2pP/r7/6P1/4R2K b - - 2 44");
        testResultTrue(isCheckmate(&bord), "5k1Q/2q2p2/4b3/5N2/2Pp2pP/r7/6P1/4R2K b - - 2 44 is a chekMate");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/3R4/1BP5/1P4K1/4k3/4Q3 b - - 4 65");
        testResultTrue(isCheckmate(&bord), "8/8/8/3R4/1BP5/1P4K1/4k3/4Q3 b - - 4 65 is a chekMate");
    }

}

void TestRunner::testIsEndedFunction() {
    Board bord{};

    /* ------------------------------------------------------------------------- */
    /* the positions after this point are final positions of games magnus played */
    /* ------------------------------------------------------------------------- */

    {
        setupEmpty(&bord);
        readInFen(&bord,"5k2/5Q2/p2q2B1/4p1PP/P3Pp2/2P2P1K/8/3r4 b - - 2 48");
        testResultTrue(isEnded(&bord),"5k2/5Q2/p2q2B1/4p1PP/P3Pp2/2P2P1K/8/3r4 b - - 2 48 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/1p6/7Q/1P6/5Rk1/r7/p4K2/8 b - - 8 50");
        testResultTrue(isEnded(&bord),"8/1p6/7Q/1P6/5Rk1/r7/p4K2/8 b - - 8 50 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"3R1k2/4p3/p5Q1/4pp2/8/8/P4PPK/1r6 b - - 0 37");
        testResultTrue(isEnded(&bord),"3R1k2/4p3/p5Q1/4pp2/8/8/P4PPK/1r6 b - - 0 37 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"5k1Q/p3qr1p/Pp2B1p1/6N1/1b1P3P/8/1P4P1/6K1 b - - 2 35");
        testResultTrue(isEnded(&bord),"5k1Q/p3qr1p/Pp2B1p1/6N1/1b1P3P/8/1P4P1/6K1 b - - 2 35 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"7Q/8/6pk/p4n1p/5Pq1/6P1/PP3R1K/8 b - - 3 47");
        testResultTrue(isEnded(&bord),"7Q/8/6pk/p4n1p/5Pq1/6P1/PP3R1K/8 b - - 3 47 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"3rk2r/p1p2pN1/2P4p/4P3/1Q6/2p5/P2nbP1P/2K3R1 b - - 0 28");
        testResultTrue(isEnded(&bord),"3rk2r/p1p2pN1/2P4p/4P3/1Q6/2p5/P2nbP1P/2K3R1 b - - 0 28 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"4Q1k1/1pr2ppp/4p1b1/1P1pn3/r2P4/2P5/1P1NBP1P/5RK1 b - - 0 21");
        testResultTrue(isEnded(&bord),"4Q1k1/1pr2ppp/4p1b1/1P1pn3/r2P4/2P5/1P1NBP1P/5RK1 b - - 0 21 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"r1b1Qk2/3p3p/2p2b2/p4N2/Pp1p2nq/1B1P2pP/1PPB2P1/4RRK1 b - - 3 24");
        testResultTrue(isEnded(&bord),"r1b1Qk2/3p3p/2p2b2/p4N2/Pp1p2nq/1B1P2pP/1PPB2P1/4RRK1 b - - 3 24 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"1r2kb1r/p1pQpppp/7n/4N3/5B2/2N5/PPP2P1P/R3K1R1 b Qk - 0 15");
        testResultTrue(isEnded(&bord),"1r2kb1r/p1pQpppp/7n/4N3/5B2/2N5/PPP2P1P/R3K1R1 b Qk - 0 15 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"r1bb1Q1k/ppp3pp/8/4N3/1PP5/Pn6/4PPPP/4KB1R b K - 1 18");
        testResultTrue(isEnded(&bord),"r1bb1Q1k/ppp3pp/8/4N3/1PP5/Pn6/4PPPP/4KB1R b K - 1 18 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"5Qk1/p1p4p/4N1pB/8/3n4/2p3P1/P4P1P/5RK1 b - - 0 29");
        testResultTrue(isEnded(&bord),"5Qk1/p1p4p/4N1pB/8/3n4/2p3P1/P4P1P/5RK1 b - - 0 29 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"rnb1k3/1pppQR1p/8/p2P4/8/2P5/PP2B1PP/RN2K3 b Q - 0 20");
        testResultTrue(isEnded(&bord),"rnb1k3/1pppQR1p/8/p2P4/8/2P5/PP2B1PP/RN2K3 b Q - 0 20 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"5rk1/6RR/6p1/p3p3/8/2P3P1/P3PP1P/6K1 b - - 2 27");
        testResultTrue(isEnded(&bord),"5rk1/6RR/6p1/p3p3/8/2P3P1/P3PP1P/6K1 b - - 2 27 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"6r1/5N1p/p5p1/6Qk/1p6/8/PPP3PP/6K1 b - - 11 37");
        testResultTrue(isEnded(&bord),"6r1/5N1p/p5p1/6Qk/1p6/8/PPP3PP/6K1 b - - 11 37 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/1p5p/q1kQ4/1N6/2p2P2/6P1/5P1P/1R4K1 b - - 8 37");
        testResultTrue(isEnded(&bord),"8/1p5p/q1kQ4/1N6/2p2P2/6P1/5P1P/1R4K1 b - - 8 37 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"6R1/7k/5NR1/3p3p/p2P3P/P4p2/5PP1/6K1 b - - 1 39");
        testResultTrue(isEnded(&bord),"6R1/7k/5NR1/3p3p/p2P3P/P4p2/5PP1/6K1 b - - 1 39 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"6r1/3b1KPk/p1p1p3/4Bp2/5P2/1P6/P7/7R b - - 12 53");
        testResultTrue(isEnded(&bord),"6r1/3b1KPk/p1p1p3/4Bp2/5P2/1P6/P7/7R b - - 12 53 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/1Q6/8/kR6/p2pB1p1/P2K4/1P3P2/8 b - - 0 42");
        testResultTrue(isEnded(&bord),"8/1Q6/8/kR6/p2pB1p1/P2K4/1P3P2/8 b - - 0 42 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"7Q/8/6pk/8/2P4P/8/6B1/6K1 b - - 4 53");
        testResultTrue(isEnded(&bord),"7Q/8/6pk/8/2P4P/8/6B1/6K1 b - - 4 53 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/6R1/5pk1/5N2/2b2KP1/r7/8 b - - 5 46");
        testResultTrue(isEnded(&bord),"8/8/6R1/5pk1/5N2/2b2KP1/r7/8 b - - 5 46 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"5Q2/7p/5Np1/5p2/6kP/3P2P1/P1r2PK1/4q3 b - - 2 33");
        testResultTrue(isEnded(&bord),"5Q2/7p/5Np1/5p2/6kP/3P2P1/P1r2PK1/4q3 b - - 2 33 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/5ppk/4p2p/3pPb2/5P2/1R3qP1/5nQP/5rBK w - - 2 46");
        testResultTrue(isEnded(&bord),"8/5ppk/4p2p/3pPb2/5P2/1R3qP1/5nQP/5rBK w - - 2 46 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/6k1/p3Q1p1/3pP1P1/P6p/2P4P/5rK1/5r2 w - - 4 49");
        testResultTrue(isEnded(&bord),"8/6k1/p3Q1p1/3pP1P1/P6p/2P4P/5rK1/5r2 w - - 4 49 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/5pkp/4b1p1/p7/7P/RPR2BP1/1r6/4r1K1 w - - 6 33");
        testResultTrue(isEnded(&bord),"8/5pkp/4b1p1/p7/7P/RPR2BP1/1r6/4r1K1 w - - 6 33 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/5k2/p1p1pp2/5r1p/PpnPK3/1NP1r2P/1P6/1R6 w - - 0 41");
        testResultTrue(isEnded(&bord),"8/5k2/p1p1pp2/5r1p/PpnPK3/1NP1r2P/1P6/1R6 w - - 0 41 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/5ppk/7p/8/3N3P/P2PP1P1/2QK1P2/1r2q3 w - - 9 35");
        testResultTrue(isEnded(&bord),"8/5ppk/7p/8/3N3P/P2PP1P1/2QK1P2/1r2q3 w - - 9 35 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"6k1/6p1/8/p3P3/6P1/P4P1p/7P/3r3K w - - 0 49");
        testResultTrue(isEnded(&bord),"6k1/6p1/8/p3P3/6P1/P4P1p/7P/3r3K w - - 0 49 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/1p4k1/4RR2/6Kp/6qP/6P1/8 w - - 14 58");
        testResultTrue(isEnded(&bord),"8/8/1p4k1/4RR2/6Kp/6qP/6P1/8 w - - 14 58 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"3Q1Q2/p1R2ppk/4p2p/3p3P/3Pn3/1P6/P4q2/5K2 w - - 0 40");
        testResultTrue(isEnded(&bord),"3Q1Q2/p1R2ppk/4p2p/3p3P/3Pn3/1P6/P4q2/5K2 w - - 0 40 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"4k3/5p1p/p3p3/2p5/1p1Pqb2/2P1N2Q/PP1B1nP1/6RK w - - 1 30");
        testResultTrue(isEnded(&bord),"4k3/5p1p/p3p3/2p5/1p1Pqb2/2P1N2Q/PP1B1nP1/6RK w - - 1 30 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"k1Q2b2/pp6/1qp2p2/3P3p/2p2B1P/2P5/PP4r1/1K1R4 b - - 1 34");
        testResultTrue(isEnded(&bord), "k1Q2b2/pp6/1qp2p2/3P3p/2p2B1P/2P5/PP4r1/1K1R4 b - - 1 34 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"1k6/pp2pp1p/1b6/1P5P/4PPb1/5K2/2RNB3/6r1 w - - 5 40");
        testResultTrue(isEnded(&bord),"1k6/pp2pp1p/1b6/1P5P/4PPb1/5K2/2RNB3/6r1 w - - 5 40 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/kp6/p1p5/3p3p/8/5pNP/6q1/6K1 w - - 2 63");
        testResultTrue(isEnded(&bord),"8/kp6/p1p5/3p3p/8/5pNP/6q1/6K1 w - - 2 63 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"1k6/pp5p/2p5/2P1pp2/Q1b5/P1N1P2P/3r2r1/R5KR w - - 0 26");
        testResultTrue(isEnded(&bord),"1k6/pp5p/2p5/2P1pp2/Q1b5/P1N1P2P/3r2r1/R5KR w - - 0 26 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/5pk1/1R4p1/p2q4/3P2Pp/3QP2P/5P1K/7r w - - 3 40");
        testResultTrue(isEnded(&bord),"8/5pk1/1R4p1/p2q4/3P2Pp/3QP2P/5P1K/7r w - - 3 40 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/5Q2/1k2pBp1/p2pP1P1/Kq1P2B1/8/P7/8 w - - 4 52");
        testResultTrue(isEnded(&bord),"8/5Q2/1k2pBp1/p2pP1P1/Kq1P2B1/8/P7/8 w - - 4 52 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"5Qk1/5p1p/6N1/8/q2P4/2P3P1/5PKP/8 b - - 0 47");
        testResultTrue(isEnded(&bord),"5Qk1/5p1p/6N1/8/q2P4/2P3P1/5PKP/8 b - - 0 47 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"5r2/pp1Q3R/2pk1b2/2q1p3/2PnK3/1P2P3/P7/3R4 b - - 4 32");
        testResultTrue(isEnded(&bord),"5r2/pp1Q3R/2pk1b2/2q1p3/2PnK3/1P2P3/P7/3R4 b - - 4 32 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/1q5p/p6k/5PPb/2PB3Q/3p3P/4P3/6K1 b - - 0 41");
        testResultTrue(isEnded(&bord),"8/1q5p/p6k/5PPb/2PB3Q/3p3P/4P3/6K1 b - - 0 41 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"2rq1r2/6Qk/3p2pp/1p3b1N/1PB2P2/3P3P/6P1/4R1K1 b - - 6 37");
        testResultTrue(isEnded(&bord),"2rq1r2/6Qk/3p2pp/1p3b1N/1PB2P2/3P3P/6P1/4R1K1 b - - 6 37 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"5R2/p7/1b6/5p2/4Rk1P/3r1P2/6K1/8 b - - 3 49");
        testResultTrue(isEnded(&bord),"5R2/p7/1b6/5p2/4Rk1P/3r1P2/6K1/8 b - - 3 49 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"6R1/5p1p/5R1k/pp6/5PP1/1Pn2K2/P6r/8 b - - 0 40");
        testResultTrue(isEnded(&bord),"6R1/5p1p/5R1k/pp6/5PP1/1Pn2K2/P6r/8 b - - 0 40 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"3r2k1/1p4pp/5p2/p2p1P1Q/3P2RN/3bq3/6PP/2R2K2 w - - 2 27");
        testResultTrue(isEnded(&bord),"3r2k1/1p4pp/5p2/p2p1P1Q/3P2RN/3bq3/6PP/2R2K2 w - - 2 27 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"1k6/1P3p2/3R2p1/5rKp/6nP/6P1/8/8 w - - 14 49");
        testResultTrue(isEnded(&bord),"1k6/1P3p2/3R2p1/5rKp/6nP/6P1/8/8 w - - 14 49 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"3k4/3b1Q2/2qK4/4P3/1PB5/p1P5/8/8 w - - 4 60");
        testResultTrue(isEnded(&bord),"3k4/3b1Q2/2qK4/4P3/1PB5/p1P5/8/8 w - - 4 60 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"r3bk1Q/1p2bp2/6R1/p1nBP3/8/8/Pq3PPP/3R2K1 b - - 0 26");
        testResultTrue(isEnded(&bord),"r3bk1Q/1p2bp2/6R1/p1nBP3/8/8/Pq3PPP/3R2K1 b - - 0 26 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/5K2/6Qk/8/8/5P2/8/8 b - - 2 76");
        testResultTrue(isEnded(&bord),"8/5K2/6Qk/8/8/5P2/8/8 b - - 2 76 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"4k1R1/7R/8/5P2/5q2/8/5P2/5BK1 b - - 8 58");
        testResultTrue(isEnded(&bord),"4k1R1/7R/8/5P2/5q2/8/5P2/5BK1 b - - 8 58 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/5R2/2bN1kpp/Pp1B4/n3PP2/P2P2P1/5K1P/2r5 b - - 0 39");
        testResultTrue(isEnded(&bord),"8/5R2/2bN1kpp/Pp1B4/n3PP2/P2P2P1/5K1P/2r5 b - - 0 39 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"r4k2/pp1n1Q2/2p1p3/3q3B/3P4/P6P/1PP2P2/2KR2r1 b - - 0 23");
        testResultTrue(isEnded(&bord),"r4k2/pp1n1Q2/2p1p3/3q3B/3P4/P6P/1PP2P2/2KR2r1 b - - 0 23 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"k7/P7/1K6/8/4B3/8/8/8 b - - 2 72");
        testResultTrue(isEnded(&bord),"k7/P7/1K6/8/4B3/8/8/8 b - - 2 72 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/6pp/4P2k/6QP/5rPK/R7 b - - 1 57");
        testResultTrue(isEnded(&bord),"8/8/8/6pp/4P2k/6QP/5rPK/R7 b - - 1 57 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"4r3/ppprqkQp/8/5P2/4b2n/PP1p4/1B1P2PP/4RRK1 b - - 4 27");
        testResultTrue(isEnded(&bord),"4r3/ppprqkQp/8/5P2/4b2n/PP1p4/1B1P2PP/4RRK1 b - - 4 27 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"2n1Rk2/pp1r1ppp/3q4/8/8/QP1P1PPP/P4PK1/4R3 b - - 0 29");
        testResultTrue(isEnded(&bord),"2n1Rk2/pp1r1ppp/3q4/8/8/QP1P1PPP/P4PK1/4R3 b - - 0 29 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"4q3/8/kQ1P4/P7/4pP1P/8/1P3PK1/8 b - - 10 69");
        testResultTrue(isEnded(&bord),"4q3/8/kQ1P4/P7/4pP1P/8/1P3PK1/8 b - - 10 69 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/7r/2R3Q1/1p1p1pk1/4q3/6P1/1N3P2/6K1 b - - 0 42");
        testResultTrue(isEnded(&bord),"8/7r/2R3Q1/1p1p1pk1/4q3/6P1/1N3P2/6K1 b - - 0 42 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"5k1Q/2q2p2/4b3/5N2/2Pp2pP/r7/6P1/4R2K b - - 2 44");
        testResultTrue(isEnded(&bord),"5k1Q/2q2p2/4b3/5N2/2Pp2pP/r7/6P1/4R2K b - - 2 44 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/3R4/1BP5/1P4K1/4k3/4Q3 b - - 4 65");
        testResultTrue(isEnded(&bord),"8/8/8/3R4/1BP5/1P4K1/4k3/4Q3 b - - 4 65 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"5K1k/7b/7R/8/8/8/8/8 b - - 39 74");
        testResultTrue(isEnded(&bord),"5K1k/7b/7R/8/8/8/8/8 b - - 39 74 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"k7/P7/PK6/8/8/8/8/8 b - - 0 86");
        testResultTrue(isEnded(&bord),"k7/P7/PK6/8/8/8/8/8 b - - 0 86 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"7k/7P/7K/8/8/8/8/8 b - - 2 76");
        testResultTrue(isEnded(&bord),"7k/7P/7K/8/8/8/8/8 b - - 2 76 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/2b5/8/8/8/7k/7p/7K w - - 2 75");
        testResultTrue(isEnded(&bord),"8/2b5/8/8/8/7k/7p/7K w - - 2 75 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/6k1/6p1/4q1N1/6R1/6K1 w - - 100 123");
        testResultTrue(isEnded(&bord),"8/8/8/6k1/6p1/4q1N1/6R1/6K1 w - - 100 123 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/1q5k/6p1/5p2/3Q3P/6KP/8/8 w - - 100 114");
        testResultTrue(isEnded(&bord),"8/1q5k/6p1/5p2/3Q3P/6KP/8/8 w - - 100 114 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/6k1/7p/7K w - - 0 67");
        testResultTrue(isEnded(&bord),"8/8/8/8/8/6k1/7p/7K w - - 0 67 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"k7/1pK5/1P6/8/8/8/8/8 b - - 4 56");
        testResultTrue(isEnded(&bord),"k7/1pK5/1P6/8/8/8/8/8 b - - 4 56 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/6k1/3KB1p1/6B1/5P2/8/8/4r3 w - - 100 123");
        testResultTrue(isEnded(&bord),"8/6k1/3KB1p1/6B1/5P2/8/8/4r3 w - - 100 123 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/6b1/8/6k1/7p/7K w - - 0 63");
        testResultTrue(isEnded(&bord),"8/8/8/6b1/8/6k1/7p/7K w - - 0 63 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"6R1/8/8/5N1k/5K2/8/8/8 b - - 0 64");
        testResultTrue(isEnded(&bord),"6R1/8/8/5N1k/5K2/8/8/8 b - - 0 64 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/6k1/6p1/6K1 w - - 2 78");
        testResultTrue(isEnded(&bord),"8/8/8/8/8/6k1/6p1/6K1 w - - 2 78 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"k7/8/K7/P3B3/8/8/8/8 b - - 2 71");
        testResultTrue(isEnded(&bord),"k7/8/K7/P3B3/8/8/8/8 b - - 2 71 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"6r1/5k1K/7P/8/8/8/8/8 w - - 0 65");
        testResultTrue(isEnded(&bord),"6r1/5k1K/7P/8/8/8/8/8 w - - 0 65 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"K3Q3/6pk/2R3p1/4p1P1/4P2P/8/8/8 b - - 0 60");
        testResultTrue(isEnded(&bord),"K3Q3/6pk/2R3p1/4p1P1/4P2P/8/8/8 b - - 0 60 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/5p2/5p2/P4P1p/1P3P1k/7P/7K b - - 0 47");
        testResultTrue(isEnded(&bord),"8/8/5p2/5p2/P4P1p/1P3P1k/7P/7K b - - 0 47 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"7K/7P/8/8/8/5kr1/8/8 w - - 0 66");
        testResultTrue(isEnded(&bord),"7K/7P/8/8/8/5kr1/8/8 w - - 0 66 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"k7/8/1K6/8/p4B2/P7/8/8 b - - 4 68");
        testResultTrue(isEnded(&bord),"k7/8/1K6/8/p4B2/P7/8/8 b - - 4 68 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"k7/PpK5/1P6/8/8/8/8/8 b - - 2 94");
        testResultTrue(isEnded(&bord),"k7/PpK5/1P6/8/8/8/8/8 b - - 2 94 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"7k/8/4B1KP/8/8/8/8/8 b - - 4 123");
        testResultTrue(isEnded(&bord),"7k/8/4B1KP/8/8/8/8/8 b - - 4 123 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"7k/7P/6K1/8/8/8/8/8 b - - 0 86");
        testResultTrue(isEnded(&bord),"7k/7P/6K1/8/8/8/8/8 b - - 0 86 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/1P6/8/8/8/4Q2K/8/7k b - - 0 69");
        testResultTrue(isEnded(&bord),"8/1P6/8/8/8/4Q2K/8/7k b - - 0 69 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/6k1/7p/7K w - - 0 59");
        testResultTrue(isEnded(&bord),"8/8/8/8/8/6k1/7p/7K w - - 0 59 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"7k/7P/7K/p7/P7/8/8/8 b - - 2 58");
        testResultTrue(isEnded(&bord),"7k/7P/7K/p7/P7/8/8/8 b - - 2 58 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"k7/P7/1K6/8/8/8/8/8 b - - 0 77");
        testResultTrue(isEnded(&bord),"k7/P7/1K6/8/8/8/8/8 b - - 0 77 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"R5bk/8/7K/8/8/8/8/8 b - - 14 74");
        testResultTrue(isEnded(&bord),"R5bk/8/7K/8/8/8/8/8 b - - 14 74 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/6k1/6p1/6K1 w - - 2 59");
        testResultTrue(isEnded(&bord),"8/8/8/8/8/6k1/6p1/6K1 w - - 2 59 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/5KR1/5B2/5k2 b - - 0 99");
        testResultTrue(isEnded(&bord), "8/8/8/8/8/5KR1/5B2/5k2 b - - 0 99 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"5K1k/8/6P1/8/7P/3B4/8/8 b - - 0 84");
        testResultTrue(isEnded(&bord),"5K1k/8/6P1/8/7P/3B4/8/8 b - - 0 84 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"5k2/5P2/5K2/8/8/8/8/8 b - - 2 100");
        testResultTrue(isEnded(&bord),"5k2/5P2/5K2/8/8/8/8/8 b - - 2 100 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"2k5/2P5/2K5/8/8/8/4R3/8 b - - 0 83");
        testResultTrue(isEnded(&bord),"2k5/2P5/2K5/8/8/8/4R3/8 b - - 0 83 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"2k5/2P1R3/2K5/8/8/8/8/8 b - - 0 88");
        testResultTrue(isEnded(&bord),"2k5/2P1R3/2K5/8/8/8/8/8 b - - 0 88 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"5k2/5P1p/5K1P/8/8/8/8/8 b - - 2 87");
        testResultTrue(isEnded(&bord),"5k2/5P1p/5K1P/8/8/8/8/8 b - - 2 87 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/7k/7p/7K w - - 2 74");
        testResultTrue(isEnded(&bord),"8/8/8/8/8/7k/7p/7K w - - 2 74 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/3r2pk/6r1/2K5/1R1BR3/8/8/8 b - - 100 110");
        testResultTrue(isEnded(&bord),"8/3r2pk/6r1/2K5/1R1BR3/8/8/8 b - - 100 110 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/8/5K1p/7k b - - 1 67");
        testResultTrue(isEnded(&bord),"8/8/8/8/8/8/5K1p/7k b - - 1 67 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"1k6/1r6/3Q4/2K5/8/8/8/8 b - - 100 129");
        testResultTrue(isEnded(&bord),"1k6/1r6/3Q4/2K5/8/8/8/8 b - - 100 129 is a game over");

        setupEmpty(&bord);
        readInFen(&bord,"8/8/8/8/8/3k4/3p4/3K4 w - - 2 69");
        testResultTrue(isEnded(&bord),"8/8/8/8/8/3k4/3p4/3K4 w - - 2 69 is a game over");
    }
}