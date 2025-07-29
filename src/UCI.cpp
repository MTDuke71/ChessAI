#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include "Board.h"
#include "Engine.h"

static std::string toInternalMove(const std::string& uci) {
    if (uci.size() < 4) return "";
    return uci.substr(0,2) + "-" + uci.substr(2,2);
}

static std::string toUCIMove(const std::string& move) {
    std::string uci = move;
    auto dash = uci.find('-');
    if (dash != std::string::npos) uci.erase(dash,1);
    return uci;
}

int main() {
    Board board;
    Engine engine;
    std::string line;
    std::atomic<bool> stopFlag(false);
    std::thread searchThread;
    std::string bestMove;

    std::cout.setf(std::ios::unitbuf);

    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            std::cout << "id name ChessAI" << '\n';
            std::cout << "id author Unknown" << '\n';
            std::cout << "uciok" << '\n';
        } else if (line == "isready") {
            std::cout << "readyok" << '\n';
        } else if (line.rfind("position", 0) == 0) {
            std::istringstream iss(line);
            std::string token;
            iss >> token; // position
            iss >> token;
            if (token == "startpos") {
                board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            } else if (token == "fen") {
                std::string fenParts[6];
                for (int i=0;i<6 && iss>>fenParts[i];++i) {}
                std::string fen = fenParts[0]+" "+fenParts[1]+" "+fenParts[2]+" "+fenParts[3]+" "+fenParts[4]+" "+fenParts[5];
                board.loadFEN(fen);
            }
            if (iss >> token && token == "moves") {
                while (iss >> token) {
                    board.makeMove(toInternalMove(token));
                }
            }
        } else if (line.rfind("go",0) == 0) {
            int depth = 0;
            int wtime = 0, btime = 0, winc = 0, binc = 0;
            bool infinite = false;
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
            }

            int timeLimit = 0;
            if (!infinite && (wtime || btime)) {
                int remain = board.isWhiteToMove() ? wtime : btime;
                int inc = board.isWhiteToMove() ? winc : binc;
                timeLimit = remain / 20 + inc / 2;
            }

            stopFlag = false;
            searchThread = std::thread([&]() {
                bestMove = engine.searchBestMoveTimed(board, depth, timeLimit, stopFlag);
            });

            if (!infinite) {
                searchThread.join();
                std::string uci = bestMove.empty() ? "0000" : toUCIMove(bestMove);
                std::cout << "bestmove " << uci << '\n';
            }
        } else if (line == "stop") {
            if (searchThread.joinable()) {
                stopFlag = true;
                searchThread.join();
                std::string uci = bestMove.empty() ? "0000" : toUCIMove(bestMove);
                std::cout << "bestmove " << uci << '\n';
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
