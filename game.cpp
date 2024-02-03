#include "game.h"

// bit manipulation macros
#define get_bit(bitboard, index) (bitboard & (1ULL << index))
#define set_bit(bitboard, index) (bitboard |= (1ULL << index))
#define pop_bit(bitboard, index) (get_bit(bitboard, index) ? bitboard ^= (1ULL << index) : 0)

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
        if (bitpattern & ((1ULL << 63) >> i)) myString[i] = character;
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

/* =====================================
 *  below is the new code
 * ===================================== */

bool checkBoard(Board* bord){
    return !(bord->pawn & (eightRow | oneRow)) &&
           (countSetBits(bord->white & bord->king) == 1) &&
           (countSetBits(bord->black & bord->king) == 1) &&
           (countSetBits(bord->white) <= 16) &&
           (countSetBits(bord->black) <= 16) &&
           (countSetBits(bord->white & bord->pawn) <= 8) &&
           (countSetBits(bord->black & bord->pawn) <= 8);
    /*
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
     */
}

void getLegalMoves(Board* bord, ActionList* legal){ //TODO try to get inline
    ActionList pseudo;
    getAllMoves(bord, &pseudo);

    /* initiate bitmaps to redo the move */
    saveBoard();

    for (int i = 0; i<pseudo.count;i++) {
        movePiece(bord, &(pseudo.moves[i]));

        // Add the move to legal when kingDangerInvers is zero
        legal->moves[legal->count] = pseudo.moves[i];
        legal->count += (calculateKingDangerInvers(bord) == 0ULL);
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