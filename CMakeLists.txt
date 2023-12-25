cmake_minimum_required(VERSION 3.27)
project(ChessFish4)

set(CMAKE_CXX_STANDARD 17)

add_executable(ChessFish4 main.cpp
        game.h
        game.cpp
        engine.h
        engine.cpp
        timing.cpp
        tests.h
        tests.cpp
        chessFish4Game.cpp
        chessFish4Game.h)
