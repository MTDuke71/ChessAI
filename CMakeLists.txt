cmake_minimum_required(VERSION 3.10)
project(Aphelion)

# Enable Interprocedural Optimization (Link Time Code Generation) in Release mode
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# Set high-speed optimization flags for MSVC in Release mode
if(MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ot /Ob2 /Oi /Oy /DNDEBUG")
  set(CMAKE_C_FLAGS_RELEASE "/O2 /Ot /Ob2 /Oi /Oy /DNDEBUG")
endif()

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
    src/MoveEncoding.cpp
    src/OpeningBook.cpp
    src/Tablebase.cpp
    src/FastMoveGenerator.cpp)

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

add_executable(EnPassantRightCaptureTest
    test/EnPassantRightCaptureTest.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(EngineMoveSelectionTest
    test/EngineMoveSelectionTest.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(GamePhaseTests
    test/GamePhaseTests.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(DrawRuleTests
    test/DrawRuleTests.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(LegalMoveGenerationTest
    test/LegalMoveGenerationTest.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(StalemateTest
    test/StalemateTest.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(CheckmateTest
    test/CheckmateTest.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(UCICastlingRegressionTest
    test/UCICastlingRegressionTest.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(PerftTest
    test/PerftTest.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/Perft.cpp ${ENGINE_SOURCES})

add_executable(PerftDivideTest
    test/PerftDivideTest.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/Perft.cpp ${ENGINE_SOURCES})

add_executable(PerftSuiteTest
    test/PerftSuiteTest.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/Perft.cpp ${ENGINE_SOURCES})

add_executable(TestPosition2
    test/TestPosition2.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/Perft.cpp ${ENGINE_SOURCES})

add_executable(InvestigatePosition3
    test/InvestigatePosition3.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/Perft.cpp ${ENGINE_SOURCES})

add_executable(MVVLVAArrayTest
    test/MVVLVAArrayTest.cpp)

add_executable(TranspositionTableResizeTest 
    test/TranspositionTableResizeTest.cpp 
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(PassedPawnEvaluationTest
    test/PassedPawnEvaluationTest.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(ActivityBonusEvaluationTest
    test/ActivityBonusEvaluationTest.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(KingSafetyEvaluationTest
    test/KingSafetyEvaluationTest.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(PawnStructureEvaluationTest
    test/PawnStructureEvaluationTest.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(OpeningBookPolyglotTest
    test/OpeningBookPolyglotTest.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})

add_executable(IncrementalAttackUpdateTest
    test/IncrementalAttackUpdateTest.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})
target_include_directories(IncrementalAttackUpdateTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Example programs
add_executable(CreatePosition examples/create_position.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})
add_executable(GenerateMoves examples/generate_moves.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})
add_executable(SearchBestMove examples/search_best_move.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})
add_executable(Perft examples/perft.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp src/Perft.cpp ${ENGINE_SOURCES})
add_executable(PrintBook examples/print_book.cpp
    src/Board.cpp src/MoveGenerator.cpp src/PrintMoves.cpp ${ENGINE_SOURCES})
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
target_include_directories(EnPassantRightCaptureTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(EngineMoveSelectionTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(GamePhaseTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(DrawRuleTests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(LegalMoveGenerationTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(StalemateTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(CheckmateTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(UCICastlingRegressionTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(CreatePosition PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(GenerateMoves PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(SearchBestMove PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(Perft PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(PrintBook PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(PerftTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(PerftDivideTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(PerftSuiteTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(Aphelion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(MVVLVAArrayTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(TranspositionTableResizeTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(PassedPawnEvaluationTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(ActivityBonusEvaluationTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(KingSafetyEvaluationTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(PawnStructureEvaluationTest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(OpeningBookPolyglotTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(Aphelion PRIVATE Threads::Threads)

# Register tests with CTest
add_test(NAME BoardTest COMMAND BoardTest)
add_test(NAME PawnMoveTests COMMAND PawnMoveTests)
add_test(NAME KnightMoveTests COMMAND KnightMoveTests)
add_test(NAME SliderMoveTests COMMAND SliderMoveTests)
add_test(NAME KingMoveTests COMMAND KingMoveTests)
add_test(NAME PerftTest COMMAND PerftTest)
add_test(NAME PerftDivideTest COMMAND PerftDivideTest)
add_test(NAME IllegalMoveTests COMMAND IllegalMoveTests)
add_test(NAME EnPassantRightCaptureTest COMMAND EnPassantRightCaptureTest)
add_test(NAME EngineMoveSelectionTest COMMAND EngineMoveSelectionTest)
add_test(NAME GamePhaseTests COMMAND GamePhaseTests)
add_test(NAME DrawRuleTests COMMAND DrawRuleTests)
add_test(NAME LegalMoveGenerationTest COMMAND LegalMoveGenerationTest)
add_test(NAME StalemateTest COMMAND StalemateTest)
add_test(NAME CheckmateTest COMMAND CheckmateTest)
add_test(NAME UCICastlingRegressionTest COMMAND UCICastlingRegressionTest)
add_test(NAME MVVLVAArrayTest COMMAND MVVLVAArrayTest)
add_test(NAME TranspositionTableResizeTest COMMAND TranspositionTableResizeTest)
add_test(NAME PassedPawnEvaluationTest COMMAND PassedPawnEvaluationTest)
add_test(NAME ActivityBonusEvaluationTest COMMAND ActivityBonusEvaluationTest)
add_test(NAME KingSafetyEvaluationTest COMMAND KingSafetyEvaluationTest)
add_test(NAME PawnStructureEvaluationTest COMMAND PawnStructureEvaluationTest)
add_test(NAME OpeningBookPolyglotTest COMMAND OpeningBookPolyglotTest)
add_test(NAME IncrementalAttackUpdateTest COMMAND IncrementalAttackUpdateTest)

add_executable(OriginalMoveTest
    test/OriginalMoveTest.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
)
target_include_directories(OriginalMoveTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(SystemComparison
    test/SystemComparison.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/FastMoveGenerator.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
)
target_include_directories(SystemComparison PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(PerftDivideComparison
    test/PerftDivideComparison.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/FastMoveGenerator.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
    src/Perft.cpp
)
target_include_directories(PerftDivideComparison PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(DebugD3Perft
    test/DebugD3Perft.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/FastMoveGenerator.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
)
target_include_directories(DebugD3Perft PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(DebugFastMoves
    test/DebugFastMoves.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/FastMoveGenerator.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
)
target_include_directories(DebugFastMoves PRIVATE ${CMAKE_SOURCE_DIR}/src)

# FastMoveGenerator Integration Test
add_executable(FastMoveGeneratorIntegrationTest 
    test/FastMoveGeneratorIntegrationTest.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/FastMoveGenerator.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
)
target_include_directories(FastMoveGeneratorIntegrationTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Original MoveGenerator Performance Test
add_executable(OriginalMoveGenPerformance 
    test/OriginalMoveGenPerformance.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
    src/Perft.cpp
)
target_include_directories(OriginalMoveGenPerformance PRIVATE ${CMAKE_SOURCE_DIR}/src)

# FastMoveGenerator Performance Test
add_executable(FastMoveGenPerformance 
    test/FastMoveGenPerformance.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/FastMoveGenerator.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
    src/Perft.cpp
)
target_include_directories(FastMoveGenPerformance PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Perft Differences Analyzer
add_executable(PerftDifferencesAnalyzer 
    test/PerftDifferencesAnalyzer.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/FastMoveGenerator.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
    src/Perft.cpp
)
target_include_directories(PerftDifferencesAnalyzer PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Drill Down Analyzer
add_executable(DrillDownAnalyzer 
    test/DrillDownAnalyzer.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/FastMoveGenerator.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
    src/Perft.cpp
)
target_include_directories(DrillDownAnalyzer PRIVATE ${CMAKE_SOURCE_DIR}/src)

# King Safety Bug Test
add_executable(KingSafetyBugTest 
    test/KingSafetyBugTest.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/FastMoveGenerator.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
)
target_include_directories(KingSafetyBugTest PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Detailed Position Analysis
add_executable(DetailedPositionAnalysis 
    test/DetailedPositionAnalysis.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/FastMoveGenerator.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
)
target_include_directories(DetailedPositionAnalysis PRIVATE ${CMAKE_SOURCE_DIR}/src)

# FastMoveGenerator Perft Validation Test
add_executable(FastMoveGeneratorPerftValidation 
    test/FastMoveGeneratorPerftValidation.cpp
    src/Board.cpp
    src/Magic.cpp
    src/Zobrist.cpp
    src/FastMoveGenerator.cpp
    src/MoveEncoding.cpp
    src/MoveGenerator.cpp
    src/MoveGeneratorUtils.cpp
    src/Perft.cpp
)
target_include_directories(FastMoveGeneratorPerftValidation PRIVATE ${CMAKE_SOURCE_DIR}/src)






