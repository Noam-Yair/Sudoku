
/*implemtation of doubly linked list using nodes
each node consists the fields of row,column, previous value - before the change, current value - after the change
the list has pointers to the head node, the last node and the current move
all commands that use the list: undo, redo and reset are also implemented*/

#pragma once
extern struct node* head;
extern struct node* last;
extern struct node* curr_move;


void printErrorMat();
void initialDDL();
void freeDDL();
int isEmpty();
void add(int move, int r, int c, int currVal, int prevVal);
void updateErrorCell(int row, int col, int val);
void allMoves(char* func, int reset);
void undo(int reset);
void redo();

void reset();
