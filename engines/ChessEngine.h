#pragma once
#include "../game.h"
#include "../configurations.h"
#include "../OpeningBook/OpeningBook.h"
#include <ctime>
#include <map>

#define GET_MOVE_FROM_OPENING_BOOK  \
    std::vector<Action> moves = openingBook.getBestMoveFromOpeningBook(bord); \
    if(!moves.empty()) { \
    int selectedIndex = getRandomNumber(moves.size()); \
    return moves[selectedIndex]; \
}

#define FILL_OPENING_BOOK openingBook.fillOpeningBookManualy();

enum Engines { // to add an engine add it in this list and in the list in the constructor in GUIEngineGame.h
    HUMAN,
    MINIMAX,
    RANDOM,
    MONTE_CARLO,
    ITERATIVE_DEEPENING,
    STOCKFISH,
    NUM_ENGINES
};

inline int getRandomNumber(int n) {
    // Seed the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Generate a random number between 0 and n-1
    return std::rand() % n;
}

struct SimulationResults{
    U64 whiteWins = 0;
    U64 blackWins = 0;
    U64 draws = 0;
public:
    void update(EndGameResult endGameResult){
        switch (endGameResult) {
            case WHITE_WINS_Checkmate:
            case WHITE_WINS_Resignation:
            case WHITE_WINS_Timeout:
                whiteWins++;
                break;
            case BLACK_WINS_Checkmate:
            case BLACK_WINS_Resignation:
            case BLACK_WINS_Timeout:
                blackWins++;
                break;
            default:
                draws++;
        }
    }

    void reset(){
        whiteWins = 0;
        blackWins = 0;
        draws = 0;
    }

    [[nodiscard]] U64 getWhiteWins() const{
        return whiteWins;
    }

    [[nodiscard]] U64 getBlackWins() const{
        return blackWins;
    }

    [[nodiscard]] U64 getDraws() const{
        return draws;
    }
};

const std::map<Pieces, int> piece_score_dic = {
        //white
        {WPAWN, 100},
        {WROOK, 500},
        {WKNIGHT, 300},
        {WBISCHOP, 300},
        {WQUEEN, 900},
        {WKING, 10'000},
        //black
        {BPAWN, 100},
        {BROOK, 500},
        {BKNIGHT, 300},
        {BBISCHOP, 300},
        {BQUEEN, 900},
        {BKING, 10'000},
        //empty
        {NOPIECE,0}
};

const int pawnEvalWhite[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, -20, -20, 10, 10, 5,
        5, -5, -10, 0, 0, -10, -5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, 5, 10, 25, 25, 10, 5, 5,
        10, 10, 20, 30, 30, 20, 10, 10,
        50, 50, 50, 50, 50, 50, 50, 50,
        0, 0, 0, 0, 0, 0, 0, 0
};

const int pawnEvalBlack[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5, 5, 10, 25, 25, 10, 5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, -5, -10, 0, 0, -10, -5, 5,
        5, 10, 10, -20, -20, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0 };

const int knightEval[64] = {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0, 0, 0, 0, -20, -40,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30,
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50
};

const int bishopEvalWhite[64] = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -20, -10, -10, -10, -10, -10, -10, -20
};

const int bishopEvalBlack[64] = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20
};

const int rookEvalWhite[64] = {
        0, 0, 0, 5, 5, 0, 0, 0,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        5, 10, 10, 10, 10, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
};

const int rookEvalBlack[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, 10, 10, 10, 10, 5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        0, 0, 0, 5, 5, 0, 0, 0
};

const int queenEval[64] = {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5, 0, 5, 5, 5, 5, 0, -5,
        0, 0, 5, 5, 5, 5, 0, -5,
        -10, 5, 5, 5, 5, 5, 0, -10,
        -10, 0, 5, 0, 0, 0, 0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20
};

const int kingEvalWhite[64] = {
        20, 30, 10, 0, 0, 10, 30, 20,
        20, 20, 0, 0, 0, 0, 20, 20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, -30, -30, -40, -40, -30, -30, -20,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30
};
const int kingEvalBlack[64] = {
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, 20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, 20, 0, 0, 0, 0, 20, 20,
        20, 30, 10, 0, 0, 10, 30, 20
};

const int kingEvalEndGameWhite[64] = {
        50, -30, -30, -30, -30, -30, -30, -50,
        -30, -30, 0, 0, 0, 0, -30, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -20, -10, 0, 0, -10, -20, -30,
        -50, -40, -30, -20, -20, -30, -40, -50
};

const int kingEvalEndGameBlack[64] = {
        -50, -40, -30, -20, -20, -30, -40, -50,
        -30, -20, -10, 0, 0, -10, -20, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -30, 0, 0, 0, 0, -30, -30, -50,
        -30, -30, -30, -30, -30, -30, 50
};

inline bool check_end_game(Board* bord) {
    /*
    *  Are we in the end game ?
    *  Per Michniewski :
    *  -Both sides have no queens or
    *  -Every side which has a queen has additionally no other pieces or one minorpiece maximum.
    */

    int queens = countSetBits(bord->queen);
    int minors = countSetBits(bord->bishop | bord->knight);
    return queens == 0 || (queens == 2 && minors <= 1);
}

class ChessEngine {
public:
    virtual void initialize() = 0;
    virtual Action getPreferredAction(Board* bord) = 0;
    virtual ~ChessEngine() = default;
};

inline int evaluateBoard(Board* bord){
    //U64 colorMask;
    //if(bord->whiteToPlay) colorMask = bord->white;
    //else colorMask = bord->black;
    U64 colorMask = (bord->white & -bord->whiteToPlay) | (bord->black & ~(-bord->whiteToPlay));
    int pawnScore = countSetBits(colorMask & bord->pawn);
    int knightScore = countSetBits(colorMask & bord->knight) * 3;
    int bishopScore = countSetBits(colorMask & bord->bishop) * 3;
    int rookScore = countSetBits(colorMask & bord->rook) * 5;
    int queenScore = countSetBits(colorMask & bord->queen) * 10;
    return pawnScore + knightScore + bishopScore+ rookScore + queenScore;
}

inline int evaluate_capture(Board* bord, Action* mv) {
    /*
    *   Given a capturing move, weight the trade being made.
    */

    Pieces _to = pieceAt(mv->dst,bord);
    Pieces _from = pieceAt(mv->src, bord);

    if (bord->enPassentValid && (bord->enPassantTarget == mv->dst)) {
        return 100;// piece_score_dic.find(WPAWN)->second;
    }
    return piece_score_dic.find(_to)->second - piece_score_dic.find(_from)->second;
}

inline int evaluate_piece(Pieces piece, int square, bool endgame) {
    switch (piece){
        case WPAWN:
            return pawnEvalWhite[square];
        case BPAWN:
            return pawnEvalBlack[square];
        case WKNIGHT:
            return knightEval[square];
        case BKNIGHT:
            return knightEval[square];
        case WBISCHOP:
            return bishopEvalWhite[square];
        case BBISCHOP:
            return bishopEvalBlack[square];
        case WROOK:
            return rookEvalWhite[square];
        case BROOK:
            return rookEvalBlack[square];
        case WQUEEN:
            return queenEval[square];
        case BQUEEN:
            return queenEval[square];
        case WKING:
            if (endgame) {
                return kingEvalEndGameWhite[square];
            }
            else {
                return kingEvalWhite[square];
            }
        case BKING:
            if (endgame) {
                return kingEvalEndGameBlack[square];
            }
            else {
                return kingEvalBlack[square];
            }
        default:
            return 0;
    }
}

inline int move_value(Board* bord, Action* mv, bool endgame) {
    /*
    * How good is a move?
    * A promotion is great.
    * A weaker piece taking a stronger piece is good.
    * A stronger piece taking a weaker piece is bad.
    * Also consider the position change via piece-square table.
    */

    if (mv->special != Non_Exceptional) {
        if (bord->whiteToPlay) return INT32_MAX;
        else return -INT32_MAX;
    }

    Pieces _piece = pieceAt(mv->src, bord);
    int _from_value = evaluate_piece(_piece, mv -> src, endgame);
    int _to_value = evaluate_piece(_piece, mv -> dst, endgame);
    int position_change = _to_value - _from_value;

    int capture_value = 0;

    if (pieceAt(mv->dst, bord) != NOPIECE) {
        capture_value = evaluate_capture(bord, mv);
    }

    int current_move_value = capture_value + position_change;
    if (!bord->whiteToPlay) current_move_value = -current_move_value;
    return current_move_value;

}

inline void reverseArray(Action* arr, const std::size_t size) {
    for (std::size_t i = 0; i < size / 2; i++) {
        std::swap(arr[i], arr[size - 1 - i]);
    }
}

inline void orderMoves(Board* bord, ActionList* moveList) {
    bool end_game = check_end_game(bord);

    int size = moveList->count;
    // Define a lambda function to compare moves by their values
    auto compareMoves = [&](Action a, Action b) {
        int value1 = move_value(bord, &a, end_game);
        int value2 = move_value(bord, &b, end_game);
        return value1 > value2;
    };

    // Sort the moves array using the comparator function
    std::stable_sort(moveList->moves, moveList->moves + size, compareMoves);

    if (!bord->whiteToPlay) reverseArray(moveList->moves, size);
}

struct TranspositionTableEntry {
    int score = 0;
    int depth = 0;
    Action bestMove;
};

class TranspositionTable { //TODO look if this cant be made faster
public:
    explicit TranspositionTable(int size) : currentSize(0), maxSizeBytes(size) {}
    TranspositionTable() : currentSize(0), maxSizeBytes(TRANSPOSITION_TABLE_SIZE_DEFAULT) {} // 100MB

    void store(Board* board, const int score, const int depth, Action bestMove) {
        //printFancyBoard(board);
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

    size_t getCurrentSize() const { return currentSize; }
    size_t getNumElements() const { return table.size(); }

    void printInfoShort() const {
        std::cout << "Current size of TranspositionTable: " << currentSize << " bytes" << std::endl;
        std::cout << "Number of elements in TranspositionTable: " << table.size() << std::endl;
    }

    void printTranspositionTable() {
        std::cout << "Current size of TranspositionTable: " << currentSize << " bytes" << std::endl;
        std::cout << "Number of elements in TranspositionTable: " << table.size() << std::endl;
        std::cout << "TranspositionTable Entries:" << std::endl;

        for (const auto& entry : table) {
            std::cout << "Hash: " << entry.first << std::endl;
            std::cout << "  Score: " << entry.second.score << std::endl;
            std::cout << "  Depth: " << entry.second.depth << std::endl;
            std::cout << "  Best Move: ";
            printAction(&entry.second.bestMove);
            std::cout << "------------------------" << std::endl;
        }
    }

    TranspositionTableEntry* lookup(Board* board) {
        auto it = table.find(std::hash<Board>{}(*board));
        if (it != table.end()) return &(it->second);
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