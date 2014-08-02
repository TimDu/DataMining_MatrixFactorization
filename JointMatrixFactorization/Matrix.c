#include "matrix.h"
#include <stdio.h>
#include <malloc.h>
#include <math.h>

void strassen_multiply(double**, int, int, int, int, double**, int,
	int, int, int, double**);
double** sum(double**, double**, int, int);
double** sub(double**, double**, int, int);
double** matrix_sum(double**, int, int, int, int, int, int, int, int);
double** matrix_sub(double**, int, int, int, int, int, int, int, int);
void matrix_pad(double***, int*, int*);

/*
	Function: multiply
	--------------------------
	Matrix multiplication method. It uses standard multiplication method
	when matrix	size is small, uses Strassen multiplication for large 
	matrix computing.

	Parameter:
	a - matrix A
	rA - row number of matrix A
	b - matrix B
	cB - column number of matrix B
	mid - column number of A/row number of B

	Returns:
	result matrix
*/
double** multiply(double **a, int rA, double **b, int cB, int mid)
{
	double **result = (double**)malloc(rA * sizeof(double*));
	for (int i = 0; i < rA; ++i) {
		result[i] = (double*)malloc(cB * sizeof(double));
	}

	if ((mid < SMALL_DIM) && (rA < SMALL_DIM) && (cB < SMALL_DIM)) {
		// Uses standard multiplication for small sized matrices
		for (int i = 0; i < rA; ++i) {
			for (int j = 0; j < cB; ++j) {
				result[i][j] = 0;
				for (int k = 0; k < mid; ++k) {
					result[i][j] += a[i][k] * b[k][j];
				}
			}
		}
	}
	else {
		// Uses strassen algorithm
		int *rA_t = (int*)malloc(sizeof(int));
		int *cB_t = (int*)malloc(sizeof(int));
		int *cA_t = (int*)malloc(sizeof(int));
		int *rB_t = (int*)malloc(sizeof(int));
		double **a_t = (double**)malloc(rA * sizeof(double*));
		double **b_t = (double**)malloc(mid * sizeof(double*));
		double **c;

		*rA_t = rA;
		*cB_t = cB;
		*cA_t = mid;
		*rB_t = mid;
		for (int i = 0; i < rA; ++i) {
			a_t[i] = (double*)malloc(mid * sizeof(double));
			for (int j = 0; j < mid; ++j) {
				a_t[i][j] = a[i][j];
			}
		}
		for (int i = 0; i < mid; ++i) {
			b_t[i] = (double*)malloc(cB * sizeof(double));
			for (int j = 0; j < cB; ++j) {
				b_t[i][j] = b[i][j];
			}
		}

		// Pads matrices
		matrix_pad(&a_t, rA_t, cA_t);
		matrix_pad(&b_t, rB_t, cB_t);

		if (*cA_t != *rB_t) {
			fprintf(stderr, "Fatal Error: matrix dimension doesn't match.\n");
			return NULL;
		}

		// Allocate result matrix space
		c = (double**)malloc(*rA_t * sizeof(double*));
		for (int i = 0; i < *rA_t; ++i) {
			c[i] = (double*)malloc(*cB_t * sizeof(double));
		}

		strassen_multiply(a_t, 0, *rA_t - 1, 0, *cA_t - 1,
			b_t, 0, *cA_t - 1, 0, *cB_t - 1, c);

		// Restore to result matrix and clean memory
		for (int i = 0; i < rA; ++i) {
			free(a_t[i]);
			for (int j = 0; j < cB; ++j) {
				result[i][j] = c[i][j];
			}
			free(c[i]);
		}

		free(a_t);
		free(c);
		for (int i = 0; i < mid; ++i) {
			free(b_t[i]);
		}
		free(b_t);
		free(rA_t);
		free(rB_t);
		free(cA_t);
		free(cB_t);
	}

	return result;
}

/*
	Function: strassen_multiply
	--------------------------
	Internal function. Matrix multiplication through Strenssen algorithm.
	Calculates C = AB. Dimensions of A and B must be power of 2.

	Parameters:
	a - matrix A
	rA_s - row start index of A
	rA_e - row end index of A
	cA_s - column start index of A
	cA_e - column end index of A
	b - matrix B
	rB_s - column start index 0f B
	rB_e - column end index of B
	cB_s - column start index of B
	cB_e - column end index of B
	c - result matrix
*/
void strassen_multiply(
	double **a, int rA_s, int rA_e, int cA_s, int cA_e,
	double **b, int rB_s, int rB_e, int cB_s, int cB_e, double **c) {
	if (((cA_e - cA_s) < 1) || ((rA_e - rA_s) < 1) ||
		((cB_e - cB_s) < 1) ||
		((cA_e - cA_s + 1 < SMALL_DIM) &&
		(rA_e - rA_s + 1 < SMALL_DIM) &&
		(cB_e - cB_s + 1 < SMALL_DIM))) {
		for (int i = 0; i <= (rA_e - rA_s); ++i) {
			for (int j = 0; j <= (cB_e - cB_s); ++j) {
				c[i][j] = 0;
				for (int k = 0; k <= (cA_e - cA_s); ++k) {
					c[i][j] += a[i + rA_s][k + cA_s] * b[k + rB_s][j + cB_s];
				}
			}
		}
	}
	else {
		// Intermediate matrix initialization
		double ***m = (double***)malloc(7 * sizeof(double**));
		double ***c_sub = (double***)malloc(4 * sizeof(double**));
		int mR = rA_e - rA_s + 1;
		int mC = cB_e - cB_s + 1;

		for (int i = 0; i < 7; ++i) {
			m[i] = (double**)malloc((mR / 2) * sizeof(double*));
			for (int j = 0; j < mR / 2; ++j) {
				m[i][j] = (double*)malloc((mC / 2) * sizeof(double));
			}
		}
		for (int i = 0; i < 4; ++i) {
			c_sub[i] = (double**)malloc((mR / 2) * sizeof(double*));
			for (int j = 0; j < mR / 2; ++j) {
				c_sub[i][j] = (double*)malloc((mC / 2) * sizeof(double));
			}
		}

		// Gets results of 7 intermediate matrices
		int rA_m = (rA_s + rA_e) / 2;
		int cA_m = (cA_s + cA_e) / 2;
		int rB_m = (rB_s + rB_e) / 2;
		int cB_m = (cB_s + cB_e) / 2;

		strassen_multiply(matrix_sum(a, rA_s, rA_m, cA_s, cA_m,
			rA_m + 1, rA_e, cA_m + 1, cA_e), 0, rA_m - rA_s, 0, cA_m - cA_s,
			matrix_sum(b, rB_s, rB_m, cB_s, cB_m, rB_m + 1, rB_e,
			cB_m + 1, cB_e), 0, rB_m - rB_s, 0, cB_m - cB_s, m[0]);
		strassen_multiply(matrix_sum(a, rA_m + 1, rA_e, cA_s, cA_m,
			rA_m + 1, rA_e, cA_m + 1, cA_e), 0, rA_m - rA_s, 0, cA_m - cA_s,
			b, rB_s, rB_m, cB_s, cB_m, m[1]);
		strassen_multiply(a, rA_s, rA_m, cA_s, cA_m,
			matrix_sub(b, rB_s, rB_m, cB_m + 1, cB_e, rB_m + 1, rB_e, cB_m + 1,
			cB_e), 0, rB_m - rB_s, 0, cB_m - cB_s, m[2]);
		strassen_multiply(a, rA_m + 1, rA_e, cA_m + 1, cA_e,
			matrix_sub(b, rB_m + 1, rB_e, cB_s, cB_m, rB_s, rB_m, cB_s, cB_m),
			0, rB_m - rB_s, 0, cB_m - cB_s, m[3]);
		strassen_multiply(matrix_sum(a, rA_s, rA_m, cA_s, cA_m, rA_s, rA_m,
			cA_m + 1, cA_e), 0, rA_m - rA_s, 0, cA_m - cA_s, b, rB_m + 1, rB_e,
			cB_m + 1, cB_e, m[4]);
		strassen_multiply(matrix_sub(a, rA_m + 1, rA_e, cA_s, cA_m, rA_s, rA_m,
			cA_s, cA_m), 0, rA_m - rA_s, 0, cA_m - cA_s, matrix_sum(b, rB_s,
			rB_m, cB_s, cB_m, rB_s, rB_m, cB_m + 1, cB_e), 0, rB_m - rB_s, 0,
			cB_m - cB_s, m[5]);
		strassen_multiply(matrix_sub(a, rA_s, rA_m, cA_m + 1, cA_e, rA_m + 1,
			rA_e, cA_m + 1, cA_e), 0, rA_m - rA_s, 0, cA_m - cA_s, matrix_sum(
			b, rB_m + 1, rB_e, cB_s, cB_m, rB_m + 1, rB_e, cB_m + 1, cB_e), 0,
			rB_m - rB_s, 0, cB_m - cB_s, m[6]);

		// Calculates all result sub-matrices
		c_sub[0] = sum(sub(sum(m[0], m[3], mR / 2, mC / 2),
			m[4], mR / 2, mC / 2), m[6], mR / 2, mC / 2);
		c_sub[1] = sum(m[2], m[4], mR / 2, mC / 2);
		c_sub[2] = sum(m[1], m[3], mR / 2, mC / 2);
		c_sub[3] = sub(sum(sum(m[0], m[2], mR / 2, mC / 2),
			m[5], mR / 2, mC / 2), m[1], mR / 2, mC / 2);

		// Free intermediate matrices
		for (int i = 0; i < 7; ++i) {
			for (int j = 0; j < mR / 2; ++j) {
				free(m[i][j]);
			}
			free(m[i]);
		}
		free(m);

		// Combine sub-matrices
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < mR / 2; ++j) {
				for (int k = 0; k < mC / 2; ++k) {
					c[(i / 2) * mR / 2 + j][(i % 2) * mC / 2 + k] =
						c_sub[i][j][k];
				}
			}
		}

		// Free sub mmatrices
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < mR / 2; ++j) {
				free(c_sub[i][j]);
			}
			free(c_sub[i]);
		}
		free(c_sub);
	}
}

/*
	Function: sum
	--------------
	Adds a matrix from a different one.

	Parameters:
	a - matrix A
	b - matrix B
	r - row number
	c - column number

	Returns:
	result matrix
*/
double** sum(double **a, double **b, int r, int c)
{
	double **result = (double**)malloc(r * sizeof(double*));

	for (int i = 0; i < r; ++i) {
		result[i] = (double*)malloc(c * sizeof(double));
		for (int j = 0; j < c; ++j) {
			result[i][j] = a[i][j] + b[i][j];
		}
	}

	return result;
}

/*
	Function: sub
	--------------
	Substracts a matrix from a different one.

	Parameters:
	a - matrix A
	b - matrix B
	r - row number
	c - column number

	Returns:
	result matrix
*/
double** sub(double **a, double **b, int r, int c)
{
	double **result = (double**)malloc(r * sizeof(double*));

	for (int i = 0; i < r; ++i) {
		result[i] = (double*)malloc(c * sizeof(double));
		for (int j = 0; j < c; ++j) {
			result[i][j] = a[i][j] - b[i][j];
		}
	}

	return result;
}

/*
	Function: matrix_sum
	---------------------
	Internal function. Adds a sub matrix from another internally.

	Parameters:
	a - matrix A
	r1_s - start row index of the first sub matrix
	r1_e - end row index of the first sub matrix
	c1_s - start column index of the first  sub matrix
	c1_e - end column index of the first sub matrix
	r2_s - start row index of the second sub matrix
	r2_e - end row index of the second sub matrix
	c2_s - start column index of the second sub matrix
	c2_e - end column index of the second sub matrix

	Returns:
	result matrix
*/
double** matrix_sum(double **a, int r1_s, int r1_e, int c1_s, int c1_e,
	int r2_s, int r2_e, int c2_s, int c2_e)
{
	int rSize = r1_e - r1_s + 1;
	int cSize = c1_e - c1_s + 1;
	double **result = (double**)malloc(rSize * sizeof(double*));

	for (int i = 0; i < rSize; ++i) {
		result[i] = (double*)malloc(cSize * sizeof(double));
		for (int j = 0; j < cSize; ++j) {
			result[i][j] = a[i + r1_s][j + c1_s] + a[i + r2_s][j + c2_s];
		}
	}

	return result;
}

/*
	Function: matrix_sub
	---------------------
	Internal function. Substracts a sub matrix from another 
	internally.

	Parameters:
	a - matrix A
	r1_s - start row index of the first sub matrix
	r1_e - end row index of the first sub matrix
	c1_s - start column index of the first  sub matrix
	c1_e - end column index of the first sub matrix
	r2_s - start row index of the second sub matrix
	r2_e - end row index of the second sub matrix
	c2_s - start column index of the second sub matrix
	c2_e - end column index of the second sub matrix

	Returns:
	result matrix
*/
double** matrix_sub(double **a, int r1_s, int r1_e, int c1_s, int c1_e,
	int r2_s, int r2_e, int c2_s, int c2_e)
{
	int rSize = r1_e - r1_s + 1;
	int cSize = c1_e - c1_s + 1;
	double **result = (double**)malloc(rSize * sizeof(double*));

	for (int i = 0; i < rSize; ++i) {
		result[i] = (double*)malloc(cSize * sizeof(double));
		for (int j = 0; j < cSize; ++j) {
			result[i][j] = a[i + r1_s][j + c1_s] - a[i + r2_s][j + c2_s];
		}
	}

	return result;
}

/*
	Function: Matrix_pad
	---------------------
	Internal function. Pad 0s to this target matrix, so that the number
	of rows and columns	comes to the power of 2.

	Parameters:
	a - target matrix
	r - row number
	c - column number
*/
void matrix_pad(double ***a, int *r, int *c) {
	int rPad = (int)pow(2, ceil(log2(*r)));
	int cPad = (int)pow(2, ceil(log2(*c)));

	// Extends matrix allocation
	if (cPad > *c) {
		for (int i = 0; i < *r; ++i) {
			a[0][i] = (double*)realloc(a[0][i], cPad * sizeof(double));
			for (int j = *c; j < cPad; ++j) {
				a[0][i][j] = 0;	// Pads columns
			}
		}
	}
	if (rPad > *r) {
		a[0] = (double**)realloc(a[0], rPad * sizeof(double*));
		for (int i = *r; i < rPad; ++i) {
			a[0][i] = (double*)malloc(cPad * sizeof(double));
			for (int j = 0; j < cPad; ++j) {
				a[0][i][j] = 0;	// Pads rows
			}
		}
	}

	*r = rPad;
	*c = cPad;
}