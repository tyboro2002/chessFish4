#define OLC_PGE_APPLICATION
#include "GUIEngine.h"
#include <optional>
#include "moves.h"

#include "MagicsTester.h"

#define CHESS_SIZE 8


#define CELL_SIZE 100
#define TOP_LEFT_X_FIELD 100
#define TOP_LEFT_y_FIELD 100
#define INTENSITY 150

#define SCREEN_SIZE 1 // the program will scale all sprites down this amount making the screen apear this amount larger


class ChessFishVisualiserUI : public olc::PixelGameEngine {
public:
    //std::vector<std::vector<std::unique_ptr<olc::Sprite>>> cardSprites;
    //std::vector<std::unique_ptr<olc::Sprite>> playerSprites;
    ChessFishVisualiserUI() {
        sAppName = "ChessFishVisualiser";
    }

public:
    bool OnUserCreate() override {
        for (int i = 0; i < 64; ++i) {
            moves[i] = bishop_attacks_on_the_fly(i,50ULL);
        }
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        // Clear the screen
        Clear(olc::BLACK);

        // Draw the button
        DrawButton("next bitboard!", ScreenWidth() / 2 - 50, ScreenHeight() / 2 - 25, 150, 50, olc::WHITE, olc::DARK_GREY);

        // Draw the counter value
        DrawString(ScreenWidth() / 2 - 10, ScreenHeight() / 2 + 30, std::to_string(bitb));

        // Optional: Provide a list of squares to mark with a purple dot
        std::vector<int> purpleSquares = { 1, 4, 5};

        // Called once per frame, draws random coloured pixels
        DrawChessboard(CHESS_SIZE, CELL_SIZE, moves[bitb], purpleSquares);

        // Check for button click
        if (GetMouse(0).bPressed){
            int buttonX = ScreenWidth() / 2 - 50;
            int buttonY = ScreenHeight() / 2 - 25;
            int buttonWidth = 150;
            int buttonHeight = 50;

            if (GetMouseX() >= buttonX && GetMouseX() <= buttonX + buttonWidth &&
                GetMouseY() >= buttonY && GetMouseY() <= buttonY + buttonHeight){
                // Button clicked, increase the counter
                bitb = (bitb+1)%64;
            }
        }
        return true;
    }

private:
    int bitb = 0;
    U64 moves[64];

    // Function to draw a chessboard
    void DrawChessboard(int size, int cellSize, std::optional<uint64_t> bitboard = std::nullopt, std::optional<std::vector<int>> purpleSquares = std::nullopt){
        for (int i = 0; i < size; ++i){
            for (int j = 0; j < size; ++j){
                // Alternate between white and black cells
                olc::Pixel color = (i + j) % 2 == 0 ? olc::WHITE : olc::BLACK;

                // If bitboard is provided, add a bit of blue and red based on the bit value
                if (bitboard.has_value())
                {
                    int squareIndex = i * size + j;
                    color = ((bitboard.value() >> squareIndex) & 1) ? olc::Pixel(color.r + INTENSITY, color.g, color.b) : olc::Pixel(color.r, color.g, color.b + INTENSITY);
                }

                // Draw a rectangle for each cell
                FillRect(TOP_LEFT_X_FIELD+ j * cellSize,TOP_LEFT_y_FIELD+ i * cellSize, cellSize, cellSize, color);

                // If purpleSquares is provided, check if the current square index is in the list
                if (purpleSquares.has_value()) {
                    int squareIndex = i * size + j;
                    if (std::find(purpleSquares.value().begin(), purpleSquares.value().end(), squareIndex) != purpleSquares.value().end()) {
                        FillCircle(TOP_LEFT_X_FIELD + (j + 0.5) * cellSize, TOP_LEFT_y_FIELD + (i + 0.5) * cellSize, cellSize / 4, olc::MAGENTA);
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