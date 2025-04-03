#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sqlite3.h>                    // for database handling
#include <time.h>                       // for calculating time and for seeding rand() with current time
#include <conio.h> 
#include <windows.h>                    // for ANSI escape codes 

#define MIN(a, b) ((a < b) ? a : b)         // used in score calculation

clock_t start_time;                         // stores the starting time of the game to calculate the total time taken
sqlite3 *db;                                // pointer to SQL database; used to access the database

typedef struct ob{                          // structure to operate on the grid and update its value through different functions
    char grid[9][9];
} obj;

char real[9][9];                            // copy array to store the correct sudoku grid; used later for comparison with user input
int mistake = 0;                            // accumulator variable to count the mistakes committed by the user
int done = 0;                               // accumulator variable to store the number of spaces filled by the user; used to mark the end of the game
int qt = 0;
int current_row = 0;
int current_col = 0;
int limit = 0;

void start_timer(){                                          // initializes the game timer by storing the current CPU clock time; marks the beginning of the timer
    start_time = clock(); 
}

double get_time_taken(){                           // subtracts start time from current time and and divides the difference with CPS to convert it into seconds   
    return (double)(clock() - start_time) / CLOCKS_PER_SEC;
}

void create_tables(){ 
    printf("\033[92m"); // ANSI code for green text
    printf("\033[40m"); // ANSI code for black background
    char *err_msg = 0; // to debug
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, username TEXT UNIQUE, password TEXT);", NULL, NULL, &err_msg);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS leaderboard (id INTEGER PRIMARY KEY, username TEXT, score INTEGER);", NULL, NULL, &err_msg);
}

int register_user(const char *username, const char *password){ 
    /*The const keyword ensures that the values of username and password cannot 
     be modified inside the function. This prevents accidental changes and improves code safety.*/
    printf("\033[92m");
    printf("\033[40m");
    char sql[256]; // to store the sql query
    // to format sql query as a string and insert values of username and password
    sprintf(sql, "INSERT INTO users (username, password) VALUES ('%s', '%s');", username, password); 
    return sqlite3_exec(db, sql, NULL, NULL, NULL) == SQLITE_OK; 
    /* 
    db = pointer of the database we've opened
    sql = command to be executed 
    1st NULL = for callback func. -> since it's an insertion function, 
               no callback(returned data) is needed, therefore left NULL
    2nd NULL = for the argument passed to the callback func.(if any)
    3rd NULL = to store an error message 
    */
}

int login_user(const char *username, const char *password){
    printf("\033[92m");
    printf("\033[40m");
    sqlite3_stmt *stmt; // prepared statement object - used for the execution of the query
    char sql[256]; // query will be stored in the form of a string in this char array
    sprintf(sql, "SELECT id FROM users WHERE username='%s' AND password='%s';", username, password);
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0); // prepares/compiles the query into a format that SQLite can execute 
    /* -1 = SQLite determines the length of the query automatically
    &stmt = compiled query will be stored in stmt
    0 = reserved for future use(always 0)*/
    int result = sqlite3_step(stmt) == SQLITE_ROW; /* executes prepared query and moves cursor to the next line 
    in result - if match of user found in db, it returns the row in which it was found(SQLITE_ROW), otherwise it 
    return SQLITE_DONE if not found.*/
    sqlite3_finalize(stmt); // frees memory allocated for statement object
    return result; // returns 1 if user already exists in the db, 0 if not found
}

void save_score(const char *username, int score) {
    printf("\033[92m");
    printf("\033[40m");
    char sql[256];

    sprintf(sql, "INSERT INTO leaderboard (username, score) VALUES ('%s', %d);", username, score);
    sqlite3_exec(db, sql, NULL, NULL, NULL);
}

void show_leaderboard(){
    printf("\033[92m");
    printf("\033[40m");
    sqlite3_stmt *stmt; // statement object
    sqlite3_prepare_v2(db, "SELECT username, score FROM leaderboard ORDER BY score DESC LIMIT 5;", -1, &stmt, 0); 
    printf("\n Leaderboard\n");
    while (sqlite3_step(stmt) == SQLITE_ROW){
        printf("%s - %d points\n", sqlite3_column_text(stmt, 0), sqlite3_column_int(stmt, 1));
    }
    sqlite3_finalize(stmt);
}

int calculate_score(int correct_entries, int mistakes, double time_taken, int difficulty_level) {
    int base_score = correct_entries * 10;  // +10 for each correct box
    int penalty = mistakes * 5;  // -5 for each mistake
    int time_bonus = (time_taken > 0) ? (300 / time_taken) * 5 : 0; // Bonus for faster completion
    int difficulty_multiplier = (difficulty_level == 1) ? 1 : (difficulty_level == 2) ? 2 : 3; // 1x, 2x, or 3x

    int final_score = (base_score - penalty + time_bonus) * difficulty_multiplier;
    return (final_score < 0) ? 0 : final_score; // Ensure score is never negative
}

// SUDOKU :
bool isValidMove(obj *x, int row, int col, char num){
    for (int j = 0; j < 9; j++)
        if (x->grid[row][j] == num)
            return false;
    for (int i = 0; i < 9; i++)
        if (x->grid[i][col] == num)
            return false;
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (x->grid[i + startRow][j + startCol] == num)
                return false;
    return true;
}

bool solve_sudoku(obj *x){
    for (int row = 0; row < 9; row++){
        for (int col = 0; col < 9; col++){
            if (x->grid[row][col] == '0'){
                // Create and shuffle numbers properly
                char numbers[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
                for (int i = 8; i > 0; i--){ 
                    int j = rand() % (i + 1);
                    char temp = numbers[i];
                    numbers[i] = numbers[j];
                    numbers[j] = temp;
                }
                for (int i = 0; i < 9; i++){
                    if (isValidMove(x, row, col, numbers[i])){
                        x->grid[row][col] = numbers[i];
                        if (solve_sudoku(x))
                            return true; // Correct return for backtracking
                        x->grid[row][col] = '0';
                    }
                }
                return false; // If no valid number fits, return false
            }
        }
    }
    return true; // Solved successfully
}

obj *generate_sudoku(obj *x){
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            x->grid[i][j] = '0';
    solve_sudoku(x);
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            real[i][j] = x->grid[i][j];
    return x;
}



void printGrid(obj *x){
    printf("\033[H\033[J");     // Move cursor to the top-left and clear screen
    printf("\033[92m\033[40m"); // Green text on black background
    printf("Use arrow keys to move | Press 1-9 to fill | Q to quit\n");
    for (int i = 0; i < 9; i++){
        if (i % 3 == 0)
            printf("-------------------------\n");
        for (int j = 0; j < 9; j++){
            if (j % 3 == 0)
                printf("| ");
            printf("\033[92m\033[40m");
            if (i == current_row && j == current_col)
                printf("\033[48;5;27m");
            // if (x->grid[i][j] == ' ')
            //     printf("\033[38;5;240m");
            printf("%c ", x->grid[i][j]);
            printf("\033[92m\033[40m");
        }
        printf("|\n");
    }
    printf("-------------------------\n");
    printf("\033[0m");
}

void handleInput(obj *x){
    int key = _getch();
    if (key == 0xE0){ // Arrow keys
        key = _getch();
        switch (key){
        case 72: // up
            if (current_row > 0)    current_row--;
            break;
        case 80: //down
            if (current_row < 8)    current_row++;
            break;
        case 75: // left
            if (current_col > 0)    current_col--;
            break;
        case 77: // right
            if (current_col < 8)    current_col++;
            break;
        }
    }
    else if (key >= '1' && key <= '9'){
        if (x->grid[current_row][current_col] == ' '){
            if (key == real[current_row][current_col]){
                done++;
                x->grid[current_row][current_col] = key;
            }
            else
                mistake++;
        }
    }
    else if (key == 'q' || key == 'Q'){
        qt = 1;
        return;
    }
}

obj *diffi(obj *x, int diff)
{
    int totalCells = 81; // 9x9 grid
    int removeCount = (diff == 1) ? 25 : (diff == 2) ? 35 : 45;
    int positions[81];
    for (int i = 0; i < 81; i++)
        positions[i] = i;
    for (int i = 80; i > 0; i--){
        int j = rand() % (i + 1);
        int temp = positions[i];
        positions[i] = positions[j];
        positions[j] = temp;
    }
    for (int i = 0; i < removeCount; i++){
        int row = positions[i] / 9;
        int col = positions[i] % 9;
        x->grid[row][col] = ' '; // Empty the cell
    }
    limit = removeCount;
    return x;
}

void show_rules(){
    printf("\n Rules of the Game:\n");
    printf("1. Fill the 9x9 grid so that each row, column, and 3x3 box contains digits 1-9.\n");
    printf("2. Use arrow keys to navigate the grid.\n");
    printf("3. Press '1'-'9' to fill numbers.\n");
    printf("4. Press 'Q' to quit the game.\n");
    printf("5. Mistakes reduce your final score.\n\n");
}

void show_score_calculation() {
    printf("\nScore Calculation:\n");
    printf("1. +10 points for each correct entry.\n");
    printf("2. -5 points for each mistake.\n");
    printf("3. Time Bonus: Faster completion earns extra points.\n");
    printf("4. Difficulty Multiplier:\n");
    printf("     - Easy (1x), Medium (2x), Hard (3x)\n");
    printf("5.Minimum score is 0 (No negative scores).\n");
}

void display_menu(){
    printf("\n===== C-DOKU =====\n");
    printf("1) Play Game\n");
    printf("2) Show Leaderboard\n");
    printf("3) Rules\n");
    printf("4) Score Calculation\n");
    printf("5) Exit\n");
    printf("Enter your choice: ");
}

int main(){
    printf("\033[92m\033[40m");
    printf("Welcome to C-doku!\n");
    sqlite3_open("cdoku.db", &db);
    create_tables();
    char username[50], password[50];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    if (!login_user(username, password)){
        printf("User not found. Registering new user...\n");
        register_user(username, password);
    }
    l1:
    srand(time(NULL));
    obj *a = (obj *)malloc(sizeof(obj));
    a = generate_sudoku(a);
    printf("\033[92m\033[40m");
    while (1){
        display_menu();
        int choice;
        scanf("%d", &choice);
        switch (choice){
        case 1:
            int difficulty;
            printf("Choose difficulty (1-Easy, 2-Medium, 3-Hard): ");
            scanf("%d", &difficulty);
            a = diffi(a, difficulty);
            start_timer();
            qt = 0;
            mistake = 0;
            done = 0;
            while (1){
                handleInput(a);
                printGrid(a);
                if(done==limit){
                    printf("CONGRATULATIONS\n");
                    break;
                }
                if (qt == 1){
                    qt = 0;
                    double time_taken = get_time_taken();
                    int score = calculate_score(done, mistake, time_taken, difficulty);
                    save_score(username, score);
                    printf("\nTime Taken : %.2f seconds", time_taken);
                    printf("\nFinal Score: %d\n", score);
                    goto l1;
                }
            }
            double time_taken = get_time_taken();
            int score = calculate_score(done, mistake, time_taken, difficulty);
            save_score(username, score);
            printf("\nTime Taken : %.2f seconds", time_taken);
            printf("\nFinal Score: %d\n", score);
            break;
        case 2:
            show_leaderboard();
            break;
        case 3:
            show_rules();
            break;
        case 4:
            show_score_calculation();
            break;
        case 5:
            printf("Exiting...\n");
            sqlite3_close(db);
            free(a);
            return 0;
        default:
            printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}