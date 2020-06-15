#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*#pragma warning(disable : 4996)*/

#include "commands_imp.h"
#include "parser.h"
#include "ilp.h"
#include "doubly_linked_list.h"

/*helper functions for commands functions and the gurobi implementation*/

int is_valid(int** arr, int row, int col, int num) {  /*checking if num is valid in BOARD[col][row], RETURN 1 IF VALID*/
    int i, j;
    int N = ROW * COL;
    int row_block = (int)row / ROW;
    int col_block = (int)col / COL;
    for (i = 0; i < N; i++) { /* checks if the number is already in the same row*/

        if (i!= col && (arr[row][i] == num || arr[row][i] == -num)) {
            return 0;
        }
    }
    for (i = 0; i < N; i++) {            /* checks if the number is already in the same column*/
        if (i != row && (arr[i][col] == num || arr[i][col] == -num)) {
            return 0;
        }
    }
    for (i = (ROW * row_block); i < ROW * (row_block + 1); i++) {  /* checks if the number is already in the same block*/
        for (j = (COL * col_block); j < (COL * (col_block + 1)); j++) {

            if ((i!=row && j!=col) && (arr[i][j] == num || arr[i][j] == -num)) {
                return 0;
            }
        }
    }

    return 1;
}

int FileToArray(char* path) { /* returns 1 if no errors ocurred*/
    
    FILE* convertFile;
    int** arr;
    int N, i = 0, j = 0;
    char curr;
    int x, k, ret;
    int invalid = 0;

   convertFile = fopen(path, "r");
    if (convertFile == NULL) {
        printf("Error in loading file, check path\n");
        return 0;
    }
    /*first line is "m n" n=rows, m=col*/
    COL = 0;
    ROW = 0;
    FILLED_CELLS = 0;
    while (COL == 0) {
        curr = (char)fgetc(convertFile);
        if (feof(convertFile)) {
            invalid = 1;
            break;
        }
            
        while (curr < 49 || curr>57) {
            curr = (char)fgetc(convertFile);
            if (feof(convertFile)) {
                invalid = 1;
                break;
            }
        }
        if (curr > 48 && curr <= 57) {
            x = curr - '0';
            if (ROW == 0)
                ROW = x;
            else
                COL = x;
        }
        
    }
    if (invalid == 1 || ROW * COL > MAX) {
        printf("Error in dimensions values\n");
        return 0;
    }
    N = ROW * COL;
    BOARD = initial_empty_board(ROW, COL);  
    arr = (int**)malloc(N * sizeof(int*));
    for (k= 0; k < N; k++)
        arr[k] = (int*)malloc(N * sizeof(int));

    while (i<N && curr!=EOF && invalid==0) {
        if (feof(convertFile)) {
            invalid = 1;
            break;
        }
        curr = (char)fgetc(convertFile);
        while (j<N && curr != EOF) {
            while (curr != 46 && (curr < 48 || curr>57)) { /*curr is not a dot and not a number*/
                curr = (char)fgetc(convertFile);
                if (feof(convertFile)) {
                    invalid = 1;
                    break;
                }
            }
            if (curr >= 48 && curr < 58) { /*curr is a number */
                x = curr - '0';
                if (x > N) {   /*if number isn't in range*/
                    invalid = 2;
                    break;
                }
                curr = (char)fgetc(convertFile);
                if (curr == 46)
                    arr[i][j] = -1 * x;
                else if (curr >= 48 && curr < 58) { /*the number has two digits*/
                    x = (x*10) + (curr - '0');
                    if (x <= N) {
                        curr = (char)fgetc(convertFile);
                        if (curr == 46) {
                            arr[i][j] = -1 * x;
                        }
                        else
                            arr[i][j] = x;
                    }
                    else { /*the number is greater than N*/
                        invalid = 2;
                        break;
                    }
                }
                else
                    arr[i][j] = x;   
                if (arr[i][j] != 0)
                    FILLED_CELLS += 1;
                j += 1;
                
            }

            if (j==N) { /*if scanned all numbers in a line*/
                i += 1;
                j = 0;
                if (i==N)
                    break;
            }
            curr = (char)fgetc(convertFile);
            if (feof(convertFile)) {
                invalid = 1;
                break;
            }
        }
    }
    if (invalid==1) {/*numbers missing in file*/        
        printf("The file has missing values, please provide a different file\n");
        ret = 0; 
    }
    else if (invalid == 2) {
        printf("values are not in range, please change to range %d-%d\n", 1, N);
        ret = 0;
    }
    else {
        BOARD = copy_board(arr);
        ret =  1;
    }
    freeBoard(arr);
    fclose(convertFile);
    return ret;
}


int** copy_board(int** board) {
    int i, j;
    int** copy;
    copy = (int**)malloc(N * sizeof(int*));
    for (i = 0; i < N; i++)
        copy[i] = (int*)malloc(N * sizeof(int));
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            copy[i][j] = board[i][j];
        }
    }
    return copy; 
}

int** all_cell_fixed(int** arr) {
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (arr[i][j] > 0) {
                arr[i][j] = -arr[i][j];
            }
        }
    }
    return arr;
}

void ArrayToFile(char* path) {
    FILE* targetFile;
    int** arr;
    int i, j;
    targetFile = fopen(path, "w");
    if (targetFile == NULL)
        printf("Error in path\n"); /*�������*/
    else {
        arr = copy_board(BOARD);
        if (MODE == 1) { /*edit mode-> all cells are saved as fixed*/
            arr = all_cell_fixed(arr);
        }

        fprintf(targetFile, "%d %d\n", COL, ROW); /*first line is "m n" n=rows, m=col*/

        for (i = 0; i < N; i++) {
            for (j = 0; j < N - 1; j++) {
                if (arr[i][j] < 0) {
                    fprintf(targetFile, "%d. ", -arr[i][j]);
                }
                else
                    fprintf(targetFile, "%d ", arr[i][j]);
            }
            if (arr[i][j] < 0) {
                fprintf(targetFile, "%d.\n", -arr[i][j]);
            }
            else
                fprintf(targetFile, "%d\n", arr[i][j]);
        }
	printf("board saved\n");
        free(arr);
        fclose(targetFile);
    }
    
}

int is_fixed(int row, int col) { /* 1 if cell is fixed*/
    if (BOARD[row][col] < 0)
        return 1;
    return 0;
}

int validFixed() {
    int i, j;
    int** onlyFixed;
    onlyFixed = initial_empty_board(ROW, COL);
    onlyFixed = copy_board(BOARD);
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) { /*creating board with only fixed cells*/
            if (BOARD[i][j] > 0) 
                onlyFixed[i][j] = 0;
        }
    }
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (BOARD[i][j] < 0) {
                if (is_valid(onlyFixed, i, j, BOARD[i][j])==0) {
                    return 0;
                }
            }
        }
    }
    freeBoard(onlyFixed);
    return 1;
}

void updateErrorMAT() {
    int i, j;
    ERRONEOUS = 0;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (BOARD[i][j] > 0 && is_valid(BOARD, i, j, BOARD[i][j]) == 0) { /*the value isn't valid*/

                ERRORMAT[i][j] = 1;
                ERRONEOUS = 1;
            }
else {
                ERRORMAT[i][j] = 0;   
        }
        }
    }
}
void fillBoard(int*** gurobiMat, double* sol) {
    int i, j, k = 0;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (BOARD[i][j] == 0) {
                for (k = 1; k < N+1; k++) {
                    if (gurobiMat[i][j][k] >= 0 && sol[gurobiMat[i][j][k]] == 1)
                        BOARD[i][j] = k;

                }

            }
            if (BOARD[i][j] < 0)
                BOARD[i][j] = -BOARD[i][j];

        }
    }
}

void noFixed() {
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (BOARD[i][j] < 0) {
                BOARD[i][j] = -BOARD[i][j];
            }
        }
    }
}

void freeBoard(int** arr) {
    int i;
    for (i = 0; i < N; i++) {
        free(arr[i]);
    }
    free(arr);
}






void fillGuessedBoard(int*** objMat, double* sol, double threshold) {
    int i, j, k, randRange=0, randNum;
    double* scores;
    
    scores = malloc(N * sizeof(double));
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            for (k = 1; k < N + 1; k++) {
                scores[k] = -1;
            }
            randRange = 0;

            for (k = 1; k < N + 1; k++) {
                if (objMat[i][j][k] != -1 && sol[objMat[i][j][k]] > threshold && is_valid(BOARD, i, j, k)==1) {
                    scores[k] = sol[objMat[i][j][k]];
                    randRange += (int)(scores[k] * 10);
                    scores[k] = randRange;
                }
            }

            randNum = rand() % (randRange + 1);
            for (k = 1; k < N + 1; k++) {
                if (scores[k] >= randNum) {
                    add(1, i, j, k, BOARD[i][j]);
                    BOARD[i][j] = k;
                    break;
                }
            }
        }
    }
    free(scores);
}




