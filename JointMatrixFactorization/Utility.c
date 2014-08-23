#include "utility.h"
#include "itemproc.h"
#include "matrix.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

/*
	Function: check_empty
	Check if this file is empty

	Parameter:
	file - source file

	Returns:
	true if the file is empty; otherwiase, false
 */
bool check_empty(FILE *file)
{
	fseek(file, 0, SEEK_END);
	if (ftell(file) == 0) {
		// File is empty
		return true;
	}
	
	// File is not empty
	fseek(file, 0, SEEK_SET);
	return false;
}

/*
	Function: find_index
	---------------------
	Finds the position of this item name from Item structure

	Parameter:
	items - Item structure
	item - item to be found
	start - start finding range
	end - end finding range

	Returns:
	Index that this item name should be at. Or -1 for not finding
	proper position
 */
int find_index(Item *items, char *item, int start, int end)
{
	int index = -1;
	if (start == end) {
		if (!strcmp(items[start].name, item)) {
			index = start;
		}
	}
	else {
		int mid = (end + start) / 2;

		if (strcmp(items[mid].name, item) > 0) {
			index = find_index(items, item, start, mid);
		}
		else if (strcmp(items[mid].name, item) < 0) {
			index = find_index(items, item, mid + 1, end);
		}
		else {
			index = mid;
		}
	}
	
	return index;
}

/*
	Function: find_number
	---------------------
	Finds number from a given string.
	Only the group of first legal digits combination is considered.
	If there is no valid number, then 0 is returned;
 
	Parameter:
	str - the string to be converted
 
	Returns:
	A converted integer; 0 if no number is found
 */
double find_number(char *str)
{
	bool hasDecimal = false;
	int index = 0;
	char num[20];

	// Finds a valid number string
	for (int i = 0; i < (int) strlen(str); ++i) {
		if ((str[i] > '9') && (str[i] < '0')) {
			if (str[i] == '.') {
				if (!hasDecimal) {
					hasDecimal = true;
					num[index] = str[i];
					++index;
					if (index > (sizeof(num) / sizeof(num[0]))) {
						printf("Warning: An integer value is truncated due "
							"to the excessive amount of digits!");
						break;
					}
					continue;
				}
			}
			if (index != 0) {
				num[index] = '\0';
				break;
			}
		}
		else {
			num[index] = str[i];
			++index;
			if (index > (sizeof(num) / sizeof(num[0]))) {
				printf("Warning: An integer value is truncated due "
					"to the excessive amount of digits!");
				break;
			}
		}
	}

	// Fails to find a number string
	if (index == 0) { return 0; }

	return strtod(num, NULL);
}

/*
	Function: vector_dist
	----------------------
	Internal function. Finds distance between same item columns in two
	different soruces.

	Parameters:
	src - source structure
	a - index of the first source
	b - index of the second source
	size - size of sources
	k - index of item column

	Returns:
	euclidean distance. Otherwise, -1 if index is out of source boundary
*/
double vector_dist(Source *src, int a, int b, int size, int k)
{
	double result = -1.0;

	if ((a < size) && (b < size)) {
		result = 0;
		for (int i = 0; i < src->C; ++i) {
			result += pow(src[b].H[i][k] - src[a].H[i][k], 2.0);
		}
		result = sqrt(result);
	}
	
	return result;
}

/*
	Function: get_reliable
	-----------------------
	Calculate final reliable score matrix.

	Parameters:
	src - source structure
	size - size of sources

	Returns:
	reliable score matrix
 */
double** get_reliable(Source *src, int size)
{
	double temp = -1.0;
	double min = -1.0;
	double max = -1.0;
	double comp;
	double **result = (double**)malloc(size * sizeof(double*));

	for (int i = 0; i < size; ++i) {
		result[i] = (double*)malloc(src->K * sizeof(double));
		for (int k = 0; k < src->K; ++k) {
			for (int j = 0; j < size; ++j) {
				if (i != j) {
					comp = vector_dist(src, i, j, size, k);
					if ((temp > comp) || (temp < 0)) {
						temp = comp;
					}
				}
			}
			result[i][k] = 1.0 / temp;
			temp = -1;
			if ((max == -1) || max < result[i][k]) {
				max = result[i][k];
			}
			if ((min == -1) || (min > result[i][k])) {
				min = result[i][k];
			}
		}
	}

	// Normalization
	if (max != min) {
		for (int i = 0; i < size; ++i) {
			for (int k = 0; k < src->K; ++k) {
				result[i][k] = (result[i][k] - min) / (max - min); 
			}
		}
	}
	else {
		for (int i = 0; i < size; ++i) {
			for (int k = 0; k < src->K; ++k) {
				result[i][k] = 1.0;
			}
		}
	}
	
	return result;
}

/*
	Function: inputs_initialize
	----------------------------
	Initializes inital input matrix.

	Parameter:
	src - source structure
 */
void inputs_initialize(Source *src)
{
	src->K = src->items->length;	// Adjust item number
	double *temp = (double*)malloc(src->N * src->K * sizeof(double));
	if (temp == NULL) {
		fprintf(stderr, "Fatal Error: Program ran out of memory!\n");
		exit(1);
	}
	src->V = (double**)malloc(src->N * sizeof(double*));
	if (src->V == NULL) {
		fprintf(stderr, "Fatal Error: Program ran out of memory!\n");
		exit(1);
	}

	for (int i = 0; i < src->N; ++i) {
		for (int j = 0; j < src->K; ++j) {
			temp[i * src->K + j] = 0;
		}
		src->V[i] = &temp[i * src->K];
	}

	src->min = -1;
	src->max = -1;
	src->W = NULL;
	src->H = NULL;
}

/*
	Function: joints_initialize
	---------------------------
	Initializes matrices of factorized matrices this source.
 
	Parameter:
	src - the object source to be initialized
	size - size of sources
	val_c - group number
 */
void joints_initialize(Source *src, int size, int val_c)
{
	double *temp;
	for (int i = 0; i < size; ++i) {
		src[i].C = val_c;
		temp = (double*)malloc(src[i].N * val_c * sizeof(double));
		if (temp == NULL) {
			fprintf(stderr, "Fatal Error: Program ran out of memory!\n");
			exit(1);
		}
		src[i].W = (double**)malloc(src[i].N * sizeof(double*));
		if (src[i].W == NULL) {
			fprintf(stderr, "Fatal Error: Program ran out of memory!\n");
			exit(1);
		}
		for (int j = 0; j < src[i].N; ++j) {
			src[i].W[j] = &temp[j * val_c];
		}
		temp = (double*)malloc(val_c * src[i].K * sizeof(double));
		if (temp == NULL) {
			fprintf(stderr, "Fatal Error: Program ran out of memory!\n");
			exit(1);
		}
		src[i].H = (double**)malloc(val_c * sizeof(double*));
		if (src[i].H == NULL) {
			fprintf(stderr, "Fatal Error: Program ran out of memory!\n");
			exit(1);
		}
		for (int j = 0; j < val_c; ++j) {
			src[i].H[j] = &temp[j * src[i].K];
		}
	}
}

/*
	Function: joint_clear
	----------------------
	Clears joint matrices.

	Parameters:
	src - source structure
	size - size of sources
 */
void joint_clear(Source *src, int size)
{
	for (int i = 0; i < size; ++i) {
		if (src[i].W != NULL) {
			free(src[i].W[0]);
			free(src[i].W);
			src[i].W = NULL;
		}
		if (src[i].H != NULL) {
			free(src[i].H[0]);
			free(src[i].H);
			src[i].H = NULL;
		}
		src[i].C = 0;
	}
}

/*
	Function: clear2D
	----------------
	Clear 2-d pointer space.

	Parameters:
	ptr - pointer space
	r - pointer dimension
*/
void clear2D(double ***ptr, int r)
{
	double **ary = *ptr;
	for (int i = 0; i < r; ++i) {
		free(ary[i]);
	}
	free(ary);
}

/*
	Function: reset
	----------------
	Clears status of source structure.

	Parameters:
	src - source structure
	size - size of sources
 */
void reset(Source *src, int size)
{
	for (int i = 0; i < size; ++i) {
		free(src[i].V[0]);
		free(src[i].V);
		if (src[i].W != NULL) {
			free(src[i].W[0]);
			free(src[i].W);
		}
		if (src[i].H != NULL) {
			free(src[i].H[0]);
			free(src[i].H);
		}
		for (int j = 0; j < src[i].K; ++j) {
			free(src[i].items[j].name);
		}
		free(src[i].items);
	}
	free(src);
}