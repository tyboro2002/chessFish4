#pragma once

#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <bitset>
#include "moves.h"

typedef unsigned long long U64;

// Calculate an upper limit to the length of a list of moves
#define MAXMOVES (27 + 2*13 + 2*14 + 2*8 + 8 + 8*4  +  3*27)
//[Q   2*B    2*R    2*N   K   8*P] +  [3*Q]
//             ^                         ^
//[calculated practical maximum   ] + [margin]

namespace std {
    template<>
    struct hash<Board> {
        size_t operator()(const Board& board) const {
            // Combine the hash values of individual components of the board
            // to create a unique hash value for the entire board.
            size_t combinedHash = hash<U64>{}(board.rook) ^
                                  hash<U64>{}(board.knight) ^
                                  hash<U64>{}(board.bishop) ^
                                  hash<U64>{}(board.queen) ^
                                  hash<U64>{}(board.king) ^
                                  hash<U64>{}(board.pawn) ^
                                  hash<U64>{}(board.white) ^
                                  hash<U64>{}(board.black);//^
            //hash<U64>{}(board.extra);
            return combinedHash;
        }
    };

    template<>
    struct equal_to<Board> {
        bool operator()(const Board& lhs, const Board& rhs) const {
            // Compare individual components of the boards to determine equality.
            return lhs.rook == rhs.rook &&
                   lhs.knight == rhs.knight &&
                   lhs.bishop == rhs.bishop &&
                   lhs.queen == rhs.queen &&
                   lhs.king == rhs.king &&
                   lhs.pawn == rhs.pawn &&
                   lhs.white == rhs.white &&
                   lhs.black == rhs.black;//&&
            //lhs.whiteToPlay == rhs.whiteToPlay &&
            //lhs.whiteKingsideCastle == rhs.whiteKingsideCastle &&
            //lhs.whiteQueensideCastle == rhs.whiteQueensideCastle &&
            //lhs.blackKingsideCastle == rhs.blackKingsideCastle &&
            //lhs.blackQueensideCastle == rhs.blackQueensideCastle &&
            //lhs.enPassentValid == rhs.enPassentValid &&
            //lhs.enPassantTarget == rhs.enPassantTarget &&
            //lhs.halfmoveClock == rhs.halfmoveClock &&
            //lhs.reserved == rhs.reserved;

            //lhs.extra == rhs.extra;
        }
    };
}

enum Square {
    H1=0, G1, F1, E1, D1, C1, B1, A1,
    H2, G2, F2, E2, D2, C2, B2, A2,
    H3, G3, F3, E3, D3, C3, B3, A3,
    H4, G4, F4, E4, D4, C4, B4, A4,
    H5, G5, F5, E5, D5, C5, B5, A5,
    H6, G6, F6, E6, D6, C6, B6, A6,
    H7, G7, F7, E7, D7, C7, B7, A7,
    H8, G8, F8, E8, D8, C8, B8, A8,
};

enum Pieces {
    NOPIECE = 0,
    WROOK,
    WKNIGHT,
    WBISCHOP,
    WQUEEN,
    WKING,
    WPAWN,
    BROOK,
    BKNIGHT,
    BBISCHOP,
    BQUEEN,
    BKING,
    BPAWN,
};

enum SPECIAL
{
    NOT_SPECIAL = 0,
    SPECIAL_WK_CASTLING,
    SPECIAL_BK_CASTLING,
    SPECIAL_WQ_CASTLING,
    SPECIAL_BQ_CASTLING,
    SPECIAL_PROMOTION_QUEEN,
    SPECIAL_PROMOTION_ROOK,
    SPECIAL_PROMOTION_BISHOP,
    SPECIAL_PROMOTION_KNIGHT,
    SPECIAL_WPAWN_2SQUARES,
    SPECIAL_BPAWN_2SQUARES,
    SPECIAL_WEN_PASSANT,
    SPECIAL_BEN_PASSANT,
};

enum DRAWTYPE
{
    NOT_DRAW,
    DRAWTYPE_50MOVE,
    DRAWTYPE_INSUFFICIENT,      // draw if superior side wants it
    //  since inferior side has insufficient
    //  mating material
    DRAWTYPE_INSUFFICIENT_AUTO, // don't wait to be asked, e.g. draw
    //  immediately if bare kings
    DRAWTYPE_REPITITION,
};

// Stalemate or checkmate game terminations
enum TERMINAL
{
    NOT_TERMINAL = 0,
    TERMINAL_WCHECKMATE = -1,   // White is checkmated
    TERMINAL_WSTALEMATE = -2,   // White is stalemated
    TERMINAL_BCHECKMATE = 1,    // Black is checkmated
    TERMINAL_BSTALEMATE = 2     // Black is stalemated
};

class Move{
public:
    // Move is a lightweight type, it is accommodated in only 32 bits
    Square  src : 8;
    Square  dst : 8;
    SPECIAL special : 8;
    int     capture : 8;
    // ' ' (empty) if move not a capture
    // for some reason Visual C++ 2005 (at least)
    // blows sizeof(Move) out to 64 bits if
    // capture is defined as char instead of int

    bool operator ==(const Move& other) const { return(*((int32_t*)this) == *((int32_t*)(&other)));}

    bool operator !=(const Move& other) const { return(*((int32_t*)this) != *((int32_t*)(&other)));}
};

// List of moves
struct MOVELIST{
    int count;  // number of moves
    Move moves[MAXMOVES];
};

struct TranspositionTableEntry {
    int score;
    int depth;
    Move bestMove;
};

class TranspositionTable {
public:
    TranspositionTable() : currentSize(0), maxSizeBytes(100 * 1024 * 1024) {} // 100MB

    void store(Board* board, const int score, const int depth, Move bestMove) {
        size_t hashValue = std::hash<Board>{}(*board);
        TranspositionTableEntry entry = { score, depth, bestMove };

        // If adding this entry will exceed the max size, evict some entries
        while (currentSize + sizeof(entry) > maxSizeBytes) {
            evictOldestEntry();
        }

        // Store the entry and update the size
        table[hashValue] = entry;
        currentSize += sizeof(entry);
    }

    size_t getCurrentSize() {
        return currentSize;
    }

    size_t getNumElements() const {
        return table.size();
    }
    void printInfo() const {
        std::cout << "Current size of TranspositionTable: " << currentSize << " bytes" << std::endl;
        std::cout << "Number of elements in TranspositionTable: " << table.size() << std::endl;
    }

    TranspositionTableEntry* lookup(Board* board) {
        auto it = table.find(std::hash<Board>{}(*board));
        if (it != table.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    void clear() {
        table.clear();
        currentSize = 0;
    }

private:
    std::unordered_map<size_t, TranspositionTableEntry> table;
    size_t currentSize;
    const size_t maxSizeBytes;

    void evictOldestEntry() {
        if (!table.empty()) {
            size_t oldestKey = table.begin()->first;
            currentSize -= sizeof(table[oldestKey]);
            table.erase(oldestKey);
        }
    }
};

void printBoard(Board* bord);
void printFancyBoard(Board* bord);

struct PositionRecord {
    int occurrences;
};

class PositionTracker {
public:
    //PositionTracker() {} //constructor

    bool addPosition(const Board* position) {
        size_t positionHash = std::hash<Board>{}(*position);
        positionRecords[positionHash].occurrences++;
        return positionRecords[positionHash].occurrences >= 3;
    }

    void removePosition(Board* position) {
        size_t positionHash = std::hash<Board>{}(*position);
        auto it = positionRecords.find(positionHash);
        if (it != positionRecords.end()) {
            if (it->second.occurrences > 1) {
                it->second.occurrences--;
                return;
            }
            else {
                positionRecords.erase(it);
                return;
            }
        }
        printBoard(position);
        std::cout << "bord nie gevonde" << std:: endl;
    }

    int getPositionOccurrences(const Board* position) const {
        size_t positionHash = std::hash<Board>{}(*position);
        auto it = positionRecords.find(positionHash);
        if (it != positionRecords.end()) {
            return it->second.occurrences;
        }
        return 0;
    }

    void clear() {
        positionRecords.clear();
    }

    const std::unordered_map<size_t, PositionRecord>& getPositionRecords() const {
        return positionRecords;
    }

private:
    std::unordered_map<size_t, PositionRecord> positionRecords;
};

void printPositionRecords(const PositionTracker* tracker);
void printMoveList(MOVELIST* moveList);

int countSetBits(U64 number);
int findMoveIndex(MOVELIST* moveList, Move* targetMove);

Pieces pieceAt(int square, Board* bord);

void makeMove(Board* bord, Move* move, PositionTracker* positionTracker);

void white_pawn_moves(int position, MOVELIST* movelist, Board* bord);
void black_pawn_moves(int position, MOVELIST* movelist, Board* bord);
void white_rook_moves(int position, MOVELIST* movelist, Board* bord);
void black_rook_moves(int position, MOVELIST* movelist, Board* bord);
void white_knight_moves(int position, MOVELIST* movelist, Board* bord);
void black_knight_moves(int position, MOVELIST* movelist, Board* bord);
void white_bishop_moves(int position, MOVELIST* movelist, Board* bord);
void black_bishop_moves(int position, MOVELIST* movelist, Board* bord);
void white_queen_moves(int position, MOVELIST* movelist, Board* bord);
void black_queen_moves(int position, MOVELIST* movelist, Board* bord);
void white_king_moves(int position, MOVELIST* movelist, Board* bord);
void black_king_moves(int position, MOVELIST* movelist, Board* bord);
void white_moves(MOVELIST* movelist, Board* bord);
void black_moves(MOVELIST* movelist, Board* bord);


U64 white_checking_pieces(Board* bord);
U64 black_checking_pieces(Board* bord);

U64 all_white_attacks(Board* bord);
U64 all_black_attacks(Board* bord);

void GenMoveList(MOVELIST* list, Board* bord);
void GenLegalMoveList(MOVELIST* list, Board* bord, PositionTracker* positionTracker);
void addLegalMoveList(MOVELIST* list, Board* bord, PositionTracker* positionTracker);
bool OpponentHasMoves(Board* bord);
bool weHaveMoves(Board* bord);
bool inCheck(Board* bord);
DRAWTYPE isDraw(Board* bord, PositionTracker* positionTracker);

U64 squaresBetweenBitmap(int startSquare, int endSquare);
U64 white_checking_bitmap(Board* bord);
U64 black_checking_bitmap(Board* bord);

void copyBoard(const Board* bordIn, Board* bordOut);

std::string squareToString(int square);
std::string specialToString(SPECIAL special);
std::string moveToString(Move* move);
std::string moveToStringShort(Move* move);


/*==========================
   below is the new code
 =========================*/

/* add pieces to the board */
void setup(Board* bord);
void setupEmpty(Board* bord);
void addPiece(Board* bord, Pieces piece, int square);

/* print the bitboard with a message */
void printBitBoard(U64 bitbord, std::string extra);

/* calculate the bitmaps of attacks */
U64 bitmap_white_pawn(int position, Board* bord);
U64 bitmap_black_pawn(int position, Board* bord);
U64 bitmap_white_king(int position, Board* bord);
U64 bitmap_black_king(int position, Board* bord);
U64 bitmap_white_rook(int position, Board* bord);
U64 bitmap_black_rook(int position, Board* bord);
U64 bitmap_white_bishop(int square, Board* bord);
U64 bitmap_black_bishop(int square, Board* bord);
U64 bitmap_white_knight(int square, Board* bord);
U64 bitmap_black_knight(int square, Board* bord);
U64 bitmap_white_queen(int square, Board* bord);
U64 bitmap_black_queen(int square, Board* bord);

/* calculate the bitmap of all squares the current player can move to or attack */
U64 all_attacks(Board* bord);

/* calculate the bitmap of all squares the current player can move to or attack from that square */
U64 is_attacked(int square, Board* bord);

/* check if the board is valid (true if it is) */
bool checkBoard(Board* bord);

enum Exceptional{
    Non_Exceptional,
    Promote_Queen,
    Promote_Rook,
    Promote_Bishop,
    Promote_Knight
};

/* a type representing the moving of a piece */
class Action{
public:
    // Action is a lightweight type, it is accommodated in only 16 bits
    int  src : 8 = 0;
    int  dst : 8 = 0;
    Exceptional special : 8 = Non_Exceptional;

    bool operator ==(Action other) const { return(*((int16_t*)this) == *((int16_t*)(&other)));}

    bool operator !=(const Move& other) const { return(*((int16_t*)this) != *((int16_t*)(&other)));}
};

/* a list of actions used to represent everything we can do from a formation */
struct ActionList{
    int count = 0;  // number of moves
    Action moves[MAXMOVES]{};

    // Function to add a move to the list
    void addMove(int src, int dst) {
        moves[count++] = {src, dst};
    }

    void popMove(){
        count--;
    }
};

/* execute the action given (basicaly moving the piece from src to dst and doing the special needs for this like castling ex) */
void movePiece(Board* bord, Action* move);

/* calculate all moves the current player can do from this square and add them to the actionlist */
void getMovesAtSquare(Board* bord, int square, ActionList* actionList);

/* calculate all possible moves from a position */
void getAllMoves(Board* bord, ActionList* actionList);

/* filter all legal moves from the getAllMoves function (this function calls that move itself) */
void getLegalMoves(Board* bord, ActionList* actionList);

/* calculate a mask of all pieces attacking the king */
U64 calculateKingDanger(Board* bord);

/* calculate a mask of all pieces attacking a square */
U64 calculateDanger(Board* bord, int square);

/* get all the destinations of moves that originate from a square and put them in a bitmap (only use this for drawing this is slow) */
U64 calculateBitmapFromSquare(int square, ActionList* actionList);

/* place a fen position on the board */
void readInFen(Board* bord, char* fen);