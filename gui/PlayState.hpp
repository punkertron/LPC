#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "Board.hpp"
#include "Checkers.hpp"
#include "Engine.hpp"
#include "Move.hpp"
#include "Position.hpp"
#include "State.hpp"

class ResourceManager;
class StateManager;
struct GameContext;

class PlayState final : public State {
private:
    // High-level UI mode derived from runtime state
    enum class UiPhase {
        // No piece is currently selected
        Idle,
        // A piece is selected and destination squares are highlighted
        SelectingPiece,
        // A piece is moving between squares
        AnimatingMove,
        GameOver,
    };

    // Selection state owned by the input flow
    struct SelectionState {
        // Currently selected origin square
        Position origin{-1, -1};
        // Legal move trees starting from `origin`
        std::vector<Move> candidates{};
        // True when `origin` points to a selectable piece
        bool hasOrigin{false};

        // Reset selection to the initial state
        void clear()
        {
            origin.reset();
            candidates.clear();
            hasOrigin = false;
        }
    };

    // Move animation and resolution state for the current turn
    struct MoveState {
        // Start square of the animated segment
        Position from{-1, -1};
        // Destination square of the animated segment
        Position to{-1, -1};
        // Piece captured by the currently animated segment (if any)
        Position segmentCapturedPiecePos{-1, -1};
        // Fully resolved move chain to commit into Checkers logic
        Move resolvedPath{};
        // True while animation is active
        bool inProgress{false};
        // True when `resolvedPath` is complete and can be committed
        bool hasResolvedPath{false};
        // [0..1] interpolation factor for piece animation
        float animationProgress{0.f};

        // Reset animation/resolution state to idle
        void clear()
        {
            from.reset();
            to.reset();
            segmentCapturedPiecePos.reset();
            resolvedPath.reset();
            inProgress = false;
            hasResolvedPath = false;
            animationProgress = 0.f;
        }
    };

    // Authoritative game rules/state
    Checkers checkers_{};
    // Local board snapshot used for rendering/intermediate animation
    Board boardView_;
    // Engine used for COMPUTER mode (null in TWO_PLAYERS mode)
    std::unique_ptr<Engine> engine_{};
    // Current board selection state
    SelectionState selection_{};
    // Current move animation/commit state
    MoveState moveState_{};
    // True when a multi-capture chain is mid-progress and next segment must be chosen
    bool isAwaitingChainContinuation_{false};
    // Cached game-over flag to avoid repeated result checks
    bool isGameOver_{false};
    // Winner color used by the end-of-game UI
    COLOUR winnerColor_{COLOUR::WHITE};
    // Timer controlling delayed "Press Esc" hint
    sf::Clock resultClock_{};

    // Reused drawable primitives to avoid per-frame allocations
    sf::CircleShape highlightCircle_;
    // Visual marker for temporary CAPTURED cells during multi-capture previews
    sf::CircleShape capturedPieceCircle_;
    sf::CircleShape whitePieceCircle_;
    sf::CircleShape blackPieceCircle_;
    sf::RectangleShape whiteSquare_;
    sf::RectangleShape blackSquare_;

    // Runtime board metrics derived from board type and window size
    int tile_;
    float pieceRadius_;
    float offsetForPiece_;
    float highlightRadius_;
    float offsetForHighlight_;
    float offsetForQueenTexture_;

    // Convert mouse pixel coordinates into board row/col
    Position getPositionOnBoardFromMouse(int x, int y) const;
    // Check whether a board position lies inside the current board dimensions
    bool isInsideBoard(Position pos) const;
    // Derive current UI phase from selection/move/game-over state
    UiPhase getUiPhase() const;
    // True when local player is allowed to choose a move
    bool isHumanTurn() const;
    // True when board click input should be processed
    bool isBoardInputAllowed() const;
    // Route a click to selection or destination handling
    void handleBoardClick(Position pos);
    // Select a square and cache legal moves from it
    void selectSquare(Position pos);
    // Clear selected square and candidate moves
    void clearSelection();
    // Process click on target square after origin was selected
    void handleDestinationClick(Position pos);

    // Draw checkerboard squares
    void drawBoard();
    // Draw all non-moving pieces
    void drawPieces();
    // Draw destination markers for currently selected piece
    void drawHighlights();
    // Draw move markers produced by a specific source square
    void highlightPossibleMovesFrom(Position from);
    // True when a clicked square is reachable by any candidate move chain
    bool isPositionInsidePossibleMoves(Position pos) const;
    // Resolve the beaten piece for a specific animated segment (from -> to), if uniquely identifiable
    bool tryResolveSegmentCapturedPiece(Position from, Position to, Position& capturedPiecePos) const;
    // True if there is at least one immediate segment starting from `from`
    bool hasSegmentsFrom(Position from) const;
    // Return final destination of a chained move
    Position getLastPositionInChain(const Move& move) const;

    // Render end-of-game overlay and text
    void renderResult();
    // Advance and render current piece animation
    void processMoveAnimation();
    // Commit resolved move into game logic when animation finishes
    void commitResolvedMoveIfNeeded();
    // Draw semi-transparent marker for temporary captured piece placeholders
    void drawCapturedPiece(Position pos);
    // Draw one piece, optionally interpolated between two squares
    void drawPiece(Position pos, bool isMoving = false);

    // Read game result from logic and update UI state
    void updateGameResultState();
    // Ask engine for move when it's computer's turn
    void triggerComputerMoveIfNeeded();
    // Build/reset engine implementation based on selected mode/difficulty
    void initializeEngine();
    // Type-specific size tuning used to compute radius
    int getPieceRadiusOffset() const;
    // Recompute rendering metrics after reset/checkers type change
    void initializeBoardMetrics();

public:
    PlayState(sf::RenderWindow& window, StateManager& stateManager, ResourceManager& resourceManager,
              GameContext& gameContext);
    void handleEvent(const sf::Event& event) override;
    void update() override;
    void render() override;
    void reset() override;
};
