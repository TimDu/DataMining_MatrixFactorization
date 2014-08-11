#include "matrix.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

void strassen_multiply(double**, int, int, int, int, double**, int,
	int, int, int, double**);
double eigenL(double**, int);
double euclidean_dist(double*, double*, int);
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
	if (result == NULL) {
		fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
		getchar();
		exit(1);
	}
	for (int i = 0; i < rA; ++i) {
		result[i] = (double*)malloc(cB * sizeof(double));
		if (result[i] == NULL) {
			fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
			getchar();
			exit(1);
		}
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
		if ((rA_t == NULL) || (cB_t == NULL) || (cA_t == NULL) || (rB_t == NULL) ||
			(a_t == NULL) || (b_t == NULL)) {
			fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
			getchar();
			exit(1);
		}

		*rA_t = rA;
		*cB_t = cB;
		*cA_t = mid;
		*rB_t = mid;
		for (int i = 0; i < rA; ++i) {
			a_t[i] = (double*)malloc(mid * sizeof(double));
			if (a_t[i] == NULL) {
				fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
				getchar();
				exit(1);
			}
			for (int j = 0; j < mid; ++j) {
				a_t[i][j] = a[i][j];
			}
		}
		for (int i = 0; i < mid; ++i) {
			b_t[i] = (double*)malloc(cB * sizeof(double));
			if (b_t[i] == NULL) {
				fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
				getchar();
				exit(1);
			}
			for (int j = 0; j < cB; ++j) {
				b_t[i][j] = b[i][j];
			}
		}

		// Pads matrices
		matrix_pad(&a_t, rA_t, cA_t);
		matrix_pad(&b_t, rB_t, cB_t);

		// Allocate result matrix space
		c = (double**)malloc(*rA_t * sizeof(double*));
		if (c == NULL) {
			fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
			getchar();
			exit(1);
		}
		for (int i = 0; i < *rA_t; ++i) {
			c[i] = (double*)malloc(*cB_t * sizeof(double));
			if (c[i] == NULL) {
				fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
				getchar();
				exit(1);
			}
		}

		strassen_multiply(a_t, 0, *rA_t - 1, 0, *cA_t - 1,
			b_t, 0, *rB_t - 1, 0, *cB_t - 1, c);

		// Restore to result matrix and clean memory
		for (int i = 0; i < rA; ++i) {
			for (int j = 0; j < cB; ++j) {
				result[i][j] = c[i][j];
			}
		}

		clear2D(&a_t, *rA_t);
		clear2D(&b_t, *rB_t);
		clear2D(&c, *rA_t);
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

		// Gets results of 7 intermediate matrices
		int rA_m = (rA_s + rA_e) / 2;
		int cA_m = (cA_s + cA_e) / 2;
		int rB_m = (rB_s + rB_e) / 2;
		int cB_m = (cB_s + cB_e) / 2;
		// Temporary pointers
		double **temp1;
		double **temp2;

		// Matrix m1
		temp1 = matrix_sum(a, rA_s, rA_m, cA_s, cA_m, rA_m + 1, rA_e,
			cA_m + 1, cA_e);
		temp2 = matrix_sum(b, rB_s, rB_m, cB_s, cB_m, rB_m + 1, rB_e,
			cB_m + 1, cB_e);
		strassen_multiply(temp1, 0, rA_m - rA_s, 0, cA_m - cA_s, temp2,
			0, rB_m - rB_s, 0, cB_m - cB_s, m[0]);
		clear2D(&temp1, rA_m - rA_s + 1);
		clear2D(&temp2, rB_m - rB_s + 1);
		// Matrix m2
		temp1 = matrix_sum(a, rA_m + 1, rA_e, cA_s, cA_m, rA_m + 1,
			rA_e, cA_m + 1, cA_e);
		strassen_multiply(temp1, 0, rA_m - rA_s, 0, cA_m - cA_s,
			b, rB_s, rB_m, cB_s, cB_m, m[1]);
		clear2D(&temp1, rA_e - rA_m);
		// Matrix m3
		temp1 = matrix_sub(b, rB_s, rB_m, cB_m + 1, cB_e, rB_m + 1,
			rB_e, cB_m + 1, cB_e);
		strassen_multiply(a, rA_s, rA_m, cA_s, cA_m, temp1, 0,
			rB_m - rB_s, 0, cB_m - cB_s, m[2]);
		clear2D(&temp1, rB_m - rB_s + 1);
		// Matrix m4
		temp1 = matrix_sub(b, rB_m + 1, rB_e, cB_s, cB_m, rB_s, rB_m,
			cB_s, cB_m);
		strassen_multiply(a, rA_m + 1, rA_e, cA_m + 1, cA_e, temp1,
			0, rB_m - rB_s, 0, cB_m - cB_s, m[3]);
		clear2D(&temp1, rB_e - rB_m);
		// Matrix m5
		temp1 = matrix_sum(a, rA_s, rA_m, cA_s, cA_m, rA_s, rA_m,
			cA_m + 1, cA_e);
		strassen_multiply(temp1, 0, rA_m - rA_s, 0, cA_m - cA_s, b,
			rB_m + 1, rB_e, cB_m + 1, cB_e, m[4]);
		clear2D(&temp1, rA_m - rA_s + 1);
		// Matrix m6
		temp1 = matrix_sub(a, rA_m + 1, rA_e, cA_s, cA_m, rA_s, rA_m,
			cA_s, cA_m);
		temp2 = matrix_sum(b, rB_s, rB_m, cB_s, cB_m, rB_s, rB_m,
			cB_m + 1, cB_e);
		strassen_multiply(temp1, 0, rA_m - rA_s, 0, cA_m - cA_s,
			temp2, 0, rB_m - rB_s, 0, cB_m - cB_s, m[5]);
		clear2D(&temp1, rA_e - rA_m);
		clear2D(&temp2, rB_m - rB_s + 1);
		// Matrix m7
		temp1 = matrix_sub(a, rA_s, rA_m, cA_m + 1, cA_e, rA_m + 1,
			rA_e, cA_m + 1, cA_e);
		temp2 = matrix_sum(b, rB_m + 1, rB_e, cB_s, cB_m, rB_m + 1,
			rB_e, cB_m + 1, cB_e);
		strassen_multiply(temp1, 0, rA_m - rA_s, 0, cA_m - cA_s, temp2,
			0, rB_m - rB_s, 0, cB_m - cB_s, m[6]);
		clear2D(&temp1, rA_m - rA_s + 1);
		clear2D(&temp2, rB_e - rB_m);

		// Calculates all result sub-matrices
		temp1 = sum(m[0], m[3], mR / 2, mC / 2);
		temp2 = sub(temp1, m[4], mR / 2, mC / 2);
		c_sub[0] = sum(temp2, m[6], mR / 2, mC / 2);
		clear2D(&temp1, mR / 2);
		clear2D(&temp2, mR / 2);
		c_sub[1] = sum(m[2], m[4], mR / 2, mC / 2);
		c_sub[2] = sum(m[1], m[3], mR / 2, mC / 2);
		temp1 = sum(m[0], m[2], mR / 2, mC / 2);
		temp2 = sum(temp1, m[5], mR / 2, mC / 2);
		c_sub[3] = sub(temp2, m[1], mR / 2, mC / 2);
		clear2D(&temp1, mR / 2);
		clear2D(&temp2, mR / 2);

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
	if (result == NULL) {
		fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
		getchar();
		exit(1);
	}

	for (int i = 0; i < r; ++i) {
		result[i] = (double*)malloc(c * sizeof(double));
		if (result[i] == NULL) {
			fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
			getchar();
			exit(1);
		}
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
	if (result == NULL) {
		fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
		getchar();
		exit(1);
	}

	for (int i = 0; i < r; ++i) {
		result[i] = (double*)malloc(c * sizeof(double));
		if (result[i] == NULL) {
			fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
			getchar();
			exit(1);
		}
		for (int j = 0; j < c; ++j) {
			result[i][j] = a[i][j] - b[i][j];
		}
	}

	return result;
}

/*
	Function: transpose
	--------------------
	Transpose a matrix.

	Parameters:
	a - matrix to be transposed
	r - row number
	c - column number

	Return:
	Transposed matrix
 */
double** transpose(double **a, int r, int c)
{
	double **trans = (double**)malloc(c * sizeof(double*));
	if (trans == NULL) {
		fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
		getchar();
		exit(1);
	}

	for (int i = 0; i < c; ++i) {
		trans[i] = (double*)malloc(r * sizeof(double));
		if (trans[i] == NULL) {
			fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
			getchar();
			exit(1);
		}
		for (int j = 0; j < r; ++j) {
			trans[i][j] = a[j][i];
		}
	}

	return trans;
}

/*
	Function: eigen
	----------------
	Internal function. Find the largest eigenvalue from the matrix.

	Parameters:
	a - square matrix to be calculated
	n - dimension of its column/row space

	Returns:
	the largest eigenvalue
*/
double eigenL(double **a, int n)
{
	int parity = 0;	// Parity of loop rounds 
	double result = 0;
	double norm = 0;
	double **eigV = (double**)malloc(2 * sizeof(double*));
	double *temp = (double*)malloc(n * sizeof(double));
	if ((eigV == NULL) || temp == NULL) {
		fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
		getchar();
		exit(1);
	}

	// Initialization
	for (int i = 0; i < 2; ++i) {
		eigV[i] = (double*)malloc(n * sizeof(double));
		if (eigV[i] == NULL) {
			fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
			getchar();
			exit(1);
		}
		for (int j = 0; j < n; ++j) {
			eigV[i][j] = i;
		}
	}
	int loop = 0;
	// Find approximate eigenvector
	while (euclidean_dist(
		eigV[(parity + 1) % 2], eigV[parity], n) > 1.0e-10) {
		for (int i = 0; i < n; ++i) {
			temp[i] = 0;
			for (int j = 0; j < n; ++j) {
				temp[i] += a[i][j] * eigV[(parity + 1) % 2][j];
			}
			norm += pow(temp[i], 2);
		}
		norm = sqrt(norm);

		if (norm != 0) {
			// Normalization
			for (int i = 0; i < n; ++i) {
				eigV[parity][i] = temp[i] / norm;
			}
			++parity;
			parity = parity % 2;
		}
		else {
			// This is a null space
			break;
		}
	}

	// Find eigenvalue
	int count = 0;
	for (int i = 0; i < n; ++i) {
		if (eigV[(parity + 1) % 2][i] != 0) {
			temp[i] = 0;
			for (int j = 0; j < n; ++j) {
				temp[i] += a[i][j] * eigV[(parity + 1) % 2][j];
			}
			result += temp[i] / eigV[(parity + 1) % 2][i];
			++count;
		}
	}
	if (count != 0) {
		result = result / count;
	}

	clear2D(&eigV, 2);
	free(temp);
	return result;
}

/*
	Function: norm
	---------------------
	Calculates matrix's 2-norm value.

	Parameters:
	a - a matrix
	r - row number
	c - column number

	Returns:
	entrywise norm of this matrix
*/
double norm(double **a, int r, int c)
{
	return sqrt(norm2(a, r, c));
}

/*
	Function: norm2
	---------------------
	Calculates matrix's squared value of its 2-norm.

	Parameters:
	a - a matrix
	r - row number
	c - column number

	Returns:
	entrywise norm of this matrix
*/
double norm2(double **a, int r, int c)
{
	double result = 0;
	double **temp = transpose(a, r, c);
	double **sqr = multiply(temp, c, a, c, r);
	result = eigenL(sqr, c);

	clear2D(&temp, c);
	clear2D(&sqr, c);
	return result;
}

/*
	Function: euclidean_dist
	-------------------------
	Internal function. Calculates distance between two vectors.

	Parameters:
	a - vector A
	b - vector b
	dim - dimension of vector

	Returns:
	result euclidean distance
*/
double euclidean_dist(double *a, double *b, int dim)
{
	double dist = 0;

	for (int i = 0; i < dim; ++i) {
		dist += pow(a[i] - b[i], 2);
	}
	dist = sqrt(dist);

	return dist;
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
	if (result == NULL) {
		fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
		getchar();
		exit(1);
	}

	for (int i = 0; i < rSize; ++i) {
		result[i] = (double*)malloc(cSize * sizeof(double));
		if (result[i] == NULL) {
			fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
			getchar();
			exit(1);
		}
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
	if (result == NULL) {
		fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
		getchar();
		exit(1);
	}

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
			if (a[0][i] == NULL) {
				fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
				getchar();
				exit(1);
			}
			for (int j = *c; j < cPad; ++j) {
				a[0][i][j] = 0;	// Pads columns
			}
		}
	}
	if (rPad > *r) {
		a[0] = (double**)realloc(a[0], rPad * sizeof(double*));
		if (a[0] == NULL) {
			fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
			getchar();
			exit(1);
		}

		for (int i = *r; i < rPad; ++i) {
			a[0][i] = (double*)malloc(cPad * sizeof(double));
			if (a[0][i] == NULL) {
				fprintf(stderr, "Fatal Error: Program runs out of memory!\n");
				getchar();
				exit(1);
			}

			for (int j = 0; j < cPad; ++j) {
				a[0][i][j] = 0;	// Pads rows
			}
		}
	}

	*r = rPad;
	*c = cPad;
}