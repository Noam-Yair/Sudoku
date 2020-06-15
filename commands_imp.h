/*defining global variables: MODE, BOARD, Mark_Errors, ROW,COL- number of rows/columns in the current board,
N = ROW*COL, FILLED_CELLS- number of filled cells in each move, ERRORMAT - binary matrix: if BOARD[i][j] is
invalid -> ERRORMAT[i][j]=1, else ERRORMAT[i][j]=0
ERRONEOUS =1 if the board has one invalid value or more, else ERRONEOUS=0
implemnetation of all possible commands
*/

extern int MODE;
extern int** BOARD;
extern int MARK_ERRORS;
extern int ROW;
extern int COL;
int N;
int FILLED_CELLS;
int** ERRORMAT;
int ERRONEOUS;
#define MAX  25


int** initial_empty_board(int row, int col);
void solve(char* path);
void edit(char* path);
void edit_noPath();
void set(int x, int y, int z);
void markErrors(int x);
void print_board();
void guess(float x);
void Save(char* path);
void hintAndGuess_hint(int x, int y, char* func);
void validate();
void generate(int x, int y);
void autofill();
void exitGame();
void num_sol();




