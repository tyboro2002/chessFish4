#include "OpeningBook.h"

// The Italian Game (E2E4, E7E5, G1F3, B8C6, F1C4)
// The Sicilian Defense (E2E4, C7C5)
// The French Defense (E2E4, E7E6, D2D4, D7D5)
// The Ruy-Lopez (E2E4, E7E5, G1F3, B8C6, F1B5)
// The Slav Defense (D2D4, D7D5, C2C4, C7C6)
// Queen's Gambit Declined (D2D4, D7D5, C2C4, E7E6)



// implemented but commented out
// Queen's Gambit Accepted (D2D4, D7D5, C2C4, D5C4)

void OpeningBook::fillOpeningBookManualy() {
    Board board{};
    Action action;

    // The Italian Game (E2E4, E7E5, G1F3, B8C6, F1C4)
    {
        setup(&board);

        action = Action{E2, E4};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{E7, E5};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{G1, F3};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{B8, C6};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{F1, C4};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);
    }

    // The Sicilian Defense (E2E4, C7C5)
    {
        setup(&board);

        action = Action{E2, E4};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{C7, C5};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);
    }

    // The French Defense (E2E4, E7E6, D2D4, D7D5)
    {
        setup(&board);

        action = Action{E2, E4};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{E7, E6};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{D2, D4};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{D7, D5};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);
    }

    // The Ruy-Lopez (E2E4, E7E5, G1F3, B8C6, F1B5)
    {
        setup(&board);

        action = Action{E2, E4};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{E7, E5};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{G1, F3};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{B8, C6};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{F1, B5};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);
    }

    // The Slav Defense (D2D4, D7D5, C2C4, C7C6)
    {
        setup(&board);

        action = Action{D2, D4};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{D7, D5};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{C2, C4};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{C7, C6};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);
    }

    // Queen's Gambit Declined (D2D4, D7D5, C2C4, E7E6)
    {
        setup(&board);

        action = Action{D2, D4};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{D7, D5};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{C2, C4};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);

        action = Action{E7, E6};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);
    }



    /***********************
     * commented out lines
     ***********************/

    // Queen's Gambit Accepted (D2D4, D7D5, C2C4, D5C4)
    /*
    {
        setup(&board);

        //Don't add the moves that are the same as the Queen's Gambit Declined so they don't overwelm the book
        action = Action{D2, D4};
        //addPositionToTable(&board,&action);
        movePiece(&board, &action);

        action = Action{D7, D5};
        //addPositionToTable(&board,&action);
        movePiece(&board, &action);

        action = Action{C2, C4};
        //addPositionToTable(&board,&action);
        movePiece(&board, &action);

        action = Action{D5, C4};
        addPositionToTable(&board, &action);
        movePiece(&board, &action);
    }
     */
}
