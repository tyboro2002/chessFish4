#include "game.h"
#include "moves.h"
#include <bitset>
#include <chrono>

#define en_passent_target(bord) (((1ULL<<63) >> (bord->enPassantTarget)) & (bord->enPassentValid ? UINT64_MAX : 0))
#define white_plays(bord) (bord->whiteToPlay == 1)

// bit manipulation macros
#define get_bit(bitboard, index) (bitboard & (1ULL << index))
#define set_bit(bitboard, index) (bitboard |= (1ULL << index))
#define pop_bit(bitboard, index) (get_bit(bitboard, index) ? bitboard ^= (1ULL << index) : 0)


//Pieces lastCapturedPiece = NOPIECE;

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

int findMoveIndex(MOVELIST* moveList, Move* targetMove) {
    for (int i = 0; i < moveList->count; ++i) {
        if (moveList->moves[i] == *targetMove) {
            return i;  // Return the index of the matching move
        }
    }
    return -1;  // Return -1 if the move was not found
}

U64 incrementByOne(U64 number) {
    U64 mask = 1;
    while (number & mask) {
        number &= ~mask; // Clear the current bit
        mask <<= 1;      // Move to the next bit
    }
    number |= mask;     // Set the first cleared bit
    return number;
}

//TODO define constexpr for diagonal moves
constexpr U64 Right(U64 num) {
    return num >> 1;
}

constexpr U64 Left(U64 num) {
    return num << 1;
}

constexpr U64 Up(U64 num) {
    return num << 8;
}

constexpr U64 Down(U64 num) {
    return num >> 8;
}

using namespace std;

bool BitScanForward64(unsigned long& index, U64 number) {
    if (number == 0) {
        return false; // No set bits found
    }

    index = 0;
    while ((number & 1) == 0) {
        number >>= 1;
        ++index;
    }

    return true; // Set bit found
}

int countTrailingZeros(U64 number) {
    unsigned long index;
    if (BitScanForward64(index, number)) {
        return static_cast<int>(index);
    }
    return 64; // Return 64 if the input number is 0
}

int getFirst1BitSquare(U64 number) {
    unsigned long index;
    if (BitScanForward64(index, number)) {
        return 63-static_cast<int>(index);
    }
    return -1; // Return 64 if the input number is 0
}

int countSetBits(U64 number) {
    int count = 0;
    while (number) {
        count++;
        number &= (number - 1); // Clear the least significant set bit
    }
    return count;
}

std::string squareToString(Square square) {
    std::string file = "ABCDEFGH";
    std::string rank = "12345678";

    int index = static_cast<int>(square);
    int fileIndex = index % 8;
    int rankIndex = 7 - index / 8; // Adjusted for reversed numbering

    return std::string(1, file[fileIndex]) + rank[rankIndex];
}

std::string moveToString(Move* move) {
    std::string captureString = "";
    if (move->capture != -52) {
        captureString = " is capturing a piece";
    }
    return "move from: " + squareToString(move->src) +
           " to: " + squareToString(move->dst) + " " +
           specialToString(move->special) +
           captureString;
}

std::string moveToStringShort(Move* move) {
    return squareToString(move->src) + squareToString(move->dst);
}

void printPositionRecords(const PositionTracker* tracker) {
    for (const auto& entry : tracker->getPositionRecords()) {
        const size_t positionHash = entry.first;
        const PositionRecord& record = entry.second;

        // You can add more information to print depending on your needs
        std::cout << "Position Hash: " << positionHash << std::endl;
        std::cout << "Occurrences: " << record.occurrences << std::endl;
        std::cout << "--------------------------" << std::endl;
    }
}

void printMoveList(MOVELIST* moveList) {
    for (int i = 0; i < moveList->count; i++) {
        cout << i << ") " << moveToString(&(moveList->moves[i])) /* << " with value: " << move_value(bord, &moveList->moves[i], false)*/ << endl; //TODO for testing
    }
}

U64 bitmap_all_white_pawns(Board* bord) {
    U64 wpawns = bord->pawn & bord->white; // all positions of white pawns
    U64 doublePawns = (wpawns & twoRow) & ~((bord->white | bord->black) >> 8); // all positions of white pawns able to move 2
    U64 nonCaptures = (((doublePawns << 8) | (doublePawns << 16) | (wpawns << 8)) & (~(bord->white | bord->black))); // all non capturing moves a pawn can do
    U64 captures = ((wpawns & (~border)) << 7 | (wpawns & (~border)) << 9 | (wpawns & H) << 9 | (wpawns & A) << 7); // all capturing moves a pawn can do
    U64 enPassent = en_passent_target(bord); // all squares that are able to be en passented
    return (nonCaptures | (captures & bord->black) | (enPassent & captures));
}

U64 bitmap_all_black_pawns(Board* bord) {
    U64 bpawns = bord->pawn & bord->black; // all positions of black pawns
    U64 doublePawns = (bpawns & sevenRow) & ~((bord->white | bord->black) << 8); // all positions of black pawns able to move 2
    U64 nonCaptures = (((doublePawns >> 8) | (doublePawns >> 16) | (bpawns >> 8)) & (~(bord->white | bord->black))); // all non capturing moves a pawn can do
    U64 captures = ((bpawns & (~border)) >> 7 | (bpawns & (~border)) >> 9 | (bpawns & H) >> 7 | (bpawns & A) >> 9); // all capturing moves a pawn can do
    U64 enPassent = en_passent_target(bord); // all squares that are able to be en passented
    return (nonCaptures | (captures & bord->white) | (enPassent & captures));
}


U64 bitmap_white_pawn(int position, Board* bord) {
    //U64 enPassent = en_passent_target(bord); // all squares that are able to be en passented
    return get_white_pawn_attacks(position, bord->white,bord->black); //TODO en passent
}

U64 bitmap_black_pawn(int position, Board* bord) {
    //U64 enPassent = en_passent_target(bord); // all squares that are able to be en passented
    return get_black_pawn_attacks(position, bord->white,bord->black); //TODO en passent
}

U64 bitmap_white_king(int position, Board* bord) { //TODO king in check
    U64 empty = ~(bord->white | bord->black);
    U64 ret = kingMoves[position];
    if (position == E1) {
        if ((wkcastle & empty) == wkcastle && bord->whiteKingsideCastle  && (h1_mask & (bord->white & bord->rook))) ret |= g1_mask;
        if ((wqcastle & empty) == wqcastle && bord->whiteQueensideCastle && (a1_mask & (bord->white & bord->rook))) ret |= c1_mask;
    }
    return ret & (~bord->white);
}

U64 bitmap_black_king(int position, Board* bord) { //TODO king in check
    U64 empty = ~(bord->white | bord->black);
    U64 ret = kingMoves[position];
    if (position == E8) {
        if ((bkcastle & empty) == bkcastle && bord->blackKingsideCastle  && (h8_mask & (bord->black & bord->rook))) ret |= g8_mask;
        if ((bqcastle & empty) == bqcastle && bord->blackQueensideCastle && (a8_mask & (bord->black & bord->rook))) ret |= c8_mask;
    }
    return ret & (~bord->black);
}

// rook attacks
U64 bitmap_white_rook(int square, Board* bord){
    return rook_attacks_on_the_fly(square, bord->white | bord->black) & (~bord->white);
}

// rook attacks
U64 bitmap_black_rook(int square, Board* bord){
    return rook_attacks_on_the_fly(square, bord->white | bord->black) & (~bord->black);
}

// bishop attacks
U64 bitmap_white_bishop(int square, Board* bord){
    //return bishop_attacks_on_the_fly(square, bord->white | bord->black) & (~bord->white);
    return get_bishop_attacks(square, bord->white | bord->black) & (~bord->white);
}

// bishop attacks
U64 bitmap_black_bishop(int square, Board* bord){
    //return bishop_attacks_on_the_fly(square, bord->white | bord->black) & (~bord->black);
    return get_bishop_attacks(square, bord->white | bord->black) & (~bord->black);
}

// mask knight attacks
U64 bitmap_white_knight(int square, Board* bord) {
    return knightMoves[square] & (~bord->white);
}

// mask knight attacks
U64 bitmap_black_knight(int square, Board* bord) {
    return knightMoves[square] & (~bord->black);
}

U64 bitmap_white_queen(int square, Board* bord) {
    return bitmap_white_rook(square, bord) | bitmap_white_bishop(square, bord);
}

U64 bitmap_black_queen(int square, Board* bord) {
    return bitmap_black_rook(square, bord) | bitmap_black_bishop(square, bord);
}

/*
* all pieces attacking the white king
*/
U64 white_checking_pieces(Board* bord) {
    U64 attackers = 0ULL; // empty bitboard
    int king_position = 63 - countTrailingZeros(bord->king & bord->white);
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
    int king_position = 63 - countTrailingZeros(bord->king & bord->black);
    attackers |= bitmap_black_rook(king_position, bord) & (bord->rook & bord->white);
    attackers |= bitmap_black_knight(king_position, bord) & (bord->knight & bord->white);
    attackers |= bitmap_black_bishop(king_position, bord) & (bord->bishop & bord->white);
    attackers |= bitmap_black_queen(king_position, bord) & (bord->queen & bord->white);
    attackers |= bitmap_black_pawn(king_position, bord) & (bord->pawn & bord->white);
    return attackers;
}

U64 squaresBetweenBitmap(int startSquare, int endSquare) {
    startSquare = 63 - startSquare;
    endSquare = 63 - endSquare;
    U64 result = 0ULL;
    /* //removed because its faster without and not realy needed (i hope)
    // Ensure valid input range (0-63)
    if (startSquare < 0 || startSquare > 63 || endSquare < 0 || endSquare > 63) {
        return result;
    }
    */

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
* returns all 1 if no checking pieces or path to white king if there are
*/
U64 white_checking_bitmap(Board* bord) {
    U64 att_path = 0ULL; //empty bitboard
    U64 checks = white_checking_pieces(bord);
    U64 att_rooks = checks & bord->rook;
    U64 att_knight = checks & bord->knight;
    U64 att_bishop = checks & bord->bishop;
    U64 att_queen = checks & bord->queen;
    U64 att_pawn = checks & bord->pawn;
    int wking_square = getFirst1BitSquare(bord->king & bord->white);
    while (att_rooks) {
        int rook_square = getFirst1BitSquare(att_rooks);
        att_path |= squaresBetweenBitmap(rook_square, wking_square);
        att_rooks &= (att_rooks - 1);
    }
    while (att_bishop) {
        int bishop_square = getFirst1BitSquare(att_bishop);
        att_path |= squaresBetweenBitmap(bishop_square, wking_square);
        att_bishop &= (att_bishop - 1);
    }
    while (att_queen) {
        int queen_square = getFirst1BitSquare(att_queen);
        att_path |= squaresBetweenBitmap(queen_square, wking_square);
        att_queen &= (att_queen - 1);
    }
    att_path |= att_knight;
    att_path |= att_pawn;
    if ((att_path | checks) == 0) {
        return all;
    }
    return att_path | checks;
}

/*
* returns all 1 if no checking pieces or path to black king if there are
*/
U64 black_checking_bitmap(Board* bord) {
    U64 att_path = 0ULL; //empty bitboard
    U64 checks = black_checking_pieces(bord);
    U64 att_rooks = checks & bord->rook;
    U64 att_knight = checks & bord->knight;
    U64 att_bishop = checks & bord->bishop;
    U64 att_queen = checks & bord->queen;
    U64 att_pawn = checks & bord->pawn;
    int bking_square = getFirst1BitSquare(bord->king & bord->black);
    while (att_rooks) {
        int rook_square = getFirst1BitSquare(att_rooks);
        att_path |= squaresBetweenBitmap(rook_square, bking_square);
        att_rooks &= (att_rooks - 1);
    }
    while (att_bishop) {
        int bishop_square = getFirst1BitSquare(att_bishop);
        att_path |= squaresBetweenBitmap(bishop_square, bking_square);
        att_bishop &= (att_bishop - 1);
    }
    while (att_queen) {
        int queen_square = getFirst1BitSquare(att_queen);
        att_path |= squaresBetweenBitmap(queen_square, bking_square);
        att_queen &= (att_queen - 1);
    }
    att_path |= att_knight;
    att_path |= att_pawn;
    if ((att_path | checks) == 0) {
        return all;
    }
    return att_path | checks;
}

/*
* bitmap of al squares reachable by white pieces
*/
U64 all_white_attacks(Board* bord) {
    U64 wrook = bord->white & bord->rook;
    U64 wknight = bord->white & bord->knight;
    U64 wbishop = bord->white & bord->bishop;
    U64 wqueen = bord->white & bord->queen;
    U64 wking = bord->white & bord->king;
    U64 wpawn = bord->white & bord->pawn;
    U64 attacks = 0ULL;
    attacks |= bitmap_white_king(63-countTrailingZeros(wking), bord);
    if (countSetBits(white_checking_pieces(bord)) > 1) {
        return attacks;
    }
    while (wrook) {
        int bitIndex = countTrailingZeros(wrook); // Get the index of the least significant set bit
        attacks |= bitmap_white_rook(63-bitIndex,bord); // Call the corresponding function with the index of the set bit
        wrook &= (wrook - 1); // Clear the least significant set bit
    }
    while (wknight) {
        int bitIndex = countTrailingZeros(wknight); // Get the index of the least significant set bit
        attacks |= bitmap_white_knight(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        wknight &= (wknight - 1); // Clear the least significant set bit
    }
    while (wbishop) {
        int bitIndex = countTrailingZeros(wbishop); // Get the index of the least significant set bit
        attacks |= bitmap_white_bishop(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        wbishop &= (wbishop - 1); // Clear the least significant set bit
    }
    while (wqueen) {
        int bitIndex = countTrailingZeros(wqueen); // Get the index of the least significant set bit
        attacks |= bitmap_white_bishop(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        attacks |= bitmap_white_rook(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        wqueen &= (wqueen - 1); // Clear the least significant set bit
    }
    attacks |= bitmap_all_white_pawns(bord);
    return attacks;// &white_checking_bitmap(bord);
}

/*
* bitmap of al squares reachable by black pieces
*/
U64 all_black_attacks(Board* bord) {
    U64 brook = bord->black & bord->rook;
    U64 bknight = bord->black & bord->knight;
    U64 bbishop = bord->black & bord->bishop;
    U64 bqueen = bord->black & bord->queen;
    U64 bking = bord->black & bord->king;
    U64 bpawn = bord->black & bord->pawn;
    U64 attacks = 0ULL;
    attacks |= bitmap_black_king(63 - countTrailingZeros(bking), bord);;
    if (countSetBits(black_checking_pieces(bord)) > 1) {
        return attacks;
    }
    while (brook) {
        int bitIndex = countTrailingZeros(brook); // Get the index of the least significant set bit
        attacks |= bitmap_black_rook(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        brook &= (brook - 1); // Clear the least significant set bit
    }
    while (bknight) {
        int bitIndex = countTrailingZeros(bknight); // Get the index of the least significant set bit
        attacks |= bitmap_black_knight(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        bknight &= (bknight - 1); // Clear the least significant set bit
    }
    while (bbishop) {
        int bitIndex = countTrailingZeros(bbishop); // Get the index of the least significant set bit
        attacks |= bitmap_black_bishop(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        bbishop &= (bbishop - 1); // Clear the least significant set bit
    }
    while (bqueen) {
        int bitIndex = countTrailingZeros(bqueen); // Get the index of the least significant set bit
        attacks |= bitmap_black_bishop(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        attacks |= bitmap_black_rook(63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        bqueen &= (bqueen - 1); // Clear the least significant set bit
    }
    attacks |= bitmap_all_black_pawns(bord);
    return attacks;// &black_checking_bitmap(bord);
}

/*
* all moves generating and putting them in a movelist
*/
void white_pawn_moves(int position, MOVELIST* movelist, Board* bord) {
    U64 destinations = bitmap_white_pawn(position, bord);// &white_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations & eightRow) {
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
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
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63-bitIndex);
        if (m->src - m->dst == 16) {
            m->special = SPECIAL_WPAWN_2SQUARES;
        }else if (m->dst == (63-countTrailingZeros(en_passent_target(bord)))) {
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

void black_pawn_moves(int position, MOVELIST* movelist, Board* bord) {
    U64 destinations = bitmap_black_pawn(position, bord);// &black_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations & oneRow) {
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
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
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
        m->src = (Square)position;
        m->dst = (Square)(63 - bitIndex);
        if (m->dst - m->src == 16) {
            m->special = SPECIAL_BPAWN_2SQUARES;
        }
        else if (m->dst == (63 - countTrailingZeros(en_passent_target(bord)))) {
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

void white_rook_moves(int position, MOVELIST* movelist, Board* bord) {
    U64 destinations = bitmap_white_rook(position, bord);// &white_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
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

void black_rook_moves(int position, MOVELIST* movelist, Board* bord) {
    U64 destinations = bitmap_black_rook(position, bord);// &black_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
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

void white_knight_moves(int position, MOVELIST* movelist, Board* bord) {
    U64 destinations = bitmap_white_knight(position, bord);// &white_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
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

void black_knight_moves(int position, MOVELIST* movelist, Board* bord) {
    U64 destinations = bitmap_black_knight(position, bord);// &black_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
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

void white_bishop_moves(int position, MOVELIST* movelist, Board* bord) {
    U64 destinations = bitmap_white_bishop(position, bord);// &white_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
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

void black_bishop_moves(int position, MOVELIST* movelist, Board* bord) {
    U64 destinations = bitmap_black_bishop(position, bord);// &black_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
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

void white_queen_moves(int position, MOVELIST* movelist, Board* bord) {
    U64 destinations = (bitmap_white_bishop(position, bord) | bitmap_white_rook(position, bord));// &white_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
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

void black_queen_moves(int position, MOVELIST* movelist, Board* bord) {
    U64 destinations = (bitmap_black_bishop(position, bord) | bitmap_black_rook(position, bord));// &black_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
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

void white_king_moves(int position, MOVELIST* movelist, Board* bord) {
    U64 destinations = bitmap_white_king(position, bord);// &~white_checking_bitmap(bord);;
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
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

void black_king_moves(int position, MOVELIST* movelist, Board* bord) {
    U64 destinations = bitmap_black_king(position, bord);// &~black_checking_bitmap(bord);
    Move* m = &movelist->moves[movelist->count];
    while (destinations) {
        int bitIndex = countTrailingZeros(destinations); // Get the index of the least significant set bit
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

void white_moves(MOVELIST* movelist, Board* bord) {
    U64 wrook = bord->white & bord->rook;
    U64 wknight = bord->white & bord->knight;
    U64 wbishop = bord->white & bord->bishop;
    U64 wqueen = bord->white & bord->queen;
    U64 wking = bord->white & bord->king;
    U64 wpawn = bord->white & bord->pawn;
    while (wking) {
        int bitIndex = countTrailingZeros(wking); // Get the index of the least significant set bit
        white_king_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        wking &= (wking - 1); // Clear the least significant set bit
    }
    if (countSetBits(white_checking_pieces(bord)) > 1) {
        return;
    }
    while (wrook) {
        int bitIndex = countTrailingZeros(wrook); // Get the index of the least significant set bit
        white_rook_moves(63 - bitIndex,movelist,bord); // Call the corresponding function with the index of the set bit
        wrook &= (wrook - 1); // Clear the least significant set bit
    }
    while (wknight) {
        int bitIndex = countTrailingZeros(wknight); // Get the index of the least significant set bit
        white_knight_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        wknight &= (wknight - 1); // Clear the least significant set bit
    }
    while (wbishop) {
        int bitIndex = countTrailingZeros(wbishop); // Get the index of the least significant set bit
        white_bishop_moves(63 - bitIndex, movelist, bord); (63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        wbishop &= (wbishop - 1); // Clear the least significant set bit
    }
    while (wqueen) {
        int bitIndex = countTrailingZeros(wqueen); // Get the index of the least significant set bit
        white_queen_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        wqueen &= (wqueen - 1); // Clear the least significant set bit
    }
    while (wpawn) {
        int bitIndex = countTrailingZeros(wpawn); // Get the index of the least significant set bit
        white_pawn_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        wpawn &= (wpawn - 1); // Clear the least significant set bit
    }
}

void black_moves(MOVELIST* movelist, Board* bord) {
    U64 brook = bord->black & bord->rook;
    U64 bknight = bord->black & bord->knight;
    U64 bbishop = bord->black & bord->bishop;
    U64 bqueen = bord->black & bord->queen;
    U64 bking = bord->black & bord->king;
    U64 bpawn = bord->black & bord->pawn;
    while (bking) {
        int bitIndex = countTrailingZeros(bking); // Get the index of the least significant set bit
        black_king_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        bking &= (bking - 1); // Clear the least significant set bit
    }
    if (countSetBits(black_checking_pieces(bord)) > 1) {
        return;
    }
    while (brook) {
        int bitIndex = countTrailingZeros(brook); // Get the index of the least significant set bit
        black_rook_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        brook &= (brook - 1); // Clear the least significant set bit
    }
    while (bknight) {
        int bitIndex = countTrailingZeros(bknight); // Get the index of the least significant set bit
        black_knight_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        bknight &= (bknight - 1); // Clear the least significant set bit
    }
    while (bbishop) {
        int bitIndex = countTrailingZeros(bbishop); // Get the index of the least significant set bit
        black_bishop_moves(63 - bitIndex, movelist, bord); (63 - bitIndex, bord); // Call the corresponding function with the index of the set bit
        bbishop &= (bbishop - 1); // Clear the least significant set bit
    }
    while (bqueen) {
        int bitIndex = countTrailingZeros(bqueen); // Get the index of the least significant set bit
        black_queen_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        bqueen &= (bqueen - 1); // Clear the least significant set bit
    }
    while (bpawn) {
        int bitIndex = countTrailingZeros(bpawn); // Get the index of the least significant set bit
        black_pawn_moves(63 - bitIndex, movelist, bord); // Call the corresponding function with the index of the set bit
        bpawn &= (bpawn - 1); // Clear the least significant set bit
    }
}

void GenMoveList(MOVELIST* list, Board* bord) {
    // Generate all moves, including illegal (e.g. put king in check) moves
    if (white_plays(bord)) {
        white_moves(list, bord);
    }
    else {
        black_moves(list, bord);
    }
}

bool EvaluateQuick(Board* bord) {
    if (!white_plays(bord)) {
        return (((bord->king & bord->white) & all_black_attacks(bord))) == 0;
    }
    else {
        return (((bord->king & bord->black) & all_white_attacks(bord))) == 0;
    }
    //return OpponentHasMoves(bord);
}

void addLegalMoveList(MOVELIST* list, Board* bord, PositionTracker* positionTracker){
    int i, j;
    bool okay;
    MOVELIST list2;
    list2.count = 0;

    // Generate all moves, including illegal (e.g. put king in check) moves
    if (white_plays(bord)) {
        white_moves(&list2, bord);
    }
    else {
        black_moves(&list2, bord);
    }
    Board bordCopy;
    // Loop copying the proven good ones
    for (i = j = 0; i < list2.count; i++)
    {
        copyBoard(bord, &bordCopy);
        makeMove(&bordCopy, &list2.moves[i],positionTracker);
        okay = EvaluateQuick(bord);
        if (isDraw(&bordCopy, positionTracker) != NOT_DRAW) {
            okay = false;
        }
        positionTracker->removePosition(&bordCopy);
        if (okay)
            list->moves[j++] = list2.moves[i];
    }
    list->count = j;
}

void GenLegalMoveList(MOVELIST* list, Board* bord, PositionTracker* positionTracker) {
    int i, j;
    bool okay;
    list->count = 0;
    MOVELIST list2;
    list2.count = 0;

    // Generate all moves, including illegal (e.g. put king in check) moves
    if (white_plays(bord)) {
        white_moves(&list2, bord);
    }
    else {
        black_moves(&list2, bord);
    }

    Board bordCopy;
    // Loop copying the proven good ones
    for (i = j = 0; i < list2.count; i++)
    {
        copyBoard(bord, &bordCopy);
        makeMove(&bordCopy, &list2.moves[i], positionTracker);
        okay = EvaluateQuick(&bordCopy);
        if (isDraw(&bordCopy, positionTracker) != NOT_DRAW) {
            //cout << "draw" << endl;
            //cout << isDraw(&bordCopy, positionTracker) << endl;
            okay = false;
        }
        positionTracker->removePosition(&bordCopy);
        if (okay)
            list->moves[j++] = list2.moves[i];
    }
    list->count = j;
}

/*
* returns true if oponent has moves and false otherwise (false also means checkmated of stalemate)
*/
bool OpponentHasMoves(Board* bord) {
    int i, j;
    bool okay;
    MOVELIST list2;
    PositionTracker positionTracker;
    list2.count = 0;

    // Generate all moves, including illegal (e.g. put king in check) moves
    if (!white_plays(bord)) { // to generate moves for oponent
        white_moves(&list2, bord);
    }
    else {
        black_moves(&list2, bord);
    }
    Board bordCopy;
    // Loop copying the proven good ones
    for (i = j = 0; i < list2.count; i++)
    {
        copyBoard(bord, &bordCopy);
        makeMove(&bordCopy, &list2.moves[i], &positionTracker);
        //positionTracker.removePosition(&bordCopy);
        okay = EvaluateQuick(&bordCopy);
        if (okay) j++;
    }
    return j > 0;
}

/*
* returns true if we has moves and false otherwise (false also means checkmated of stalemate)
*/
bool weHaveMoves(Board* bord) {
    int i, j;
    bool okay;
    MOVELIST list2;
    PositionTracker positionTracker;
    list2.count = 0;

    // Generate all moves, including illegal (e.g. put king in check) moves
    if (white_plays(bord)) { // to generate moves for oponent
        white_moves(&list2, bord);
    }
    else {
        black_moves(&list2, bord);
    }
    Board bordCopy;
    // Loop copying the proven good ones
    for (i = j = 0; i < list2.count; i++)
    {
        copyBoard(bord, &bordCopy);
        makeMove(&bordCopy, &list2.moves[i], &positionTracker);
        //positionTracker.removePosition(&bordCopy);
        okay = EvaluateQuick(&bordCopy);
        if (okay) j++;
    }
    return j > 0;
}

bool inCheck(Board* bord) {
    if (white_plays(bord)) {
        return countSetBits(white_checking_pieces(bord)) != 0;
        //return ((bord->white & bord->king) & all_black_attacks(bord)) != 0;
    }else {
        return countSetBits(black_checking_pieces(bord)) != 0;
        //return ((bord->black & bord->king) & all_white_attacks(bord)) != 0;
    }
}

DRAWTYPE isDraw(Board* bord, PositionTracker* positionTracker) {
    if (bord->halfmoveClock >= 100) return DRAWTYPE_50MOVE;
    if (positionTracker->getPositionOccurrences(bord) >= 3) return DRAWTYPE_REPITITION;
    U64 nonKingPieces = bord->bishop | bord->rook | bord->knight | bord->queen | bord->pawn; //TODO test
    if((nonKingPieces & bord->white) == 0 || (nonKingPieces & bord->black) == 0) return DRAWTYPE_INSUFFICIENT_AUTO; //TODO test
    if (countSetBits(bord->white | bord->black) <= 2) return DRAWTYPE_INSUFFICIENT_AUTO;
    return NOT_DRAW;
}

// Function to convert 12 sets of 64-bit numbers to a 64-character string
std::string convertTo64CharString(U64 rook, U64 knight, U64 bishop, U64 queen, U64 king, U64 pawn, U64 white, U64 black) {
    std::string result;
    for (int i = 0; i < 64; ++i) {
        uint64_t bitMask = ((1ULL << 63) >> i);
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


std::string convertTo64PieceString(U64 rook, U64 knight, U64 bishop, U64 queen, U64 king, U64 pawn, U64 white, U64 black) { //TODO convert the board to a list of unicode chess pieces
    std::string result;
    for (int i = 0; i < 64; ++i) {
        uint64_t bitMask = ((1ULL << 63) >> i);
        char representativeChar = '.'; // Default character
        if ((rook & bitMask) != 0) {
            if ((white & bitMask) != 0) { representativeChar = 'R' /*'\u2656'*/; }  // Unicode white rook
            else if ((black & bitMask) != 0) { representativeChar = 'r' /*'\u265C'*/; } //unicode black rook
        }
        else if ((knight & bitMask) != 0) {
            if ((white & bitMask) != 0) { representativeChar = 'N'; }
            else if ((black & bitMask) != 0) { representativeChar = 'n'; }
        }
        else if ((bishop & bitMask) != 0) {
            if ((white & bitMask) != 0) { representativeChar = 'B'; }
            else if ((black & bitMask) != 0) { representativeChar = 'b'; }
        }
        else if ((queen & bitMask) != 0) {
            if ((white & bitMask) != 0) { representativeChar = 'Q'; }
            else if ((black & bitMask) != 0) { representativeChar = 'q'; }
        }
        else if ((king & bitMask) != 0) {
            if ((white & bitMask) != 0) { representativeChar = 'K'; }
            else if ((black & bitMask) != 0) { representativeChar = 'k'; }
        }
        else if ((pawn & bitMask) != 0) {
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
void overlay(std::string* str, U64 bitpattern, char character) {
    std::string& myString = *str;
    size_t strLength = myString.length();

    for (size_t i = 0; i < strLength; ++i) {
        if (bitpattern & ((1ULL << 63) >> i)) {
            myString[i] = character;
        }
    }
}

void printBitBoard(U64 bitbord, std::string extra) {
    std::cout << endl;
    std::cout << extra << endl;
    std::string temp = std::bitset<64>(bitbord).to_string();
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
    //std::string temp = convertTo64PieceString(bord->rook, bord->knight, bord->bishop, bord->queen, bord->king, bord->pawn, bord->white, bord->black);
    //overlay(&temp, bitmap_white_rook(63,bord), 'X');
    //overlay(&temp, bqcastle, 'O');
    // 0b000000000000000000000000000000000000000000000000000000000000000
    std::cout << endl;
    if (white_plays(bord)) {
        cout << "white to play:" << endl;
    }
    else {
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

    //printBitBoard(all_white_attacks(bord), "all white attacks");
    //printBitBoard(en_passent_target(bord), "all white pawn attacks");
    //cout << std::bitset<18>(bord->extra) << endl;
    //cout << std::bitset<64>((((1ULL << 63) >> (((bord->extra >> 7) << 58) >> 58)))) << endl;
    //cout << std::bitset<64>((bord->extra & ((1ULL << 13))) >> 13) << endl;
    //cout << std::bitset<64>((~((((bord->extra & ((1ULL << 13))) >> 13) << 64) - 1)) & ((((1ULL << 63) >> (((bord->extra >> 7) << 58) >> 58))))) << endl;
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
    //bord->extra = 0b1111100000000000000;
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
    //bord->extra = 0b1111100000000000000;
}

void addPiece(Board* bord, Pieces piece, int square) {
    U64 placeBit = ((1ULL << 63) >> square);
    if (piece == WROOK) {
        bord->white |= placeBit;
        bord->rook  |= placeBit;
    }else if (piece == WKNIGHT) {
        bord->white |= placeBit;
        bord->knight |= placeBit;
    }else if (piece == WBISCHOP) {
        bord->white |= placeBit;
        bord->bishop |= placeBit;
    }else if (piece == WQUEEN) {
        bord->white |= placeBit;
        bord->queen |= placeBit;
    }else if (piece == WKING) {
        bord->white |= placeBit;
        bord->king |= placeBit;
    }else if (piece == WPAWN) {
        bord->white |= placeBit;
        bord->pawn |= placeBit;
    }else if (piece == BROOK) {
        bord->black |= placeBit;
        bord->rook |= placeBit;
    }else if (piece == BKNIGHT) {
        bord->black |= placeBit;
        bord->knight |= placeBit;
    }else if (piece == BBISCHOP) {
        bord->black |= placeBit;
        bord->bishop |= placeBit;
    }else if (piece == BQUEEN) {
        bord->black |= placeBit;
        bord->queen |= placeBit;
    }else if (piece == BKING) {
        bord->black |= placeBit;
        bord->king |= placeBit;
    }else if (piece == BPAWN) {
        bord->black |= placeBit;
        bord->pawn |= placeBit;
    }
}

void clearSquare(Board* bord, int square) {
    U64 placeBit = ~((1ULL << 63) >> square);
    bord->rook &= placeBit;
    bord->knight &= placeBit;
    bord->bishop &= placeBit;
    bord->queen &= placeBit;
    bord->king &= placeBit;
    bord->pawn &= placeBit;

    bord->white &= placeBit;
    bord->black &= placeBit;
}

Pieces pieceAt(int square, Board* board) {
    U64 sq = 1ULL << (63 - square);  // Calculate the bit corresponding to the square

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

Square stringToSquare(std::string inp) {
    static const std::unordered_map<std::string, Square> squareMap = {
            {"a8", A8},
            {"a7", A7},
            {"a6", A6},
            {"a5", A5},
            {"a4", A4},
            {"a3", A3},
            {"a2", A2},
            {"a1", A1},
            {"b8", B8},
            {"b7", B7},
            {"b6", B6},
            {"b5", B5},
            {"b4", B4},
            {"b3", B3},
            {"b2", B2},
            {"b1", B1},
            {"c8", C8},
            {"c7", C7},
            {"c6", C6},
            {"c5", C5},
            {"c4", C4},
            {"c3", C3},
            {"c2", C2},
            {"c1", C1},
            {"d8", D8},
            {"d7", D7},
            {"d6", D6},
            {"d5", D5},
            {"d4", D4},
            {"d3", D3},
            {"d2", D2},
            {"d1", D1},
            {"e8", E8},
            {"e7", E7},
            {"e6", E6},
            {"e5", E5},
            {"e4", E4},
            {"e3", E3},
            {"e2", E2},
            {"e1", E1},
            {"f8", F8},
            {"f7", F7},
            {"f6", F6},
            {"f5", F5},
            {"f4", F4},
            {"f3", F3},
            {"f2", F2},
            {"f1", F1},
            {"g8", G8},
            {"g7", G7},
            {"g6", G6},
            {"g5", G5},
            {"g4", G4},
            {"g3", G3},
            {"g2", G2},
            {"g1", G1},
            {"h8", H8},
            {"h7", H7},
            {"h6", H6},
            {"h5", H5},
            {"h4", H4},
            {"h3", H3},
            {"h2", H2},
            {"h1", H1}
    };

    auto it = squareMap.find(inp);
    return (it != squareMap.end()) ? it->second : A8;
}

std::string specialToString(SPECIAL special) {
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
Pieces charToPiece(char c) {
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

void readInFen(Board* bord, std::string* fen) {
    setupEmpty(bord);
    fen->erase(std::remove(fen->begin(), fen->end(), '/'), fen->end());
    //cout << *fen << endl;
    int index = -1;
    for (char c : *fen) {
        if (c != ' ') {
            if (std::isdigit(c)) index += (c - '0');
            else index += 1;
            addPiece(bord, charToPiece(c), index);
        }
        else {
            //printBoard(bord);
            // Find the position of the first space
            size_t spacePos = fen->find(' ');

            // Extract the substring starting from the character immediately after the space
            std::string result = fen->substr(spacePos + 1);
            //bord->extra = 0b0000000000000000000;
            bord->whiteToPlay = 0;
            bord->whiteKingsideCastle = 0;
            bord->whiteQueensideCastle = 0;
            bord->blackKingsideCastle = 0;
            bord->blackQueensideCastle = 0;
            bord->enPassentValid = 0;
            bord->enPassantTarget = 0;
            bord->halfmoveClock = 0;
            bord->reserved = 0;
            if (result.front() == 'w') {
                bord->whiteToPlay = 1;
                //bord->extra |= 0b1000000000000000000;
            }
            result.erase(0, 2);
            if (result.front() == 'K') {
                result.erase(0, 1);
                bord->whiteKingsideCastle = 1;
                //bord->extra |= 0b0100000000000000000;
            }
            if (result.front() == 'Q') {
                result.erase(0, 1);
                bord->whiteQueensideCastle = 1;
                //bord->extra |= 0b0010000000000000000;
            }
            if (result.front() == 'k') {
                result.erase(0, 1);
                bord->blackKingsideCastle = 1;
                //bord->extra |= 0b0001000000000000000;
            }
            if (result.front() == 'q') {
                result.erase(0, 1);
                bord->blackQueensideCastle = 1;
                //bord->extra |= 0b0000100000000000000;
            }
            if (result.front() == '-') {
                result.erase(0, 1);
            }
            result.erase(0, 1);
            if (result.front() != '-') {
                std::string temp = result.substr(0,2);
                Square enPassent = stringToSquare(temp);
                result.erase(0, 3);
                bord->enPassentValid = 1;
                bord->enPassantTarget = enPassent;
                //bord->extra |= 0b0000010000000000000;
                //bord->extra |= (enPassent << 7);
            }
            else {
                result.erase(0, 2);
            }
            std::string halfm = result.substr(0, result.find(' '));
            int halfmcount = std::stoi(halfm);
            bord->halfmoveClock = halfmcount;
            //bord->extra |= (halfmcount);


            //std::cout << std::bitset<19>(bord->extra) << std::endl;
            //std::cout << "Stripped string: " << result << std::endl;
            break; // Stop when a space is encountered
        }
    }
}

// Function to make a move
void makeMove(Board* bord, Move* move, PositionTracker* positionTracker) {
    //bord->extra = (bord->extra & ~0x7F) | ((bord->extra + 1) & 0x7F); //TODO halfmove clock
    //lastCapturedPiece = NOPIECE; //TODO needed if i make a popMove
    // Update the pawns bitboard to reflect the move
    U64 fromBit = ((1ULL << 63) >> move->src);
    U64 toBit = ((1ULL << 63) >> move->dst);
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
            //lastCapturedPiece = pieceAt(move->dst, bord); //TODO needed if i make a popMove
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
            if (!white_plays(bord)) {
                U64 promotionPawn = oneRow & bord->pawn & bord->black;
                bord->pawn &= ~promotionPawn;
                bord->bishop |= promotionPawn;
            }
            else {
                U64 promotionPawn = eightRow & bord->pawn & bord->white;
                bord->pawn &= ~promotionPawn;
                bord->bishop |= promotionPawn;
            }
        }
        else if (move->special == SPECIAL_PROMOTION_KNIGHT) {
            if (!white_plays(bord)) {
                U64 promotionPawn = oneRow & bord->pawn & bord->black;
                bord->pawn &= ~promotionPawn;
                bord->knight |= promotionPawn;
            }
            else {
                U64 promotionPawn = eightRow & bord->pawn & bord->white;
                bord->pawn &= ~promotionPawn;
                bord->knight |= promotionPawn;
            }
        }
        else if (move->special == SPECIAL_PROMOTION_QUEEN) {
            if (!white_plays(bord)) {
                U64 promotionPawn = oneRow & bord->pawn & bord->black;
                bord->pawn &= ~promotionPawn;
                bord->queen |= promotionPawn;
            }
            else {
                U64 promotionPawn = eightRow & bord->pawn & bord->white;
                bord->pawn &= ~promotionPawn;
                bord->queen |= promotionPawn;
            }
        }
        else if (move->special == SPECIAL_PROMOTION_ROOK) {
            if (!white_plays(bord)) {
                U64 promotionPawn = oneRow & bord->pawn & bord->black;
                bord->pawn &= ~promotionPawn;
                bord->rook |= promotionPawn;
            }
            else {
                U64 promotionPawn = eightRow & bord->pawn & bord->white;
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

/*
// Function to remove a move
void popMove(Board* bord, Move* move) {
	// Update the pawns bitboard to reflect the move
	U64 fromBit = ((1ULL << 63) >> move->src);
	U64 toBit = ((1ULL << 63) >> move->dst);

	if ((move->special == NOT_SPECIAL) ||
		(move->special == SPECIAL_WPAWN_2SQUARES) ||
		(move->special == SPECIAL_BPAWN_2SQUARES) ||
		(move->special == SPECIAL_WEN_PASSANT) ||
		(move->special == SPECIAL_BEN_PASSANT) ||
		(move->special == SPECIAL_PROMOTION_BISHOP) ||
		(move->special == SPECIAL_PROMOTION_KNIGHT) ||
		(move->special == SPECIAL_PROMOTION_QUEEN) ||
		(move->special == SPECIAL_PROMOTION_ROOK)) {

		// reset the color bitboards
		if ((bord->white & toBit) != 0) {
			bord->white |= fromBit; // set the source square
			bord->white ^= toBit;   // clear the destination square
		}
		else if ((bord->black & toBit) != 0) {
			bord->black |= fromBit; // set the source square
			bord->black ^= toBit;   // clear the destination square
		}


		if ((bord->rook & toBit) != 0) {
		*/
/* //TODO
if ((((1ULL << 63) >> (A8)) & fromBit) != 0) {
    bord->extra |= (1ULL << 14); // re add white kingside casteling ability
}
else if ((((1ULL << 63) >> (H8)) & fromBit) != 0) {
    bord->extra |= (1ULL << 15); // re add white queenside casteling ability
}
else if ((((1ULL << 63) >> (A1)) & fromBit) != 0) {
    bord->extra |= (1ULL << 16); // re add black kingside casteling ability
}
else if ((((1ULL << 63) >> (H1)) & fromBit) != 0) {
    bord->extra |= (1ULL << 17); // re add black queenside casteling ability
}
*/
/*
			bord->rook |= fromBit; // set the source square
			bord->rook ^= toBit;   // clear the destination square
		}
		else if ((bord->knight & toBit) != 0) {
			bord->knight |= fromBit; // Clear the source square
			bord->knight ^= toBit;   // Set the destination square
		}
		else if ((bord->bishop & toBit) != 0) {
			bord->bishop |= fromBit; // Clear the source square
			bord->bishop ^= toBit;   // Set the destination square
		}
		else if ((bord->queen & toBit) != 0) {
			bord->queen |= fromBit; // Clear the source square
			bord->queen ^= toBit;   // Set the destination square
		}
		else if ((bord->king & toBit) != 0) {
		*/
/* //TODO
if ((bord->white & fromBit) != 0) {
    bord->extra &= ~((((1ULL << 2) - 1) << 16));
}
else if ((bord->black & fromBit) != 0) {
    bord->extra &= ~((((1ULL << 2) - 1) << 14));
}
*/
/*
			bord->king |= fromBit; // Clear the source square
			bord->king ^= toBit;   // Set the destination square
		}
		else if ((bord->pawn & toBit) != 0) {
			bord->pawn |= fromBit; // Clear the source square
			bord->pawn ^= toBit;   // Set the destination square
		}
	}

	//bord->extra &= ~((((1ULL << 7) - 1) << 7)); // remove en passent target //TODO
	if (move->special != NOT_SPECIAL) {
		if (move->special == SPECIAL_WPAWN_2SQUARES) {
			bord->extra &= ~((((1ULL << 7) - 1) << 7));
		}
		else if (move->special == SPECIAL_BPAWN_2SQUARES) {
			bord->extra &= ~((((1ULL << 7) - 1) << 7));
		}
		*/
/* //TODO
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
*/
/* //TODO
else if (move->special == SPECIAL_WK_CASTLING) {
    bord->extra &= ~((((1ULL << 2) - 1) << 16));
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
else if (move->special == SPECIAL_BK_CASTLING) {
    bord->extra &= ~((((1ULL << 2) - 1) << 14));
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
else if (move->special == SPECIAL_WQ_CASTLING) {
    bord->extra &= ~((((1ULL << 2) - 1) << 16));
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
else if (move->special == SPECIAL_BQ_CASTLING) {
    bord->extra &= ~((((1ULL << 2) - 1) << 14));
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
*/
/*
		else if (move->special == SPECIAL_PROMOTION_BISHOP) {
			U64 promotionPawn = bord->bishop & ((1ULL << move->dst));
			bord->pawn |= ((1ULL << move->src));
			bord->bishop &= ~promotionPawn;
		}
		else if (move->special == SPECIAL_PROMOTION_KNIGHT) {
			U64 promotionPawn = bord->knight & ((1ULL << move->dst));
			bord->pawn |= ((1ULL << move->src));
			bord->knight &= ~promotionPawn;
		}
		else if (move->special == SPECIAL_PROMOTION_QUEEN) {
			U64 promotionPawn = bord->queen & ((1ULL << move->dst));
			bord->pawn |= ((1ULL << move->src));
			bord->queen &= ~promotionPawn;
		}
		else if (move->special == SPECIAL_PROMOTION_ROOK) {
			U64 promotionPawn = bord->rook & ((1ULL << move->dst));
			bord->pawn |= ((1ULL << move->src));
			bord->rook &= ~promotionPawn;
		}
	}
	bord->extra ^= (1ULL << 18); // swap playing player
	// add captured pieces back
	if (move->capture) {
		addPiece(bord, lastCapturedPiece, move->dst);
	}
}
*/