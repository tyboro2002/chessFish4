// game.h : Include file for standard system include files,
// or project specific include files.

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
    A8=0, B8, C8, D8, E8, F8, G8, H8,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A1, B1, C1, D1, E1, F1, G1, H1,
};

/*
 * // old format
A8, B8, C8, D8, E8, F8, G8, H8,
A7, B7, C7, D7, E7, F7, G7, H7,
A6, B6, C6, D6, E6, F6, G6, H6,
A5, B5, C5, D5, E5, F5, G5, H5,
A4, B4, C4, D4, E4, F4, G4, H4,
A3, B3, C3, D3, E3, F3, G3, H3,
A2, B2, C2, D2, E2, F2, G2, H2,
A1, B1, C1, D1, E1, F1, G1, H1,
 */

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

// Results of a test for legal position, note that they are powers
//  of 2, allowing a mask of reasons
enum ILLEGAL_REASON
{
    IR_NULL = 0, IR_PAWN_POSITION = 1, //pawns on 1st or 8th rank
    IR_NOT_ONE_KING_EACH = 2, IR_CAN_TAKE_KING = 4,
    IR_WHITE_TOO_MANY_PIECES = 8, IR_WHITE_TOO_MANY_PAWNS = 16,
    IR_BLACK_TOO_MANY_PIECES = 32, IR_BLACK_TOO_MANY_PAWNS = 64
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

struct PositionRecord {
    int occurrences;
    // Add any other information you want to track per position
    // For example, moves or other relevant data
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
int getFirst1BitSquare(U64 number);
int findMoveIndex(MOVELIST* moveList, Move* targetMove);

Pieces pieceAt(int square, Board* bord);

void setup(Board* bord);
void setupEmpty(Board* bord);
void addPiece(Board* bord, Pieces piece, int square);
void clearSquare(Board* bord, int square);


void printBitBoard(U64 bitbord, std::string extra);
void makeMove(Board* bord, Move* move, PositionTracker* positionTracker);
void popMove(Board* bord, Move* move);

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

U64 bitmap_all_white_pawns(Board* bord);
U64 bitmap_all_black_pawns(Board* bord);
U64 bitmap_all_white_king(Board* bord, int diepte);
U64 bitmap_all_black_king(Board* bord, int diepte);

U64 white_checking_pieces(Board* bord);
U64 black_checking_pieces(Board* bord);

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
void readInFen(Board* bord, std::string* fen);


std::string squareToString(Square square);
std::string specialToString(SPECIAL special);
std::string moveToString(Move* move);
std::string moveToStringShort(Move* move);

U64 incrementByOne(U64 number);

U64 all_attacks(Board* bord);
U64 is_attacked(int square, Board* bord);

class Action{
public:
    // Action is a lightweight type, it is accommodated in only 16 bits
    int  src : 8;
    int  dst : 8;

    bool operator ==(const Move& other) const { return(*((int16_t*)this) == *((int16_t*)(&other)));}

    bool operator !=(const Move& other) const { return(*((int16_t*)this) != *((int16_t*)(&other)));}
};

void movePiece(Board* bord, Action* move);