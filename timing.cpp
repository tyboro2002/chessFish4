#include "timing.h"

using namespace std;

void time_code() {
    Board bord;
    Move move;
    MOVELIST moveList;
    // Clear move list
    moveList.count = 0;   // set each field for each move

    setup(&bord);
    //printBoard(&bord);
    //timing code
    // Get the starting timestamp
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000000; ++i) {
        //bitmap_white_pawns(bord) | bitmap_black_pawns(bord) | bitmap_black_king(bord) | bitmap_white_king(bord);
        //all_white_attacks(&bord, 1) | all_black_attacks(&bord, 1);
        //bitmap_white_knight(E5, &bord);
    }

    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    cout << "first aproach needed:" << endl;
    std::cout << "Time taken: " << duration << " microseconds" << std::endl;


    moveList.count = 0;   // set each field for each move

    setup(&bord);
    //printBoard(&bord);
    //timing code
    // Get the starting timestamp
    startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000000; ++i) {
        //bitmap_white_pawns(bord) | bitmap_black_pawns(bord) | bitmap_black_king(bord) | bitmap_white_king(bord);
        //all_white_attacks(&bord, 1) | all_black_attacks(&bord, 1);
        //bitmap_white_knight_speed_test(E5, &bord);
    }

    // Get the ending timestamp
    endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    cout << "second aproach needed:" << endl;
    std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}

void time_parralel_evaluation() {
    Board bord;
    MOVELIST moveList;
    setup(&bord);
    PositionTracker positionTracker;
    GenLegalMoveList(&moveList, &bord, &positionTracker);
    auto startTime = std::chrono::high_resolution_clock::now();

    //for (int i = 0; i < 1000000; ++i) orderMoves(&bord,&moveList);

    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    printMoveList(&moveList);
    cout << "first aproach needed:" << endl;
    std::cout << "Time taken: " << duration << " microseconds" << std::endl;

    GenLegalMoveList(&moveList, &bord, &positionTracker);
    startTime = std::chrono::high_resolution_clock::now();

    //for (int i = 0; i < 1000; ++i) orderMovesThreaded(&bord, &moveList);

    // Get the ending timestamp
    endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    printMoveList(&moveList);
    cout << "second aproach needed:" << endl;
    std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}

void time_bitwise_code() {
    //timing code
    // Get the starting timestamp
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000000; ++i) {
        countSetBits(3);
    }

    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}


void time_minimax_code() {
    Board bord;
    Move move;
    MOVELIST moveList;
    TranspositionTable transpositionTable;
    PositionTracker positionTracker;
    // Clear move list
    moveList.count = 0;   // set each field for each move
    setup(&bord);

    //timing code
    // Get the starting timestamp
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1; ++i){
        minimax_root(&bord, 1000, false, &move, &moveList, &transpositionTable, &positionTracker);
    }

    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}

void time_mate_test_code() {
    //timing code
    // Get the starting timestamp
    auto startTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1; ++i) {
        runAutomatedTestsSilent();
    }

    // Get the ending timestamp
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds (change to other duration units as needed)
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "Time taken: " << duration << " microseconds" << std::endl;
}