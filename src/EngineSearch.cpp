#include "Engine.h"
#include "BitUtils.h"
#include "MVVLVA.h"
#include "MoveEncoding.h"
#include <algorithm>
#include <chrono>
#include <atomic>
#include <future>
#include <iostream>
#include <sstream>
#include <array>

// -----------------------------------------------------------------------------
// Converts an encoded move into the compact UCI format (e.g., e2e4).
// -----------------------------------------------------------------------------
static std::string toUCIMove(uint16_t move) {
    std::string uci = decodeMove(move);
    auto dash = uci.find('-');
    if (dash != std::string::npos) uci.erase(dash, 1);
    return uci;
}

// -----------------------------------------------------------------------------
// Determines if the provided move results in a capture on the target square or
// via en passant.
// -----------------------------------------------------------------------------
static bool isCaptureMove(const Board& board, uint16_t move) {
    int from = moveFrom(move);
    int to = moveTo(move);
    Board::Color colorAtTo = board.pieceColorAt(to);
    if (board.isWhiteToMove()) {
        if (colorAtTo == Board::Color::Black) return true;
    } else {
        if (colorAtTo == Board::Color::White) return true;
    }
    if (board.getEnPassantSquare() == to) {
        uint64_t fromMask = 1ULL << from;
        if (board.isWhiteToMove()) {
            if (board.getWhitePawns() & fromMask) return true;
        } else {
            if (board.getBlackPawns() & fromMask) return true;
        }
    }
    return false;
}

// -----------------------------------------------------------------------------
// Returns the material value of the piece located at the given board index.
// -----------------------------------------------------------------------------
static int pieceValueAt(const Board& board, int index) {
    uint64_t mask = 1ULL << index;
    if (board.getWhitePawns() & mask || board.getBlackPawns() & mask) return 100;
    if (board.getWhiteKnights() & mask || board.getBlackKnights() & mask) return 320;
    if (board.getWhiteBishops() & mask || board.getBlackBishops() & mask) return 330;
    if (board.getWhiteRooks() & mask || board.getBlackRooks() & mask) return 500;
    if (board.getWhiteQueens() & mask || board.getBlackQueens() & mask) return 900;
    if (board.getWhiteKing() & mask || board.getBlackKing() & mask) return 20000;
    return 0;
}

// -----------------------------------------------------------------------------
// Returns the MVVLVA piece type at a given board index used for move ordering.
// -----------------------------------------------------------------------------
static int pieceTypeAt(const Board& board, int index) {
    uint64_t mask = 1ULL << index;
    if (board.getWhitePawns() & mask || board.getBlackPawns() & mask) return MVVLVA::Pawn;
    if (board.getWhiteKnights() & mask || board.getBlackKnights() & mask) return MVVLVA::Knight;
    if (board.getWhiteBishops() & mask || board.getBlackBishops() & mask) return MVVLVA::Bishop;
    if (board.getWhiteRooks() & mask || board.getBlackRooks() & mask) return MVVLVA::Rook;
    if (board.getWhiteQueens() & mask || board.getBlackQueens() & mask) return MVVLVA::Queen;
    if (board.getWhiteKing() & mask || board.getBlackKing() & mask) return MVVLVA::King;
    return MVVLVA::Pawn; // default, should not happen for valid board
}

// -----------------------------------------------------------------------------
// Recursively evaluates a capture sequence starting on the given square using
// static exchange evaluation (SEE).
// -----------------------------------------------------------------------------
static int seeRec(const MoveGenerator& gen, Board& board, int square) {
    bool side = board.isWhiteToMove();
    auto pseudo = gen.generateAllMoves(board, side);
    int best = -1000000;
    bool any = false;
    for (auto mv : pseudo) {
        int to = moveTo(mv);
        if (to != square) continue;
        if (!board.isMoveLegal(mv)) continue;
        any = true;
        int from = moveFrom(mv);
        int val = pieceValueAt(board, from);
        Board::MoveState state;
        board.makeMove(mv, state);
        int gain = val - seeRec(gen, board, square);
        board.unmakeMove(state);
        if (gain > best) best = gain;
    }
    if (!any) return 0;
    return best;
}

// -----------------------------------------------------------------------------
// Performs static exchange evaluation for a given move by simulating the capture
// sequence on the target square.
// -----------------------------------------------------------------------------
static int staticExchangeEval(const MoveGenerator& gen,
                              Board& board,
                              uint16_t move) {
    int from = moveFrom(move);
    int to = moveTo(move);
    if (!isCaptureMove(board, move)) return 0;
    int captured = pieceValueAt(board, to);
    if (!captured && board.getEnPassantSquare() == to)
        captured = 100;
    Board::MoveState state;
    board.makeMove(move, state);
    int result = captured - seeRec(gen, board, to);
    board.unmakeMove(state);
    return result;
}

// -----------------------------------------------------------------------------
// Computes a heuristic score for move ordering using MVV/LVA and SEE.
// -----------------------------------------------------------------------------
static int moveScore(Board& board,
                     uint16_t move,
                     const MoveGenerator& gen) {
    int from = moveFrom(move);
    int to = moveTo(move);
    int capturedVal = pieceValueAt(board, to);
    if (!capturedVal && board.getEnPassantSquare() == to)
        capturedVal = 100;
    if (capturedVal) {
        int attackerType = pieceTypeAt(board, from);
        int victimType = pieceTypeAt(board, to);
        if (board.getEnPassantSquare() == to)
            victimType = MVVLVA::Pawn;
        int score = MVVLVA::Table[victimType][attackerType];
        int seeVal = staticExchangeEval(gen, board, move);
        return score * 10 + seeVal;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Engine constructor ensures Zobrist keys are initialized once.
// -----------------------------------------------------------------------------
Engine::Engine() {
    static bool init = false;
    if (!init) { Zobrist::init(); init = true; }
}

// -----------------------------------------------------------------------------
// Quiescence search that explores only capture moves to stabilize evaluation.
// -----------------------------------------------------------------------------
int Engine::quiescence(Board& board, int alpha, int beta, bool maximizing,
                       const std::chrono::steady_clock::time_point& end,
                       const std::atomic<bool>& stop) {
    if (stop || std::chrono::steady_clock::now() >= end)
        return (board.isWhiteToMove() ? 1 : -1) * evaluate(board);
    nodes++;
    int standPat = (board.isWhiteToMove() ? 1 : -1) * evaluate(board);
    if (maximizing) {
        if (standPat >= beta) return standPat;
        if (standPat > alpha) alpha = standPat;
    } else {
        if (standPat <= alpha) return standPat;
        if (standPat < beta) beta = standPat;
    }

    auto pseudoMoves = generator.generateAllMoves(board, board.isWhiteToMove());
    std::vector<uint16_t> moves;
    for (auto mv : pseudoMoves) {
        if (board.isMoveLegal(mv) && isCaptureMove(board, mv) &&
            staticExchangeEval(generator, board, mv) >= 0)
            moves.push_back(mv);
    }

    for (auto m : moves) {
        Board::MoveState state;
        board.makeMove(m, state);
        int score = quiescence(board, alpha, beta, !maximizing, end, stop);
        board.unmakeMove(state);
        if (maximizing) {
            if (score > alpha) alpha = score;
        } else {
            if (score < beta) beta = score;
        }
        if (beta <= alpha) break;
    }
    return maximizing ? alpha : beta;
}

// -----------------------------------------------------------------------------
// Negamax search with alpha-beta pruning. Returns the evaluated score from the
// perspective of the current player.
// -----------------------------------------------------------------------------
int Engine::negamaxAlphaBeta(Board& board, int depth,
                             int alpha, int beta, int color,
                             const std::chrono::steady_clock::time_point& end,
                             const std::atomic<bool>& stop) {
    if (stop || std::chrono::steady_clock::now() >= end)
        return color * evaluate(board);
    if (board.isFiftyMoveDraw() || board.isThreefoldRepetition())
        return 0;
    if (depth == 0)
        return color * evaluate(board);

    auto pseudo = generator.generateAllMoves(board, board.isWhiteToMove());
    std::vector<uint16_t> moves;
    for (auto mv : pseudo) {
        std::string sm = decodeMove(mv);
        if (board.isMoveLegal(sm))
            moves.push_back(mv);
    }

    if (moves.empty()) {
        if (generator.isKingInCheck(board, board.isWhiteToMove()))
            return -1000000 * color;
        return 0;
    }

    for (auto m : moves) {
        std::string sm = decodeMove(m);
        Board::MoveState st;
        board.makeMove(sm, st);
        int score = -negamaxAlphaBeta(board, depth - 1,
                                      -beta, -alpha, -color, end, stop);
        board.unmakeMove(st);
        if (score >= beta)
            return score;
        if (score > alpha)
            alpha = score;
    }
    return alpha;
}

// -----------------------------------------------------------------------------
// Full-featured minimax search with alpha-beta pruning, null-move pruning and
// killer move heuristics. Returns the evaluation score and principal variation.
// -----------------------------------------------------------------------------
std::pair<int, std::string> Engine::minimax(
        Board& board, int depth, int alpha, int beta, bool maximizing,
        const std::chrono::steady_clock::time_point& end,
        const std::atomic<bool>& stop, int ply) {
    constexpr int MAX_PLY = 64;
    static thread_local std::array<std::array<uint16_t, 2>, MAX_PLY> killerMoves{};
    static thread_local int historyTable[2][64][64];
    if (ply == 0) {
        for (auto& km : killerMoves) { km[0] = km[1] = 0; }
        for (int s = 0; s < 2; ++s)
            for (int f = 0; f < 64; ++f)
                for (int t = 0; t < 64; ++t)
                    historyTable[s][f][t] = 0;
    }
    if (stop || std::chrono::steady_clock::now() >= end)
        return {(board.isWhiteToMove() ? 1 : -1) * evaluate(board), ""};
    if (board.isFiftyMoveDraw() || board.isThreefoldRepetition())
        return {0, ""};
    uint64_t key = Zobrist::hashBoard(board);
    TTEntry entry{};
    uint16_t ttMove = 0;
    bool hit = tt.probe(key, entry);
    if (hit) ttMove = entry.move;
    if (hit && entry.depth >= depth) {
        if (entry.flag == 0)
            return {entry.value, decodeMove(ttMove)};
        if (entry.flag == 1 && entry.value >= beta)
            return {entry.value, decodeMove(ttMove)};
        if (entry.flag == -1 && entry.value <= alpha)
            return {entry.value, decodeMove(ttMove)};
    }
    nodes++;
    int alphaOrig = alpha;
    if (depth == 0)
        return {quiescence(board, alpha, beta, maximizing, end, stop), ""};

    const int NULL_REDUCTION = 2;
    uint64_t otherPieces =
            ((board.getWhitePieces() | board.getBlackPieces()) &
             ~(board.getWhiteKing() | board.getBlackKing()));
    if (depth >= 3 && otherPieces &&
        !generator.isKingInCheck(board, board.isWhiteToMove())) {
        Board nullBoard = board;
        nullBoard.setWhiteToMove(!board.isWhiteToMove());
        nullBoard.setEnPassantSquare(-1);
        int rDepth = depth - 1 - NULL_REDUCTION;
        if (rDepth < 0) rDepth = 0;
        std::pair<int, std::string> nullRes;
        if (maximizing) {
            nullRes = minimax(nullBoard, rDepth, beta - 1, beta,
                              false, end, stop, ply + 1);
            if (nullRes.first >= beta)
                return {nullRes.first, ""};
        } else {
            nullRes = minimax(nullBoard, rDepth, alpha, alpha + 1,
                              true, end, stop, ply + 1);
            if (nullRes.first <= alpha)
                return {nullRes.first, ""};
        }
    }
    auto pseudoMoves = generator.generateAllMoves(board, board.isWhiteToMove());
    std::vector<uint16_t> moves;
    for (auto mv : pseudoMoves) {
        if (board.isMoveLegal(mv))
            moves.push_back(mv);
    }
    int sideIndex = board.isWhiteToMove() ? 0 : 1;
    std::sort(moves.begin(), moves.end(), [&](uint16_t a, uint16_t b) {
        int scoreA = (a == ttMove) ? 1000000 : moveScore(board, a, generator);
        int scoreB = (b == ttMove) ? 1000000 : moveScore(board, b, generator);
        if (scoreA == 0 && a != ttMove) {
            if (killerMoves[ply][0] == a) scoreA = 900;
            else if (killerMoves[ply][1] == a) scoreA = 800;
            else {
                int fa = moveFrom(a);
                int ta = moveTo(a);
                scoreA = historyTable[sideIndex][fa][ta];
            }
        }
        if (scoreB == 0 && b != ttMove) {
            if (killerMoves[ply][0] == b) scoreB = 900;
            else if (killerMoves[ply][1] == b) scoreB = 800;
            else {
                int fb = moveFrom(b);
                int tb = moveTo(b);
                scoreB = historyTable[sideIndex][fb][tb];
            }
        }
        return scoreA > scoreB;
    });
    if (moves.empty()) {
        if (generator.isKingInCheck(board, board.isWhiteToMove())) {
            int mateScore = board.isWhiteToMove() ? -1000000 : 1000000;
            return {mateScore, ""};
        }
        return {0, ""};
    }
    if (maximizing) {
        int bestEval = -1000000;
        uint16_t bestMove = 0;
        std::string bestPV;
        bool first = true;
        for (auto m : moves) {
            bool capture = isCaptureMove(board, m);
            int from = moveFrom(m);
            int to = moveTo(m);
            int plySide = board.isWhiteToMove() ? 0 : 1;
            Board::MoveState state;
            board.makeMove(m, state);
            std::pair<int, std::string> child;
            if (first) {
                child = minimax(board, depth - 1, alpha, beta, false, end, stop, ply + 1);
            } else {
                child = minimax(board, depth - 1, alpha, alpha + 1,
                                false, end, stop, ply + 1);
                int eval = child.first;
                if (eval > alpha && eval < beta) {
                    child = minimax(board, depth - 1, eval, beta,
                                    false, end, stop, ply + 1);
                }
            }
            int eval = child.first;
            if (eval > bestEval) {
                bestEval = eval;
                bestMove = m;
                bestPV = decodeMove(m);
                if (!child.second.empty()) bestPV += " " + child.second;
            }
            if (eval > alpha) {
                alpha = eval;
                if (!capture)
                    historyTable[plySide][from][to] += depth * depth;
            }
            if (alpha >= beta) {
                if (!capture && ply < MAX_PLY) {
                    if (killerMoves[ply][0] != m) {
                        killerMoves[ply][1] = killerMoves[ply][0];
                        killerMoves[ply][0] = m;
                    }
                }
                board.unmakeMove(state);
                break;
            }
            first = false;
            board.unmakeMove(state);
        }
        TTEntry save{depth, bestEval, 0, bestMove};
        if (bestEval <= alphaOrig) save.flag = -1;
        else if (bestEval >= beta) save.flag = 1;
        tt.store(key, save);
        return {bestEval, bestPV};
    } else {
        int bestEval = 1000000;
        uint16_t bestMove = 0;
        std::string bestPV;
        bool first = true;
        for (auto m : moves) {
            bool capture = isCaptureMove(board, m);
            int from = moveFrom(m);
            int to = moveTo(m);
            int plySide = board.isWhiteToMove() ? 0 : 1;
            Board::MoveState state;
            board.makeMove(m, state);
            std::pair<int, std::string> child;
            if (first) {
                child = minimax(board, depth - 1, alpha, beta, true, end, stop, ply + 1);
            } else {
                child = minimax(board, depth - 1, beta - 1, beta,
                                true, end, stop, ply + 1);
                int eval = child.first;
                if (eval < beta && eval > alpha) {
                    child = minimax(board, depth - 1, alpha, eval,
                                    true, end, stop, ply + 1);
                }
            }
            int eval = child.first;
            if (eval < bestEval) {
                bestEval = eval;
                bestMove = m;
                bestPV = decodeMove(m);
                if (!child.second.empty()) bestPV += " " + child.second;
            }
            if (eval < beta) {
                beta = eval;
                if (!capture)
                    historyTable[plySide][from][to] += depth * depth;
            }
            if (beta <= alpha) {
                if (!capture && ply < MAX_PLY) {
                    if (killerMoves[ply][0] != m) {
                        killerMoves[ply][1] = killerMoves[ply][0];
                        killerMoves[ply][0] = m;
                    }
                }
                board.unmakeMove(state);
                break;
            }
            first = false;
            board.unmakeMove(state);
        }
        TTEntry save{depth, bestEval, 0, bestMove};
        if (bestEval <= alphaOrig) save.flag = -1;
        else if (bestEval >= beta) save.flag = 1;
        tt.store(key, save);
        return {bestEval, bestPV};
    }
}

// -----------------------------------------------------------------------------
// Iteratively deepens search up to the specified depth to find the best move.
// -----------------------------------------------------------------------------
std::string Engine::searchBestMove(Board& board, int depth) {
    if (auto tb = tablebase.lookupMove(board))
        return *tb;
    if (useOwnBook) {
        if (auto bm = book.getBookMove(board))
            return *bm;
    }

    std::atomic<bool> dummyStop(false);
    uint16_t bestMove = 0;

    for (int d = 1; d <= depth; ++d) {
        auto pseudoMoves = generator.generateAllMoves(board, board.isWhiteToMove());
        std::vector<uint16_t> moves;
        for (auto mv : pseudoMoves) {
            if (board.isMoveLegal(mv))
                moves.push_back(mv);
        }
        std::sort(moves.begin(), moves.end(), [&](uint16_t a, uint16_t b) {
            return moveScore(board, a, generator) > moveScore(board, b, generator);
        });
        if (d > 1 && bestMove) {
            auto it = std::find(moves.begin(), moves.end(), bestMove);
            if (it != moves.end()) {
                std::rotate(moves.begin(), it, it + 1);
            }
        }

        int bestScore = board.isWhiteToMove() ? -1000000 : 1000000;
        std::vector<std::future<std::pair<int, std::string>>> futures;
        futures.reserve(moves.size());
        for (auto m : moves) {
            futures.emplace_back(pool.enqueue([&, m, d]() {
                Board copy = board;
                copy.makeMove(m);
                return minimax(copy, d - 1, -1000000, 1000000,
                               !board.isWhiteToMove(),
                               std::chrono::steady_clock::time_point::max(),
                               dummyStop, 0);
            }));
        }

        for (size_t i = 0; i < moves.size(); ++i) {
            auto res = futures[i].get();
            int score = res.first;
            uint16_t m = moves[i];
            if (board.isWhiteToMove()) {
                if (score > bestScore) { bestScore = score; bestMove = m; }
            } else {
                if (score < bestScore) { bestScore = score; bestMove = m; }
            }
        }
    }

    return decodeMove(bestMove);
}

// -----------------------------------------------------------------------------
// Time-limited iterative deepening search. Stops when the time limit is reached
// or search is externally halted.
// -----------------------------------------------------------------------------
std::string Engine::searchBestMoveTimed(Board& board, int maxDepth,
                                        int timeLimitMs,
                                        std::atomic<bool>& stopFlag) {
    auto start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point endTime;
    if (timeLimitMs <= 0)
        endTime = std::chrono::steady_clock::time_point::max();
    else
        endTime = start + std::chrono::milliseconds(timeLimitMs);

    if (auto tb = tablebase.lookupMove(board))
        return *tb;
    if (useOwnBook) {
        if (auto bm = book.getBookMove(board))
            return *bm;
    }

    uint16_t bestMove = 0;
    std::string bestPV;
    uint16_t completedMove = 0; // best move from the last fully searched depth
    int bestScore = 0;
    bool lastDepthComplete = true;
    for (int depth = 1; maxDepth == 0 || depth <= maxDepth; ++depth) {
        nodes = 0;
        auto depthStart = std::chrono::steady_clock::now();
        auto pseudoMoves = generator.generateAllMoves(board, board.isWhiteToMove());
        std::vector<uint16_t> moves;
        for (auto mv : pseudoMoves) {
            if (board.isMoveLegal(mv))
                moves.push_back(mv);
        }
        std::sort(moves.begin(), moves.end(), [&](uint16_t a, uint16_t b) {
            return moveScore(board, a, generator) > moveScore(board, b, generator);
        });
        if (depth > 1 && completedMove) {
            auto it = std::find(moves.begin(), moves.end(), completedMove);
            if (it != moves.end()) {
                std::rotate(moves.begin(), it, it + 1);
            }
        }
        bestScore = board.isWhiteToMove() ? -1000000 : 1000000;
        bestPV.clear();
        lastDepthComplete = true;
        std::vector<std::future<std::pair<int, std::string>>> futures;
        futures.reserve(moves.size());
        for (auto m : moves) {
            futures.emplace_back(pool.enqueue([&, m]() {
                Board copy = board;
                copy.makeMove(m);
                return minimax(copy, depth - 1, -1000000, 1000000,
                               !board.isWhiteToMove(), endTime, stopFlag, 0);
            }));
        }
        for (size_t i = 0; i < moves.size(); ++i) {
            auto res = futures[i].get();
            uint16_t m = moves[i];
            int score = res.first;
            std::string pvCandidate = decodeMove(m);
            if (!res.second.empty()) pvCandidate += " " + res.second;
            if (board.isWhiteToMove()) {
                if (score > bestScore) { bestScore = score; bestMove = m; bestPV = pvCandidate; }
            } else {
                if (score < bestScore) { bestScore = score; bestMove = m; bestPV = pvCandidate; }
            }
            if (stopFlag || std::chrono::steady_clock::now() >= endTime)
                lastDepthComplete = false;
        }
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::steady_clock::now() - depthStart)
                          .count();
        std::string pvUCI;
        int pvCount = 0;
        {
            std::istringstream iss(bestPV);
            std::string token;
            while (iss >> token) {
                ++pvCount;
                uint16_t mv = encodeMove(token);
                if (!pvUCI.empty()) pvUCI += " ";
                pvUCI += toUCIMove(mv);
            }
        }
        int hashPercent = static_cast<int>(tt.used() * 1000 / tt.size());
        uint64_t nodeCount = nodes.load();
        uint64_t nps = elapsed > 0 ? (nodeCount * 1000 / elapsed) : nodeCount;
        if (!lastDepthComplete) break;
        int displayScore = board.isWhiteToMove() ? bestScore : -bestScore;
        if (displayScore >= 900000 || displayScore <= -900000) {
            int mateMoves = (pvCount + 1) / 2;
            if (displayScore < 0) mateMoves = -mateMoves;
            std::cout << "info depth " << depth << " score mate " << mateMoves
                      << " nodes " << nodeCount << " nps " << nps
                      << " hashfull " << hashPercent << " time " << elapsed;
        } else {
            std::cout << "info depth " << depth << " score cp " << displayScore
                      << " nodes " << nodeCount << " nps " << nps
                      << " hashfull " << hashPercent << " time " << elapsed;
        }
        if (!pvUCI.empty())
            std::cout << " pv " << pvUCI;
        std::cout << '\n';
        completedMove = bestMove;
        if (stopFlag || std::chrono::steady_clock::now() >= endTime) break;
    }
    if (completedMove)
        return decodeMove(completedMove);
    return decodeMove(bestMove);
}

// -----------------------------------------------------------------------------
// Resizes the transposition table to hold approximately the given number of
// megabytes.
// -----------------------------------------------------------------------------
void Engine::setHashSizeMB(size_t mb) {
    size_t bytes = mb * 1024 * 1024;
    size_t entries = bytes / sizeof(TTSlot);
    if (entries == 0) entries = 1;
    tt.resize(entries);
}
