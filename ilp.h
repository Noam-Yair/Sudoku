/*Gurobi implemntation for generate, validate, hint, guess and guess_hint*/

#pragma once
void free_all(int*** objMat, double* sol, int* ind, double* val, double* obj, char* vtype, double* ub);
int* legal_val(int** arr, int row, int col);
void initial3Dmat(int*** retMat);
int*** all_val(int** arr);
int all_val_size(int*** arr);
int all_func_gurobi(int row, int col, char* func, float threshold);