cmake_minimum_required(VERSION 3.10)
project(Aphelion)

find_package(Threads REQUIRED)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)

set(ENGINE_SOURCES
    src/Engine.cpp
    src/EngineEvaluation.cpp
    src/EngineSearch.cpp
    src/Zobrist.cpp
    src/Magic.cpp
    src/MoveGeneratorUtils.cpp
    src/OpeningBook.cpp
    src/Tablebase.cpp)

add_executable(Example src/Example.cpp src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})
add_executable(BoardTest test/BoardTest.cpp src/Board.cpp  src/PrintMoves.cpp src/MoveGenerator.cpp ${ENGINE_SOURCES})
add_executable(PawnMoveTests
    test/PawnMoveTests.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(KnightMoveTests
    test/KnightMoveTests.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(SliderMoveTests
    test/SliderMoveTests.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(KingMoveTests
    test/KingMoveTests.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(IllegalMoveTests
    test/IllegalMoveTests.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(GamePhaseTests
    test/GamePhaseTests.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(LegalMoveGenerationTest
    test/LegalMoveGenerationTest.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(PerftTest
    test/PerftTest.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/Perft.cpp ${ENGINE_SOURCES})

# Example programs
add_executable(CreatePosition examples/create_position.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})
add_executable(GenerateMoves examples/generate_moves.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})
add_executable(SearchBestMove examples/search_best_move.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})
add_executable(Perft examples/perft.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp src/Perft.cpp ${ENGINE_SOURCES})
add_executable(Aphelion
    src/UCI.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})

# Include directories
target_include_directories(Example PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(BoardTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(PawnMoveTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(KnightMoveTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(SliderMoveTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(KingMoveTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(IllegalMoveTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(GamePhaseTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(LegalMoveGenerationTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(CreatePosition PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(GenerateMoves PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(SearchBestMove PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(Perft PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(PerftTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(Aphelion PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(Aphelion PRIVATE Threads::Threads)

# Register tests with CTest
add_test(NAME BoardTest COMMAND BoardTest)
add_test(NAME PawnMoveTests COMMAND PawnMoveTests)
add_test(NAME KnightMoveTests COMMAND KnightMoveTests)
add_test(NAME SliderMoveTests COMMAND SliderMoveTests)
add_test(NAME KingMoveTests COMMAND KingMoveTests)
add_test(NAME PerftTest COMMAND PerftTest)
add_test(NAME IllegalMoveTests COMMAND IllegalMoveTests)
add_test(NAME GamePhaseTests COMMAND GamePhaseTests)
add_test(NAME LegalMoveGenerationTest COMMAND LegalMoveGenerationTest)






