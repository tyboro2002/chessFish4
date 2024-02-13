cmake_minimum_required(VERSION 3.27)
project(ChessFish4)

set(CMAKE_CXX_STANDARD 20)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(ChessFish4 main.cpp
        moves.cpp
        moves.h
        game.h
        game.cpp
        old_code/engine.h
        old_code/engine.cpp
        timing/timing.cpp
        tests/tests.h
        tests/tests.cpp
        old_code/chessFish4Game.cpp
        old_code/chessFish4Game.h
        tests/testMoves.h
        visuals/GUIEngine.h
        visuals/GUIEngineGame.h
        magic_numbers/MagicNumberGenerator.cpp
        magic_numbers/MagicNumberGenerator.h
        magic_numbers/MagicsTester.cpp
        magic_numbers/MagicsTester.h
        engines/ChessEngine.h
        engines/RandomEngine.cpp
        engines/RandomEngine.h
        engines/MonteCarloEngine.cpp
        engines/MonteCarloEngine.h
        engines/MiniMaxEngine.cpp
        engines/MiniMaxEngine.h
        configurations.h
        engines/IterativeDeepeningMinimaxEngine.cpp
        engines/IterativeDeepeningMinimaxEngine.h
        engines/Extra/StockFishEngine.cpp
        engines/Extra/StockFishEngine.h
        UCI/UCISupport.cpp
        UCI/UCISupport.h
        OpeningBook/OpeningBook.cpp
        OpeningBook/OpeningBook.h
)

target_link_libraries(ChessFish4 PRIVATE
        -lpng             # for libpng
        -lGL              # for OpenGL
        -lX11             # for X Window System
        -pthread          # for pthread
)