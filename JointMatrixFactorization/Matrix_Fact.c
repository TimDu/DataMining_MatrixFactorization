#include "algorithms.h"
#include <stdlib.h>

void _initialize(Source *src, int size);
double** _pos_matrix(double**, int, int);
double** _neg_matrix(double**, int, int);

/*
	Function: matrix_factorization
	-------------------------------
	NMF algorithm.

	Parameters:
	src - source contents
	size - number of sources
	valC - group number
 */
void matrix_factorization(Source *src, int size, int valC)
{
	for (int i = 0; i < size; ++i) {
		src[i].C = valC;

		for (int j = 0; j < valC; ++j) {

		}
	}
}

/*
	FUnction: _initialize
	---------------------
	Internal function.
	Initializes joint matrix values. Also rescales data range.

	Parameters:
	src - source structure
	size - number of sources
 */
void _initialize(Source *src, int size)
{
	double iniValue = 1 / src->C;

	for (int n = 0; n < size; ++n) {
		for (int i = 0; i < src[n].N; ++i) {
			for (int j = 0; j < src[n].K; ++j) {
				double tempV = src[n].V[i][j];
				if (j < src[n].C) {
					// User group matrix
					src[n].W[i][j] = iniValue;
				}
				if (i < src[n].C) {
					// Item matrix
					src[n].H[i][j] = 1 / 2;
				}
				if (!tempV) {
					src[n].V[i][j] = (tempV - src[n].min) /
						(src[n].max - src[n].min);
				}
			}
		}
	}
}

/*
	Function: _pos_matrix
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
double** _pos_matrix(double **matrix, int row, int col) {
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
	Function: _neg_matrix
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
double** _neg_matrix(double **matrix, int row, int col) {
	double **temp = (double**)malloc(sizeof(double*)* row);

	for (int i = 0; i < row; ++i) {
		temp[i] = (double*)malloc(sizeof(double)* col);
		for (int j = 0; j < col; ++j) {
			temp[i][j] = (matrix[i][j] > 0) ? 0 : matrix[i][j];
		}
	}

	return temp;
}