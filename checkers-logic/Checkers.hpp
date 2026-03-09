#pragma once

#include <deque>
#include <unordered_map>
#include <vector>

#include "Board.hpp"
#include "Move.hpp"
#include "Position.hpp"

enum class COLOUR {
    WHITE = 0,
    BLACK
};

enum class CHECKERS_TYPE {
    RUSSIAN = 0,
    INTERNATIONAL,
    CANADIAN,
    BRAZILIAN,
};

class Checkers {
private:
    struct GameStateSnapshot {
        Board board;
        COLOUR currentColour;
    };

    Board board_;
    std::unordered_map<Position, std::vector<Move>> validMoves_;
    COLOUR currentColour_;
    CHECKERS_TYPE checkersType_ = CHECKERS_TYPE::RUSSIAN;
    std::deque<GameStateSnapshot> undoHistory_{};
    std::deque<GameStateSnapshot> redoHistory_{};

    bool isWithinBoard(const Position& p) const;
    void generateValidMoves();
    GameStateSnapshot captureSnapshot() const;
    void restoreSnapshot(const GameStateSnapshot& snapshot);
    void makeMoveInternal(const Move& m, bool trackHistory);
    void addBeatMoves(const Position& p, std::vector<Move>& res) const;
    void findCaptures(const Position& initial, Board& boardCopy, std::vector<Move>& moves) const;
    void processCapture(const Position& initial, const Position& enemy, const Position& landing, Board& boardCopy,
                        std::vector<Move>& moves) const;
    void removeQueenWrongMoves(const Position& initial, const Position& enemy, std::vector<Move>& moves) const;
    void addNonBeatMoves(const Position& p, std::vector<Move>& res) const;
    void addOneStepMoves(const Position& p, std::vector<Move>& res) const;
    void addQueenNonBeatMoves(const Position& p, std::vector<Move>& res) const;

public:
    Checkers();
    Checkers(const Checkers& other);
    Checkers& operator=(const Checkers&) = delete;

    void reset();
    void setCheckersType(CHECKERS_TYPE ct);

    std::vector<Move> getValidMoves(const Position& p) const;
    const std::unordered_map<Position, std::vector<Move>>& getValidMoves() const;
    void makeMove(const Move& m);
    void makeMoveWithoutHistory(const Move& m);
    bool undoMove();
    bool redoMove();
    bool canUndo() const;
    bool canRedo() const;
    const Board& getBoard() const;
    Board getCopyBoard() const;
    COLOUR getCurrentColour() const;

    struct GameResult {
        bool isOver;
        COLOUR winner;
    };
    GameResult getResult() const;
};
