#define OLC_PGE_APPLICATION
#include "GUIEngine.h"
#include <optional>
#include "../moves.h"
#include "../magic_numbers/MagicsTester.h"
#include "../engines/ChessEngine.h"
#include "../engines/RandomEngine.h"
#include "../engines/MonteCarloEngine.h"
#include "../engines/MiniMaxEngine.h"

#define LOOP 1 // 1 to loop 0 to not loop
#define LOOP_FRAMES 30

#include "../magic_numbers/MagicsTester.h"

#define CHESS_SIZE 8
#define BITMAPS 64


#define CELL_SIZE 100
#define TOP_LEFT_X_FIELD 100
#define TOP_LEFT_y_FIELD 100
#define INTENSITY 150

#define CASTELING_RIGHTS_X (ScreenWidth() / 2 - 10)
#define CASTELING_RIGHTS_Y (ScreenHeight() / 2 + 60)
#define CASTELING_RIGHTS_D_TEXT_RIGHTS 30
#define CASTELING_RIGHTS_DIST_RIGHTS 100

#define WHITE_ENGINE_SELECTION_X 1000
#define WHITE_ENGINE_SELECTION_Y 100

#define BLACK_ENGINE_SELECTION_X 1300
#define BLACK_ENGINE_SELECTION_Y 100

#define ENGINE_TOLERANCE 100 //the amount of pixels we can click away from the center of the engine to rotate it


#define SCREEN_SIZE 1 // the program will scale all sprites down this amount making the screen appear this amount larger

// Function to calculate the distance between two points (x1, y1) and (x2, y2)
inline double calculateDistance(int x1, int y1, int x2, int y2) {
    return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

enum Engines {
    HUMAN,
    MINIMAX,
    RANDOM,
    MONTE_CARLO,
    NUM_ENGINES
};

class ChessFishVisualiserUI : public olc::PixelGameEngine {
public:
    //std::vector<std::vector<std::unique_ptr<olc::Sprite>>> cardSprites;
    //std::vector<std::unique_ptr<olc::Sprite>> playerSprites;
    ChessFishVisualiserUI() {
        sAppName = "ChessFishVisualiser";
    }

public:
    bool OnUserCreate() override {
        spriteSheet = olc::Sprite("../assets/pieces.png");

        minimaxLogo = olc::Sprite("../assets/engine_logos/minimax.png");
        randomLogo = olc::Sprite("../assets/engine_logos/random.png");
        monteCarloLogo = olc::Sprite("../assets/engine_logos/monte-carlo.png");

        humanLogo = olc::Sprite("../assets/engine_logos/human.png");

        //setupEmpty(&bord);
        //readInFen(&bord,"rn1qkbnr/p1p1ppp1/8/3p4/4Q3/7P/1PPP1P1P/RNB1KBNR w KQkq - 0 1");
        setup(&bord);
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        // Clear the screen
        Clear(olc::Pixel(50, 50, 50));

        loopNumber++;
        if(LOOP_FRAMES < loopNumber) {
            loopNumber %= LOOP_FRAMES;
            //if(LOOP && !humanPlay()) doMove = true;
        }

        DrawChessboard(CHESS_SIZE, CELL_SIZE,  /*calculateKingDanger(&bord)*/ /* is_attacked(E8,&bord)*/ mask /*selectedSquare==-1 ? 0ULL : mask */ /*1ULL << (63-selectedSquare)*/  /*moves[bitb]*/ /*, purpleSquares, greenSquares*/);
        if (gameOver) DrawSprite(300,200,&spriteSheet); // TODO make this display endgame screen

        SetPixelMode(olc::Pixel::MASK); // Don't draw pixels which have any transparency
        DrawSprite(WHITE_ENGINE_SELECTION_X,WHITE_ENGINE_SELECTION_Y,getEngineSprite(selectedWhiteEngine));
        DrawString((WHITE_ENGINE_SELECTION_X+BLACK_ENGINE_SELECTION_X) /2 + humanLogo.width/2, (WHITE_ENGINE_SELECTION_Y+BLACK_ENGINE_SELECTION_Y) /2 + humanLogo.height/2, "VS");
        DrawSprite(BLACK_ENGINE_SELECTION_X,BLACK_ENGINE_SELECTION_Y,getEngineSprite(selectedBlackEngine));
        SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels

        // Check for button click
        if (GetMouse(0).bPressed){
            doMove = false;
            int x = GetMouseX();
            int y = GetMouseY();

            // Calculate the row and column of the clicked cell
            int row = (y - TOP_LEFT_y_FIELD) / CELL_SIZE;
            int col = (x - TOP_LEFT_X_FIELD) / CELL_SIZE;

            // Check if the click is within the chessboard boundaries
            if (row >= 0 && row < CHESS_SIZE &&
                col >= 0 && col < CHESS_SIZE) {
                // The mouse click is within the chessboard
                // Now, 'row' and 'col' represent the clicked cell
                int toSq = 63-(row*8+col);
                if(selectedSquare != -1 || !humanPlay()){
                    if((selectedSquare != toSq && 1ULL<<toSq & mask) || (!humanPlay()) && !gameOver){
                        if(bord.whiteToPlay){
                            switch (selectedWhiteEngine) {
                                case RANDOM:
                                    randomEngine->makeMove(&bord);
                                    break;
                                case MONTE_CARLO:
                                    randomMonteCarloEngine->makeMove(&bord);
                                    break;
                                case MINIMAX:
                                    miniMaxEngine->makeMove(&bord);
                                    break;
                                case HUMAN:
                                    Action action = {.src = selectedSquare, .dst = toSq};
                                    if((selectedSquare >= 48 && bord.whiteToPlay) || (selectedSquare <= 15 && !bord.whiteToPlay) ) action.special = Promote_Queen; //TODO make player choose
                                    movePiece(&bord, &action);
                                    break;
                            }
                            cout << "white move made" << endl;
                            printFancyBoard(&bord);
                        }else{
                            switch (selectedBlackEngine) {
                                case RANDOM:
                                    randomEngine->makeMove(&bord);
                                    break;
                                case MONTE_CARLO:
                                    randomMonteCarloEngine->makeMove(&bord);
                                    break;
                                case MINIMAX:
                                    miniMaxEngine->makeMove(&bord);
                                    break;
                                case HUMAN:
                                    Action action = {.src = selectedSquare, .dst = toSq};
                                    if((selectedSquare >= 48 && bord.whiteToPlay) || (selectedSquare <= 15 && !bord.whiteToPlay) ) action.special = Promote_Queen; //TODO make player choose
                                    movePiece(&bord, &action);
                                    break;
                            }
                            cout << "black move made" << endl;
                            printFancyBoard(&bord);
                        }

                        if(isEnded(&bord)) {
                            cout << "game ended before this" << endl;
                            gameOver = True;
                        }
                    }
                    selectedSquare = -1;
                    mask = 0ULL;
                }else{
                    selectedSquare = toSq;
                    ActionList actionList;
                    //printf("cliked at %d\n",toSq);
                    getLegalMoves(&bord, &actionList);
                    mask = calculateBitmapFromSquare(selectedSquare, &actionList);
                }
                message = "Clicked Row: " + std::to_string(row) + ", Col: " + std::to_string(col) + ", resulting in square: " + std::to_string(selectedSquare);
            } else if (calculateDistance(x, y, WHITE_ENGINE_SELECTION_X + humanLogo.width / 2, WHITE_ENGINE_SELECTION_Y + humanLogo.height / 2) <= 100){
                selectedWhiteEngine = (selectedWhiteEngine +1 ) % amountOfEngines;
                message = "white engine rotated";
            }
            else if (calculateDistance(x, y, BLACK_ENGINE_SELECTION_X + humanLogo.width / 2, BLACK_ENGINE_SELECTION_Y + humanLogo.height / 2) <= 100){
                selectedBlackEngine = (selectedBlackEngine +1 ) % amountOfEngines;
                message = "black engine rotated";
            }
            else {
                message = "Clicked outside the chessboard"; // The mouse click is outside the chessboard
            }
        }
        DrawString(ScreenWidth() / 2 - 10, ScreenHeight() / 2 + 30, message);
        DrawString(CASTELING_RIGHTS_X, CASTELING_RIGHTS_Y, "castling rights:",olc::WHITE,2);
        SetPixelMode(olc::Pixel::MASK); // Don't draw pixels which have any transparency
        //draw white king side castling rights
        if (bord.whiteKingsideCastle) DrawPartialSprite(CASTELING_RIGHTS_X,CASTELING_RIGHTS_Y+CASTELING_RIGHTS_D_TEXT_RIGHTS, &spriteSheet, 0,0,CELL_SIZE,CELL_SIZE);
        //draw white queen side castling rights
        if (bord.whiteQueensideCastle) DrawPartialSprite(CASTELING_RIGHTS_X+CASTELING_RIGHTS_DIST_RIGHTS,CASTELING_RIGHTS_Y+CASTELING_RIGHTS_D_TEXT_RIGHTS, &spriteSheet, CELL_SIZE,0,CELL_SIZE,CELL_SIZE);
        //draw black king side castling rights
        if (bord.blackKingsideCastle) DrawPartialSprite(CASTELING_RIGHTS_X+CASTELING_RIGHTS_DIST_RIGHTS*2,CASTELING_RIGHTS_Y+CASTELING_RIGHTS_D_TEXT_RIGHTS, &spriteSheet, 0,CELL_SIZE,CELL_SIZE,CELL_SIZE);
        //draw black queen side castling rights
        if (bord.blackQueensideCastle) DrawPartialSprite(CASTELING_RIGHTS_X+CASTELING_RIGHTS_DIST_RIGHTS*3,CASTELING_RIGHTS_Y+CASTELING_RIGHTS_D_TEXT_RIGHTS, &spriteSheet, CELL_SIZE,CELL_SIZE,CELL_SIZE,CELL_SIZE);

        if (bord.whiteToPlay) DrawPartialSprite(CASTELING_RIGHTS_X,CASTELING_RIGHTS_Y+4*CASTELING_RIGHTS_D_TEXT_RIGHTS, &spriteSheet, 0,0,CELL_SIZE,CELL_SIZE);
        else DrawPartialSprite(CASTELING_RIGHTS_X,CASTELING_RIGHTS_Y+4*CASTELING_RIGHTS_D_TEXT_RIGHTS, &spriteSheet, 0,CELL_SIZE,CELL_SIZE,CELL_SIZE);
        SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
        return true;
    }

private:
    U64 loopNumber = 0;
    bool doMove = false;

    Board bord{};
    olc::Sprite spriteSheet;
    /* load the engine logos */
    olc::Sprite minimaxLogo;
    olc::Sprite randomLogo;
    olc::Sprite monteCarloLogo;

    olc::Sprite humanLogo;

    int selectedSquare = -1;
    std::string message = "";
    U64 mask = 0ULL;
    bool gameOver = false;

    int selectedWhiteEngine = 0;
    int selectedBlackEngine = 0;

    int amountOfEngines = NUM_ENGINES;

    ChessEngine* randomEngine = new RandomChessEngine();
    ChessEngine* randomMonteCarloEngine = new MonteCarloEngine(false, 10, 100, randomEngine);
    ChessEngine* miniMaxEngine = new MiniMaxEngine(4);

    bool humanPlay() {
       return (bord.whiteToPlay ? (selectedWhiteEngine == HUMAN) : (selectedBlackEngine == HUMAN));
    }

    olc::Sprite* getEngineSprite(int engine){
        switch (engine) {
            case HUMAN:
                return &humanLogo;
            case MINIMAX:
                return &minimaxLogo;
            case RANDOM:
                return &randomLogo;
            case MONTE_CARLO:
                return &monteCarloLogo;
            default:
                return &humanLogo; //TODO not implemented logo
        }
    }

    // Function to draw a chessboard
    void DrawChessboard(int size, int cellSize, std::optional<uint64_t> bitboard = std::nullopt, std::optional<std::vector<int>> purpleSquares = std::nullopt, std::optional<std::vector<int>> greenSquares = std::nullopt){
        for (int i = 0; i < size; ++i){
            for (int j = 0; j < size; ++j){
                // Alternate between white and black cells
                olc::Pixel color = (i + j) % 2 == 0 ? olc::WHITE : olc::BLACK;

                int squareIndex = 63-(i * size + j);
                // If bitboard is provided, add a bit of blue and red based on the bit value
                if (bitboard.has_value()){
                    color = ((bitboard.value() >> squareIndex) & 1) ? olc::Pixel(color.r + INTENSITY, color.g, color.b) : olc::Pixel(color.r, color.g, color.b + INTENSITY);
                }

                // Draw a rectangle for each cell
                FillRect(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, cellSize, cellSize, color);

                SetPixelMode(olc::Pixel::MASK); // Dont draw pixels which have any transparency
                // draw white pieces at this square
                if(((bord.white & bord.king) >> squareIndex) & 1) DrawPartialSprite(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, &spriteSheet, 0,0,CELL_SIZE,CELL_SIZE);
                if(((bord.white & bord.queen) >> squareIndex) & 1) DrawPartialSprite(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, &spriteSheet, CELL_SIZE,0,CELL_SIZE,CELL_SIZE);
                if(((bord.white & bord.bishop) >> squareIndex) & 1) DrawPartialSprite(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, &spriteSheet, CELL_SIZE*2,0,CELL_SIZE,CELL_SIZE);
                if(((bord.white & bord.knight) >> squareIndex) & 1) DrawPartialSprite(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, &spriteSheet, CELL_SIZE*3,0,CELL_SIZE,CELL_SIZE);
                if(((bord.white & bord.rook) >> squareIndex) & 1) DrawPartialSprite(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, &spriteSheet, CELL_SIZE*4,0,CELL_SIZE,CELL_SIZE);
                if(((bord.white & bord.pawn) >> squareIndex) & 1) DrawPartialSprite(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, &spriteSheet, CELL_SIZE*5,0,CELL_SIZE,CELL_SIZE);

                // draw black pieces at this square
                if(((bord.black & bord.king) >> squareIndex) & 1) DrawPartialSprite(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, &spriteSheet, 0,CELL_SIZE,CELL_SIZE,CELL_SIZE);
                if(((bord.black & bord.queen) >> squareIndex) & 1) DrawPartialSprite(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, &spriteSheet, CELL_SIZE,CELL_SIZE,CELL_SIZE,CELL_SIZE);
                if(((bord.black & bord.bishop) >> squareIndex) & 1) DrawPartialSprite(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, &spriteSheet, CELL_SIZE*2,CELL_SIZE,CELL_SIZE,CELL_SIZE);
                if(((bord.black & bord.knight) >> squareIndex) & 1) DrawPartialSprite(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, &spriteSheet, CELL_SIZE*3,CELL_SIZE,CELL_SIZE,CELL_SIZE);
                if(((bord.black & bord.rook) >> squareIndex) & 1) DrawPartialSprite(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, &spriteSheet, CELL_SIZE*4,CELL_SIZE,CELL_SIZE,CELL_SIZE);
                if(((bord.black & bord.pawn) >> squareIndex) & 1) DrawPartialSprite(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, &spriteSheet, CELL_SIZE*5,CELL_SIZE,CELL_SIZE,CELL_SIZE);


                SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels

                // If purpleSquares is provided, check if the current square index is in the list
                if (purpleSquares.has_value()) {
                    int squareIndex = 63-(i * size + j);
                    if (std::find(purpleSquares.value().begin(), purpleSquares.value().end(), squareIndex) != purpleSquares.value().end()) {
                        FillCircle(TOP_LEFT_X_FIELD + (j + 0.5) * cellSize, TOP_LEFT_y_FIELD + (i + 0.5) * cellSize, cellSize / 4, olc::MAGENTA);
                    }
                }
                // If purpleSquares is provided, check if the current square index is in the list
                if (greenSquares.has_value()) {
                    int squareIndex = 63-(i * size + j);
                    if (std::find(greenSquares.value().begin(), greenSquares.value().end(), squareIndex) != greenSquares.value().end()) {
                        FillCircle(TOP_LEFT_X_FIELD + (j + 0.5) * cellSize, TOP_LEFT_y_FIELD + (i + 0.5) * cellSize, cellSize / 4, olc::GREEN);
                    }
                }
            }
        }
    }

    void DrawButton(const std::string& text, int x, int y, int width, int height, olc::Pixel textCol, olc::Pixel buttonCol)
    {
        // Draw the button background
        FillRect(x, y, width, height, buttonCol);

        // Draw the button border
        DrawRect(x, y, width, height, olc::WHITE);

        // Draw the button text
        int textWidth = text.length() * 8; // NOLINT(*-narrowing-conversions)
        int textHeight = 8;
        DrawString(x + (width - textWidth) / 2, y + (height - textHeight) / 2, text, textCol);
    }

};