# ♟️ C++ Chess Game with SFML

A complete 2D chess game built in **C++** using the **SFML** graphics library. 
This game implements the full set of chess rules, supports both human and AI opponents (via **Stockfish**), 
and provides a clean graphical user interface with mouse controls.

---

## 🎯 Features

- Full chess logic with rule validation
- Check, checkmate, stalemate, and various draw rules
- Castling, en passant, and pawn promotion
- Player vs Player and Player vs Stockfish AI
- Visual 2D chessboard using SFML
- Turn-based input via mouse clicks
- Adjustable AI difficulty (easy, medium, hard)
- Built-in undo for internal AI move simulations

---

## 📜 Supported Chess Rules

### ✅ Movement Rules
- All piece movements (Pawn, Knight, Bishop, Rook, Queen, King)
- Pawn double-step from starting square
- Blocked path and line-of-sight logic for sliding pieces

### ✅ Special Moves
- **Castling** (Kingside and Queenside)
- **En Passant**
- **Pawn Promotion** (promotes to queen by default)

### ✅ Check System
- Detection of **check**
- Restriction of king movement to safe squares
- Illegal move prevention when king is exposed

### ✅ Game End Conditions
- **Checkmate**
- **Stalemate**
- **Draw by Fifty-Move Rule**
- **Draw by Insufficient Material**
- **Draw by Three-Fold Repetition**
- **Draw by Stalemate**

### ✅ Legal Move Validation
- Each move checked for legality
- "ResolveCheck" logic to evaluate moves under check

---

## 🎮 Controls

| Action              | Control                     |
|---------------------|-----------------------------|
| Select & move piece | 🖱️ Left Mouse Click         |
| Resign Game         | Press `R`                   |
| Exit game           | Press `Esc` or close window |

---

## 🧠 AI Difficulty Levels

- **Easy** – Quick responses
- **Medium** – Balanced challenge
- **Hard** – Longer think time

Stockfish UCI depth is adjusted accordingly.

---

## 🧱 Technologies Used

- **C++17**
- **SFML**
- **Stockfish** (UCI engine)
- **Visual Studio (Windows)**

---

## 🧰 Setup Instructions

1. Download & install SFML and Stockfish
2. Clone repo & configure includes/libs
3. Link required SFML modules
4. Compile and run

---


## 👨‍💻 Author

**Uzair Majeed**  
🧠 Software Engineer

---
