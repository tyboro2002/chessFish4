#define OLC_PGE_APPLICATION
#include "GUIEngine.h"
#include <optional>
#include "moves.h"
#include "MagicsTester.h"

#define LOOP
#define LOOP_FRAMES 30

#include "MagicsTester.h"

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

#define SCREEN_SIZE 1 // the program will scale all sprites down this amount making the screen appear this amount larger

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
        setup(&bord);
        //clearSquare(&bord,B1);
        //clearSquare(&bord,A2);
        /*
        for (int i = 0; i < BITMAPS; i++) {
            moves[i] = all_attacks(&bord);
            //moves[i] = bischopMovesONE_OFF[i];
            //moves[i] = bishop_attacks_on_the_fly(i,blocks);
            //moves[i] = rook_attacks_on_the_fly(i,blocks);
            //moves[i] = get_bishop_attacks(i,blocks);
            //moves[i] = get_rook_attacks(i,blocks);
            //moves[i] = get_queen_attacks(i,blocks);
            //moves[i] = get_white_pawn_attacks(i,0ULL,all);
            //moves[i] = whitePawnAttacks[i];
            //moves[i] = 1ULL << i;
            //moves[i] = blocks;
        }
         */
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        // Clear the screen
        Clear(olc::Pixel(50, 50, 50));
#ifdef LOOP
        /*
        loopNumber++;
        if(LOOP_FRAMES < loopNumber) {
            loopNumber %= LOOP_FRAMES;
            bitb++;
            bitb %= BITMAPS;
        }
         */
#endif
        // Draw the button
        //DrawButton("next bitboard!", ScreenWidth() / 2 - 50, ScreenHeight() / 2 - 25, 150, 50, olc::WHITE, olc::DARK_GREY);

        // Draw the counter value
        //DrawString(ScreenWidth() / 2 - 10, ScreenHeight() / 2 + 30, std::to_string(bitb));

        // Optional: Provide a list of squares to mark with a purple dot
        std::vector<int> purpleSquares = { 1, 3, 4,6,9,10,11, 13,14,15, 21,22,24,28,32};

        // Optional: Provide a list of squares to mark with a purple dot
        //std::vector<int> greenSquares = {63-bitb};

        // Called once per frame, draws random coloured pixels
        //DrawChessboard(CHESS_SIZE, CELL_SIZE, moves[bitb], purpleSquares, greenSquares);
        DrawChessboard(CHESS_SIZE, CELL_SIZE,  /*calculateKingDanger(&bord)*/ mask /*selectedSquare==-1 ? 0ULL : mask */ /*1ULL << (63-selectedSquare)*/  /*moves[bitb]*/ /*, purpleSquares, greenSquares*/);
        //DrawSprite(300,200,&spriteSheet);


        // Check for button click
        if (GetMouse(0).bPressed){
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
                if(selectedSquare != -1){
                    if(selectedSquare != toSq && 1ULL<<toSq & mask ){
                        Action action = {.src = selectedSquare, .dst = toSq};
                        movePiece(&bord, &action);
                    }
                    selectedSquare = -1;
                    mask = 0ULL;
                }else{
                    selectedSquare = toSq;
                    //mask = is_attacked(selectedSquare,&bord);
                    ActionList actionList;
                    getLegalMoves(&bord, &actionList);
                    mask = calculateBitmapFromSquare(selectedSquare, &actionList);
                    actionList.count = 0;
                }
                message = "Clicked Row: " + std::to_string(row) + ", Col: " + std::to_string(col) + ", resulting in square: " + std::to_string(selectedSquare);
            } else {
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
    //U64 loopNumber = 0;
    //int bitb = 0;
    //U64 moves[BITMAPS];
    //U64 blocks = 4586532442ULL;
    Board bord;
    olc::Sprite spriteSheet;
    int selectedSquare = -1;
    std::string message = "";
    U64 mask = 0ULL;

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
        int textWidth = text.length() * 8;
        int textHeight = 8;
        DrawString(x + (width - textWidth) / 2, y + (height - textHeight) / 2, text, textCol);
    }

};