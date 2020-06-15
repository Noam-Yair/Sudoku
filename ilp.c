#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "gurobi_c.h"
#include "commands_imp.h"
#include "parser.h"
#include "ilp.h"

/*Gurobi implemntation for generate, validate, hint, guess and guess_hint*/

void free_all(int*** objMat, double* sol, int* ind, double* val, double* obj, char* vtype, double* ub) {
    int i, j;
    free(sol);
    free(ind);
    free(val);
    free(obj);
    free(vtype);
    free(ub);
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            free(objMat[i][j]);
        }
        free(objMat[i]);
    }
    free(objMat);
     
}

int* legal_val(int** arr, int row, int col) {  /*checking if num is valid in BOARD[col][row]*/
    int num;
    int arraySize = N + 1;
    int* legal_arr;
    legal_arr= malloc(arraySize * sizeof(int));
    if (arr[row][col]>=0) {
        legal_arr[0] = 0;
        for (num = 1; num <= N; num++) {
            if (is_valid(arr, row, col, num) == 1) { /*num is valid in cell*/
                legal_arr[num] = 1;
            }
            else
                legal_arr[num] = 0;
        }
        return legal_arr;
    }

    else
        return NULL;
}

void initial3Dmat(int*** retMat) {
    int i, j, k, size = N+1;
    for (i = 0; i < N; ++i)
        retMat[i] = (int**)malloc(N* sizeof(int*));
    for (i = 0; i <N; i++) {
        for (j = 0; j <N; j++) {
            retMat[i][j] = (int*)malloc(size*sizeof(int));
            for (k = 0; k <N + 1; k++) {
                retMat[i][j][k] = -1;
            }
        }
    }
}

int*** all_val(int** arr) {
    int*** retArr;
    int* tmp;
    int tmpSize =N+1;
    int i, j,k, count=0;
    retArr = (int***)malloc(N * sizeof(int**));
    initial3Dmat(retArr); 
    tmp = malloc(tmpSize * sizeof(int));
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            if (arr[i][j] >= 0) {   /*cell is not fixed*/
                tmp = legal_val(arr, i, j);
                for (k = 1; k < N+1; k++) {
                    if (tmp[k] == 1) /*the value is valid*/
                        retArr[i][j][k] = count++;   /*count represents x_ijk*/
                }
               
            }
        }
    }
    /*free(tmp);*/
    return retArr; 
}
int all_val_size(int*** arr) {
    int size = 0;
    int i, j, k;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            for (k = 0; k < N + 1; k++) {
                if (arr[i][j][k] >= 0)
                    size += 1;
            }
        }
    }
    return size;
}

int all_func_gurobi(int row, int col, char* func, float threshold) {
    int i, k, j, count = 0, randNum, row_block, col_block, i_row, j_col;
    GRBenv* env = NULL;
    GRBmodel* model = NULL;
    int       error = 0;
    double* sol;
    int* ind;
    double* val;
    double* obj;
    char* vtype;
    int       optimstatus;
    double    objval;
    int*** objMat;
    int object_size;
    double* ub;
    int intlp=0;
    

    objMat = (int***)malloc(N * sizeof(int**));
    objMat = all_val(BOARD);
    object_size = all_val_size(objMat) ;

    vtype = (char*)calloc(object_size, sizeof(char));
    val = (double*)calloc(N, sizeof(double));
    ind = (int*)calloc(N, sizeof(int));
    sol = (double*)calloc(object_size, sizeof(double));
    obj = (double*)calloc(object_size, sizeof(double));
    ub = (double*)calloc(object_size, sizeof(double));


    if (object_size == 0) {
        printf("there aren't any valid values\n");
        free_all(objMat, sol, ind, val, obj, vtype, ub);
        return -1;
    }


    /* Create environment - log file is mip1.log */
    error = GRBloadenv(&env, "mip1.log");
    if (error) {
        printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
        free_all(objMat, sol, ind, val, obj, vtype, ub);
        return -1;
    }

    error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if (error) {
        printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
        free_all(objMat, sol, ind, val, obj, vtype, ub);
        return -1;
    }

    /* Create an empty model named "mip1" */
    error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
    if (error) {
        printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
        free_all(objMat, sol, ind, val, obj, vtype, ub);
        return -1;
    }

    /* Add variables */

    /* coefficients - for x,y,z (cells 0,1,2 in "obj") */
    if (strcmp(func, "validate")==0 || strcmp(func, "generate")==0 || strcmp(func, "hint")==0) { /*need to check all options!!*/
        intlp = 1;
    }
    else if (strcmp(func, "guess")==0 || strcmp(func, "guess_hint") == 0) /*need to check all options!!*/
        intlp = 0;

    srand(time(NULL));
    for (i = 0; i < object_size; i++) {
        if (intlp == 1) { /*ILP*/
            vtype[i] = GRB_BINARY;
            obj[i] = 0;
        }
        else{ /*LP*/
        vtype[i] = GRB_CONTINUOUS;
        randNum = (rand() % N) + 1;
        obj[i] = randNum;
        ub[i] = 1.0;
        
        }
    }
  

    /* add variables to model */
    if (intlp == 1) {
        error = GRBaddvars(model, object_size, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
    }
    else {
        error = GRBaddvars(model, object_size, 0, NULL, NULL, NULL, obj, NULL, ub, vtype, NULL);
    }
    if (error) {
        printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
        free_all(objMat, sol, ind, val, obj, vtype, ub);
        return -1;
    }

    /* Change objective sense to maximization */
    error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
    if (error) {
        printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
        free_all(objMat, sol, ind, val, obj, vtype, ub);
        return -1;
    }

    /* update the model - to integrate new variables */

    error = GRBupdatemodel(model);
    if (error) {
        printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
        free_all(objMat, sol, ind, val, obj, vtype, ub);
        return -1;
    }



    /* add constraint to model*/
   /* sum of values in cell is 1 */
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            count = 0;
            for (k = 1; k < N + 1; k++) {
                if (objMat[i][j][k] >= 0) {
                    ind[count] = objMat[i][j][k];
                    val[count] = 1.0;
                    count += 1;
                }
            }
            if (count > 0) {
                error = GRBaddconstr(model, count, ind, val, GRB_EQUAL, 1, "c0");
                if (error) {
                    printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                    free_all(objMat, sol, ind, val, obj, vtype, ub);
                    return -1;
                }
            }
        }
    }
    /* add constraint to model*/
    /*sum of values in row is 1*/
    for (k = 1; k < N + 1; k++) {
        for (i = 0; i < N; i++) {
            count = 0;
            for (j = 0; j < N; j++) {
                if (objMat[i][j][k] >= 0) {
                    ind[count] = objMat[i][j][k];
                    val[count] = 1.0;
                    count += 1;
                }
            }
            if (count > 0) {
                error = GRBaddconstr(model, count, ind, val, GRB_EQUAL, 1, "c0");
                if (error) {
                    printf("ERROR %d 2nd GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                    free_all(objMat, sol, ind, val, obj, vtype, ub);
                    return -1;
                }
            }
        }
    }
    /* add constraint to model*/
    /*sum of values in column is 1 */
    for (k = 1; k < N + 1; k++) {
        for (j = 0; j < N; j++) {
            count = 0;
            for (i = 0; i < N; i++) {
                if (objMat[i][j][k] >= 0) {
                    ind[count] = objMat[i][j][k];
                    val[count] = 1.0;
                    count += 1;
                }
            }
            if (count > 0) {
                error = GRBaddconstr(model, count, ind, val, GRB_EQUAL, 1, "c0");
                if (error) {
                    printf("ERROR %d 3rd GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                    free_all(objMat, sol, ind, val, obj, vtype, ub);
                    return -1;
                }
            }
        }
    }
    /* add constraint to model*/
    /* sum of values in block is 1 */
    for (k = 1; k < N + 1; k++) {
        for (i = 0; i < N; i = i + ROW) {
            for (j = 0; j < N; j = j + COL) {
                row_block = (int)i / ROW;
                col_block = (int)j / COL;
                count = 0;
                for (i_row = (ROW * row_block); i_row < (ROW * (row_block + 1)); i_row++) {
                    for (j_col = (COL * col_block); j_col < (COL * (col_block + 1)); j_col++) {
                        if (objMat[i_row][j_col][k] >= 0) {
                            ind[count] = objMat[i_row][j_col][k];
                            val[count] = 1.0;
                            count++;
                        }
                    }
                }
                if (count > 0) {
                    error = GRBaddconstr(model, count, ind, val, GRB_EQUAL, 1.0, NULL);
                    if (error) {
                        printf("ERROR %d 4th GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
                        free_all(objMat, sol, ind, val, obj, vtype, ub);
                        return -1;
                    }
                }
            }
        }
    }

    /* Optimize model - need to call this before calculation */
    error = GRBoptimize(model);
    if (error) {
        printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
        free_all(objMat, sol, ind, val, obj, vtype, ub);
        return -1;
    }

    /* Write model to 'mip1.lp' - this is not necessary but very helpful */
    error = GRBwrite(model, "mip1.lp");
    if (error) {
        printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
        free_all(objMat, sol, ind, val, obj, vtype, ub);
        return -1;
    }

    /* Get solution information */

    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if (error) {
        printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
        free_all(objMat, sol, ind, val, obj, vtype, ub);
        return -1;
    }

    /* solution found */
    if (optimstatus == GRB_OPTIMAL) {
        /* get the objective -- the optimal result of the function */
        error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
        if (error) {
            printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
            free_all(objMat, sol, ind, val, obj, vtype, ub);
            return -1;
        }

        /* get the solution - the assignment to each variable */
        error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, object_size, sol);
        if (error) {
            printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
            free_all(objMat, sol, ind, val, obj, vtype, ub);
            return -1;
        }

        if (strcmp(func, "hint") == 0) {
            for (k = 1; k < N + 1; k++) {
                if (objMat[row][col][k] != 0) {
                    if (sol[objMat[row][col][k]] == 1) {
                        printf("Hint: cell <%d,%d> = %d\n", col + 1, row + 1, k);
                    }
                }
            }
        }
        else if (strcmp(func, "generate") == 0) {
            fillBoard(objMat, sol);
        }

        else if (strcmp(func, "guess_hint") == 0) {
            printf("The legal values for cell<%d,%d> are: ", col + 1, row + 1);
            for (k = 1; k < N + 1; k++) {
                if (objMat[row][col][k] != 0) {
                    if (sol[objMat[row][col][k]] > 0) {
                        printf("value = %d, score = %.2f ", k, sol[objMat[row][col][k]]);
                    }
                }
            }
            printf("\n");
        }

        else if (strcmp(func, "guess") == 0) {
            fillGuessedBoard(objMat, sol, threshold);
        }

    }

        /* no solution found */
        else if (optimstatus == 3 || optimstatus == 5) {
            if (strcmp(func, "generate") != 0)
                printf("Model is infeasible or unbounded\n");
            GRBfreemodel(model);
            GRBfreeenv(env);

            return 0;
        }
        /* error or calculation stopped */
        else {
            if (strcmp(func, "generate") != 0)
                printf("Optimization was stopped early\n");

            free_all(objMat, sol, ind, val, obj, vtype, ub);
            GRBfreemodel(model);
            GRBfreeenv(env);

            return 0;
        }

        /* IMPORTANT !!! - Free model and environment */
        free_all(objMat, sol, ind, val, obj, vtype, ub);
        GRBfreemodel(model);
        GRBfreeenv(env);

        return 1;
    }


    

   