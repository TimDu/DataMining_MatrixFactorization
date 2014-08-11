#include "algorithms.h"
#include "utility.h"
#include <stdlib.h>
#include <math.h>

#define MAX_LOOP 1000

double **_getW(double**, int, int);
double** _sumH(Source*, int);
double** _n_sumH(Source*, int);
void _initialize(Source*, int);
double** _pos_matrix(double**, int, int);
double** _neg_matrix(double**, int, int);
double _getCost(Source*, int, double);

/*
	Function: matrix_factorization
	-------------------------------
	NMF algorithm.

	Parameters:
	src - source contents
	size - number of sources
	valC - group number
 */
void matrix_factorization(Source *src, int size, double alpha)
{
	int loop = 0;
	// Function cost
	double old_cost = 0;
	double cost;
	// Matrices componnents
	double **vh;
	double **n_vh;
	double **whh;
	double **n_whh;
	double **wwh;
	double **n_wwh;
	double **wv;
	double **n_wv;
	double **w;
	double **h;
	double **n_h;
	double **sum_h;
	double **n_sum_h;
	double **trans;

	double **temp = NULL;	// Pointer used to free memory

	_initialize(src, size);

	cost = _getCost(src, size, alpha);
	printf("%f ", cost);
	while ((fabs(old_cost - cost) > 1.0e-8) && (loop < MAX_LOOP)) {
		++loop; printf(" %d\n", loop);
		old_cost = cost;

		sum_h = _sumH(src, size);
		n_sum_h = _n_sumH(src, size);
		// Loop until converge
		for (int i = 0; i < size; ++i) {
			// Computes components for W matrix update
			trans = transpose(src[i].H, src[i].C, src[i].K);

			vh = multiply(
				src[i].V, src[i].N, trans, src[i].C, src[i].K);
			temp = vh;
			n_vh = _neg_matrix(vh, src[i].N, src[i].C);
			vh = _pos_matrix(vh, src[i].N, src[i].C);
			clear2D(&temp, src[i].N);

			temp = multiply(
				src[i].H, src[i].C, trans, src[i].C, src[i].K);
			whh = multiply(
				src[i].W, src[i].N, temp, src[i].C, src[i].C);
			clear2D(&temp, src[i].C);
			temp = whh;
			n_whh = _neg_matrix(whh, src[i].N, src[i].C);
			whh = _pos_matrix(whh, src[i].N, src[i].C);
			clear2D(&temp, src[i].N);
			clear2D(&trans, src[i].K);

			w = _getW(src[i].W, src[i].N, src[i].C);

			for (int j = 0; j < src->N; ++j) {
				for (int k = 0; k < src->C; ++k) {
					src[i].W[j][k] = src[i].W[j][k] * sqrt(
						(vh[j][k] + n_whh[j][k]) /
						(n_vh[j][k] + whh[j][k]));
				}
			}

			clear2D(&vh, src[i].N);
			clear2D(&n_vh, src[i].N);
			clear2D(&whh, src[i].N);
			clear2D(&n_whh, src[i].N);

			// Computes components for H matrix update
			trans = transpose(w, src[i].N, src[i].C);

			wv = multiply(
				trans, src[i].C, src[i].V, src[i].K, src[i].N);
			temp = wv;
			n_wv = _neg_matrix(wv, src[i].C, src[i].K);
			wv = _pos_matrix(wv, src[i].C, src[i].K);
			clear2D(&temp, src[i].C);

			temp = multiply(trans, src[i].C, w, src[i].C, src[i].N);
			clear2D(trans, src[i].N);
			wwh = multiply(
				temp, src[i].C, src[i].H, src[i].K, src[i].C);
			clear2D(&temp, src[i].C);
			temp = wwh;
			n_wwh = _neg_matrix(wwh, src[i].C, src[i].K);
			wwh = _pos_matrix(wwh, src[i].C, src[i].K);
			clear2D(&temp, src[i].C);

			h = _pos_matrix(src[i].H, src[i].C, src[i].K);
			n_h = _neg_matrix(src[i].H, src[i].C, src[i].K);

			for (int j = 0; j < src[i].C; ++j) {
				for (int k = 0; k < src[i].K; ++k) {
					src[i].H[j][k] = src[i].H[j][k] * sqrt(
						(wv[j][k] + n_wwh[j][k] +
						alpha * size * n_h[j][k] +
						alpha * (sum_h[j][k] - h[j][k])) /
						(n_wv[j][k] + wwh[j][k] +
						alpha * size * h[j][k] +
						alpha * (n_sum_h[j][k] - n_h[j][k])));
				}
			}

			clear2D(&w, src[i].N);
			clear2D(&wv, src[i].C);
			clear2D(&n_wv, src[i].C);
			clear2D(&wwh, src[i].C);
			clear2D(&n_wwh, src[i].C);
			clear2D(&h, src[i].C);
			clear2D(&n_h, src[i].C);
		}
		clear2D(&sum_h, src->C);
		clear2D(&n_sum_h, src->C);

		cost = _getCost(src, size, alpha);
		printf("%f ", cost);
	}
	printf("\n");
	FILE *f = (FILE*)malloc(sizeof(FILE));
	fopen_s(&f, "example.txt", "w");
	for (int i = 0; i < src->N; ++i) {
		for (int j = 0; j < src->K; ++j) {
			fprintf(f, "%3.1f ", src->V[i][j]);
		}
		fprintf(f, "\n");
	}
	fprintf(f, "\n");
	double **wh = multiply(src->W, src->N, src->H, src->K, src->C);
	for (int i = 0; i < src->N; ++i) {
		for (int j = 0; j < src->K; ++j) {
			fprintf(f, "%3.1f ", wh[i][j]);
		}
		fprintf(f, "\n");
	}
}

/*
	Function: _getCost
	--------------------
	Internal function. Calculate total cost with current
	matrices W and H.

	Parameters:
	src - source structure
	size - number of sources

	Returns:
	Cost value
 */
double _getCost(Source *src, int size, double alpha)
{
	double result = 0;
	double tempH = 0;
	double **wh;
	double **temp;

	// Calculate squared norms of all (Hs - Ht)
	for (int i = 0; i < size; ++i) {
		for (int j = i; j < size; ++j) {

			temp = sub(src[i].H, src[j].H, src[i].C, src[i].K);
			tempH += norm2(temp, src[i].C, src[i].K);
			clear2D(&temp, src[i].C);
		}
	}
	tempH = tempH * alpha * 2;
	for (int i = 0; i < size; ++i) {
		wh = multiply(
			src[i].W, src[i].N, src[i].H, src[i].K, src[i].C);
		temp = sub(src[i].V, wh, src[i].N, src[i].K);
		clear2D(&wh, src[i].N);
		result += norm2(temp, src[i].N, src[i].K);
		clear2D(&temp, src[i].N);
	}
	result += tempH;

	return result;
}

/*
	Function: _getW
	----------------
	Internal function. Get matrix snapshot on matrix W.

	Parameters:
	w - matrix W
	r - row number
	c - column number

	Returns:
	A copy of matrix W
 */
double** _getW(double **w, int r, int c)
{
	double **copy = (double**)malloc(r * sizeof(double*));
	if (copy == NULL) {
		fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
		getchar();
		exit(1);
	}

	for (int i = 0; i < r; ++i) {
		copy[i] = (double*)malloc(c * sizeof(double));
		for (int j = 0; j < c; ++j) {
			copy[i][j] = w[i][j];
		}
	}

	return copy;
}

/*
	Function: _sumH
	----------------
	Internal function. Sum up all item-group matrices.
	NOTE: Each source has same items in it.

	Parameters:
	src - source structures array
	size - size of source array

	Returns:
	Sum of H matrices from all sources
 */
double** _sumH(Source *src, int size) {
	double **result = (double**)malloc(src->C * sizeof(double*));
	if (result == NULL) {
		fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
		getchar();
		exit(1);
	}

	for (int i = 0; i < src->C; ++i) {
		result[i] = (double*)malloc(src->K * sizeof(double));
		for (int j = 0; j < src->K; ++j) {
			result[i][j] = 0;
		}
	}

	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < src->C; ++j) {
			for (int k = 0; k < src->K; ++k) {
				result[j][k] += (src[i].H[j][k] > 0) ? src[i].H[j][k] : 0;
			}
		}
	}

	return result;
}

/*
	Function: _n_sumH
	----------------
	Internal function. Sum up all item-group matrices.
	NOTE: Each source has same items in it.

	Parameters:
	src - source structures array
	size - size of source array

	Returns:
	Sum of H matrices from all sources
*/
double** _n_sumH(Source *src, int size) {
	double **result = (double**)malloc(src->C * sizeof(double*));
	if (result == NULL) {
		fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
		getchar();
		exit(1);
	}

	for (int i = 0; i < src->C; ++i) {
		result[i] = (double*)malloc(src->K * sizeof(double));
		for (int j = 0; j < src->K; ++j) {
			result[i][j] = 0;
		}
	}

	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < src->C; ++j) {
			for (int k = 0; k < src->K; ++k) {
				result[j][k] += (src[i].H[j][k] < 0) ? src[i].H[j][k] : 0;
			}
		}
	}

	return result;
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
	double iniValue = 1.0 / (double) src->C;

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
					src[n].H[i][j] = 1.0 / 2.0;
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
	double **temp = (double**)malloc(sizeof(double*)* row);
	if (temp == NULL) {
		fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
		getchar();
		exit(1);
	}

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
	if (temp == NULL) {
		fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
		getchar();
		exit(1);
	}

	for (int i = 0; i < row; ++i) {
		temp[i] = (double*)malloc(sizeof(double)* col);
		for (int j = 0; j < col; ++j) {
			temp[i][j] = (matrix[i][j] > 0) ? 0 : matrix[i][j];
		}
	}

	return temp;
}