#pragma once

#include <string>
#include <iostream>
#include "../game.h"
#include "../engines/MiniMaxEngine.h"
#include "../engines/IterativeDeepeningMinimaxEngine.h"

#define ENGINE_DEPTH MiniMaxEngine
#define ENGINE_TIME IterativeDeepeningMinimaxEngine

class UCISupport {
    Board board;
    ChessEngine* engine = new MiniMaxEngine(5);
public:
    [[noreturn]] void runUCI();
private:
    void processUCICommand(const std::string& command);
    void sendUCIResponse(const std::string& response);

    void goDepth(const std::string& command);
    void goMoveTime(const std::string& command);

    //UCI
    /*
    tell engine to use the uci (universal chess interface),
    this will be sent once as a first command after program boot to tell the engine to switch to uci mode.
    After receiving the uci command the engine must identify itself with the "id" command
    and sent the "option" commands to tell the GUI which engine settings the engine supports if any.
    After that the engine should send "uciok" to acknowledge the uci mode.
    If no uciok is sent within a certain time period, the engine task will be killed by the GUI.
     */

    //isready
    /*
    this is used to synchronize the engine with the GUI. When the GUI has sent a command or
    multiple commands that can take some time to complete,
    this command can be used to wait for the engine to be ready again or
    to ping the engine to find out if it is still alive.
    E.g. this should be sent after setting the path to the tablebases as this can take some time.
    This command is also required once before the engine is asked to do any search
    to wait for the engine to finish initializing.
    This command must always be answered with "readyok" and can be sent also when the engine is calculating
    in which case the engine should also immediately answer with "readyok" without stopping the search.
    */

};

