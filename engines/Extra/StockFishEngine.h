#pragma once
#include "../ChessEngine.h"
#include "../../configurations.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>

class StockFishEngine : public ChessEngine {
public:
    explicit StockFishEngine(int timeGiven) : time(timeGiven == 60 ? 1 : timeGiven) {}

    void initialize() override {
        std::cout << "Stockfish Chess Engine initialized.\n";
        startStockfish();
    }

    void makeMove(Board* board) override {
        // Use Stockfish to find the best move
        Action move = getStockFishMove(board);
        //printAction(&move);
        movePiece(board, &move);
    }

private:
    int time = 0;

    void startStockfish() {
        std::string command = "touch ";
        command += COMMANDS_PATH;
        system(command.c_str());
    }

    Action getStockFishMove(Board* bord){
        // Open the file for writing
        std::ofstream file(COMMANDS_PATH);

        // Check if the file is open
        if (!file.is_open()) {
            // Handle the error (e.g., print an error message)
            return {0,0,Non_Exceptional};
        }

        // Write the content to the file
        file << "position fen " << convertToFEN(bord) << "\n";
        file << "go movetime " << time << "000\n";
        file << "d\n";
        file << "go movetime 1000\n";

        // Close the file
        file.close();

        std::string command = "stockfish < ";
        command += COMMANDS_PATH;
        command += " > ";
        command += OUTPUT_PATH;
        system(command.c_str());


        // Open the file for reading
        std::ifstream readfile(OUTPUT_PATH);

        // Check if the file is open
        if (!readfile.is_open()) {
            // Handle the error (e.g., print an error message)
            std::cerr << "Error opening file for reading." << std::endl;
            return {0,0,Non_Exceptional};
        }

        std::string line;

        // Read lines from the file
        while (std::getline(readfile, line)) {
            // Check if the line starts with "bestmove"
            if (line.find("bestmove") != std::string::npos) {
                // Print the line with "bestmove"
                //std::cout << "Found line: " << line << std::endl;
                std::istringstream iss(line);
                std::string bestmove;
                iss >> bestmove >> std::ws;  // Skip whitespace

                if (!(iss >> bestmove)) {
                    std::cerr << "Error extracting move from line." << std::endl;
                    return {0,0,Non_Exceptional};
                }

                Action action;
                action.src = (7-(bestmove[0] - 'a')) + (bestmove[1] - '1') * 8;
                action.dst = (7-(bestmove[2] - 'a')) + (bestmove[3] - '1') * 8;
                return action;
            }
        }

        // Close the file
        readfile.close();

        return {0,0,Non_Exceptional};
    }
};
