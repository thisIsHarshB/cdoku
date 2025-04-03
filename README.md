# C-Doku: A Terminal-Based Sudoku Game  

## 📌 Introduction  
C-Doku is a **command-line Sudoku game** written in C, featuring an interactive grid, difficulty levels, a leaderboard, and an SQLite-based scoring system. Players can solve randomly generated Sudoku puzzles while tracking their scores based on time, accuracy, and difficulty.  

## 🎮 Features  
- **Random Sudoku Generation** 🧩  
- **Three Difficulty Levels** (Easy, Medium, Hard)  
- **Scoring System** based on correct moves, penalties, and time bonus  
- **SQLite Database Integration** for user authentication and leaderboard  
- **User-Friendly Controls** with arrow keys for navigation  

## 🛠 Installation & Compilation  
### Prerequisites  
Ensure you have the following installed:  
- **GCC Compiler** (`gcc`)  
- **SQLite3 Library** (`sqlite3`)  

### Compiling the Game  
Run the following command in the terminal:  
```bash
gcc -o cdoku sudoku.c -lsqlite3
```
### 🏃 Running the Game  

After compiling, execute:  
```bash
./cdoku
```

### 🕹 Controls

- **Arrow Keys** → Navigate the Sudoku grid  
- **1-9 Keys** → Fill a cell with a number  
- **Q Key** → Quit the game  

---

### 📊 Scoring System  

| Action             | Points                        |
|--------------------|-----------------------------|
| Correct entry     | +10                          |
| Incorrect entry   | -5                           |
| Time bonus       | (300 / time_taken) * 5       |
| Difficulty multiplier | x1 (Easy), x2 (Medium), x3 (Hard) |

---

### 📜 Database Schema  

C-Doku uses **SQLite3** for user authentication and score tracking:  

- **Users Table:** Stores usernames and passwords  
- **Leaderboard Table:** Stores usernames and their highest scores  

---

### 🚀 Future Enhancements  

- Add **hint system** for players as in the actual mobile games.
- Implement **Graphical UI version** to make it visually appealing.
- Enable **multiplayer mode** to make it more interesting.
- Make it **platform-independent** so that it should work on Linux/MacOS, etc.
- Maintain a database with pre-generated puzzles to randomly fetch them instead of generating it each time.

---

### 👨‍💻 Contributing  

Feel free to fork this repository, create pull requests, and report issues. Contributions are always welcome!  

---
