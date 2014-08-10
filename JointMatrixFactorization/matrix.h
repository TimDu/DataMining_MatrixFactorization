#ifndef MATRIX_H_
#define MATRIX_H_

#define SMALL_DIM 64

double** multiply(double **a, int rA, double **b, int cB, int mid);
double** sum(double **a, double **b, int r, int c);
double** sub(double **a, double **b, int r, int c);
double** transpose(double **a, int r, int c);
double norm2(double **a, int r, int c);
double norm(double **a, int r, int c);

#endif