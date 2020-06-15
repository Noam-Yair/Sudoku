
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "commands_imp.h"
#include "parser.h"
#include "stack.h"
#include "print.h"

/*stack implementation and backtracking function for num_sols*/

struct Stack* createStack(int capacity)
{
    struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (int*)malloc(stack->capacity * sizeof(int));
    return stack;
}

int isFull(struct Stack* stack)
{
    return stack->top == stack->capacity - 1;
}

int empty(struct Stack* stack)
{
    return stack->top == -1;
}

void push(struct Stack* stack, int item)
{
    if (isFull(stack))
        return;
    stack->top++;
    stack->array[stack->top] = item;
}

int pop(struct Stack* stack)
{
    if (empty(stack))
        return -1;
    return stack->array[stack->top--];
}

int* find_empty_cell(int**board) {
    int cell[2] = { -1,-1 };
    int i, j;
    int *ret=cell;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (board[i][j] == 0){
                cell[0] = i;
                cell[1] = j;
                ret=cell;
                return ret;
            }
        }
    }
    return ret;
}

int board_is_full(int**board) {
    int i , j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (board[i][j] == 0)
                return 0;
        }
    }
    return 1;
}

void free_stack(struct Stack *stack){
    free(stack->array);
    free(stack);
}

int EBT(int ** board) {
    struct Stack *val = createStack(N * N);
    struct Stack *ind = createStack(N * N);
    int i, j, row, col, counter=0, r, c, degel = 0, index=0, value,start=0;/*degel=0 means going forward*/
    int *cell;
    if (board_is_full(board)){
        return 0;
    }
    while (ind->top!=-1||start==0) {
        start=1;
        if (board_is_full(board)) {
            counter++;
            degel=1;
        }
        if (degel==0){
            cell = find_empty_cell(board); /*cell is an array [row,col] of the next empty cell*/
            row = cell[0];
            col = cell[1];
            for (i = 1; i < N + 1; i++) {
                if (is_valid(board, row, col, i) == 1) {
                    board[row][col]=i;
                    push(val, i);
                    push(ind, ((row) * (COL*ROW)) + col);
                    break;
                }
                else if (i == N) {
                    degel = 1;
                }
            }

        }
        if(degel==1){
            value = val->array[val->top];
            index = ind->array[ind->top];
            pop(val);
            r = index/N;
            c = index%N;
            j = value + 1;
            if (j>=N){
                if (j==N&&r==0&&c==0){
                    board[r][c]=j;
                    push(val,j);
                    degel=0;
                }
                else {
                    if(j==N){
                        if (is_valid(board, r, c, j) == 1) {
                            board[r][c] = j;
                            push(val, j);
                            degel = 0;
                        }
                        else{
                            board[r][c] = 0;
                            pop(ind);
                        }
                    }
                    else {
                        board[r][c] = 0;
                        pop(ind);
                    }
                }

            }
            else {
                for (j=j ; j < N + 1; j++) {
                    if (is_valid(board, r, c, j) == 1) {
                        board[r][c] = j;
                        push(val, j);
                        degel = 0;
                        break;
                    }
                    else if (j == N) {
                        pop(ind);
                        board[r][c] = 0;
                    }
                }
            }

        }
    }

    free_stack(val);
    free_stack(ind);

    return counter;
}
