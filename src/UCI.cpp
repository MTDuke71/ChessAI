#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include "Board.h"
#include "Engine.h"
#include "MoveEncoding.h"

static std::string toInternalMove(const std::string& uci, const Board& board) {
    if (uci.size() < 4) return "";
    std::string from = uci.substr(0,2);
    std::string to = uci.substr(2,2);
    int file = from[0] - 'a';
    int rank = from[1] - '1';
    int fromIdx = rank * 8 + file;
    uint64_t fromMask = 1ULL << fromIdx;
    bool kingAtFrom = (board.getWhiteKing() | board.getBlackKing()) & fromMask;

    if (kingAtFrom) {
        // For castling moves, UCI notation matches internal notation
        // e1g1 -> e1-g1, e1c1 -> e1-c1, e8g8 -> e8-g8, e8c8 -> e8-c8
        // No conversion needed
    }

    std::string move = from + "-" + to;
    if (uci.size() >= 5)
        move += std::string(1, static_cast<char>(std::tolower(uci[4])));
    return move;
}

static std::string toUCIMove(const std::string& move) {
    auto dash = move.find('-');
    if (dash == std::string::npos) return move;

    std::string from = move.substr(0, 2);
    std::string to = move.substr(dash + 1, 2);

    if (from == "e1" && to == "h1") to = "g1";
    else if (from == "e1" && to == "a1") to = "c1";
    else if (from == "e8" && to == "h8") to = "g8";
    else if (from == "e8" && to == "a8") to = "c8";

    std::string uci = from + to;

    if (move.size() > dash + 3) {
        char c = std::tolower(move.back());
        if (c == 'q' || c == 'r' || c == 'b' || c == 'n')
            uci += c;
    }

    return uci;
}

int main() {
    Board board;
    Engine engine;
    std::string line;
    std::atomic<bool> stopFlag(false);
    std::thread searchThread;
    std::string bestMove;
    bool pondering = false;

    std::cout.setf(std::ios::unitbuf);

    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            std::cout << "id name Aphelion 1.1" << '\n';
            std::cout << "id author Matt LaDuke ChatGPT and Claude" << '\n';
            std::cout << "option name OwnBook type check default false" << '\n';
            std::cout << "uciok" << '\n';
        } else if (line == "isready") {
            std::cout << "readyok" << '\n';
        } else if (line.rfind("setoption", 0) == 0) {
            auto namePos = line.find("name ");
            auto valuePos = line.find(" value ");
            if (namePos != std::string::npos) {
                std::string name = line.substr(namePos + 5,
                                              valuePos == std::string::npos ?
                                              std::string::npos :
                                              valuePos - (namePos + 5));
                if (name == "Hash" && valuePos != std::string::npos) {
                    int mb = std::stoi(line.substr(valuePos + 7));
                    engine.setHashSizeMB(static_cast<size_t>(mb));
                } else if (name == "OwnBook" && valuePos != std::string::npos) {
                    std::string val = line.substr(valuePos + 7);
                    for (auto &c : val) c = static_cast<char>(std::tolower(c));
                    bool enable = (val == "true" || val == "1");
                    engine.setOwnBook(enable);
                }
            }
        } else if (line == "ucinewgame") {
            if (searchThread.joinable()) {
                stopFlag = true;
                searchThread.join();
            }
            board = Board();
            engine.clearTranspositionTable();
        } else if (line.rfind("position", 0) == 0) {
            std::istringstream iss(line);
            std::string token;
            iss >> token; // position
            iss >> token;
            if (token == "startpos") {
                if (!board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")) {
                    std::cout << "info string Failed to load startpos" << '\n';
                }
            } else if (token == "fen") {
                std::string fenParts[6];
                for (int i=0;i<6 && iss>>fenParts[i];++i) {}
                std::string fen = fenParts[0]+" "+fenParts[1]+" "+fenParts[2]+" "+fenParts[3]+" "+fenParts[4]+" "+fenParts[5];
                board.loadFEN(fen);
				board.printBoard();
            }
            if (iss >> token && token == "moves") {
                while (iss >> token) {
                    std::string internal = toInternalMove(token, board);
                    // For UCI moves, use castling-aware encoding
                    uint16_t moveCode = encodeMove(internal, board.isWhiteToMove(), true);
                    if (!board.isMoveLegal(moveCode)) {
                        std::cout << "info string Illegal move in position command: " << token << '\n';
                        break;
                    }
                    board.makeMove(internal);

                }
                board.printBoard();
            }
        } else if (line.rfind("go",0) == 0) {
            if (searchThread.joinable()) {
                stopFlag = true;
                searchThread.join();
            }
            int depth = 0;
            int wtime = 0, btime = 0, winc = 0, binc = 0;
            bool infinite = false;
            bool ponder = false;
            std::istringstream iss(line);
            std::string token;
            iss >> token; // go
            while (iss >> token) {
                if (token == "depth" && iss >> depth) {}
                else if (token == "wtime" && iss >> wtime) {}
                else if (token == "btime" && iss >> btime) {}
                else if (token == "winc" && iss >> winc) {}
                else if (token == "binc" && iss >> binc) {}
                else if (token == "infinite") infinite = true;
                else if (token == "ponder") ponder = true;
            }

            int timeLimit = 0;
            if (!infinite && (wtime || btime)) {
                int remain = board.isWhiteToMove() ? wtime : btime;
                int inc = board.isWhiteToMove() ? winc : binc;
                timeLimit = remain / 20 + inc / 2;
            }

            stopFlag = false;
            pondering = ponder;
            bool autoPrint = !infinite && !ponder;
            searchThread = std::thread([&, autoPrint]() {
                bestMove = engine.searchBestMoveTimed(board, depth, timeLimit, stopFlag);
                if (autoPrint && !stopFlag) {
                    std::string uci = bestMove.empty() ? "0000" : toUCIMove(bestMove);
                    std::cout << "bestmove " << uci << '\n';
                }
            });
        } else if (line == "ponderhit") {
            if (searchThread.joinable() && pondering) {
                stopFlag = true;
                searchThread.join();
                std::string uci = bestMove.empty() ? "0000" : toUCIMove(bestMove);
                std::cout << "bestmove " << uci << '\n';
                pondering = false;
            }
        } else if (line == "stop") {
            if (searchThread.joinable()) {
                stopFlag = true;
                searchThread.join();
                if (!pondering) {
                    std::string uci = bestMove.empty() ? "0000" : toUCIMove(bestMove);
                    std::cout << "bestmove " << uci << '\n';
                } else {
                    pondering = false;
                }
            }
        } else if (line == "quit") {
            if (searchThread.joinable()) {
                stopFlag = true;
                searchThread.join();
            }
            break;
        }
    }
    return 0;
}
