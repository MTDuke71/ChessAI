cmake_minimum_required(VERSION 3.10)
project(ChessAI)

set(CMAKE_CXX_STANDARD 17)

add_executable(ChessAI src/main.cpp src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp )
add_executable(BoardTest src/BoardTest.cpp src/Board.cpp  src/PrintMoves.cpp src/MoveGenerator.cpp)
add_executable(PawnMoveTests 
src/PawnMoveTests.cpp 
src/Board.cpp 
src/MoveGenerator.cpp
src/PrintMoves.cpp)

# Include directories
target_include_directories(ChessAI PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(PawnMoveTests PRIVATE ${CMAKE_SOURCE_DIR}/src)






