#include "game.h"

// bit manipulation macros
#define get_bit(bitboard, index) (bitboard & (1ULL << index))
#define set_bit(bitboard, index) (bitboard |= (1ULL << index))
#define pop_bit(bitboard, index) (get_bit(bitboard, index) ? bitboard ^= (1ULL << index) : 0)


// Function to copy values from bordIn to bordOut
void copyBoard(const Board* bordIn, Board* bordOut) {
    if (!bordIn || !bordOut) return; // Handle nullptr input
    // Copy the members from bordIn to bordOut
    bordOut->rook = bordIn->rook;
    bordOut->knight = bordIn->knight;
    bordOut->bishop = bordIn->bishop;
    bordOut->queen = bordIn->queen;
    bordOut->king = bordIn->king;
    bordOut->pawn = bordIn->pawn;
    bordOut->white = bordIn->white;
    bordOut->black = bordIn->black;
    bordOut->whiteToPlay = bordIn->whiteToPlay;
    bordOut->whiteKingsideCastle = bordIn->whiteKingsideCastle;
    bordOut->whiteQueensideCastle = bordIn->whiteQueensideCastle;
    bordOut->blackKingsideCastle = bordIn->blackKingsideCastle;
    bordOut->blackQueensideCastle = bordIn->blackQueensideCastle;
    bordOut->enPassentValid = bordIn->enPassentValid;
    bordOut->enPassantTarget = bordIn->enPassantTarget;
    bordOut->halfmoveClock = bordIn->halfmoveClock;
    bordOut->reserved = bordIn->reserved;
    //bordOut->extra = bordIn->extra;
}

int findMoveIndex(MOVELIST* moveList, Move* targetMove) { //only used in old code
    for (int i = 0; i < moveList->count; ++i) {
        if (moveList->moves[i] == *targetMove) return i;  // Return the index of the matching move
    }
    return -1;  // Return -1 if the move was not found
}

using namespace std;

std::string squareToString(const int square) {
    std::string file = "HGFEDCBA";
    std::string rank = "12345678";

    int index = square;
    int fileIndex = index % 8;
    int rankIndex = index / 8;

    return std::string(1, file[fileIndex]) + rank[rankIndex];
}

std::string actionToString(const Action* move) {
    return squareToString(move->src) + squareToString(move->dst) + (move->special == Promote_Rook ? "r" : "") + (move->special == Promote_Knight ? "n" : "") + (move->special == Promote_Bishop ? "b" : "") + (move->special == Promote_Queen ? "q" : "");
}

std::string moveToStringShort(const Move* move) {
    return squareToString(move->src) + squareToString(move->dst);
}

void printPositionRecords(const PositionTracker* tracker) { //only used in old code
    for (const auto& entry : tracker->getPositionRecords()) {
        const size_t positionHash = entry.first;
        const PositionRecord& record = entry.second;

        // You can add more information to print depending on your needs
        std::cout << "Position Hash: " << positionHash << std::endl;
        std::cout << "Occurrences: " << record.occurrences << std::endl;
        std::cout << "--------------------------" << std::endl;
    }
}

/*
* all pieces attacking the white king
*/
U64 white_checking_pieces(Board* bord) {
    U64 attackers = 0ULL; // empty bitboard
    int king_position = get_ls1b_index(bord->king & bord->white);
    attackers |= bitmap_white_rook(king_position, bord) & (bord->rook & bord->black);
    attackers |= bitmap_white_knight(king_position, bord) & (bord->knight & bord->black);
    attackers |= bitmap_white_bishop(king_position, bord) & (bord->bishop & bord->black);
    attackers |= bitmap_white_queen(king_position, bord) & (bord->queen & bord->black);
    attackers |= bitmap_white_pawn(king_position, bord) & (bord->pawn & bord->black);
    return attackers;
}

/*
* all pieces attacking the black king
*/
U64 black_checking_pieces(Board* bord) {
    U64 attackers = 0ULL; // empty bitboard
    int king_position = get_ls1b_index(bord->king & bord->black);
    attackers |= bitmap_black_rook(king_position, bord) & (bord->rook & bord->white);
    attackers |= bitmap_black_knight(king_position, bord) & (bord->knight & bord->white);
    attackers |= bitmap_black_bishop(king_position, bord) & (bord->bishop & bord->white);
    attackers |= bitmap_black_queen(king_position, bord) & (bord->queen & bord->white);
    attackers |= bitmap_black_pawn(king_position, bord) & (bord->pawn & bord->white);
    return attackers;
}

U64 squaresBetweenBitmap(int startSquare, int endSquare) { //only used in old code
    startSquare = 63 - startSquare;
    endSquare = 63 - endSquare;
    U64 result = 0ULL;

    // Swap if endSquare is smaller than startSquare
    if (endSquare < startSquare) {
        std::swap(startSquare, endSquare);
    }

    // Calculate the difference in ranks (rows) and files (columns)
    int rankDifference = (endSquare / 8) - (startSquare / 8);
    int fileDifference = (endSquare % 8) - (startSquare % 8);

    /* //removed because normaly all inputs are valid
    if(rankDifference != fileDifference){
        return 0ULL;
    }
    */

    // Calculate the direction of movement for rank and file
    int rankDirection = (rankDifference > 0) ? 1 : (rankDifference < 0) ? -1 : 0;
    int fileDirection = (fileDifference > 0) ? 1 : (fileDifference < 0) ? -1 : 0;

    int currentSquare = startSquare + rankDirection * 8 + fileDirection;
    while (currentSquare != endSquare) {
        result |= (1ULL << currentSquare);
        currentSquare += rankDirection * 8 + fileDirection;
    }

    return result;
}

/*
* all moves generating and putting them in a movelist
*/
void white_pawn_moves(int position, MOVELIST* movelist, Board* bord) { //only used in old code
    U64 destinations = bitmap_white_pawn(position, bord);// &white_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations & eightRow) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = SPECIAL_PROMOTION_QUEEN;
        if (((1ULL << 63) >> m->dst) & bord->black) {
            m->capture = m->dst;
        }
        else {
            m->capture = -52;
        }
        m++;
        movelist->count++;

        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = SPECIAL_PROMOTION_KNIGHT;
        if (((1ULL << 63) >> m->dst) & bord->black) {
            m->capture = m->dst;
        }
        else {
            m->capture = -52;
        }
        m++;
        movelist->count++;

        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = SPECIAL_PROMOTION_ROOK;
        if (((1ULL << 63) >> m->dst) & bord->black) {
            m->capture = m->dst;
        }
        else {
            m->capture = -52;
        }
        m++;
        movelist->count++;

        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = SPECIAL_PROMOTION_BISHOP;
        if (((1ULL << 63) >> m->dst) & bord->black) {
            m->capture = m->dst;
        }
        else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
    while (destinations) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63-bitIndex);
        if (m->src - m->dst == 16) {
            m->special = SPECIAL_WPAWN_2SQUARES;
        }else if (m->dst == (63 - get_ls1b_index(en_passent_target(bord)))) {
            m->special = SPECIAL_WEN_PASSANT;
            m->capture = m->dst + 8;
        }else {
            m->special = NOT_SPECIAL;
        }

        if (((1ULL << 63) >> m->dst) & bord->black) {
            m->capture = m->dst;
        }else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
}

void black_pawn_moves(int position, MOVELIST* movelist, Board* bord) { //only used in old code
    U64 destinations = bitmap_black_pawn(position, bord);// &black_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations & oneRow) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = SPECIAL_PROMOTION_QUEEN;
        if (((1ULL << 63) >> m->dst) & bord->white) {
            m->capture = m->dst;
        }
        else {
            m->capture = -52;
        }
        m++;
        movelist->count++;

        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = SPECIAL_PROMOTION_KNIGHT;
        if (((1ULL << 63) >> m->dst) & bord->white) {
            m->capture = m->dst;
        }
        else {
            m->capture = -52;
        }
        m++;
        movelist->count++;

        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = SPECIAL_PROMOTION_ROOK;
        if (((1ULL << 63) >> m->dst) & bord->white) {
            m->capture = m->dst;
        }
        else {
            m->capture = -52;
        }
        m++;
        movelist->count++;

        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = SPECIAL_PROMOTION_BISHOP;
        if (((1ULL << 63) >> m->dst) & bord->white) {
            m->capture = m->dst;
        }
        else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
    while (destinations) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        if (m->dst - m->src == 16) {
            m->special = SPECIAL_BPAWN_2SQUARES;
        }
        else if (m->dst == (63 - get_ls1b_index(en_passent_target(bord)))) {
            m->special = SPECIAL_BEN_PASSANT;
            m->capture = m->dst - 8;
        }
        else {
            m->special = NOT_SPECIAL;
        }
        if (((1ULL << 63) >> m->dst) & bord->white) {
            m->capture = m->dst;
        }else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
}

void white_rook_moves(int position, MOVELIST* movelist, Board* bord) { //only used in old code
    U64 destinations = bitmap_white_rook(position, bord);// &white_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = NOT_SPECIAL;
        if (((1ULL << 63) >> m->dst) & bord->black) {
            m->capture = m->dst;
        }else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
}

void black_rook_moves(int position, MOVELIST* movelist, Board* bord) { //only used in old code
    U64 destinations = bitmap_black_rook(position, bord);// &black_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = NOT_SPECIAL;
        if (((1ULL << 63) >> m->dst) & bord->white) {
            m->capture = m->dst;
        }else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
}

void white_knight_moves(int position, MOVELIST* movelist, Board* bord) { //only used in old code
    U64 destinations = bitmap_white_knight(position, bord);// &white_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = NOT_SPECIAL;
        if (((1ULL << 63) >> m->dst) & bord->black) {
            m->capture = m->dst;
        }else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
}

void black_knight_moves(int position, MOVELIST* movelist, Board* bord) { //only used in old code
    U64 destinations = bitmap_black_knight(position, bord);// &black_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = NOT_SPECIAL;
        if (((1ULL << 63) >> m->dst) & bord->white) {
            m->capture = m->dst;
        }else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
}

void white_bishop_moves(int position, MOVELIST* movelist, Board* bord) { //only used in old code
    U64 destinations = bitmap_white_bishop(position, bord);// &white_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = NOT_SPECIAL;
        if (((1ULL << 63) >> m->dst) & bord->black) {
            m->capture = m->dst;
        }else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
}

void black_bishop_moves(int position, MOVELIST* movelist, Board* bord) { //only used in old code
    U64 destinations = bitmap_black_bishop(position, bord);// &black_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = NOT_SPECIAL;
        if (((1ULL << 63) >> m->dst) & bord->white) {
            m->capture = m->dst;
        }else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
}

void white_queen_moves(int position, MOVELIST* movelist, Board* bord) { //only used in old code
    U64 destinations = (bitmap_white_bishop(position, bord) | bitmap_white_rook(position, bord));// &white_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = NOT_SPECIAL;
        if (((1ULL << 63) >> m->dst) & bord->black) {
            m->capture = m->dst;
        }else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
}

void black_queen_moves(int position, MOVELIST* movelist, Board* bord) { //only used in old code
    U64 destinations = (bitmap_black_bishop(position, bord) | bitmap_black_rook(position, bord));// &black_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        m->special = NOT_SPECIAL;
        if (((1ULL << 63) >> m->dst) & bord->white) {
            m->capture = m->dst;
        }else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
}

void white_king_moves(int position, MOVELIST* movelist, Board* bord) { //only used in old code
    U64 destinations = bitmap_white_king(position, bord);// &~white_checking_bitmap(bord);;
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        if (m->src == E1) {
            if (m->dst == G1) {
                m->special = SPECIAL_WK_CASTLING;
            }
            else if (m->dst == C1) {
                m->special = SPECIAL_WQ_CASTLING;
            }
            else {
                m->special = NOT_SPECIAL;
            }
        }else {
            m->special = NOT_SPECIAL;
        }
        if (((1ULL << 63) >> m->dst) & bord->black) {
            m->capture = m->dst;
        }else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
}

void black_king_moves(int position, MOVELIST* movelist, Board* bord) { //only used in old code
    U64 destinations = bitmap_black_king(position, bord);// &~black_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = get_ls1b_index(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        if (m->src == E8) {
            if (m->dst == G8) {
                m->special = SPECIAL_BK_CASTLING;
            }
            else if (m->dst == C8) {
                m->special = SPECIAL_BQ_CASTLING;
            }
            else {
                m->special = NOT_SPECIAL;
            }
        }else {
            m->special = NOT_SPECIAL;
        }
        if (((1ULL << 63) >> m->dst) & bord->white) {
            m->capture = m->dst;
        }else {
            m->capture = -52;
        }
        m++;
        movelist->count++;
        destinations &= (destinations - 1); // Clear the least significant set bit
    }
}

void white_moves(MOVELIST* movelist, Board* bord) { //only used in old code
    U64 wrook = bord->white & bord->rook;
    U64 wknight = bord->white & bord->knight;
    U64 wbishop = bord->white & bord->bishop;
    U64 wqueen = bord->white & bord->queen;
    U64 wking = bord->white & bord->king;
    U64 wpawn = bord->white & bord->pawn;
    while (wking) {
        int bitIndex = get_ls1b_index(wking); // Get the index of the least significant set bit
        white_king_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        wking &= (wking - 1); // Clear the least significant set bit
    }
    if (countSetBits(white_checking_pieces(bord)) > 1) {
        return;
    }
    while (wrook) {
        int bitIndex = get_ls1b_index(wrook); // Get the index of the least significant set bit
        white_rook_moves(63 - bitIndex,movelist,bord); // Call the corresponding function with the index of the set bit
        wrook &= (wrook - 1); // Clear the least significant set bit
    }
    while (wknight) {
        int bitIndex = get_ls1b_index(wknight); // Get the index of the least significant set bit
        white_knight_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        wknight &= (wknight - 1); // Clear the least significant set bit
    }
    while (wbishop) {
        int bitIndex = get_ls1b_index(wbishop); // Get the index of the least significant set bit
        white_bishop_moves(63 - bitIndex, movelist, bord); (63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        wbishop &= (wbishop - 1); // Clear the least significant set bit
    }
    while (wqueen) {
        int bitIndex = get_ls1b_index(wqueen); // Get the index of the least significant set bit
        white_queen_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        wqueen &= (wqueen - 1); // Clear the least significant set bit
    }
    while (wpawn) {
        int bitIndex = get_ls1b_index(wpawn); // Get the index of the least significant set bit
        white_pawn_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        wpawn &= (wpawn - 1); // Clear the least significant set bit
    }
}

void black_moves(MOVELIST* movelist, Board* bord) { //only used in old code
    U64 brook = bord->black & bord->rook;
    U64 bknight = bord->black & bord->knight;
    U64 bbishop = bord->black & bord->bishop;
    U64 bqueen = bord->black & bord->queen;
    U64 bking = bord->black & bord->king;
    U64 bpawn = bord->black & bord->pawn;
    while (bking) {
        int bitIndex = get_ls1b_index(bking); // Get the index of the least significant set bit
        black_king_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        bking &= (bking - 1); // Clear the least significant set bit
    }
    if (countSetBits(black_checking_pieces(bord)) > 1) {
        return;
    }
    while (brook) {
        int bitIndex = get_ls1b_index(brook); // Get the index of the least significant set bit
        black_rook_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        brook &= (brook - 1); // Clear the least significant set bit
    }
    while (bknight) {
        int bitIndex = get_ls1b_index(bknight); // Get the index of the least significant set bit
        black_knight_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        bknight &= (bknight - 1); // Clear the least significant set bit
    }
    while (bbishop) {
        int bitIndex = get_ls1b_index(bbishop); // Get the index of the least significant set bit
        black_bishop_moves(63 - bitIndex, movelist, bord); (63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        bbishop &= (bbishop - 1); // Clear the least significant set bit
    }
    while (bqueen) {
        int bitIndex = get_ls1b_index(bqueen); // Get the index of the least significant set bit
        black_queen_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        bqueen &= (bqueen - 1); // Clear the least significant set bit
    }
    while (bpawn) {
        int bitIndex = get_ls1b_index(bpawn); // Get the index of the least significant set bit
        black_pawn_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        bpawn &= (bpawn - 1); // Clear the least significant set bit
    }
}

bool EvaluateQuick(Board* bord) { //only used in old code
    return calculateKingDanger(bord) == 0ULL;
}


/*
* returns true if we have moves and false otherwise (false also means checkmated of stalemate)
*/
bool weHaveMoves(Board* bord) { //only used in old code
    ActionList actionList;
    getLegalMoves(bord,&actionList);
    return actionList.count;
}

bool inCheck(Board* bord) { //only used in old code (maybe used in new code if optimised)
    return calculateKingDanger(bord);
}

DRAWTYPE isDraw(Board* bord, PositionTracker* positionTracker) { //only used in old code
    if (bord->halfmoveClock >= 100) return DRAWTYPE_50MOVE;
    if (positionTracker->getPositionOccurrences(bord) >= 3) return DRAWTYPE_REPITITION;
    U64 nonKingPieces = bord->bishop | bord->rook | bord->knight | bord->queen | bord->pawn;
    if((nonKingPieces & bord->white) == 0 || (nonKingPieces & bord->black) == 0) return DRAWTYPE_INSUFFICIENT_AUTO;
    if (countSetBits(bord->white | bord->black) <= 2) return DRAWTYPE_INSUFFICIENT_AUTO;
    return NOT_DRAW;
}

// Function to convert 12 sets of 64-bit numbers to a 64-character string
std::string convertTo64CharString(U64 rook, U64 knight, U64 bishop, U64 queen, U64 king, U64 pawn, U64 white, U64 black) {
    std::string result;
    for (int i = 63; i >= 0; i--) {
        uint64_t bitMask = (1ULL << i);
        char representativeChar = '.'; // Default character
        if ((rook & bitMask) != 0) {
            if ((white & bitMask) != 0) { representativeChar = 'R';}
            else if ((black & bitMask) != 0) { representativeChar = 'r';}
        }else if ((knight & bitMask) != 0) {
            if ((white & bitMask) != 0) { representativeChar = 'N'; }
            else if ((black & bitMask) != 0) { representativeChar = 'n'; }
        }else if ((bishop & bitMask) != 0) {
            if ((white & bitMask) != 0) { representativeChar = 'B'; }
            else if ((black & bitMask) != 0) { representativeChar = 'b'; }
        }else if ((queen & bitMask) != 0) {
            if ((white & bitMask) != 0) { representativeChar = 'Q'; }
            else if ((black & bitMask) != 0) { representativeChar = 'q'; }
        }else if ((king & bitMask) != 0) {
            if ((white & bitMask) != 0) { representativeChar = 'K'; }
            else if ((black & bitMask) != 0) { representativeChar = 'k'; }
        }else if ((pawn & bitMask) != 0) {
            if ((white & bitMask) != 0) { representativeChar = 'P'; }
            else if ((black & bitMask) != 0) { representativeChar = 'p'; }
        }
        result.push_back(representativeChar);
    }
    return result;
}

/*
* this function overlays the string with the character for each place where the coresponding bit in bitpatroon is 1
* (bitpatroon and string always have same length)
*/
void overlay(std::string* str, U64 bitpattern, char character) { //only used in old code
    std::string& myString = *str;
    size_t strLength = myString.length();

    for (size_t i = 0; i < strLength; ++i) {
        if (bitpattern & ((1ULL << 63) >> i)) {
            myString[i] = character;
        }
    }
}

void printBitBoard(U64 bitbord, const std::string& extra) {
    std::cout << endl;
    std::cout << extra << endl;
    std::string temp;
    temp = std::bitset<64>(bitbord).to_string();
    std::cout << "8 " << temp.substr(0, 8) << endl;
    std::cout << "7 " << temp.substr(8, 8) << endl;
    std::cout << "6 " << temp.substr(16, 8) << endl;
    std::cout << "5 " << temp.substr(24, 8) << endl;
    std::cout << "4 " << temp.substr(32, 8) << endl;
    std::cout << "3 " << temp.substr(40, 8) << endl;
    std::cout << "2 " << temp.substr(48, 8) << endl;
    std::cout << "1 " << temp.substr(56, 8) << endl;
    std::cout << "  abcdefgh" << endl;
}

void printBoard(Board* bord){
    std::string temp = convertTo64CharString(bord->rook, bord->knight, bord->bishop, bord->queen, bord->king, bord->pawn, bord->white, bord->black);
    std::cout << endl;
    if (bord->whiteToPlay) {
        cout << "white to play:" << endl;
    }else {
        cout << "black to play:" << endl;
    }
    std::cout << "8 " << temp.substr(0,8) << endl;
    std::cout << "7 " << temp.substr(8,8) << endl;
    std::cout << "6 " << temp.substr(16,8) << endl;
    std::cout << "5 " << temp.substr(24,8) << endl;
    std::cout << "4 " << temp.substr(32,8) << endl;
    std::cout << "3 " << temp.substr(40,8) << endl;
    std::cout << "2 " << temp.substr(48,8) << endl;
    std::cout << "1 " << temp.substr(56,8) << endl;
    std::cout << "  abcdefgh" << endl;
}

void printFancyBoard(Board* bord){
    std::string temp = convertTo64CharString(bord->rook, bord->knight, bord->bishop, bord->queen, bord->king, bord->pawn, bord->white, bord->black);
    // Insert '|' between each character
    for (size_t i = 1; i < temp.length(); i += 2) temp.insert(i, 1, '|');
    for (size_t i = 1; i < temp.length(); i += 2) temp.insert(i, 1, ' ');
    // Replace all "." with " "
    std::replace(temp.begin(), temp.end(), '.', ' ');

    std::cout << endl;
    if (bord->whiteToPlay) {
        cout << "white to play:" << endl;
    }else {
        cout << "black to play:" << endl;
    }
    std::cout << " +---+---+---+---+---+---+---+---+" << std::endl;
    std::cout << " | " << temp.substr(0,29) << " | 8" << endl;
    std::cout << " +---+---+---+---+---+---+---+---+" << std::endl;
    std::cout << " | " << temp.substr(32,29) << " | 7" << endl;
    std::cout << " +---+---+---+---+---+---+---+---+" << std::endl;
    std::cout << " | " << temp.substr(64,29) << " | 6" << endl;
    std::cout << " +---+---+---+---+---+---+---+---+" << std::endl;
    std::cout << " | " << temp.substr(96,29) << " | 5" << endl;
    std::cout << " +---+---+---+---+---+---+---+---+" << std::endl;
    std::cout << " | " << temp.substr(128,29) << " | 4" << endl;
    std::cout << " +---+---+---+---+---+---+---+---+" << std::endl;
    std::cout << " | " << temp.substr(160,29) << " | 3" << endl;
    std::cout << " +---+---+---+---+---+---+---+---+" << std::endl;
    std::cout << " | " << temp.substr(192,29) << " | 2" << endl;
    std::cout << " +---+---+---+---+---+---+---+---+" << std::endl;
    std::cout << " | " << temp.substr(224,29) << " | 1" << endl;
    std::cout << " +---+---+---+---+---+---+---+---+" << std::endl;
    std::cout << "   a   b   c   d   e   f   g   h" << endl;
    std::cout << "casteling: " << (bord->whiteKingsideCastle?"K":"-") << (bord->whiteQueensideCastle?"Q":"-") << (bord->blackKingsideCastle?"k":"-") << (bord->blackQueensideCastle?"q":"-") << endl;
    //std::cout << "Fen: " << generateFEN(bord) << endl;
    //std::cout << "Key: " << generateZobristKey(bord) << endl;
    //std::cout << "Checkers: "<< generateCheckers(bord) << endl;
}

void setup(Board* bord) {
    bord->rook   = 0b1000000100000000000000000000000000000000000000000000000010000001;
    bord->knight = 0b0100001000000000000000000000000000000000000000000000000001000010;
    bord->bishop = 0b0010010000000000000000000000000000000000000000000000000000100100;
    bord->queen  = 0b0001000000000000000000000000000000000000000000000000000000010000;
    bord->king   = 0b0000100000000000000000000000000000000000000000000000000000001000;
    bord->pawn   = 0b0000000011111111000000000000000000000000000000001111111100000000;
    bord->white  = 0b0000000000000000000000000000000000000000000000001111111111111111;
    bord->black  = 0b1111111111111111000000000000000000000000000000000000000000000000;
    //extra setup
    bord->whiteToPlay = 1;
    bord->whiteKingsideCastle = 1;
    bord->whiteQueensideCastle = 1;
    bord->blackKingsideCastle = 1;
    bord->blackQueensideCastle = 1;
    bord->enPassentValid = 0;
    bord->enPassantTarget = 0;
    bord->halfmoveClock = 0;
    bord->reserved = 0;
}

void setupEmpty(Board* bord) {
    bord->rook   = 0b0000000000000000000000000000000000000000000000000000000000000000;
    bord->knight = 0b0000000000000000000000000000000000000000000000000000000000000000;
    bord->bishop = 0b0000000000000000000000000000000000000000000000000000000000000000;
    bord->queen  = 0b0000000000000000000000000000000000000000000000000000000000000000;
    bord->king   = 0b0000000000000000000000000000000000000000000000000000000000000000;
    bord->pawn   = 0b0000000000000000000000000000000000000000000000000000000000000000;
    bord->white  = 0b0000000000000000000000000000000000000000000000000000000000000000;
    bord->black  = 0b0000000000000000000000000000000000000000000000000000000000000000;
    // extra setup
    bord->whiteToPlay = 1;
    bord->whiteKingsideCastle = 1;
    bord->whiteQueensideCastle = 1;
    bord->blackKingsideCastle = 1;
    bord->blackQueensideCastle = 1;
    bord->enPassentValid = 0;
    bord->enPassantTarget = 0;
    bord->halfmoveClock = 0;
    bord->reserved = 0;
}

void addPiece(Board* bord, const Pieces piece,const int square) {
    U64 placeBit = 1ULL << square;
    switch (piece) {
        case WROOK:
            bord->white |= placeBit;
            bord->rook  |= placeBit;
            return;
        case WKNIGHT:
            bord->white |= placeBit;
            bord->knight |= placeBit;
            return;
        case WBISCHOP:
            bord->white |= placeBit;
            bord->bishop |= placeBit;
            return;
        case WQUEEN:
            bord->white |= placeBit;
            bord->queen |= placeBit;
            return;
        case WKING:
            bord->white |= placeBit;
            bord->king |= placeBit;
            return;
        case WPAWN:
            bord->white |= placeBit;
            bord->pawn |= placeBit;
            return;
        case BROOK:
            bord->black |= placeBit;
            bord->rook |= placeBit;
            return;
        case BKNIGHT:
            bord->black |= placeBit;
            bord->knight |= placeBit;
            return;
        case BBISCHOP:
            bord->black |= placeBit;
            bord->bishop |= placeBit;
            return;
        case BQUEEN:
            bord->black |= placeBit;
            bord->queen |= placeBit;
            return;
        case BKING:
            bord->black |= placeBit;
            bord->king |= placeBit;
            return;
        case BPAWN:
            bord->black |= placeBit;
            bord->pawn |= placeBit;
            return;
        case NOPIECE:
            return;
    }
}

Pieces pieceAt(const int square,const Board* board) { //only used in old code
    U64 sq = 1ULL << square;  // Calculate the bit corresponding to the square

    // Check for white pieces
    if (board->white & sq) {
        if (board->rook & sq) return WROOK;
        if (board->knight & sq) return WKNIGHT;
        if (board->bishop & sq) return WBISCHOP;
        if (board->queen & sq) return WQUEEN;
        if (board->king & sq) return WKING;
        if (board->pawn & sq) return WPAWN;
    }
        // Check for black pieces
    else if (board->black & sq) {
        if (board->rook & sq) return BROOK;
        if (board->knight & sq) return BKNIGHT;
        if (board->bishop & sq) return BBISCHOP;
        if (board->queen & sq) return BQUEEN;
        if (board->king & sq) return BKING;
        if (board->pawn & sq) return BPAWN;
    }

    return NOPIECE;
}

Square stringToSquare(const std::string& inp) { //only used in old code
    static const std::unordered_map<std::string, Square> squareMap = {
            {"a8", A8}, {"a7", A7}, {"a6", A6}, {"a5", A5}, {"a4", A4}, {"a3", A3}, {"a2", A2}, {"a1", A1},
            {"b8", B8}, {"b7", B7}, {"b6", B6}, {"b5", B5}, {"b4", B4}, {"b3", B3}, {"b2", B2}, {"b1", B1},
            {"c8", C8}, {"c7", C7}, {"c6", C6}, {"c5", C5}, {"c4", C4}, {"c3", C3}, {"c2", C2}, {"c1", C1},
            {"d8", D8}, {"d7", D7}, {"d6", D6}, {"d5", D5}, {"d4", D4}, {"d3", D3}, {"d2", D2}, {"d1", D1},
            {"e8", E8}, {"e7", E7}, {"e6", E6}, {"e5", E5}, {"e4", E4}, {"e3", E3}, {"e2", E2}, {"e1", E1},
            {"f8", F8}, {"f7", F7}, {"f6", F6}, {"f5", F5}, {"f4", F4}, {"f3", F3}, {"f2", F2}, {"f1", F1},
            {"g8", G8}, {"g7", G7}, {"g6", G6}, {"g5", G5}, {"g4", G4}, {"g3", G3}, {"g2", G2}, {"g1", G1},
            {"h8", H8}, {"h7", H7}, {"h6", H6}, {"h5", H5}, {"h4", H4}, {"h3", H3}, {"h2", H2}, {"h1", H1}
    };
    auto it = squareMap.find(inp);
    return (it != squareMap.end()) ? it->second : A8;
}

std::string specialToString(SPECIAL special) { //only used in old code
    static const std::unordered_map<SPECIAL, std::string> specialStrings = {
            {NOT_SPECIAL, ""},
            {SPECIAL_WK_CASTLING, "White is kingside castling"},
            {SPECIAL_BK_CASTLING, "Black is kingside castling"},
            {SPECIAL_WQ_CASTLING, "White is queenside castling"},
            {SPECIAL_BQ_CASTLING, "Black is queenside castling"},
            {SPECIAL_PROMOTION_QUEEN, "Promotes a pawn to a Queen"},
            {SPECIAL_PROMOTION_ROOK, "Promotes a pawn to a Rook"},
            {SPECIAL_PROMOTION_BISHOP, "Promotes a pawn to a Bishop"},
            {SPECIAL_PROMOTION_KNIGHT, "Promotes a pawn to a Knight"},
            {SPECIAL_WPAWN_2SQUARES, "Moves a White pawn 2 squares"},
            {SPECIAL_BPAWN_2SQUARES, "Moves a black pawn 2 squares"},
            {SPECIAL_WEN_PASSANT, "White captures en passant"},
            {SPECIAL_BEN_PASSANT, "Black captures en passant"}
    };

    auto it = specialStrings.find(special);
    return (it != specialStrings.end()) ? it->second : "";
}

// Helper function to map characters to piece enums
Pieces charToPiece(char c) { //only used in old code
    switch (std::toupper(c)) {
        case 'R': return WROOK;
        case 'N': return WKNIGHT;
        case 'B': return WBISCHOP;
        case 'Q': return WQUEEN;
        case 'K': return WKING;
        case 'P': return WPAWN;
        case 'r': return BROOK;
        case 'n': return BKNIGHT;
        case 'b': return BBISCHOP;
        case 'q': return BQUEEN;
        case 'k': return BKING;
        case 'p': return BPAWN;
        default: return NOPIECE;
    }
}


// Function to make a move
void makeMove(Board* bord, Move* move, PositionTracker* positionTracker) { //only used in old code
    //bord->extra = (bord->extra & ~0x7F) | ((bord->extra + 1) & 0x7F);
    U64 fromBit = 1ULL << move->src;
    U64 toBit = 1ULL << move->dst;
    bool capture = false;

    if ((move->special == NOT_SPECIAL) ||
        ( move->special == SPECIAL_WPAWN_2SQUARES) ||
        (move->special == SPECIAL_BPAWN_2SQUARES) ||
        (move->special == SPECIAL_WEN_PASSANT) ||
        (move->special == SPECIAL_BEN_PASSANT) ||
        (move->special == SPECIAL_PROMOTION_BISHOP) ||
        (move->special == SPECIAL_PROMOTION_KNIGHT) ||
        (move->special == SPECIAL_PROMOTION_QUEEN) ||
        (move->special == SPECIAL_PROMOTION_ROOK)) {
        if (((bord->white | bord->black) & toBit) != 0) {
            //clear all bitboards on the to position
            bord->rook &= ~toBit;
            bord->knight &= ~toBit;
            bord->bishop &= ~toBit;
            bord->queen &= ~toBit;
            bord->king &= ~toBit;
            bord->pawn &= ~toBit;
            bord->white &= ~toBit;
            bord->black &= ~toBit;
            capture = true;
        }

        if ((bord->white & fromBit) != 0) {
            bord->white ^= fromBit; // Clear the source square
            bord->white |= toBit;   // Set the destination square
        }else if ((bord->black & fromBit) != 0) {
            bord->black ^= fromBit; // Clear the source square
            bord->black |= toBit;   // Set the destination square
        }

        if ((bord->rook & fromBit) != 0) {
            bord->halfmoveClock += 1;
            //bord->extra = (bord->extra & ~0x7F) | (((bord->extra & 0x7F) + 1) & 0x7F);
            if ((((1ULL << 63) >> (A8)) & fromBit) != 0) {
                bord->blackQueensideCastle = 0;
                //bord->extra &= ~(1ULL << 14); // remove black queenside casteling ability
            }
            else if ((((1ULL << 63) >> (H8)) & fromBit) != 0) {
                bord->blackKingsideCastle = 0;
                //bord->extra &= ~(1ULL << 15); // remove black kingside casteling ability
            }
            else if ((((1ULL << 63) >> (A1)) & fromBit) != 0) {
                bord->whiteQueensideCastle = 0;
                //bord->extra &= ~(1ULL << 16); // remove white queenside casteling ability
            }
            else if ((((1ULL << 63) >> (H1)) & fromBit) != 0) {
                bord->whiteKingsideCastle = 0;
                //bord->extra &= ~(1ULL << 17); // remove white kingside casteling ability
            }
            bord->rook ^= fromBit; // Clear the source square
            bord->rook |= toBit;   // Set the destination square
        }else if ((bord->knight & fromBit) != 0) {
            bord->halfmoveClock += 1;
            bord->knight ^= fromBit; // Clear the source square
            bord->knight |= toBit;   // Set the destination square
        }else if ((bord->bishop & fromBit) != 0) {
            bord->halfmoveClock += 1;
            bord->bishop ^= fromBit; // Clear the source square
            bord->bishop |= toBit;   // Set the destination square
        }else if ((bord->queen & fromBit) != 0) {
            bord->halfmoveClock += 1;
            bord->queen ^= fromBit; // Clear the source square
            bord->queen |= toBit;   // Set the destination square
        }else if ((bord->king & fromBit) != 0) {
            bord->halfmoveClock += 1;
            if ((bord->white & fromBit) != 0) { //remove white casteling ability
                bord->whiteKingsideCastle = 0;
                bord->whiteQueensideCastle = 0;
                //bord->extra &= ~((((1ULL << 2) - 1) << 16));
            }
            else if ((bord->black & fromBit) != 0) { //remove black casteling ability
                bord->blackKingsideCastle = 0;
                bord->blackQueensideCastle = 0;
                //bord->extra &= ~((((1ULL << 2) - 1) << 14));
            }
            bord->king ^= fromBit; // Clear the source square
            bord->king |= toBit;   // Set the destination square
        }else if ((bord->pawn & fromBit) != 0) {
            bord->halfmoveClock = 0;
            bord->pawn ^= fromBit; // Clear the source square
            bord->pawn |= toBit;   // Set the destination square
        }
    }
    bord->enPassentValid = 0;
    //bord->extra &= ~((((1ULL << 7) - 1) << 7)); // remove en passent target
    if (move->special != NOT_SPECIAL) {
        if (move->special == SPECIAL_WPAWN_2SQUARES) {
            bord->enPassentValid = 1;
            bord->enPassantTarget = (move->dst + 8);
            //bord->extra |= ((1ULL << 13));
            //bord->extra |= ((move->dst + 8) << 7);
        }
        else if (move->special == SPECIAL_BPAWN_2SQUARES) {
            bord->enPassentValid = 1;
            bord->enPassantTarget = (move->dst - 8);
            //bord->extra |= ((1ULL << 13));
            //bord->extra |= ((move->dst - 8) << 7);
        }
        else if (move->special == SPECIAL_WEN_PASSANT) {
            U64 enPassentBit = ((1ULL << 63) >> (move->dst + 8));
            //clear all bitboards on the to position of the en passant pawn
            bord->pawn &= ~enPassentBit;
            bord->white &= ~enPassentBit;
            bord->black &= ~enPassentBit;
        }
        else if (move->special == SPECIAL_BEN_PASSANT) {
            U64 enPassentBit = ((1ULL << 63) >> (move->dst - 8));
            //clear all bitboards on the to position of the en passant pawn
            bord->pawn &= ~enPassentBit;
            bord->white &= ~enPassentBit;
            bord->black &= ~enPassentBit;
        }
        else if (move->special == SPECIAL_WK_CASTLING) { //remove white casteling ability
            bord->whiteKingsideCastle = 0;
            bord->whiteQueensideCastle = 0;
            //bord->extra &= ~((((1ULL << 2) - 1) << 16));
            U64 rookSQ = ((1ULL << 63) >> (H1));
            bord->rook &= ~rookSQ;
            bord->white &= ~rookSQ;
            U64 kingSQ = ((1ULL << 63) >> (E1));
            bord->king &= ~kingSQ;
            bord->white &= ~kingSQ;

            U64 newRookSQ = ((1ULL << 63) >> (F1));
            bord->rook |= newRookSQ;
            bord->white |= newRookSQ;
            U64 newKingSQ = ((1ULL << 63) >> (G1));
            bord->king |= newKingSQ;
            bord->white |= newKingSQ;
        }
        else if (move->special == SPECIAL_BK_CASTLING) { //remove black casteling ability
            bord->blackKingsideCastle = 0;
            bord->blackQueensideCastle = 0;
            //bord->extra &= ~((((1ULL << 2) - 1) << 14));
            U64 rookSQ = ((1ULL << 63) >> (H8));
            bord->rook &= ~rookSQ;
            bord->black &= ~rookSQ;
            U64 kingSQ = ((1ULL << 63) >> (E8));
            bord->king &= ~kingSQ;
            bord->black &= ~kingSQ;

            U64 newRookSQ = ((1ULL << 63) >> (F8));
            bord->rook |= newRookSQ;
            bord->black |= newRookSQ;
            U64 newKingSQ = ((1ULL << 63) >> (G8));
            bord->king |= newKingSQ;
            bord->black |= newKingSQ;
        }
        else if (move->special == SPECIAL_WQ_CASTLING) { //remove white casteling ability
            bord->whiteKingsideCastle = 0;
            bord->whiteQueensideCastle = 0;
            //bord->extra &= ~((((1ULL << 2) - 1) << 16));
            U64 rookSQ = ((1ULL << 63) >> (A1));
            bord->rook &= ~rookSQ;
            bord->white &= ~rookSQ;
            U64 kingSQ = ((1ULL << 63) >> (E1));
            bord->king &= ~kingSQ;
            bord->white &= ~kingSQ;

            U64 newRookSQ = ((1ULL << 63) >> (D1));
            bord->rook |= newRookSQ;
            bord->white |= newRookSQ;
            U64 newKingSQ = ((1ULL << 63) >> (C1));
            bord->king |= newKingSQ;
            bord->white |= newKingSQ;
        }
        else if (move->special == SPECIAL_BQ_CASTLING) { //remove black casteling ability
            bord->blackKingsideCastle = 0;
            bord->blackQueensideCastle = 0;
            //bord->extra &= ~((((1ULL << 2) - 1) << 14));
            U64 rookSQ = ((1ULL << 63) >> (A8));
            bord->rook &= ~rookSQ;
            bord->black &= ~rookSQ;
            U64 kingSQ = ((1ULL << 63) >> (E8));
            bord->king &= ~kingSQ;
            bord->black &= ~kingSQ;

            U64 newRookSQ = ((1ULL << 63) >> (D8));
            bord->rook |= newRookSQ;
            bord->black |= newRookSQ;
            U64 newKingSQ = ((1ULL << 63) >> (C8));
            bord->king |= newKingSQ;
            bord->black |= newKingSQ;
        }
        else if (move->special == SPECIAL_PROMOTION_BISHOP) {
            if (bord->whiteToPlay) {
                U64 promotionPawn = eightRow & bord->pawn & bord->white;
                bord->pawn &= ~promotionPawn;
                bord->bishop |= promotionPawn;
            }else{
                U64 promotionPawn = oneRow & bord->pawn & bord->black;
                bord->pawn &= ~promotionPawn;
                bord->bishop |= promotionPawn;
            }
        }
        else if (move->special == SPECIAL_PROMOTION_KNIGHT) {
            if (bord->whiteToPlay) {
                U64 promotionPawn = eightRow & bord->pawn & bord->white;
                bord->pawn &= ~promotionPawn;
                bord->knight |= promotionPawn;
            }else {
                U64 promotionPawn = oneRow & bord->pawn & bord->black;
                bord->pawn &= ~promotionPawn;
                bord->knight |= promotionPawn;
            }
        }
        else if (move->special == SPECIAL_PROMOTION_QUEEN) {
            if (bord->whiteToPlay) {
                U64 promotionPawn = eightRow & bord->pawn & bord->white;
                bord->pawn &= ~promotionPawn;
                bord->queen |= promotionPawn;
            }else {
                U64 promotionPawn = oneRow & bord->pawn & bord->black;
                bord->pawn &= ~promotionPawn;
                bord->queen |= promotionPawn;
            }
        }
        else if (move->special == SPECIAL_PROMOTION_ROOK) {
            if (bord->whiteToPlay) {
                U64 promotionPawn = eightRow & bord->pawn & bord->white;
                bord->pawn &= ~promotionPawn;
                bord->rook |= promotionPawn;
            }else {
                U64 promotionPawn = oneRow & bord->pawn & bord->black;
                bord->pawn &= ~promotionPawn;
                bord->rook |= promotionPawn;
            }
        }
    }
    bord->whiteToPlay = bord->whiteToPlay ? 0 : 1;
    //bord->extra ^= (1ULL << 18); // swap playing player
    positionTracker->addPosition(bord);
    if (capture) bord->halfmoveClock = 0;

}

/* =====================================
 *  below is the new code
 * ===================================== */

bool checkBoard(Board* bord){
    // check if there are no pawns on first or last row
    if(bord->pawn & (eightRow | oneRow)) return false;
    // check if each side has a king
    if(countSetBits(bord->white & bord->king) != 1) return false;
    if(countSetBits(bord->black & bord->king) != 1) return false;
    // TODO check if kings can be taken
    // check if a side has more pieces than it starts with
    if(countSetBits(bord->white) > 16) return false;
    if(countSetBits(bord->black) > 16) return false;
    // check if a side has more pawns than it starts with
    if(countSetBits(bord->white & bord->pawn) > 8) return false;
    if(countSetBits(bord->black & bord->pawn) > 8) return false;
    return true;
}

void getLegalMoves(Board* bord, ActionList* legal){
    ActionList pseudo;
    getAllMoves(bord, &pseudo);

    /* initiate bitmaps to redo the move */
    saveBoard();

    for (int i = 0; i<pseudo.count;i++) {
        movePiece(bord, &(pseudo.moves[i]));
        bord->whiteToPlay ^= 1;
        if (calculateKingDanger(bord) == 0ULL) legal->moves[legal->count++] = pseudo.moves[i];
        resetBoard();
    }
}

U64 calculateBitmapFromSquare(int square, ActionList* actionList){
    U64 att = 0ULL;
    for (int i = 0; i<actionList->count; i++){
        Action action = actionList->moves[i];
        if(action.src == square) att |= (1ULL << action.dst);
    }
    return att;
}

void readInFen(Board* bord,const char* fen) {
    int index = 63;
    while (*fen != ' '){
        //cout << "'" << fen << "' index: " << index << endl;
        switch (*fen) {
            case 'r':
                bord->black |= 1ULL << index;
                bord->rook |= 1ULL << index;
                break;
            case 'b':
                bord->black |= 1ULL << index;
                bord->bishop |= 1ULL << index;
                break;
            case 'q':
                bord->black |= 1ULL << index;
                bord->queen |= 1ULL << index;
                break;
            case 'p':
                bord->black |= 1ULL << index;
                bord->pawn |= 1ULL << index;
                break;
            case 'n':
                bord->black |= 1ULL << index;
                bord->knight |= 1ULL << index;
                break;
            case 'k':
                bord->black |= 1ULL << index;
                bord->king |= 1ULL << index;
                break;
            case '/':
                index++;
                break;
            case 'R':
                bord->white |= 1ULL << index;
                bord->rook |= 1ULL << index;
                break;
            case 'B':
                bord->white |= 1ULL << index;
                bord->bishop |= 1ULL << index;
                break;
            case 'Q':
                bord->white |= 1ULL << index;
                bord->queen |= 1ULL << index;
                break;
            case 'P':
                bord->white |= 1ULL << index;
                bord->pawn |= 1ULL << index;
                break;
            case 'N':
                bord->white |= 1ULL << index;
                bord->knight |= 1ULL << index;
                break;
            case 'K':
                bord->white |= 1ULL << index;
                bord->king |= 1ULL << index;
                break;
            case '1':
                break;
            case '2':
                index -= 1;
                break;
            case '3':
                index -= 2;
                break;
            case '4':
                index -= 3;
                break;
            case '5':
                index -= 4;
                break;
            case '6':
                index -= 5;
                break;
            case '7':
                index -= 6;
                break;
            case '8':
                index -= 7;
                break;
            default:
                std::cerr << "Invalid character in FEN: " << *fen << std::endl;
                exit(EXIT_FAILURE);
        }
        index--;
        fen++;
    }
    //cout << "'" << fen << "' index: " << index << endl;
    fen++; // parse the space
    bord->whiteToPlay = (*fen == 'w');
    fen+=2; // parse the playing side and the space
    while (*fen != ' '){
        switch (*fen) {
            case 'K': bord->whiteKingsideCastle = 1;
            case 'k': bord->blackKingsideCastle = 1;
            case 'Q': bord->whiteQueensideCastle = 1;
            case 'q': bord->blackQueensideCastle = 1;
            case '-': break;
        }
        fen++;
    }
    fen++; // parse the space
    if(*fen != '-'){
        bord->enPassentValid = 1;
        // Concatenate characters to form a string
        std::string target = std::string(1, fen[0]) + fen[1];
        bord->enPassantTarget = stringToSquare(target);
        fen += 2; // parse the 2 characters
    }else{
        bord->enPassentValid = 0;
        fen++; // parse the -
    }
    fen++; // parse the space
    bord->halfmoveClock = 0;
    while (*fen >= '0' && *fen <= '9') {
        bord->halfmoveClock = bord->halfmoveClock * 10 + (*fen - '0');
        fen++;
    }
    //fen++; // parse the space
    //TODO? parse the fullmove clock (The number of the full moves. It starts at 1 and is incremented after Black's move.)
}

void printAction(const Action* action){
    cout << squareToString(action->src)<< squareToString(action->dst) << (action->special == Promote_Rook ? "r" : "") << (action->special == Promote_Knight ? "n" : "") << (action->special == Promote_Bishop ? "b" : "") << (action->special == Promote_Queen ? "q" : "") << endl;

}

void printActionList(const ActionList* actionList){
    for (int i = 0; i<actionList->count; i++) printAction(&actionList->moves[i]);
}