#define OLC_PGE_APPLICATION
#include "GUIEngine.h"
#include <optional>
#include <utility>
#include "../moves.h"
#include "../magic_numbers/MagicsTester.h"
#include "../engines/ChessEngine.h"
#include "../engines/RandomEngine.h"
#include "../engines/MonteCarloEngine.h"
#include "../engines/MiniMaxEngine.h"
#include "../configurations.h" /* include all configurations for the visual engine */

#include "../magic_numbers/MagicsTester.h"

struct EngineInfo {
    std::string name;  // Name of the engine
    Engines id; //Engine id
    std::unique_ptr<olc::Sprite> pLogo; // the logo for the engine used in the selection draw

    // Constructor to load the logo
    EngineInfo(const char* name, Engines id, const std::string& logoPath)
            : name(name), id(id), pLogo(std::make_unique<olc::Sprite>(logoPath)) {}
};

struct DetailedEndGameResult {
    std::string description; // Detailed description of the end game result
    std::unique_ptr<olc::Sprite> pSprite; // Sprite associated with the result
    EndGameResult id; // the effective result

    // Constructor to load the sprite
    DetailedEndGameResult(std::string  desc, EndGameResult id, const std::string& spritePath)
            : description(std::move(desc)), id(id), pSprite(std::make_unique<olc::Sprite>(spritePath)) {}
};

// Function to calculate the distance between two points (x1, y1) and (x2, y2)
inline double calculateDistance(int x1, int y1, int x2, int y2) {
    return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

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

        engineInfoArray.emplace_back("Human Engine", HUMAN, "../assets/engine_logos/human.png");
        engineInfoArray.emplace_back("Minimax Engine", MINIMAX, "../assets/engine_logos/minimax.png");
        engineInfoArray.emplace_back("Random Engine", RANDOM, "../assets/engine_logos/random.png");
        engineInfoArray.emplace_back("Monte Carlo Engine", MONTE_CARLO, "../assets/engine_logos/monte-carlo.png");


        detailedResults.emplace_back("white wins by Checkmate", WHITE_WINS_Checkmate, "../assets/end_screens/white/white-won-Checkmate.png");
        detailedResults.emplace_back("white wins by Resignation", WHITE_WINS_Resignation, "../assets/end_screens/white/white-won-Resignation.png");
        detailedResults.emplace_back("white wins by Timeout", WHITE_WINS_Timeout, "../assets/end_screens/white/white-won-Timeout.png");

        detailedResults.emplace_back("black wins by Checkmate", BLACK_WINS_Checkmate, "../assets/end_screens/black/black-won-Checkmate.png");
        detailedResults.emplace_back("black wins by Resignation", BLACK_WINS_Resignation, "../assets/end_screens/black/black-won-Resignation.png");
        detailedResults.emplace_back("black wins by Timeout", BLACK_WINS_Timeout, "../assets/end_screens/black/black-won-Timeout.png");

        detailedResults.emplace_back("draw by Stalemate", DRAW_Stalemate, "../assets/end_screens/draw/draw-Stalemate.png");
        detailedResults.emplace_back("draw by Insufficient material", DRAW_Insufficient_material, "../assets/end_screens/draw/draw-Insufficient_material.png");
        detailedResults.emplace_back("draw by 50 move-rule", DRAW_50_move_rule, "../assets/end_screens/draw/draw-50-move-rule.png");
        detailedResults.emplace_back("draw by Repetition", DRAW_Repetition, "../assets/end_screens/draw/draw-Repetition.png");
        detailedResults.emplace_back("draw by Agreement", DRAW_Agreement, "../assets/end_screens/draw/draw-Agreement.png");

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
            if(loop && !humanPlay()) doMove = true;
        }

        DrawChessboard(CHESS_SIZE, CELL_SIZE,  /*calculateKingDanger(&bord)*/ /* is_attacked(E8,&bord)*/ mask /*selectedSquare==-1 ? 0ULL : mask */ /*1ULL << (63-selectedSquare)*/  /*moves[bitb]*/ /*, purpleSquares, greenSquares*/);

        SetPixelMode(olc::Pixel::MASK); // Don't draw pixels which have any transparency
        DrawSprite(WHITE_ENGINE_SELECTION_X,WHITE_ENGINE_SELECTION_Y,getEngineSprite(selectedWhiteEngine));
        DrawString((WHITE_ENGINE_SELECTION_X+BLACK_ENGINE_SELECTION_X) /2 + engineInfoArray.at(0).pLogo->width/2, (WHITE_ENGINE_SELECTION_Y+BLACK_ENGINE_SELECTION_Y) /2 + engineInfoArray.at(0).pLogo->height/2, "VS");
        DrawSprite(BLACK_ENGINE_SELECTION_X,BLACK_ENGINE_SELECTION_Y,getEngineSprite(selectedBlackEngine));
        SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels

        // Check for button click
        if (GetMouse(0).bPressed || doMove){
            int x = GetMouseX();
            int y = GetMouseY();

            // Calculate the row and column of the clicked cell
            int row = (y - TOP_LEFT_y_FIELD) / CELL_SIZE;
            int col = (x - TOP_LEFT_X_FIELD) / CELL_SIZE;

            if(gameOver){
                if(calculateDistance(x, y, END_GAME_X + detailedResults.at(0).pSprite->height * END_GAME_SIZE / 2, END_GAME_Y + detailedResults.at(0).pSprite->width * END_GAME_SIZE / 2) <= END_GAME_TOLERANCE*END_GAME_SIZE || loopGames){
                    reset();
                }
            }else{
                // Check if the click is within the chessboard boundaries
                if (row >= 0 && row < CHESS_SIZE &&
                    col >= 0 && col < CHESS_SIZE || doMove) {
                    doMove = false;
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
                                //cout << "white move made" << endl;
                                //printFancyBoard(&bord);
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
                                //cout << "black move made" << endl;
                                //printFancyBoard(&bord);
                            }

                            if(isEnded(&bord)) {
                                //cout << "game ended before this" << endl;
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
                } else if (calculateDistance(x, y, WHITE_ENGINE_SELECTION_X + engineInfoArray.at(0).pLogo->width / 2, WHITE_ENGINE_SELECTION_Y + engineInfoArray.at(0).pLogo->height / 2) <= ENGINE_TOLERANCE && !doMove){
                    selectedWhiteEngine = (selectedWhiteEngine +1 ) % amountOfEngines;
                    message = "white engine rotated";
                }
                else if (calculateDistance(x, y, BLACK_ENGINE_SELECTION_X + engineInfoArray.at(0).pLogo->width / 2, BLACK_ENGINE_SELECTION_Y + engineInfoArray.at(0).pLogo->height / 2) <= ENGINE_TOLERANCE && !doMove){
                    selectedBlackEngine = (selectedBlackEngine +1 ) % amountOfEngines;
                    message = "black engine rotated";
                }else if (x >= START_SIMULATION_X && x < START_SIMULATION_X+START_SIMULATION_WIDTH*TEXT_SIZE &&
                          y >= START_SIMULATION_Y && y < START_SIMULATION_Y+START_SIMULATION_HEIGHT*TEXT_SIZE){
                    loop = true;
                }else if (x >= RESET_SIMULATION_X && x < RESET_SIMULATION_X+RESET_SIMULATION_WIDTH*TEXT_SIZE &&
                          y >= RESET_SIMULATION_Y && y < RESET_SIMULATION_Y+RESET_SIMULATION_HEIGHT*TEXT_SIZE){
                    simulationResults.reset();
                    loopGames = 0;
                    loop = false;
                    reset();
                }else if (x >= RESET_SIMULATION_X+SIMULATION_BUTTON_INC_X*2 && x < RESET_SIMULATION_X+SIMULATION_BUTTON_INC_X*2 +INC_SIMULATION_WIDTH*TEXT_SIZE &&
                          y >= RESET_SIMULATION_Y && y < RESET_SIMULATION_Y+INC_SIMULATION_HEIGHT*TEXT_SIZE){
                    loopGames += 1;
                }else if (x >= RESET_SIMULATION_X+SIMULATION_BUTTON_INC_X*3 && x < RESET_SIMULATION_X+SIMULATION_BUTTON_INC_X*3 +INC_SIMULATION_WIDTH*TEXT_SIZE &&
                         y >= RESET_SIMULATION_Y && y < RESET_SIMULATION_Y+INC_SIMULATION_HEIGHT*TEXT_SIZE){
                    loopGames += 10;
                }else if (x >= RESET_SIMULATION_X+SIMULATION_BUTTON_INC_X*4 && x < RESET_SIMULATION_X+SIMULATION_BUTTON_INC_X*4 +INC_SIMULATION_WIDTH*TEXT_SIZE &&
                          y >= RESET_SIMULATION_Y && y < RESET_SIMULATION_Y+INC_SIMULATION_HEIGHT*TEXT_SIZE){
                    loopGames += 100;
                }else if (x >= RESET_SIMULATION_X+SIMULATION_BUTTON_INC_X*5 && x < RESET_SIMULATION_X+SIMULATION_BUTTON_INC_X*5 +INC_SIMULATION_WIDTH*TEXT_SIZE &&
                          y >= RESET_SIMULATION_Y && y < RESET_SIMULATION_Y+INC_SIMULATION_HEIGHT*TEXT_SIZE){
                    loopGames += 1'000;
                }
                else {
                    message = "Clicked outside the chessboard"; // The mouse click is outside the chessboard
                }
            }
        }

        DrawString(ScreenWidth() / 2 - 10, ScreenHeight() / 2 + 30, message);
        DrawString(CASTELING_RIGHTS_X, CASTELING_RIGHTS_Y, "castling rights:",olc::WHITE,TEXT_SIZE);
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

        /* draw results of the simulations */
        DrawString(SIMULATION_RESULT_X, SIMULATION_RESULT_Y, "games to go: " + std::to_string(loopGames) + " simulation results: ",olc::WHITE,TEXT_SIZE);
        DrawString(SIMULATION_RESULT_X, SIMULATION_RESULT_Y+30, "White wins: " + std::to_string(simulationResults.getWhiteWins()/LOOP_FRAMES) +   " Black Wins: " +std::to_string(simulationResults.getBlackWins()/LOOP_FRAMES) + " Draws: " +std::to_string(simulationResults.getDraws()/LOOP_FRAMES) ,olc::WHITE,TEXT_SIZE);
        DrawButton("start",START_SIMULATION_X,START_SIMULATION_Y,START_SIMULATION_WIDTH,START_SIMULATION_HEIGHT,olc::WHITE,olc::GREY,TEXT_SIZE);
        DrawButton("reset",RESET_SIMULATION_X,RESET_SIMULATION_Y,RESET_SIMULATION_WIDTH,RESET_SIMULATION_HEIGHT,olc::WHITE,olc::GREY,TEXT_SIZE);
        DrawButton("1",RESET_SIMULATION_X+SIMULATION_BUTTON_INC_X*2,RESET_SIMULATION_Y,INC_SIMULATION_WIDTH,INC_SIMULATION_HEIGHT,olc::WHITE,olc::GREY,TEXT_SIZE);
        DrawButton("10",RESET_SIMULATION_X+SIMULATION_BUTTON_INC_X*3,RESET_SIMULATION_Y,INC_SIMULATION_WIDTH,INC_SIMULATION_HEIGHT,olc::WHITE,olc::GREY,TEXT_SIZE);
        DrawButton("100",RESET_SIMULATION_X+SIMULATION_BUTTON_INC_X*4,RESET_SIMULATION_Y,INC_SIMULATION_WIDTH,INC_SIMULATION_HEIGHT,olc::WHITE,olc::GREY,TEXT_SIZE);
        DrawButton("1000",RESET_SIMULATION_X+SIMULATION_BUTTON_INC_X*5,RESET_SIMULATION_Y,INC_SIMULATION_WIDTH,INC_SIMULATION_HEIGHT,olc::WHITE,olc::GREY,TEXT_SIZE);


        /* draw the end game screen */
        if (gameOver){
            SetPixelMode(olc::Pixel::MASK); // Don't draw pixels which have any transparency
            if(isChekmate(&bord)){
                if (bord.whiteToPlay){
                    DrawSprite(END_GAME_X,END_GAME_Y, getEndGameSprite(BLACK_WINS_Checkmate),END_GAME_SIZE);
                    if(loopGames > 0) simulationResults.update(BLACK_WINS_Checkmate);
                }
                else {
                    DrawSprite(END_GAME_X,END_GAME_Y,getEndGameSprite(WHITE_WINS_Checkmate),END_GAME_SIZE);
                    if(loopGames > 0) simulationResults.update(WHITE_WINS_Checkmate);
                }
            }else{ //we assume it's a draw else (this is the only way in this implementation to get here but can change later)
                if(bord.halfmoveClock >= 100){
                    DrawSprite(END_GAME_X,END_GAME_Y,getEndGameSprite(DRAW_50_move_rule),END_GAME_SIZE);
                    if(loopGames > 0) simulationResults.update(DRAW_50_move_rule);
                }
                else{
                    DrawSprite(END_GAME_X,END_GAME_Y,getEndGameSprite(DRAW_Stalemate),END_GAME_SIZE);
                    if(loopGames > 0) simulationResults.update(DRAW_Stalemate);
                }
            }
            if(loopGames > 0) reset();
            SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels
        }

        return true;
    }

private:
    U64 loopNumber = 0;
    bool doMove = false;
    int loopGames = 0;
    bool loop = false;
    SimulationResults simulationResults;

    Board bord{};
    olc::Sprite spriteSheet;
    /* engine info structs */
    std::vector<EngineInfo> engineInfoArray;
    std::vector<DetailedEndGameResult> detailedResults;
    /*
    olc::Sprite minimaxLogo;
    olc::Sprite randomLogo;
    olc::Sprite monteCarloLogo;

    olc::Sprite humanLogo;
     */

    int selectedSquare = -1;
    std::string message;
    U64 mask = 0ULL;
    bool gameOver = false;

    int selectedWhiteEngine = 0;
    int selectedBlackEngine = 0;

    int amountOfEngines = NUM_ENGINES;

    ChessEngine* randomEngine = new RandomChessEngine();
    ChessEngine* randomMonteCarloEngine = new MonteCarloEngine(false, 10, 100, randomEngine);
    ChessEngine* miniMaxEngine = new MiniMaxEngine(5);

    void reset(){
        gameOver = false;
        setup(&bord);
        loopGames--;
        if(loopGames <= 0){
            loopGames = 0;
            loop = false;
            doMove = false;
        }
    }

    bool humanPlay() const {
       return (bord.whiteToPlay ? (selectedWhiteEngine == HUMAN) : (selectedBlackEngine == HUMAN));
    }

    olc::Sprite* getEngineSprite(int engine){
        auto it = std::find_if(engineInfoArray.begin(), engineInfoArray.end(),
                               [engine](const EngineInfo& engineCurr) { return engineCurr.id == engine; });

        if (it != engineInfoArray.end()) {
            // Found the engine with the desired ID
            return it->pLogo.get();
        } else {
            // Engine with the desired ID not found
            return engineInfoArray.at(0).pLogo.get(); //TODO not implemented logo
        }
    }

    olc::Sprite* getEndGameSprite(EndGameResult endGameResult ){
        auto it = std::find_if(detailedResults.begin(), detailedResults.end(),
                               [endGameResult](const DetailedEndGameResult& detailedEndGameResult) { return detailedEndGameResult.id == endGameResult; });

        if (it != detailedResults.end()) {
            // Found the engine with the desired ID
            return it->pSprite.get();
        } else {
            // Engine with the desired ID not found
            return detailedResults.at(0).pSprite.get(); //TODO not implemented logo
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

    void DrawButton(const std::string& text, int x, int y, int width, int height, olc::Pixel textCol, olc::Pixel buttonCol, int scale)
    {
        width *= scale;
        height *= scale;

        // Draw the button background
        FillRect(x, y, width, height, buttonCol);

        // Draw the button border
        DrawRect(x, y, width, height, olc::WHITE);

        // Draw the button text
        int textWidth = text.length() * 8 * scale; // NOLINT(*-narrowing-conversions)
        int textHeight = 8 * scale;
        DrawString(x + (width - textWidth) / 2, y + (height - textHeight) / 2, text, textCol, scale);
    }

};