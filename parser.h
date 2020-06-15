/*helper functions for commands functions and the gurobi implementation*/


int is_valid(int** arr, int row, int col, int num);
int FileToArray(char* path);
int** copy_board(int** board);
int** all_cell_fixed(int** arr);
void ArrayToFile(char* path);
int is_fixed(int row, int col);
int validFixed();
void updateErrorMAT();
void fillBoard(int*** gurobiMat, double* sol);
void noFixed();
void freeBoard(int** arr);
void fillGuessedBoard(int*** objMat, double* sol, double threshold);