cmake_minimum_required(VERSION 3.10)
project(ChessAI)

set(CMAKE_CXX_STANDARD 17)

set(ENGINE_SOURCES src/Engine.cpp)

add_executable(ChessAI src/main.cpp src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})
add_executable(BoardTest src/BoardTest.cpp src/Board.cpp  src/PrintMoves.cpp src/MoveGenerator.cpp ${ENGINE_SOURCES})
add_executable(PawnMoveTests
src/PawnMoveTests.cpp
src/Board.cpp
src/MoveGenerator.cpp
src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(KnightMoveTests
    src/KnightMoveTests.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(SliderMoveTests
    src/SliderMoveTests.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(KingMoveTests
    src/KingMoveTests.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

# Include directories
target_include_directories(ChessAI PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(PawnMoveTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(KnightMoveTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(SliderMoveTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(KingMoveTests PRIVATE ${CMAKE_SOURCE_DIR}/src)






