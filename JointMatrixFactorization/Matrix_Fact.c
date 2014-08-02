#include "algorithms.h"

/*
	Function: matrix_factorization
	-------------------------------
	NMF algorithm.

	Parameters:
	src - source contents
	valC - group number
 */
void matrix_factorization(Source *src, int size, int valC)
{
	for (int i = 0; i < size; ++i) {
		src[i].C = valC;
		joints_initialize(src);	// Initializes all joint matrices

		for (int j = 0; j < valC; ++j) {

		}
	}
}

/*
	Function: pos_matrix
	---------------------
	Internal function. First part of matrix factorized from 
	original matrix.

	Parameters:
	matrix - original matrix
	row - row number of matix
	col - column number of matrix

	Returns:
	factorized matrix
 */
double** pos_matrix(double **matrix, int row, int col) {
	double **temp = (double**)malloc(sizeof(double*) * row);

	for (int i = 0; i < row; ++i) {
		temp[i] = (double*)malloc(sizeof(double) * col);
		for (int j = 0; j < col; ++j) {
			temp[i][j] = (matrix[i][j] > 0) ? matrix[i][j] : 0;
		}
	}

	return temp;
}

/*
	Function: neg_matrix
	---------------------
	Internal function. Second part of matrix factorized from 
	original matrix.

	Parameters:
	matrix - original matrix
	row - row number of matrix
	col - column number of matrix

	Returns:
	factorized matrix
 */
double** neg_matrix(double **matrix, int row, int col) {
	double **temp = (double**)malloc(sizeof(double*)* row);

	for (int i = 0; i < row; ++i) {
		temp[i] = (double*)malloc(sizeof(double)* col);
		for (int j = 0; j < col; ++j) {
			temp[i][j] = (matrix[i][j] > 0) ? 0 : matrix[i][j];
		}
	}

	return temp;
}