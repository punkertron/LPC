#include "PlayState.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

#include "Game.hpp"
#include "MinimaxEngine.hpp"
#include "Piece.hpp"
#include "RandomEngine.hpp"
#include "ResourceManager.hpp"
#include "StateManager.hpp"

namespace
{
constexpr float kMoveAnimationStep = 0.05f;
constexpr float kResultSpriteMaxWidthFactor = 0.7f;
constexpr float kResultSpriteMaxHeightFactor = 0.42f;
constexpr float kResultSpriteTopFactor = 0.2f;
constexpr float kResultMessageGapFactor = 0.06f;
constexpr unsigned int kMinResultMessageCharacterSize = 16;
constexpr unsigned int kMaxResultMessageCharacterSize = 24;

bool hasPrimaryShortcutModifier(const sf::Event::KeyPressed& key)
{
#ifdef __APPLE__
    return key.system || key.control;
#else
    return key.control;
#endif
}

bool isUndoShortcut(const sf::Event::KeyPressed& key)
{
    return hasPrimaryShortcutModifier(key) && !key.shift && key.code == sf::Keyboard::Key::Z;
}

bool isRedoShortcut(const sf::Event::KeyPressed& key)
{
    return hasPrimaryShortcutModifier(key) &&
           (key.code == sf::Keyboard::Key::Y || (key.shift && key.code == sf::Keyboard::Key::Z));
}

float calculateContainedScale(const sf::Vector2u textureSize, sf::Vector2f maxSize)
{
    const float widthScale = maxSize.x / static_cast<float>(textureSize.x);
    const float heightScale = maxSize.y / static_cast<float>(textureSize.y);
    return std::min(widthScale, heightScale);
}
}  // namespace

PlayState::PlayState(sf::RenderWindow& window, StateManager& stateManager, ResourceManager& resourceManager,
                     GameContext& gameContext) :
    State{window, stateManager, resourceManager, gameContext},
    winSound_{resourceManager.getWinSoundBuffer()},
    lostSound_{resourceManager.getLostSoundBuffer()}
{
    highlightCircle_.setFillColor(sf::Color{0, 255, 0, 150});  // Semi-transparent green
    capturedPieceCircle_.setFillColor(sf::Color{128, 128, 128, 110});
    capturedPieceCircle_.setOutlineThickness(2);
    capturedPieceCircle_.setOutlineColor(sf::Color{90, 30, 30, 150});

    whitePieceCircle_.setFillColor(sf::Color{240, 240, 210});
    whitePieceCircle_.setOutlineThickness(3);
    whitePieceCircle_.setOutlineColor(sf::Color{150, 150, 150});  // grey

    blackPieceCircle_.setFillColor(sf::Color{75, 75, 75});
    blackPieceCircle_.setOutlineThickness(3);
    blackPieceCircle_.setOutlineColor(sf::Color{150, 150, 150});  // grey

    whiteSquare_.setFillColor(sf::Color(240, 217, 181));
    blackSquare_.setFillColor(sf::Color{181, 136, 99});
}

Position PlayState::getPositionOnBoardFromMouse(int x, int y) const
{
    if (tile_ <= 0.f) {
        return {-1, -1};
    }

    const float localX = static_cast<float>(x) - boardOrigin_.x;
    const float localY = static_cast<float>(y) - boardOrigin_.y;
    if (localX < 0.f || localY < 0.f) {
        return {-1, -1};
    }

    return mapPos({static_cast<int>(localY / tile_), static_cast<int>(localX / tile_)});
}

Position PlayState::mapPos(Position pos) const
{
    if (!isBoardFlipped_) {
        return pos;
    }

    const int boardWidth = boardView_.getWidth();
    return {boardWidth - pos.row - 1, boardWidth - pos.col - 1};
}

bool PlayState::isInsideBoard(Position pos) const
{
    const int boardWidth = boardView_.getWidth();
    return pos.row >= 0 && pos.col >= 0 && pos.row < boardWidth && pos.col < boardWidth;
}

PlayState::UiPhase PlayState::getUiPhase() const
{
    if (isGameOver_) {
        return UiPhase::GameOver;
    }
    if (moveState_.inProgress) {
        return UiPhase::AnimatingMove;
    }
    if (selection_.hasOrigin) {
        return UiPhase::SelectingPiece;
    }
    return UiPhase::Idle;
}

bool PlayState::isHumanTurn() const
{
    return gameContext_.mode == MODE::TWO_PLAYERS || checkers_.getCurrentColour() == gameContext_.playerColour;
}

bool PlayState::isBoardInputAllowed() const
{
    if (!isHumanTurn()) {
        return false;
    }

    switch (getUiPhase()) {
        case UiPhase::GameOver:
            return false;
        case UiPhase::Idle:
        case UiPhase::SelectingPiece:
            return true;
        case UiPhase::AnimatingMove:
            // Ignore clicks while an animation frame sequence is running
            return false;
        default:
            return false;
    }
}

bool PlayState::isPositionInsidePossibleMoves(Position pos) const
{
    for (const auto& candidate : selection_.candidates) {
        const Move* move = &candidate;
        while (move != nullptr && move->from != selection_.origin) {
            move = move->nextMove.get();
        }

        for (const Move* continuation = move; continuation != nullptr; continuation = continuation->nextMove.get()) {
            if (continuation->to == pos) {
                return true;
            }
        }
    }
    return false;
}

bool PlayState::tryResolveSegmentCapturedPiece(Position from, Position to, Position& capturedPiecePos) const
{
    bool found = false;
    for (const auto& candidate : selection_.candidates) {
        const Move* move = &candidate;
        do {
            if (move->from == from && move->to == to && move->beatenPiecePos.isValid()) {
                if (!found) {
                    capturedPiecePos = move->beatenPiecePos;
                    found = true;
                } else if (capturedPiecePos != move->beatenPiecePos) {
                    return false;
                }
            }
            move = move->nextMove.get();
        } while (move != nullptr);
    }
    return found;
}

bool PlayState::hasSegmentsFrom(Position from) const
{
    for (const auto& candidate : selection_.candidates) {
        const Move* move = &candidate;
        do {
            if (move->from == from) {
                return true;
            }
            move = move->nextMove.get();
        } while (move != nullptr);
    }
    return false;
}

void PlayState::clearSelection()
{
    selection_.clear();
    isAwaitingChainContinuation_ = false;
}

void PlayState::selectSquare(Position pos)
{
    selection_.origin = pos;
    selection_.candidates = checkers_.getValidMoves(pos);
    selection_.hasOrigin = !selection_.candidates.empty();
    isAwaitingChainContinuation_ = false;
}

void PlayState::handleDestinationClick(Position pos)
{
    if (pos == selection_.origin) {
        if (isAwaitingChainContinuation_) {
            return;
        }
        clearSelection();
        return;
    }

    if (!isPositionInsidePossibleMoves(pos)) {
        if (isAwaitingChainContinuation_) {
            return;
        }
        selectSquare(pos);
        return;
    }

    moveState_.from = selection_.origin;

    std::vector<Move> filteredCandidates;
    filteredCandidates.reserve(selection_.candidates.size());

    Position commonImmediateTo{-1, -1};
    Position commonImmediateCaptured{-1, -1};
    bool hasCommonImmediate{false};
    bool hasAmbiguousImmediate{false};

    for (const auto& candidate : selection_.candidates) {
        const Move* firstSegment = &candidate;
        while (firstSegment != nullptr && firstSegment->from != selection_.origin) {
            firstSegment = firstSegment->nextMove.get();
        }
        if (firstSegment == nullptr) {
            continue;
        }

        bool reachesClickedDestination = false;
        for (const Move* continuation = firstSegment; continuation != nullptr; continuation = continuation->nextMove.get()) {
            if (continuation->to == pos) {
                reachesClickedDestination = true;
                break;
            }
        }
        if (!reachesClickedDestination) {
            continue;
        }

        filteredCandidates.push_back(cloneMove(candidate));
        if (!hasCommonImmediate) {
            commonImmediateTo = firstSegment->to;
            commonImmediateCaptured = firstSegment->beatenPiecePos;
            hasCommonImmediate = true;
        } else if (commonImmediateTo != firstSegment->to || commonImmediateCaptured != firstSegment->beatenPiecePos) {
            hasAmbiguousImmediate = true;
        }
    }

    if (filteredCandidates.empty()) {
        if (isAwaitingChainContinuation_) {
            return;
        }
        selectSquare(pos);
        return;
    }

    selection_.candidates = std::move(filteredCandidates);
    selection_.hasOrigin = true;

    moveState_.segmentCapturedPiecePos.reset();
    if (selection_.candidates.size() == 1) {
        // Clicking a unique chain destination allows skipping directly to that position
        moveState_.to = pos;
        moveState_.hasResolvedPath = true;
        moveState_.resolvedPath = cloneMove(selection_.candidates.front());
    } else {
        // Multiple chains remain; animate only the next deterministic segment
        if (!hasCommonImmediate || hasAmbiguousImmediate) {
            return;
        }
        moveState_.to = commonImmediateTo;
        moveState_.hasResolvedPath = false;
        moveState_.resolvedPath.reset();
        if (commonImmediateCaptured.isValid()) {
            moveState_.segmentCapturedPiecePos = commonImmediateCaptured;
        } else {
            Position capturedPos;
            if (tryResolveSegmentCapturedPiece(moveState_.from, moveState_.to, capturedPos)) {
                moveState_.segmentCapturedPiecePos = capturedPos;
            }
        }
    }

    moveState_.animationProgress = 0.f;
    moveState_.inProgress = true;
}

void PlayState::handleBoardClick(Position pos)
{
    if (!selection_.hasOrigin) {
        selectSquare(pos);
        return;
    }

    handleDestinationClick(pos);
}

void PlayState::handleEvent(const sf::Event& event)
{
    updateLayoutMetrics();

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>(); keyPressed != nullptr) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            stopResultSounds();
            stateManager_.setActiveState(STATE_TYPE::MenuState);
            return;
        }
        if (isUndoShortcut(*keyPressed)) {
            undoMove();
            return;
        }
        if (isRedoShortcut(*keyPressed)) {
            redoMove();
            return;
        }
    }

    const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>();
    if (mousePressed == nullptr) {
        return;
    }

    if (mousePressed->button == sf::Mouse::Button::Right) {
        if (isAwaitingChainContinuation_) {
            return;
        }
        clearSelection();
        return;
    }

    if (mousePressed->button != sf::Mouse::Button::Left) {
        return;
    }

    if (!isBoardInputAllowed()) {
        return;
    }

    const Position boardPos = getPositionOnBoardFromMouse(mousePressed->position.x, mousePressed->position.y);
    if (!isInsideBoard(boardPos)) {
        return;
    }
    handleBoardClick(boardPos);
}

void PlayState::updateGameResultState()
{
    if (isGameOver_) {
        return;
    }

    if (auto result = checkers_.getResult(); result.isOver) {
        enterGameOverState(result.winner);
    }
}

void PlayState::enterGameOverState(COLOUR winner)
{
    const bool shouldPlaySound = !isGameOver_ && gameContext_.mode == MODE::COMPUTER;

    isGameOver_ = true;
    winnerColor_ = winner;
    resultClock_.restart();

    if (shouldPlaySound) {
        playComputerResultSound();
    }
}

void PlayState::playComputerResultSound()
{
    stopResultSounds();

    if (winnerColor_ == gameContext_.playerColour) {
        winSound_.play();
        return;
    }

    lostSound_.play();
}

void PlayState::stopResultSounds()
{
    winSound_.stop();
    lostSound_.stop();
}

void PlayState::synchronizeBoardFromLogic()
{
    boardView_ = checkers_.getCopyBoard();
    clearSelection();
    moveState_.clear();
    isAwaitingChainContinuation_ = false;

    if (auto result = checkers_.getResult(); result.isOver) {
        enterGameOverState(result.winner);
    } else {
        isGameOver_ = false;
        winnerColor_ = COLOUR::WHITE;
        stopResultSounds();
    }
}

void PlayState::undoMove()
{
    if (moveState_.inProgress || !checkers_.canUndo()) {
        return;
    }

    bool hasChanged = false;
    if (gameContext_.mode == MODE::COMPUTER) {
        while (checkers_.canUndo()) {
            if (!checkers_.undoMove()) {
                break;
            }
            hasChanged = true;

            // In computer mode, stop on the next turn where the human can act.
            if (isHumanTurn()) {
                break;
            }
        }
    } else {
        hasChanged = checkers_.undoMove();
    }

    if (hasChanged) {
        synchronizeBoardFromLogic();
    }
}

void PlayState::redoMove()
{
    if (moveState_.inProgress || !checkers_.canRedo()) {
        return;
    }

    bool hasChanged = false;
    if (gameContext_.mode == MODE::COMPUTER) {
        while (checkers_.canRedo()) {
            if (!checkers_.redoMove()) {
                break;
            }
            hasChanged = true;

            // In computer mode, stop when control returns to the human player.
            if (isHumanTurn()) {
                break;
            }
        }
    } else {
        hasChanged = checkers_.redoMove();
    }

    if (hasChanged) {
        synchronizeBoardFromLogic();
    }
}

Position PlayState::getLastPositionInChain(const Move& move) const
{
    const Move* last = &move;
    while (last->nextMove) {
        last = last->nextMove.get();
    }
    return last->to;
}

void PlayState::triggerComputerMoveIfNeeded()
{
    if (isGameOver_ || gameContext_.mode != MODE::COMPUTER || isHumanTurn() || moveState_.inProgress ||
        (checkers_.canRedo() && checkers_.canUndo())) {
        return;
    }

    moveState_.resolvedPath = engine_->getBestMove();
    moveState_.from = moveState_.resolvedPath.from;
    moveState_.to = getLastPositionInChain(moveState_.resolvedPath);
    moveState_.hasResolvedPath = true;
    moveState_.animationProgress = 0.f;
    moveState_.inProgress = true;
}

void PlayState::update()
{
    updateGameResultState();
    triggerComputerMoveIfNeeded();
}

void PlayState::drawBoard()
{
    for (int row = 0; row < boardView_.getWidth(); ++row) {
        for (int col = 0; col < boardView_.getWidth(); ++col) {
            sf::RectangleShape& square = ((row + col) % 2 == 0 ? whiteSquare_ : blackSquare_);
            square.setPosition(
                {boardOrigin_.x + static_cast<float>(col) * tile_, boardOrigin_.y + static_cast<float>(row) * tile_});
            window_.draw(square);
        }
    }
}

void PlayState::drawPiece(Position pos, bool isMoving)
{
    auto piece = boardView_(pos);
    if (piece.isEmpty() || piece.isCaptured()) {
        return;
    }

    sf::CircleShape& shape = (piece.getColour() == COLOUR::WHITE ? whitePieceCircle_ : blackPieceCircle_);
    const Position displayPos = mapPos(pos);
    sf::Vector2f currentPos(boardOrigin_.x + static_cast<float>(displayPos.col) * tile_ + offsetForPiece_,
                            boardOrigin_.y + static_cast<float>(displayPos.row) * tile_ + offsetForPiece_);

    if (isMoving) {
        const Position displayFrom = mapPos(moveState_.from);
        const Position displayTo = mapPos(moveState_.to);
        const sf::Vector2f startPos(boardOrigin_.x + static_cast<float>(displayFrom.col) * tile_ + offsetForPiece_,
                                    boardOrigin_.y + static_cast<float>(displayFrom.row) * tile_ + offsetForPiece_);
        const sf::Vector2f endPos(boardOrigin_.x + static_cast<float>(displayTo.col) * tile_ + offsetForPiece_,
                                  boardOrigin_.y + static_cast<float>(displayTo.row) * tile_ + offsetForPiece_);

        moveState_.animationProgress += kMoveAnimationStep;
        const float t = std::min(moveState_.animationProgress, 1.0f);
        currentPos = (endPos - startPos) * t + startPos;
    }

    shape.setPosition(currentPos);
    window_.draw(shape);

    if (piece.isQueen()) {
        sf::Sprite sprite(resourceManager_.getQueenTexture());
        const auto textureSize = resourceManager_.getQueenTexture().getSize();
        const float queenTargetSize = std::max(1.f, pieceRadius_ * 1.4f);
        const float queenScale = calculateContainedScale(textureSize, {queenTargetSize, queenTargetSize});
        const sf::Vector2f queenSize{static_cast<float>(textureSize.x) * queenScale,
                                     static_cast<float>(textureSize.y) * queenScale};
        sprite.setScale({queenScale, queenScale});
        sprite.setPosition(currentPos +
                           sf::Vector2f((2.f * pieceRadius_ - queenSize.x) / 2.f, (2.f * pieceRadius_ - queenSize.y) / 2.f));
        window_.draw(sprite);
    }
}

void PlayState::drawCapturedPiece(Position pos)
{
    const Position displayPos = mapPos(pos);
    capturedPieceCircle_.setPosition({boardOrigin_.x + static_cast<float>(displayPos.col) * tile_ + offsetForPiece_,
                                      boardOrigin_.y + static_cast<float>(displayPos.row) * tile_ + offsetForPiece_});
    window_.draw(capturedPieceCircle_);
}

void PlayState::highlightPossibleMovesFrom(Position from)
{
    std::vector<Position> positionsToHighlight;
    for (const auto& candidate : selection_.candidates) {
        const Move* move = &candidate;
        do {
            if (move->from == from) {
                // Highlight the whole continuation chain from the currently selected origin
                const Move* continuation = move;
                do {
                    if (std::ranges::find(positionsToHighlight, continuation->to) == positionsToHighlight.end()) {
                        positionsToHighlight.push_back(continuation->to);
                    }
                    continuation = continuation->nextMove.get();
                } while (continuation != nullptr);
                break;
            }
            move = move->nextMove.get();
        } while (move != nullptr);
    }

    for (const Position pos : positionsToHighlight) {
        const Position displayPos = mapPos(pos);
        highlightCircle_.setPosition({boardOrigin_.x + static_cast<float>(displayPos.col) * tile_ + offsetForHighlight_,
                                      boardOrigin_.y + static_cast<float>(displayPos.row) * tile_ + offsetForHighlight_});
        window_.draw(highlightCircle_);
    }
}

void PlayState::drawHighlights()
{
    if (moveState_.inProgress && moveState_.hasResolvedPath) {
        return;
    }

    if (selection_.candidates.empty() || !selection_.hasOrigin) {
        return;
    }

    // Show all continuation destinations reachable from the currently selected square
    highlightPossibleMovesFrom(selection_.origin);
}

void PlayState::drawPieces()
{
    for (int row = 0; row < boardView_.getWidth(); ++row) {
        for (int col = (row % 2 ? 0 : 1); col < boardView_.getWidth(); col += 2) {
            if (auto piece = boardView_(row, col); piece.isNotEmpty()) {
                if (piece.isCaptured()) {
                    drawCapturedPiece({row, col});
                    continue;
                }

                const Position pos{row, col};
                if (!moveState_.inProgress || !(moveState_.from == pos)) {
                    drawPiece(pos);
                }
            }
        }
    }
}

void PlayState::renderResult()
{
    const sf::Vector2u windowSize = window_.getSize();
    sf::RectangleShape greyOverlay(sf::Vector2f{static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    greyOverlay.setFillColor(sf::Color{128, 128, 128, 190});
    window_.draw(greyOverlay);

    const sf::Texture& resultTexture =
        (gameContext_.mode == MODE::COMPUTER)
            ? (winnerColor_ == gameContext_.playerColour ? resourceManager_.getYouWinTexture()
                                                         : resourceManager_.getYouLostTexture())
            : resourceManager_.getColourWinsTexture(winnerColor_);
    sf::Sprite sprite{resultTexture};

    const float spriteScale = calculateContainedScale(
        resultTexture.getSize(), {boardSizePx_ * kResultSpriteMaxWidthFactor, boardSizePx_ * kResultSpriteMaxHeightFactor});
    const sf::Vector2f spriteSize{static_cast<float>(resultTexture.getSize().x) * spriteScale,
                                  static_cast<float>(resultTexture.getSize().y) * spriteScale};
    sprite.setScale({spriteScale, spriteScale});

    const sf::Vector2f spritePos{(static_cast<float>(windowSize.x) - spriteSize.x) / 2.f,
                                 boardOrigin_.y + boardSizePx_ * kResultSpriteTopFactor};
    sprite.setPosition(spritePos);
    window_.draw(sprite);

    if (resultClock_.getElapsedTime().asSeconds() > 2) {
        const auto messageCharacterSize =
            static_cast<unsigned int>(std::clamp(boardSizePx_ * 0.03f, static_cast<float>(kMinResultMessageCharacterSize),
                                                 static_cast<float>(kMaxResultMessageCharacterSize)));
        sf::Text message{resourceManager_.getFont(), "Press Esc to return to the menu", messageCharacterSize};
        message.setFillColor(sf::Color::White);
        const auto messageBounds = message.getLocalBounds();
        message.setPosition(
            {(static_cast<float>(windowSize.x) - messageBounds.size.x) / 2.f - messageBounds.position.x,
             spritePos.y + spriteSize.y + boardSizePx_ * kResultMessageGapFactor - messageBounds.position.y});
        window_.draw(message);
    }
}

void PlayState::commitResolvedMoveIfNeeded()
{
    if (!moveState_.hasResolvedPath) {
        return;
    }

    // Commit only once after animation finished
    moveState_.hasResolvedPath = false;
    checkers_.makeMove(moveState_.resolvedPath);
    boardView_ = checkers_.getCopyBoard();
    clearSelection();
}

void PlayState::processMoveAnimation()
{
    drawPiece(moveState_.from, true);

    if (moveState_.animationProgress < 1.f) {
        return;
    }

    moveState_.animationProgress = 0.f;
    if (moveState_.hasResolvedPath) {
        // Fully resolved move: animation ends and we can commit into Checkers
        moveState_.inProgress = false;
    } else {
        // Ambiguous chained capture visualization: move one segment and wait for next click
        const Position previousFrom = moveState_.from;
        std::swap(boardView_(moveState_.to), boardView_(moveState_.from));
        if (moveState_.segmentCapturedPiecePos.isValid()) {
            boardView_(moveState_.segmentCapturedPiecePos).setCaptured();
            moveState_.segmentCapturedPiecePos.reset();
        }
        selection_.origin = moveState_.to;
        selection_.hasOrigin = true;
        isAwaitingChainContinuation_ = true;

        // If there is no continuation from the new origin, this segment closes the chain
        if (!hasSegmentsFrom(selection_.origin) && !selection_.candidates.empty()) {
            moveState_.resolvedPath = cloneMove(selection_.candidates.front());
            moveState_.hasResolvedPath = true;
            moveState_.inProgress = false;
        }

        moveState_.from = moveState_.to;
        moveState_.to = previousFrom;
        moveState_.inProgress = false;
    }

    commitResolvedMoveIfNeeded();
}

void PlayState::render()
{
    updateLayoutMetrics();
    drawBoard();
    drawPieces();
    drawHighlights();

    if (isGameOver_) {
        renderResult();
        return;
    }

    if (moveState_.inProgress) {
        processMoveAnimation();
    }
}

void PlayState::initializeEngine()
{
    if (gameContext_.mode != MODE::COMPUTER) {
        engine_.reset();
        return;
    }

    if (gameContext_.engineMode == ENGINE_MODE::NOVICE) {
        engine_ = std::make_unique<RandomEngine>(checkers_);
    } else {
        engine_ = std::make_unique<MinimaxEngine>(checkers_, gameContext_.engineMode);
    }
}

int PlayState::getPieceRadiusOffset() const
{
    switch (gameContext_.checkersType) {
        case CHECKERS_TYPE::BRAZILIAN:
        case CHECKERS_TYPE::RUSSIAN:
            return 22;
        case CHECKERS_TYPE::INTERNATIONAL:
            return 20;
        case CHECKERS_TYPE::CANADIAN:
            return 15;
        default:
            throw std::logic_error("Unknown CHECKERS_TYPE");
    }
}

void PlayState::updateLayoutMetrics()
{
    const int boardWidth = boardView_.getWidth();
    if (boardWidth <= 0) {
        return;
    }

    const sf::Vector2u windowSize = window_.getSize();
    const int radiusOffset = getPieceRadiusOffset();
    if (hasLayoutMetrics_ && windowSize == lastLayoutWindowSize_ && boardWidth == lastLayoutBoardWidth_ &&
        radiusOffset == lastLayoutRadiusOffset_) {
        return;
    }

    lastLayoutWindowSize_ = windowSize;
    lastLayoutBoardWidth_ = boardWidth;
    lastLayoutRadiusOffset_ = radiusOffset;
    hasLayoutMetrics_ = true;

    boardSizePx_ = static_cast<float>(std::min(windowSize.x, windowSize.y));
    boardOrigin_ = {(static_cast<float>(windowSize.x) - boardSizePx_) / 2.f,
                    (static_cast<float>(windowSize.y) - boardSizePx_) / 2.f};
    tile_ = boardSizePx_ / static_cast<float>(boardWidth);

    pieceRadius_ = std::max(1.f, (tile_ - static_cast<float>(radiusOffset)) / 2.0f);
    offsetForPiece_ = std::max(0.f, (tile_ - 2.f * pieceRadius_) / 2.0f);
    highlightRadius_ = pieceRadius_ / 2;
    offsetForHighlight_ = (tile_ - 2.f * highlightRadius_) / 2.0f;

    highlightCircle_.setRadius(highlightRadius_);
    capturedPieceCircle_.setRadius(pieceRadius_);
    whitePieceCircle_.setRadius(pieceRadius_);
    blackPieceCircle_.setRadius(pieceRadius_);
    capturedPieceCircle_.setOutlineThickness(std::max(2.f, tile_ * 0.03f));
    whitePieceCircle_.setOutlineThickness(std::max(2.f, tile_ * 0.04f));
    blackPieceCircle_.setOutlineThickness(std::max(2.f, tile_ * 0.04f));
    whiteSquare_.setSize(sf::Vector2f(tile_, tile_));
    blackSquare_.setSize(sf::Vector2f(tile_, tile_));
}

void PlayState::reset()
{
    stopResultSounds();

    checkers_.setCheckersType(gameContext_.checkersType);
    checkers_.reset();
    boardView_ = checkers_.getCopyBoard();
    isBoardFlipped_ = gameContext_.mode == MODE::COMPUTER && gameContext_.playerColour == COLOUR::BLACK;

    clearSelection();
    moveState_.clear();
    isAwaitingChainContinuation_ = false;
    isGameOver_ = false;
    winnerColor_ = COLOUR::WHITE;

    initializeEngine();
    updateLayoutMetrics();
}
