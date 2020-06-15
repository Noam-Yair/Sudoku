
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "commands_imp.h"
#include "parser.h"
#include "doubly_linked_list.h"
#include "stack.h"
#include "print.h"

/*implementation of processing the user's input
calling the functions according to the input, if was entered right*/

/*char commands_arr[20][15] = { "solve","edit","mark_errors","print_board","set","validate","guess","generate","undo",
 "redo","save","hint","guess_hint", "num_solution","autofill","reset","exit"};*/



void print_MNC(){
    printf("the current mode is not compatible for this command\n");
}

int check(char* arr[3], int ind) {
    int i, x,j;
    char tmp;
    char * c;
    if (ind == 5) {
        for (i = 0; i < 3; i++) {
            c = arr[i];
            j = 0;
            tmp = c[j];
            while (*c) {
                if (tmp < '0' || tmp > '9') {
                    return 0;
                }
                c++;
            }
            if (i == 0 || i == 1) {
                sscanf(arr[i], "%d", &x);
                if (x <= 0 || x> N) {
                    return 1;
                }
            }
            else if (i == 2) {
                sscanf(arr[i], "%d", &x);
                if (x < 0 || x > N) {
                    return 1;
                }
            }
        }
    }
    if (ind==3){
	c = arr[0];
            j = 0;
            tmp = c[j];
            while (*c) {
                if (tmp < '0' || tmp > '9') {
                    return 0;
                }
                c++;
            }
	sscanf(arr[0], "%d", &x);
	if (x!=0 && x!=1){
		return 1;
	}
	 }
    else {
        for (i = 0; i < 2; i++) {
            char* c = arr[i];
            int j = 0;
            char tmp = c[j];
            while (*c) {
                if (tmp < '0' || tmp > '9') {
                    return 0;
                }
                c++;
            }

            sscanf(arr[i], "%d", &x);
            if (x <= 0 || x > N) {
                return 1;

            }
        }
    }
    return 2;
}



int check_parameter(char* arr[3], int ind) {/*returns 0=not an int, returns 1=not in the right range, returns 2=OK */
    if (ind == 5) {
        return check(arr, 5);
    }

   if (ind==3){
	return check(arr,3);
}
    else {
        return check(arr, 8);
    }


}
void exe_func(int ind,  char* arr[3], int edit_flag) { /*mode==0 ->init mode==1 ->solve mode==2 ->edit*/
    int x,i;
    float f;
    int int_arr[3]={0};
    printf("\n");
    switch (ind+1) {
        case 1:/*solve*/
            solve(arr[0]);
            break;
        case 2:/*edit*/
	    if (edit_flag==1){
		edit_noPath();
		break;
}
            edit(arr[0]);
            break;

        case 3: /*mark_errors*/
            if (MODE != 1) {
                print_MNC();
                break;
            }
	    if (check_parameter(arr,3)==0){
                printf("please enter variables of type int\n");
                break;
            }
            else if (check_parameter(arr,3)==1){
                printf("variables are not in the right range\n");
                break;
            }
            sscanf(arr[0], "%d", &x);
            markErrors(x);
            break;
        case 4: /*print_board*/
            if (MODE == 0) {
                print_MNC();
                break;
            }
            else {
                print_board();
                break;
            }
        case 5: /*set*/
            if (MODE==0){
                print_MNC();
                break;
            }
            else if (check_parameter(arr,5)==0){
                printf("please enter variables of type int\n");
                break;
            }
            else if (check_parameter(arr,5)==1){
                printf("variables are not in the right range\n");
                break;
            }
            for (i=0;i<3;i++){
                sscanf(arr[i], "%d", &x);
                int_arr[i]=x;
            }
            set(int_arr[0],int_arr[1],int_arr[2]);
            break;
        case 6: /*validate*/
            if (MODE==0){
                print_MNC();
                break;
            }
            validate();
            break;
        case 7: /*guess*/
            if (MODE!=1){
                print_MNC();
                break;
            }
            sscanf(arr[0], "%f", &f);
            guess(f);
            break;
        case 8: /*generate*/
            if (MODE!=2){
                print_MNC();
                break;
            }
            else if (check_parameter(arr,8)==0){
                printf("please enter variables of type int\n");
                break;
            }
            else if (check_parameter(arr,8)==1){
                printf("variables are not in the right range\n");
                break;
            }
            for (i=0;i<2;i++){
                sscanf(arr[i], "%d", &x);
                int_arr[i]=x;
            }
            generate(int_arr[0],int_arr[1]);
            break;
        case 9: /*undo*/
            if (MODE==0){
                print_MNC();
                break;
            }
            undo(0);
            break;
        case 10: /*redo*/
            if (MODE==0){
                print_MNC();
                break;
            }
            redo();
            break;
        case 11: /*save*/
            if (MODE==0){
                print_MNC();
                break;
            }
            Save(arr[0]);
            break;
        case 12: /*hint*/
            if (MODE!=1){
                print_MNC();
                break;
            }
            else if (check_parameter(arr,12)==0){
                printf("please enter variables of type int\n");
                break;
            }
            else if (check_parameter(arr,12)==1){
                printf("variables are not in the right range\n");
                break;
            }
            for (i=0;i<2;i++){
                sscanf(arr[i], "%d", &x);
                int_arr[i]=x;
            }
            hintAndGuess_hint(int_arr[0],int_arr[1], "hint");
            break;
        case 13: /*guess_hint*/
            if (MODE!=1){
                print_MNC();
                break;
            }
            else if (check_parameter(arr,13)==0){
                printf("please enter variables of type int\n");
                break;
            }
            else if (check_parameter(arr,13)==1){
                printf("variables are not in the right range\n");
                break;
            }
            for (i=0;i<2;i++){
                sscanf(arr[i], "%d", &x);
                int_arr[i]=x;
            }
            hintAndGuess_hint(int_arr[0],int_arr[1],"guess_hint");
            break;
        case 14: /*num_solutions*/
            if (MODE==0){
                print_MNC();
                break;
            }
            num_sol();
            break;
        case 15: /*auto_fill*/
            if (MODE!=1){
                print_MNC();
                break;
            }
            autofill();
            break;
        case 16: /*reset*/
            if (MODE==0){
                print_MNC();
                break;
            }
            reset();
            break;
        case 17: /*exit*/
            exitGame();
            break;

    }

}


void initial() {
    char str[258];
    str[256]='\0';
    printf("Please enter a command:");
    if (board_is_full(BOARD)&&MODE==1){
        print();
        updateErrorMAT();
        if (ERRONEOUS==0){
            printf("you are AWESOME! you successfully solved the board!\n");
            MODE=0;
        }
        else{
            printf("board is erroneous, try again!\n");
        }
    }
    while (fgets(str,1025,stdin)){
        char* str_split;
        char* arr_command[3];
        int counter=0;
        int i;
        int var_arr[17]={1,1,1,0,3,0,1,2,0,0,1,2,2,0,0,0,0};
        char commands_arr[17][15]={ "solve","edit","mark_errors","print_board","set","validate","guess","generate","undo","redo","save","hint","guess_hint", "num_solutions","autofill","reset","exit" };
      if(str[256]!='\0'){
		printf("you entered too many characters. Exiting...\n");
		return;
	}
	 str_split = strtok(str, " \t\r\n");
        while (str_split!=NULL) {
            i=0;
            for (i = 0; i < 17; i++) {
                if (strcmp(str_split, commands_arr[i]) == 0) {
                    break;
                }
            }
            if (i==17){
                printf("Error: invalid command\n");
                break;
            }
            while (str_split != NULL) {
                str_split = strtok(NULL, " \t\r\n");
                if (counter>var_arr[i]){
                    printf("Error: this command requires %d parameters\n",var_arr[i]);
                    break;
                }
                if (str_split == NULL) {
                    break;
                }
                arr_command[counter] = str_split;
                counter++;

            }

            if (counter<var_arr[i]&&i!=1) {
                printf("Error: this command requires %d parameters\n", var_arr[i]);
                break;
            }

            else if (counter==var_arr[i]||(counter==0&&i==1)) {
                if (counter==var_arr[i]){
                    exe_func(i, arr_command,0);
                }
                else{
                    exe_func(i,arr_command,1);

                }
            }

        }
        if (board_is_full(BOARD)&&MODE==1){
            updateErrorMAT();
            if (ERRONEOUS==0){
                printf("you are AWESOME! you successfully solved the board!\n");
                MODE=0;
            }
            else{
                printf("board is erroneous, try again!\n");
            }
        }
        printf("Please enter a command:");



    }
    exitGame();

}







