
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

/*defining global variables: MODE, BOARD, Mark_Errors, ROW,COL- number of rows/columns in the current board,
N = ROW*COL, FILLED_CELLS- number of filled cells in each move, ERRORMAT - binary matrix: if BOARD[i][j] is
invalid -> ERRORMAT[i][j]=1, else ERRORMAT[i][j]=0
ERRONEOUS =1 if the board has one invalid value or more, else ERRONEOUS=0
implemnetation of all possible commands
*/

/*#pragma warning(disable : 4996)*/
#include "commands_imp.h"
#include "doubly_linked_list.h"
#include "print.h"
#include "parser.h"
#include "ilp.h"
#include "user_commands.h"
#include "stack.h"

int MODE; /*MODE = 0 --> Init , MODE=1 --> Edit , MODE=2 --> Solve*/
int** BOARD;
int MARK_ERRORS;
int ROW;
int COL;
int N;
int FILLED_CELLS;
int** ERRORMAT;
int ERRONEOUS;
#define MAX  25


int** initial_empty_board(int row, int col) {/* row/col- number of rows/cols in the block */
    int i, j;
    ROW = row;
    COL = col;
    N = row * col;
    if (N > MAX || N<=0) {
        printf("board's dimensions are invalid\n"); /*need to write about right dimensions*/
    }
    else {
        int** arr;
        arr = (int**)malloc(N * sizeof(int*));
        for (i = 0; i < N; i++)
            arr[i] = (int*)malloc(N * sizeof(int));
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                arr[i][j] = 0;
            }
        }
        return arr;
    }
    return NULL;
}


void solve(char* path) {
    FILE* fromFile;
    int valid; 
     
    
    fromFile = fopen(path, "r");
    if (fromFile != NULL) {
        
        if ((valid = FileToArray(path)) == 1) {
            if (validFixed() == 1) { /*checking if the fixed cells create a valid board*/
		        MODE=1;
MARK_ERRORS =1;
                initialDDL();
                ERRORMAT = initial_empty_board(ROW,COL);
                updateErrorMAT();
                print();
            }
            else {
                printf("The board is erroneous, please provide a different file\n");
            }
        }
    }
    else {
        printf("Error in opening the file\n");
    }
}

void edit(char* path) {
    FILE* fromFile;
    MARK_ERRORS=1;
    MODE = 2;
    initialDDL();
    fromFile = fopen(path, "r");
    if ( fromFile!= NULL) {
        if (FileToArray(path) == 1) {
            initialDDL();
            ERRORMAT = initial_empty_board(ROW, COL);
            updateErrorMAT();
            noFixed();
            print();
        }
    }

    else {
        printf("Error in opening the file\n");
    }
}

void edit_noPath() {
    MARK_ERRORS=1;
    BOARD = initial_empty_board(3, 3);
    FILLED_CELLS = 0;
    MODE = 2;
    ERRORMAT = initial_empty_board(3,3);
    ERRONEOUS = 0;
    initialDDL();
    print();
}

void markErrors(int x) {

     MARK_ERRORS = x;


}

void print_board() {
    print();
}


/* arr[i][j] = *(ptr + (i x no_of_cols + j))*/
void set(int x, int y, int z) { /*x is the column,  y is the row*/
    if (x > 0 && y > 0 && x <= N && y <= N && z >= 0 && z <= N) {
        if (is_fixed(y - 1, x - 1) == 1 && MODE == 1) {
            printf("cell is fixed, try another cell\n");
        }
        else {
            add(1, y - 1, x - 1, z, BOARD[y - 1][x - 1]); /*adding move to linked list*/
            if (BOARD[y - 1][x - 1] == 0)
                FILLED_CELLS += 1;
            BOARD[y - 1][x - 1] = z;
            if (is_valid(BOARD, x - 1, y - 1, z) == 0) {
                updateErrorCell(y - 1, x - 1, z);            }
	    else{
		ERRORMAT[y-1][x-1]=0;
        	}
	    updateErrorMAT();
	    print();
        }
    }
    else {
        printf("values are not in range, please change to range %d-%d\n", 1, N);
    }
}

void validate() {
    int check;
    if (ERRONEOUS == 0) {
        check = all_func_gurobi(0, 0, "validate", 0);
        if (check == 1)
            printf("The board is solvable\n");
        else
            printf("The board isn't solvable\n");
    } else
        printf("The board is erroneous, command is not available\n");

}
   
void guess(float x) {
    if (ERRONEOUS == 0) {
        add(4, -1, -1, -1, -1);
        all_func_gurobi(0, 0, "guess", x); 
        add(4, 0, 0, 0, 0);
        print();
    }
    else
        printf("The board is erroneous, command is not available\n");
}

void Save(char* path) {
    if (ERRONEOUS == 1){
        printf("The board is erroneous, command is not available\n");
    }
    else if (all_func_gurobi(0, 0, "validate",0) != 1)
        printf("The board isn't solvable, can't save\n");
    else{
        ArrayToFile(path);
}
}

void hintAndGuess_hint(int x, int y, char* func) {
    x -= 1;
    y -= 1;
    if (ERRONEOUS == 1) 
        printf("The board is erroneous, command is not available\n");
    else if (is_fixed(y, x) == 1) 
        printf("cell is fixed, try another cell\n");
    else if (BOARD[y][x] > 0) 
        printf("cell contains a value, try another cell\n");
    else 
         all_func_gurobi(y, x, func,0);

}



/*num = (rand() % (upper � lower + 1)) + lower*/

void generate(int x, int y) {
    int iter = 0,i,j,cnt=0, row, col, num;
    int* legal_arr;
    int** origBoard;
    int origX;
    origX = x;
    
    origBoard = initial_empty_board(ROW, COL);
    origBoard = copy_board(BOARD);

    if (x < 0 || y <= 0) {
        printf("please enter non negative values\n"); 
        return;
    }
    if (x > (N * N) - FILLED_CELLS) {
        printf("there are not enough cells to fill, please enter a number smaller than %d\n", (N * N) - FILLED_CELLS);
        return;
    }
    if (ERRONEOUS == 1) {
        printf("The board is erroneous, command is not available\n");
        return;
    }
    while (iter < 1000 && x>0) { 
    /*random cell to fill*/
        row = (rand() % N );
        col = (rand() % N );
        if (BOARD[row][col] == 0) {
            legal_arr = calloc(N , sizeof(int));
            cnt = 0;
            for (num = 1; num < N+1 ; num++) {
                if (is_valid(BOARD, row, col, num) == 1) {
                    legal_arr[cnt++] = num;
                }
            }
            if (cnt > 0) { /*there are valid values to place in cell*/
                x -= 1;
                num = (rand() % cnt ); /*random index from the legal values*/
                BOARD[row][col] = legal_arr[num];
            }
            else { /*no valid values for cell - reset to original board and try again*/
                x = origX;
                for (i = 0; i < N; i++) {
                    for (j = 0; j < N; j++) {
                        BOARD[i][j] = origBoard[i][j]; /*return BOARD to original*/
                    }
                }
                
            }
        }
        if (x == 0) { /*filled X random cells*/
            all_cell_fixed(BOARD);

            if (all_func_gurobi(0, 0, "generate",0) != 1) {
                BOARD = copy_board(origBoard);
                x = origX;
            }
            else { /*the board has a solution*/
            i = 0;
                while (i < ((N*N) - y)){
                    row = (rand() % N);
                    col = (rand() % N);
                    if (BOARD[row][col] != 0) {
                        BOARD[row][col] = 0;
                        i += 1;
                    }
                    
                }
		updateErrorMAT();
                FILLED_CELLS =  y;
                print();
            }
        }
        iter += 1;

    }
    if (iter == 1000 && x > 0) {
        BOARD = origBoard;
    }
    else {
        add(3, 0, 0, 0, 0); /*start of generate in linked list*/
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                if (BOARD[i][j] != origBoard[i][j])
                    add(1, i, j, BOARD[i][j], origBoard[i][j]); /*maintain linked list*/
            }
        }
        add(3, -1, -1, -1, -1); /*end of generate in linked list*/
    }
    freeBoard(origBoard);
}

void autofill() {
    int i, j, cnt, num, validNum,setCount=0;
    int** tmpBoard;
    tmpBoard = initial_empty_board(ROW, COL);
    if (ERRONEOUS == 1)
        printf("The board is erroneous, command is not available\n");
    else {
        add(2, -1, -1, -1, -1); /*adding to linked list autofill move */
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                if (BOARD[i][j] == 0) {
                    cnt = 0;
                    for (num = 1; num < N + 1; num++) {
                        if (is_valid(BOARD, i, j, num) == 1) {
                            if (cnt == 0)
                                validNum = num;
                            cnt++;
                        }
                        if (cnt == 2)
                            break;
                    }
                    if (cnt == 1) { /*there's a single valid value*/
                        tmpBoard[i][j] = validNum;
                        FILLED_CELLS+=1;
                        setCount+=1;
                    }
                }
            }
        }
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                if (tmpBoard[i][j] > 0) {
                    BOARD[i][j] = tmpBoard[i][j];
                    printf("setting cell<%d,%d> to %d\n", j+1, i+1, tmpBoard[i][j]);
                    add(1, i, j, BOARD[i][j], 0); /*adding to linked list set move*/
                }
            }
        }
        if (setCount==0){
            printf("no cell with only one option to fill was found\n");
        }
        add(2, 0, 0, 0, 0); /*end of autofill*/
        updateErrorMAT();
        freeBoard(tmpBoard);
        print();
        
    }
}

void exitGame() {
    freeBoard(BOARD);
    printf("Exiting game...Bye girlll, don't forget you are a QUEEN!\n");
    exit(0);
}

void num_sol() {
    int num;
    int **b;
    if (all_func_gurobi(0, 0, "validate", 0) != 1) {
        printf("the board is not solvable\n");
        return;
    }

    b = copy_board(BOARD);
    num = EBT(b);
    printf("the current board has %d solutions\n", num);
}
