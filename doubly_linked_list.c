
#include "print.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands_imp.h"
#include "parser.h"


/*implemtation of doubly linked list using nodes
each node consists the fields of row,column, previous value - before the change, current value - after the change
the list has pointers to the head node, the last node and the current move
all commands that use the list: undo, redo and reset are also implemented*/

struct node {
    int move;    /*1 for set, 2 for autofill, 3 for generate, 4 for guess*/
    int row, col,currVal, prevVal;
    struct node* next;
    struct node* prev;
};


struct node* head;
struct node* last;
struct node* curr_move;
int** initialBoard;

void initialDDL() {
    head = (struct node*)malloc(sizeof(struct node));
    last = (struct node*)malloc(sizeof(struct node));
    curr_move = (struct node*)malloc(sizeof(struct node));
    head->next = NULL;
    head->prev = NULL;
    last->next = NULL;
    last->prev = NULL;
    curr_move->next = NULL;
    curr_move->prev = NULL;
    curr_move = last;
    head=curr_move;
}

void freeDDL() {
    free(head);
    free(last);
    free(curr_move);
}
int isEmpty() {
    if (head->next == NULL)
        return 1;
    return 0;
}


void printErrorMat() {
    int i, j;
    printf("THIS IS ERRORMAT\n");
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++){
        printf("%d ", ERRORMAT[i][j]);
        }
        printf("\n");
    }
}


void add(int move, int r, int c, int currVal, int prevVal) {  /*r and c are for the row and column that changed*/
    struct node* new_move = (struct node*)malloc(sizeof(struct node));
    new_move->move = move;
    new_move->row = r;
    new_move->col = c;
    new_move->currVal = currVal;
    new_move->prevVal = prevVal;
    new_move->next = NULL;
    if (curr_move != last) {  /*if the adding is after redo*/
        while (curr_move != last) {
            last = last->prev;
            last->next = NULL;
        }
        }
    if (isEmpty() == 0) { /*the list is not empty*/
        new_move->prev = last;
        last->next = new_move;
    }
    else {    /*if list is empty the first and the last element are the same*/
        head->next = new_move;
        new_move->prev = head;
    }
    last = new_move;
    curr_move = new_move;



}

void updateErrorCell(int row, int col, int val) { 
    int i,j;
    int row_block = (int)row / ROW;
    int col_block = (int)col / COL;
   /* printErrorMat();*/ 
    if (val == 0)
        ERRORMAT[row][col] = 0;
    else if (is_valid(BOARD, row, col, val) == 0) {
        ERRORMAT[row][col] = 1;
        ERRONEOUS = 1;
    }
    else {
        ERRORMAT[row][col] = 0;
    }
    for (i = 0; i < N; i++) {
        if (BOARD[row][i] != 0) {
            if (is_valid(BOARD, row, i, BOARD[row][i]) == 0) {
                ERRORMAT[row][i] = 1;
                ERRONEOUS = 1;
            }
            else {
                ERRORMAT[row][i] = 0;
            }
        }
        if (BOARD[i][col] != 0) {
            if (is_valid(BOARD, i, col, BOARD[i][col]) == 0) {
                ERRORMAT[i][col] = 1;
                ERRONEOUS = 1;
            }
            else {
                ERRORMAT[row][i] = 0;
            }
        }
     }
    
    for (i = (ROW * row_block); i < ROW * (row_block + 1); i++) { 
        for (j = (COL * col_block); j < (COL * (col_block + 1)); j++) {
            if (BOARD[i][j] != 0){
                if (is_valid(BOARD, i, j, BOARD[i][j]) == 0) {
                    ERRORMAT[i][j] = 1;
                        ERRONEOUS = 1;
                }
                else {
                    ERRORMAT[i][j] = 0;
                }
            }
        }
    }

}

void allMoves(char* func, int reset) {
    /*1 for set, 2 for autofill, 3 for generate, 4 for guess*/
    int move, r, c;
    int currVal, prevVal;
    move = curr_move->move;
    r = curr_move->row;
    c = curr_move->col;
    currVal = curr_move->currVal;
    prevVal = curr_move->prevVal;
    
    if (move == 1) { /*set*/
        if (reset == 0)
            printf("%s set %d in cell<%d,%d>\n", func, currVal, c + 1, r + 1);
        if (strcmp(func, "undo") == 0) {
            BOARD[r][c] = prevVal;
            updateErrorCell(r, c, prevVal);
        }
        else {
            BOARD[r][c] = currVal;
            updateErrorCell(r, c, currVal);
        }
    }
    else if (move == 2 || move == 3 || move == 4) { /*autofill and generate*/
        if (move == 2 && reset == 0)
            printf("%s autofill\n", func);
        else if (move == 3 && reset == 0)
            printf("%s generate\n", func);
        else if (move == 4 && reset == 0)
            printf("%s guess\n", func);
        if (strcmp(func, "undo") == 0) {
            curr_move = curr_move->prev;
            r = curr_move->row;
            c = curr_move->col;
            while (curr_move->move != move) {
                BOARD[r][c] = curr_move->prevVal;
                updateErrorCell(r, c, curr_move->prevVal);
                curr_move = curr_move->prev;
                r = curr_move->row;
                c = curr_move->col;
            }
        }
        else if (strcmp(func, "redo") == 0) {
            curr_move = curr_move->next;
            r = curr_move->row;
            c = curr_move->col;
            while (curr_move->move != move) {
                BOARD[r][c] = curr_move->currVal;
                updateErrorCell(r, c, curr_move->currVal);
                curr_move = curr_move->next;
                r = curr_move->row;
                c = curr_move->col;
            }
        }
    }
    
}


void undo(int reset) {
    if (curr_move != head) {
        allMoves("undo",reset);
        curr_move = curr_move->prev;
        if (reset==0){
	updateErrorMAT();
         print();
}

    }
    else if (reset==0){
            printf("unable to undo\n");
	    return;
    
}
}



void redo() {
    if (curr_move->next != NULL) {
        curr_move = curr_move->next;
        allMoves("redo",0); 
	updateErrorMAT();
        print();
	return;
    }
       printf("unable to redo\n");
	return;
    
}




void reset() {
    while (curr_move != head) {
        undo(1);
    }
    printf("resetting board\n");
    print();
    updateErrorMAT();
    }

