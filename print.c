#include <stdio.h>
#include <stdlib.h>
#include "commands_imp.h"

/*functions to print current board*/

void separator_row(){
    int i;
    for (i=0;i<4*N+ROW+1;i++){
        printf("-");
    }
}
void cell_separator(){
    printf("|");
}
void print() {
    int i, j;
    separator_row();
    for (i=0;i<N;i++){
        if (i % (ROW) == 0 && i != 0 && i != N) {
            printf("\n");
            separator_row();
        }
        for (j=0;j<N;j++){
            if (j==0&&j!=N){
                printf("\n");
            }
            if (j%COL==0){
                printf("|");
                if (BOARD[i][j] < 0) {
                    printf(" %2d.", -BOARD[i][j]);
                }
                else if (((MODE==2&&ERRORMAT[i][j]==1)||(MARK_ERRORS==1&&ERRORMAT[i][j]==1))&&BOARD[i][j]!=0){
                    printf(" %2d*", BOARD[i][j]);
                }
                else if (BOARD[i][j] == 0) {
                    printf("    ");
                }
                else {
                    printf(" %2d ", BOARD[i][j]);
                }
            }
            else if (BOARD[i][j] < 0) {
                printf(" %2d.", -BOARD[i][j]);
            }
            else if (((MODE==2&&ERRORMAT[i][j]==1)||(MARK_ERRORS==1&&ERRORMAT[i][j]==1))&&BOARD[i][j]!=0){
                printf(" %2d*", BOARD[i][j]);
            }
            else if (BOARD[i][j] == 0) {
                printf("    ");
            }
            else {
                printf(" %2d ", BOARD[i][j]);
            }
        }
        cell_separator();
    }
    printf("\n");
    separator_row();
    printf("\n");

}
