/*stack implementation and backtracking function for num_sols*/


struct Stack {
    int top;
    int capacity;
    int* array;
};
void end_stack(struct Stack* stack);
int pop(struct Stack* stack);
void push(struct Stack* stack, int item);
int empty(struct Stack* stack);
int isFull(struct Stack* stack);
struct Stack* createStack(int capacity);
int* find_empty_cell(int**board);
int board_is_full(int**board);
int EBT(int ** board);
void free_stack(struct Stack *stack);
