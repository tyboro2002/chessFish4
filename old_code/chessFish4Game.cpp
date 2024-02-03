#include "chessFish4Game.h"

using namespace std;

void runGame() {
    Board bord{};
    Action action;
    ActionList actionList;
    TranspositionTable transpositionTable;
    PositionTracker positionTracker;
    //setupEmpty(&bord);
    //std::string fen = "4kb1r/p2ppppp/8/8/8/8/P1PPPPPP/RQ2KB1R w - - 0 1";
    //readInFen(&bord, &fen);
    setup(&bord);
    printBoard(&bord);
    int duration;
    cout << "how many moves do you want to play max?" << endl;
    cin >> duration;
    if (duration == -1) {
        duration = std::numeric_limits<int>::max();
    }
    cout << "wich engine do you want to player A to be?" << endl;
    printEngines();
    int engineNumberA;
    cin >> engineNumberA;
    cout << "wich engine do you want to player B to be?" << endl;
    printEngines();
    int engineNumberB;
    cin >> engineNumberB;
    int i = 0;
    int depth;
    int time; //time in milliseconds
    if (engineNumberA == 2 || engineNumberB == 2 || engineNumberA == 3 || engineNumberB == 3) {
        cout << "what depth do you want" << endl;
        cin >> depth;
    }else {
        depth = 0;
    }

    if (engineNumberA == 4 || engineNumberB == 4) {
        cout << "what time do you wanna give each action (in milliseconds)" << endl;
        cin >> time;
        cout << "what max depth do you want" << endl;
        cin >> depth;
    }else {
        time = 0;
    }
    bool finished = false;
    while (i < duration && !finished /*&& !isDraw(&bord, &positionTracker)*/) { //old code removed
        cout << "action: " << i+1<< endl;
        printBoard(&bord);
        //GenLegalMoveList(&actionList, &bord, &positionTracker);
        getLegalMoves(&bord,&actionList);
        if (actionList.count == 0) {
            cout << "white has no more legal moves" << endl;
            finished = true;
        }
        if (engineNumberA == 0) {
            askForMove(&bord, &action, &actionList);
            cout << "You selected: " << actionToString(&action) << endl;
            //makeMove(&bord, &action, &positionTracker);
            movePiece(&bord,&action);
        }else if (engineNumberA == 1) {
            makeRandomMove(&bord, &actionList);
        }else if (engineNumberA == 2) {
            //makeMiniMaxMove(&bord, &actionList, depth, true, &transpositionTable, &positionTracker);
        }else if (engineNumberA == 3) {
            //makeMiniMaxOptimizedMove(&bord, &actionList, depth, true, &transpositionTable, &positionTracker);
        }else if (engineNumberA == 4) {
            //makeMiniMaxOptimizedItterativeDeepeningMove(&bord, &actionList, depth, true, &transpositionTable, &positionTracker, time);
        }
        printBoard(&bord);
        getLegalMoves(&bord, &actionList);
        if (actionList.count == 0) {
            cout << "black has no more legal moves" << endl;
            finished = true;
        }
        if (!finished /*&& !isDraw(&bord, &positionTracker)*/) { //old code removed
            if (engineNumberB == 0) {
                askForMove(&bord, &action, &actionList);
                cout << "You selected: " << actionToString(&action) << endl;
                movePiece(&bord, &action);
            }
            else if (engineNumberB == 1) {
                makeRandomMove(&bord, &actionList);
            }else if (engineNumberB == 2) {
                //makeMiniMaxMove(&bord, &actionList, depth, true, &transpositionTable, &positionTracker);
            }else if (engineNumberB == 3) {
                //makeMiniMaxOptimizedMove(&bord, &actionList, depth, true, &transpositionTable, &positionTracker);
            }else if (engineNumberB == 4) {
                //makeMiniMaxOptimizedItterativeDeepeningMove(&bord, &actionList, depth, true, &transpositionTable, &positionTracker, time);
            }
            i++;
        }
    }
    /*//old code removed
    if (isDraw(&bord, &positionTracker) && actionList.count != 0) {
        cout << "a draw occured" << endl;
    }
    if (isDraw(&bord, &positionTracker) && (engineNumberA == 0 || engineNumberB == 0) ) {
        printPositionRecords(&positionTracker);
    }
     */
}
