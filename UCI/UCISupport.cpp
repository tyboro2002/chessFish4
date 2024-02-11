#include "UCISupport.h"

[[noreturn]] void UCISupport::runUCI() {
    std::string command;

    while (true) {
        // Wait for input from the GUI
        std::getline(std::cin, command);

        // Process the received command
        processUCICommand(command);
    }
}

void UCISupport::processUCICommand(const std::string& command) {
    // Check the received command and take appropriate actions
    if (command == "uci") {
        setup(&board);
        // Engine identification
        sendUCIResponse("id name ChessFish");
        sendUCIResponse("id author Tybo Verslype");
        // Send supported options if any
        // ...

        // Acknowledge UCI mode
        sendUCIResponse("uciok");
    } else if (command == "isready") {
        // Respond to "isready" command
        sendUCIResponse("readyok");
    } else if (command == "d") {
        // Respond to "isready" command
        printFancyBoard(&board);
    } else if (command.substr(0, 8) == "position") {
        // Handle the "position" command
        if (command.find("startpos") != std::string::npos) {
            // Handle "position startpos"
            setup(&board);

        } else if (command.substr(9, 3) == "fen") {
            // Handle "position fen <FEN>"
            // Extract the FEN (Forsyth-Edwards Notation) from the command
            std::string fen = command.substr(13);
            // Process the position as needed
            setupEmpty(&board);
            readInFen(&board,fen.c_str());
        }
    } else if (command.substr(0, 2) == "go") {
        std::istringstream iss(command);
        std::string subCommand;
        int depth = -1;
        int moveTime = -1;

        // Parse and run the "go" command options immediately
        while (iss >> subCommand) {
            if (subCommand == "depth" && iss >> depth) {
                goDepth(command);
                return;  // Exit after processing the command
            } else if (subCommand == "movetime" && iss >> moveTime) {
                goMoveTime(command);
                return;  // Exit after processing the command
            }
            // Add other "go" options as needed
        }
    } else if (command == "quit") {
        // Handle the "quit" command
        // ...

        // Exit the loop and terminate the engine
        exit(0);
    } else {
        // Handle other UCI commands as needed
        // ...
    }
}

void UCISupport::sendUCIResponse(const std::string& response) {
    // Send the UCI response to the GUI
    std::cout << response << std::endl;
}

void UCISupport::goDepth(const std::string& command) {
    // Handle the "go" command with "depth" option
    // Extract depth from the command string
    size_t pos = command.find("depth");
    if (pos != std::string::npos) {
        // parse the depth
        std::string depthString = command.substr(pos + 5);
        int depth = std::stoi(depthString);

        // initiate the engine for this depth
        engine = new ENGINE_DEPTH(depth);
        Action action;

        // get the best move from the engine
        action = engine->getPreferredAction(&board);
        std::string move = actionToString(&action);

        // return the best move
        sendUCIResponse("bestmove " + move);  // Replace <best move> with the actual best move
    }
}

void UCISupport::goMoveTime(const std::string& command) {
    // Handle the "go" command with "movetime" option
    // Extract moveTime from the command string
    size_t pos = command.find("movetime");
    if (pos != std::string::npos) {
        // parse the time
        std::string moveTimeString = command.substr(pos + 8);
        int moveTime = std::stoi(moveTimeString);

        // initiate the engine for this time
        engine = new ENGINE_TIME(moveTime);
        Action action;

        // get the best move from the engine
        action = engine->getPreferredAction(&board);
        std::string move = actionToString(&action);

        // return the best move
        sendUCIResponse("bestmove " + move);  // Replace <best move> with the actual best move
    }
}