cmake_minimum_required(VERSION 3.27)
project(ChessFish4)

set(CMAKE_CXX_STANDARD 20)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(ChessFish4 main.cpp
        game.h
        game.cpp
        engine.h
        engine.cpp
        timing.cpp
        tests.h
        tests.cpp
        chessFish4Game.cpp
        chessFish4Game.h
        testMoves.h
        GUIEngine.h
        GUIEngineGame.h
        MagicNumberGenerator.cpp
        MagicNumberGenerator.h
        MagicsTester.cpp
        MagicsTester.h
        moves.cpp
        moves.h)

target_link_libraries(ChessFish4 PRIVATE
        -lpng             # for libpng
        -lGL              # for OpenGL
        -lX11             # for X Window System
        -pthread          # for pthread
)