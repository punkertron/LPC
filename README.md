# LPC -  Let's Play Checkers!!

A classic Checkers (Draughts) game written in modern C++20, featuring both local two-player mode and various AI opponents, rendered with SFML.<br/>
Only Russian checkers is supported now.

---

## Features

- **Board Logic**  
  - 8×8 board with automatic move generation and capture rules.  
  - Piece promotion (regular to queen) at the opposite side.  
  - Chained captures supported (multiple jumps in one turn).  

- **AI Engines**  
  - **Random Engine**: Picks a random valid move.  
  - **Minimax Engine**: Employs the Minimax algorithm (with Alpha-Beta pruning) with variable difficulty levels (`EASY`, `MEDIUM`, `HARD`, `GRANDMASTER`).  

- **GUI**  
  - SFML-based user interface with clickable squares.  
  - Menu system to select either two-player or computer-vs-human mode, as well as difficulty/AI level.  
  - Visual highlights for valid moves, chain captures, and game result screens.  

- **State Machine**  
  - Easily extendable system to manage game states (Menu, Play, etc.).  

---
## Some screenshots


<div style="display: flex; justify-content: center;">
  <img src="assets/Main_menu.png" alt="Main menu" width="400" style="margin-right: 5px;" />
  <img src="assets/select_colour.png" alt="Select colour" width="400" />
</div>


<div style="display: flex; justify-content: center;">
  <img src="assets/select_mode.png" alt="Select mode" width="400" style="margin-right: 5px;" />
  <img src="assets/game.png" alt="Game in progress" width="400" />
</div>

<div style="display: flex; justify-content: center;">
  <img src="assets/game_result.png" alt="Final" width="400" />
</div>

---

## Architecture Overview

This project is split into three main parts:

1. **Board**  
   - **`Board`** class: Maintains an 8×8 grid of `std::shared_ptr<Piece>` pointers.  
   - Handles all move logic, captures, piece promotion, and maintains the turn order.  
   - Exposes functions for retrieving valid moves, making moves, and resetting the board.  
   - Tracks game result and winner determination.

2. **Engine**  
   - **`Engine`** is an abstract base class providing a `getBestMove()` method.  
   - **`RandomEngine`**: Returns a random valid move.  
   - **`MinimaxEngine`**: Implements a Minimax search. Different difficulty levels limit the search depth. Implemented Alpha-Beta pruning. Added a random component when choosing the optimal move to minimize the probability of getting exactly the same games.
   - Uses an evaluation function in `EvaluationFunction.cpp` to score board states.

3. **GUI**  
   - **SFML**-driven interface in the `gui` directory.  
   - **`StateManager`**: Maintains a stack of states.  
   - **`MenuState`**: Lets user select mode (two-player or vs. AI), color for player, and AI difficulty.  
   - **`PlayState`**: Renders the board, pieces, handles user input, and orchestrates moves from the chosen `Engine`.  
   - **`ResourceManager`**: Loads and provides textures and fonts.  
   - **`Button`**: Simple clickable UI element.  
<br/>

### Notable Design Patterns

- **Strategy Pattern**  
  - `Engine` as the strategy interface; `RandomEngine` and `MinimaxEngine` as concrete implementations.
  
- **State Pattern**  
  - `StateManager` switches between different GUI states (`MenuState`, `PlayState`).

- **Factory/Builder-Like**  
  - Creation of specific engines (`ENGINE_MODE` → `RandomEngine` or `MinimaxEngine`) depending on user choice.

- **Observer-Like**  
  - While not a strict observer pattern, the GUI “observes” changes in the `Board` state by calling `Board::getResult()` and re-rendering accordingly.

<br/>

### Core Algorithms

- **Move Generation**  
  - Generates valid moves using `Board::generateValidMoves()`.  
  - Forces capture moves if any are available (as in checkers rules).  
  - Supports piece promotion and chain captures.

- **Minimax with Depth (with Alpha-Beta pruning)**  
  - Evaluates board states up to a maximum depth (`EASY = 2`, `MEDIUM = 4`, etc.)  
  - Scores positions using `evaluatePosition()` for piece advantage and positional bonuses.  

---

## Getting Started

1. **Clone the Repository**
2. **Build** (Developed on Debian 12)<br/>Make sure you have a C++20 compiler and CMake ≥ 3.25.1 installed, plus SFML (version ≥ 2.5).
```bash
mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make
```
3. **Run**
```bash
./LPC
```

---

## Usage

1. Launch the game — a window will open with the main menu.
2. Select Game Mode — either play against the computer (`With Computer`) or another human locally (`Two Players`).
3. (Computer mode only) Choose Your Colour — play as White or Black.
4. (Computer mode only) Select Difficulty — choose from `Novice`, `Easy`, `Medium`, `Hard`, or `Grandmaster`.
5. Enjoy playing — click the squares to select a piece and move it. Possible moves are highlighted automatically.
6. Press `ESC` anytime to return to the main menu.

---

## TODO
1. **Add Additional Variants / Rules**
    - Option for `10×10` International Checkers.

2. **Implement Undo / Redo**
    - Track and revert moves for convenience.

3. **Improve AI**
    - Implement a transposition table or caching.
    - Add multhithreading (`Grandmaster` mode can be faster).

4. **Online Multiplayer**
    - Introduce network play for remote two-player matches.

5. **UI Enhancements**
    - Turn the board around if playing for black against the computer.
    - Animate piece captures more smoothly.
    - Add sounds and a game timer.

6. **Automated Testing**
    - Add unit tests for critical Board methods and engine functionality.