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

#define en_passent_target(bord) ((1ULL << bord->enPassantTarget) & (bord->enPassentValid ? UINT64_MAX : 0))

/* used for white casteling */
#define MOVE_WHITE_ROOK(srcSquare, dstSquare) \
    bord->white &= ~(1ULL << (srcSquare)); \
    bord->rook &= ~(1ULL << (srcSquare)); \
    \
    bord->white |= 1ULL << (dstSquare); \
    bord->rook |= 1ULL << (dstSquare)

/* used for black casteling */
#define MOVE_BLACK_ROOK(srcSquare, dstSquare) \
    bord->black &= ~(1ULL << (srcSquare)); \
    bord->rook &= ~(1ULL << (srcSquare)); \
    \
    bord->black |= 1ULL << (dstSquare); \
    bord->rook |= 1ULL << (dstSquare)

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

int findMoveIndex(MOVELIST* moveList, Move* targetMove);

Pieces pieceAt(const int square,const Board* bord);

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
std::string moveToString(const Move* move);
std::string moveToStringShort(const Move* move);


/*==========================
   below is the new code
 =========================*/

/* add pieces to the board */
void setup(Board* bord);
void setupEmpty(Board* bord);
void addPiece(Board* bord,const Pieces piece,const int square);

/* print the bitboard with a message */
void printBitBoard(U64 bitbord, std::string extra);

/* calculate the bitmaps of attacks */

/* calculate the bitmap of all squares the current player can move to or attack */
//U64 all_attacks(const Board* bord);

/* calculate the bitmap of all squares the current player can move to or attack from that square */
//U64 is_attacked(const int square,const Board* bord);

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

    bool operator==(const Action& other) const {
        return src == other.src && dst == other.dst && special == other.special;
    }

    bool operator!=(const Action& other) const {
        return !(*this == other);
    }
};

/* a list of actions used to represent everything we can do from a formation */
struct ActionList{
    int count = 0;  // number of moves
    Action moves[MAXMOVES]{};

    // Function to add a move to the list
    void addMove(int src, int dst, Exceptional exp = Non_Exceptional) {
        moves[count++] = {src, dst, exp};
    }

    void popMove(){
        count--;
    }
};

/* execute the action given (basicaly moving the piece from src to dst and doing the special needs for this like castling ex) */
//void movePiece(Board* bord, Action* move);

/* calculate all moves the current player can do from this square and add them to the actionlist */
void getMovesAtSquare(Board* bord, int square, ActionList* actionList);

/* calculate all possible moves from a position */
void getAllMoves(Board* bord, ActionList* actionList);

/* filter all legal moves from the getAllMoves function (this function calls that move itself) */
void getLegalMoves(Board* bord, ActionList* actionList);

/* calculate a mask of all pieces attacking the king */
//U64 calculateKingDanger(const Board* bord);

/* calculate a mask of all pieces attacking a square */
//U64 calculateDanger(const Board* bord,const  int square);

/* calculate if there is something attacking a square */
//bool calculateIfInDanger(const Board* bord,const int square);

/* get all the destinations of moves that originate from a square and put them in a bitmap (only use this for drawing this is slow) */
U64 calculateBitmapFromSquare(int square, ActionList* actionList);

/* place a fen position on the board */
void readInFen(Board* bord, char* fen);

/* print an actionlist */
void printActionList(const ActionList* actionList);

/* print an action */
void printAction(const Action* action);

/*
 * inlined functions
 */
inline U64 get_white_pawn_attacks(const int square,const U64 white,const U64 black){
    U64 att = whitePawnAttacks[square] & black; // the squares this piece can attack
    U64 whiteBlack = white | black;
    if(((1ULL << square)&twoRow) && !((1ULL<<(square+8))&(whiteBlack)) && !((1ULL<<(square+16))&(whiteBlack))) att |= (1ULL<<(square+16));
    att |= ((1ULL<<(square+8)) & ~(whiteBlack));
    return att & ~oneRow;
}

inline U64 get_black_pawn_attacks(const int square, const U64 white, const U64 black){
    U64 att = blackPawnAttacks[square] & white; // the squares this piece can attack
    U64 whiteBlack = white | black;
    if(((1ULL << square)&sevenRow) && !((1ULL<<(square-8))&(whiteBlack)) && !((1ULL<<(square-16))&(whiteBlack))) att |= (1ULL<<(square-16));
    att |= ((1ULL<<(square-8)) & ~(whiteBlack));
    return att & ~eightRow;
}

inline int countSetBits(U64 number) {return __builtin_popcountll(number);}

// Function to get the index of the least significant 1-bit
static inline int get_ls1b_index_game(U64 bitboard){ return bitboard ? __builtin_ctzll(bitboard) : -1;}

// mask knight attacks
inline U64 bitmap_white_knight(const int square,const  Board* bord) {return knightMoves[square] & (~bord->white);}

// mask knight attacks
inline U64 bitmap_black_knight(const int square,const  Board* bord) {return knightMoves[square] & (~bord->black);}

// rook attacks
inline U64 bitmap_white_rook(const int square,const  Board* bord){return get_rook_attacks(square, bord->white | bord->black) & (~bord->white);}

// rook attacks
inline U64 bitmap_black_rook(const int square,const  Board* bord){return get_rook_attacks(square, bord->white | bord->black) & (~bord->black);}

// bishop attacks
inline U64 bitmap_white_bishop(const int square,const  Board* bord){return get_bishop_attacks(square, bord->white | bord->black) & (~bord->white);}

// bishop attacks
inline U64 bitmap_black_bishop(const int square,const  Board* bord){return get_bishop_attacks(square, bord->white | bord->black) & (~bord->black);}

inline U64 bitmap_white_pawn(const int position,const Board* bord) {return get_white_pawn_attacks(position, bord->white,bord->black) | (en_passent_target(bord) & whitePawnAttacks[position]);}

inline U64 bitmap_black_pawn(const int position,const Board* bord) {return get_black_pawn_attacks(position, bord->white,bord->black) | (en_passent_target(bord) & blackPawnAttacks[position]);}

inline U64 bitmap_white_queen(const int square,const  Board* bord) {return get_queen_attacks(square, bord->white | bord->black) & (~bord->white);}

inline U64 bitmap_black_queen(const int square,const  Board* bord) {return get_queen_attacks(square, bord->white | bord->black) & (~bord->black);}

inline U64 bitmap_white_king(const int position,const Board* bord) {
    U64 empty = ~(bord->white | bord->black);
    U64 ret = kingMoves[position];
    if (position == E1) {
        if ((wkcastle & empty) == wkcastle && bord->whiteKingsideCastle  && (h1_mask & (bord->white & bord->rook))) ret |= g1_mask;
        if ((wqcastle & empty) == wqcastle && bord->whiteQueensideCastle && (a1_mask & (bord->white & bord->rook))) ret |= c1_mask;
        /*
         * maybe optimalisation code for the above code
            int condwkcastle = (wkcastle & empty) == wkcastle && bord->whiteKingsideCastle  && (h1_mask & (bord->white & bord->rook));
            int condwqcastle = (wqcastle & empty) == wqcastle && bord->whiteQueensideCastle && (a1_mask & (bord->white & bord->rook));
            int mask = -condwkcastle;  // -1 if cond is 1, 0 if cond is 0
            ret |=  (0ULL & ~mask) | (g1_mask & mask);
            mask = -condwqcastle;  // -1 if cond is 1, 0 if cond is 0
            ret |=  (0ULL & ~mask) | (c1_mask & mask);
         */
    }
    return ret & (~bord->white);
}

inline U64 bitmap_black_king(const int position,const  Board* bord) {
    U64 empty = ~(bord->white | bord->black);
    U64 ret = kingMoves[position];
    if (position == E8) {
        if ((bkcastle & empty) == bkcastle && bord->blackKingsideCastle  && (h8_mask & (bord->black & bord->rook))) ret |= g8_mask;
        if ((bqcastle & empty) == bqcastle && bord->blackQueensideCastle && (a8_mask & (bord->black & bord->rook))) ret |= c8_mask;
        /*
         * maybe optimalisation code for the above code
            int condbkcastle = (bkcastle & empty) == bkcastle && bord->blackKingsideCastle  && (h8_mask & (bord->black & bord->rook));
            int condbqcastle = (bqcastle & empty) == bqcastle && bord->blackQueensideCastle && (a8_mask & (bord->black & bord->rook));
            int mask = -condbkcastle;  // -1 if cond is 1, 0 if cond is 0
            ret |=  (0ULL & ~mask) | (g8_mask & mask);
            mask = -condbqcastle;  // -1 if cond is 1, 0 if cond is 0
            ret |=  (0ULL & ~mask) | (c8_mask & mask);
         */
    }
    return ret & (~bord->black);
}

inline U64 calculateKingDanger(const Board* bord){
    if(bord->whiteToPlay) {
        /* find the square of the king */
        int kingSquare = get_ls1b_index_game(bord->white & bord->king);
        /* if there are certainly no pieces able to capture the queen return */
        //if((bitmap_white_queen(kingSquare,bord) & bord->white) == 0ULL) return 0ULL;
        /* get a bitmap of all the pieces that attack the king */
        U64 queen = bitmap_white_queen(kingSquare,bord) & (bord->black & bord->queen);
        U64 bishop = bitmap_white_bishop(kingSquare,bord) & (bord->black & bord->bishop);
        U64 rook = bitmap_white_rook(kingSquare,bord) & (bord->black & bord->rook);
        U64 knight = bitmap_white_knight(kingSquare,bord) & (bord->black & bord->knight);
        U64 pawn = whitePawnAttacks[kingSquare] & (bord->black & bord->pawn);
        U64 king = bitmap_white_king(kingSquare,bord) & (bord->black & bord->king);
        return queen | bishop | rook | knight | pawn | king;
    }
    else{
        /* find the square of the king */
        int kingSquare = get_ls1b_index_game(bord->black & bord->king);
        /* if there are certainly no pieces able to capture the queen return */
        //if((bitmap_black_queen(kingSquare,bord) & bord->black) == 0ULL) return 0ULL;
        /* get a bitmap of all the pieces that attack the king */
        U64 queen = bitmap_black_queen(kingSquare,bord) & (bord->white & bord->queen);
        U64 bishop = bitmap_black_bishop(kingSquare,bord) & (bord->white & bord->bishop);
        U64 rook = bitmap_black_rook(kingSquare,bord) & (bord->white & bord->rook);
        U64 knight = bitmap_black_knight(kingSquare,bord) & (bord->white & bord->knight);
        U64 pawn = blackPawnAttacks[kingSquare] & (bord->white & bord->pawn);
        U64 king = bitmap_black_king(kingSquare,bord) & (bord->white & bord->king);
        return queen | bishop | rook | knight | pawn | king;
    }
}

inline U64 calculateDanger(const Board* bord,const int square){
    if(bord->whiteToPlay) {
        /* find the square of the king */
        /* if there are certainly no pieces able to capture the queen return */
        //if((bitmap_black_queen(square,bord) & bord->white) == 0ULL) return 0ULL;
        /* get a bitmap of all the pieces that attack the king */
        U64 queen = bitmap_white_queen(square,bord) & (bord->black & bord->queen);
        U64 bishop = bitmap_white_bishop(square,bord) & (bord->black & bord->bishop);
        U64 rook = bitmap_white_rook(square,bord) & (bord->black & bord->rook);
        U64 knight = bitmap_white_knight(square,bord) & (bord->black & bord->knight);
        U64 pawn = whitePawnAttacks[square] & (bord->black & bord->pawn);
        U64 king = bitmap_white_king(square,bord) & (bord->black & bord->king);
        return queen | bishop | rook | knight | pawn | king;
    }
    else{
        /* find the square of the king */
        /* if there are certainly no pieces able to capture the queen return */
        //if((bitmap_white_queen(square,bord) & bord->black) == 0ULL) return 0ULL;
        /* get a bitmap of all the pieces that attack the king */
        U64 queen = bitmap_black_queen(square,bord) & (bord->white & bord->queen);
        U64 bishop = bitmap_black_bishop(square,bord) & (bord->white & bord->bishop);
        U64 rook = bitmap_black_rook(square,bord) & (bord->white & bord->rook);
        U64 knight = bitmap_black_knight(square,bord) & (bord->white & bord->knight);
        U64 pawn = blackPawnAttacks[square] & (bord->white & bord->pawn);
        U64 king = bitmap_black_king(square,bord) & (bord->white & bord->king);
        return queen | bishop | rook | knight | pawn | king;
    }
}

inline bool calculateIfInDanger(const Board* bord,const int square){
    if(bord->whiteToPlay) {
        return bitmap_white_queen(square,bord) & (bord->black & bord->queen)||
               bitmap_white_bishop(square,bord) & (bord->black & bord->bishop) ||
               bitmap_white_rook(square,bord) & (bord->black & bord->rook) ||
               bitmap_white_knight(square,bord) & (bord->black & bord->knight) ||
               whitePawnAttacks[square] & (bord->black & bord->pawn) ||
               bitmap_white_king(square,bord) & (bord->black & bord->king);
        //return queen | bishop | rook | knight | pawn | king;
    }
    else{
        return bitmap_black_queen(square,bord) & (bord->white & bord->queen) ||
               bitmap_black_bishop(square,bord) & (bord->white & bord->bishop) ||
               bitmap_black_rook(square,bord) & (bord->white & bord->rook) ||
               bitmap_black_knight(square,bord) & (bord->white & bord->knight) ||
               blackPawnAttacks[square] & (bord->white & bord->pawn) ||
               bitmap_black_king(square,bord) & (bord->white & bord->king);
        //return queen | bishop | rook | knight | pawn | king;
    }
}

inline U64 all_attacks(const Board* bord){
    U64 att = 0ULL;
    if(bord->whiteToPlay){
        U64 wrook = bord->white & bord->rook;
        U64 wknight = bord->white & bord->knight;
        U64 wbishop = bord->white & bord->bishop;
        U64 wqueen = bord->white & bord->queen;
        U64 wking = bord->white & bord->king;
        U64 wpawn = bord->white & bord->pawn;

        att |= bitmap_white_king(get_ls1b_index_game(wking), bord);
        int checks = countSetBits(calculateKingDanger(bord));
        if (get_ls1b_index_game(wking) == E1){
            if(checks) att &= ~whiteCastles;
            if (calculateIfInDanger(bord, F1) || calculateIfInDanger(bord,G1)) att &= ~g1_mask;
            if (calculateIfInDanger(bord, D1) || calculateIfInDanger(bord,C1)) att &= ~c1_mask;
        }
        if (checks > 1) return att;

        while (wrook) {
            att |= bitmap_white_rook(get_ls1b_index_game(wrook),bord); // get the attacks of the rook at this position
            wrook &= (wrook - 1); // Clear the least significant set bit
        }
        while (wbishop) {
            att |= bitmap_white_bishop(get_ls1b_index_game(wbishop),bord);
            wbishop &= (wbishop - 1);
        }
        while (wqueen) {
            att |= bitmap_white_queen(get_ls1b_index_game(wqueen),bord);
            wqueen &= (wqueen - 1);
        }
        while (wknight) {
            att |= bitmap_white_knight(get_ls1b_index_game(wknight),bord);
            wknight &= (wknight - 1);
        }
        while (wpawn) {
            att |= bitmap_white_pawn(get_ls1b_index_game(wpawn),bord);
            wpawn &= (wpawn - 1);
        }
        return att;
    }else{
        U64 brook = bord->black & bord->rook;
        U64 bknight = bord->black & bord->knight;
        U64 bbishop = bord->black & bord->bishop;
        U64 bqueen = bord->black & bord->queen;
        U64 bking = bord->black & bord->king;
        U64 bpawn = bord->black & bord->pawn;

        att |= bitmap_black_king(get_ls1b_index_game(bking), bord);
        int checks = countSetBits(calculateKingDanger(bord));
        if (get_ls1b_index_game(bking) == E8){
            if(checks) att &= ~whiteCastles;
            if (calculateIfInDanger(bord, F8) || calculateIfInDanger(bord,G8)) att &= ~g8_mask;
            if (calculateIfInDanger(bord, D8) || calculateIfInDanger(bord,C8)) att &= ~c8_mask;
        }
        if (checks > 1) return att;

        while (brook) {
            att |= bitmap_black_rook(get_ls1b_index_game(brook),bord);
            brook &= (brook - 1);
        }
        while (bbishop) {
            att |= bitmap_black_bishop(get_ls1b_index_game(bbishop),bord);
            bbishop &= (bbishop - 1);
        }
        while (bqueen) {
            att |= bitmap_black_queen(get_ls1b_index_game(bqueen),bord);
            bqueen &= (bqueen - 1);
        }
        while (bknight) {
            att |= bitmap_black_knight(get_ls1b_index_game(bknight),bord);
            bknight &= (bknight - 1);
        }
        while (bpawn) {
            att |= bitmap_black_pawn(get_ls1b_index_game(bpawn),bord);
            bpawn &= (bpawn - 1);
        }
        return att;
    }
}

inline U64 is_attacked(const int square,const  Board *bord) {
    U64 sq_mask = 1ULL << square;
    U64 att = 0ULL;
    if(bord->whiteToPlay){
        U64 wrook = bord->white & bord->rook & sq_mask;
        U64 wknight = bord->white & bord->knight & sq_mask;
        U64 wbishop = bord->white & bord->bishop & sq_mask;
        U64 wqueen = bord->white & bord->queen & sq_mask;
        U64 wking = bord->white & bord->king & sq_mask;
        U64 wpawn = bord->white & bord->pawn & sq_mask;

        att |= bitmap_white_king(get_ls1b_index_game(wking), bord);

        int checks = countSetBits(calculateKingDanger(bord));
        if (square == E1){
            if(checks) att &= ~whiteCastles;
            if (calculateIfInDanger(bord, F1) || calculateIfInDanger(bord,G1)) att &= ~g1_mask;
            if (calculateIfInDanger(bord, D1) || calculateIfInDanger(bord,C1)) att &= ~c1_mask;
        }
        if (checks > 1) return att;

        while (wrook) {
            att |= bitmap_white_rook(get_ls1b_index_game(wrook),bord); // get the attacks of the rook at this position
            wrook &= (wrook - 1); // Clear the least significant set bit
        }
        while (wbishop) {
            att |= bitmap_white_bishop(get_ls1b_index_game(wbishop),bord);
            wbishop &= (wbishop - 1);
        }
        while (wqueen) {
            att |= bitmap_white_queen(get_ls1b_index_game(wqueen),bord);
            wqueen &= (wqueen - 1);
        }
        while (wknight) {
            att |= bitmap_white_knight(get_ls1b_index_game(wknight),bord);
            wknight &= (wknight - 1);
        }
        while (wpawn) {
            att |= bitmap_white_pawn(get_ls1b_index_game(wpawn),bord);
            wpawn &= (wpawn - 1);
        }
        return att;
    }else{
        U64 brook = bord->black & bord->rook & sq_mask;
        U64 bknight = bord->black & bord->knight & sq_mask;
        U64 bbishop = bord->black & bord->bishop & sq_mask;
        U64 bqueen = bord->black & bord->queen & sq_mask;
        U64 bking = bord->black & bord->king & sq_mask;
        U64 bpawn = bord->black & bord->pawn & sq_mask;

        att |= bitmap_black_king(get_ls1b_index_game(bking), bord);

        int checks = countSetBits(calculateKingDanger(bord));
        if (square == E8){
            if(checks) att &= ~blackCastles;
            if (calculateIfInDanger(bord, F8) || calculateIfInDanger(bord,G8)) att &= ~g8_mask;
            if (calculateIfInDanger(bord, D8) || calculateIfInDanger(bord,C8)) att &= ~c8_mask;
        }
        if (checks > 1) return att;

        while (brook) {
            att |= bitmap_black_rook(get_ls1b_index_game(brook),bord);
            brook &= (brook - 1);
        }
        while (bbishop) {
            att |= bitmap_black_bishop(get_ls1b_index_game(bbishop),bord);
            bbishop &= (bbishop - 1);
        }
        while (bqueen) {
            att |= bitmap_black_queen(get_ls1b_index_game(bqueen),bord);
            bqueen &= (bqueen - 1);
        }
        while (bknight) {
            att |= bitmap_black_knight(get_ls1b_index_game(bknight),bord);
            bknight &= (bknight - 1);
        }
        while (bpawn) {
            att |= bitmap_black_pawn(get_ls1b_index_game(bpawn),bord);
            bpawn &= (bpawn - 1);
        }
        return att;
    }
}

inline void movePiece(Board* bord, Action* move){
    U64 fromBit = 1ULL << move->src;
    U64 toBit = 1ULL << move->dst;
    bool normalCapture = false;
    bord->whiteToPlay ^= 1;
    bord->enPassentValid = 0;
    if (((bord->white | bord->black) & toBit) != 0) {
        //clear all bitboards on the to position
        bord->halfmoveClock = 0;
        bord->rook &= ~toBit;
        bord->knight &= ~toBit;
        bord->bishop &= ~toBit;
        bord->queen &= ~toBit;
        bord->king &= ~toBit;
        bord->pawn &= ~toBit;
        bord->white &= ~toBit;
        bord->black &= ~toBit;
        normalCapture = true;
    }

    if ((bord->white & fromBit) != 0) {
        bord->white ^= fromBit; // Clear the source square
        bord->white |= toBit;   // Set the destination square
    }else if ((bord->black & fromBit) != 0) {
        bord->black ^= fromBit;
        bord->black |= toBit;
    }

    if ((bord->pawn & fromBit) != 0) {
        bord->halfmoveClock += 1;
        bord->pawn ^= fromBit;
        bord->pawn |= toBit;
        if(bord->whiteToPlay){ /* black is playing */
            if(move->src-move->dst == 16){
                bord->enPassentValid = 1;
                bord->enPassantTarget = move->src-8;
            }else if (move->src-move->dst != 8 && !normalCapture) {
                U64 enPassented = (1ULL << move->dst) << 8;
                bord->halfmoveClock = 0;
                bord->pawn &= ~enPassented;
                bord->white &= ~enPassented;
            }
        }else{ /* white is playing */
            if(move->dst-move->src == 16){
                bord->enPassentValid = 1;
                bord->enPassantTarget = move->src+8;
            }else if (move->dst-move->src != 8 && !normalCapture){
                U64 enPassented = (1ULL << move->dst) >> 8;
                bord->halfmoveClock = 0;
                bord->pawn &= ~enPassented;
                bord->black &= ~enPassented;
            }
        }

        if(move->special != Non_Exceptional){
            bord->pawn ^= toBit;
            switch (move->special) {
                case Promote_Rook:
                    bord->rook |= toBit;
                    return;
                case Promote_Knight:
                    bord->knight |= toBit;
                    return;
                case Promote_Bishop:
                    bord->bishop |= toBit;
                    return;
                case Promote_Queen:
                    bord->queen |= toBit;
                    return;
                default:
                    return;
            }
        }
        return;
    }

    if ((bord->rook & fromBit) != 0) {
        bord->halfmoveClock += 1;
        bord->rook ^= fromBit;
        bord->rook |= toBit;
        switch (move->src) {
            case H1:
                bord->whiteKingsideCastle = 0;
                return;
            case A1:
                bord->whiteQueensideCastle = 0;
                return;
            case H8:
                bord->blackKingsideCastle = 0;
                return;
            case A8:
                bord->blackQueensideCastle = 0;
                return;
            default:
                return;
        }
    }

    if ((bord->bishop & fromBit) != 0) {
        bord->halfmoveClock += 1;
        bord->bishop ^= fromBit;
        bord->bishop |= toBit;
        return;
    }

    if ((bord->queen & fromBit) != 0) {
        bord->halfmoveClock += 1;
        bord->queen ^= fromBit;
        bord->queen |= toBit;
        return;
    }

    if ((bord->king & fromBit) != 0) {
        bord->halfmoveClock += 1;
        bord->king ^= fromBit;
        bord->king |= toBit;
        if(!bord->whiteToPlay){ // we already changed the player to play so we need to invert here
            bord->whiteKingsideCastle = 0;
            bord->whiteQueensideCastle = 0;
            if(move->src == E1){
                if(move->dst == G1){ /* we are king side castling for white */
                    MOVE_WHITE_ROOK(H1,F1);
                }else if (move->dst == C1){ /* we are queen side castling for white */
                    MOVE_WHITE_ROOK(A1,D1);
                }
            }
        }else{
            bord->blackKingsideCastle = 0;
            bord->blackQueensideCastle = 0;
            if(move->src == E8){
                if(move->dst == G8){ /* we are king side castling for black */
                    MOVE_BLACK_ROOK(H8,F8);
                }else if (move->dst == C8){ /* we are queen side castling for black */
                    MOVE_BLACK_ROOK(A8,D8);
                }
            }
        }
        return;
    }

    if ((bord->knight & fromBit) != 0) {
        bord->halfmoveClock += 1;
        bord->knight ^= fromBit;
        bord->knight |= toBit;
        return;
    }
}

//TODO a function to convert from board to fen (see: string Position::fen() const { from https://github.com/official-stockfish/Stockfish/blob/master/src/position.cpp)
