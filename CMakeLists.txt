cmake_minimum_required(VERSION 3.10)
project(ChessAI)

set(CMAKE_CXX_STANDARD 17)

add_executable(ChessAI src/main.cpp src/Board.cpp src/MoveGenerator.cpp )
add_executable(BoardTest src/BoardTest.cpp src/Board.cpp)



