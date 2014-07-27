#include "utility.h"
#include "itemproc.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

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
int find_number(char *str)
{
	int index = 0;
	char num[20];

	// Finds a valid number string
	for (int i = 0; i < (int) strlen(str); ++i) {
		if ((str[i] > '9') && (str[i] < '0')) {
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

	return strtol(num, NULL, 0);
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
	int *temp1 = (int*)malloc(src->N * src->K * sizeof(int));
	src->V = (int**)malloc(src->N * sizeof(int*));

	for (int i = 0; i < src->N; ++i) {
		for (int j = 0; j < src->K; ++j) {
			temp1[i * src->K + j] = 0;
		}
		src->V[i] = &temp1[i * src->K];
	}
}

/*
	Function: joints_initialize
	---------------------------
	Initializes matrices of factorized matrices this source.
 
	Parameter:
	src - the object source to be initialized
 */
void joints_initialize(Source *src)
{
	double *temp2 = (double*)malloc(src->N * src->C * sizeof(double));

	src->W = (double**)malloc(src->N * sizeof(double*));
	for (int i = 0; i < src->N; ++i) {
		src->W[i] = &temp2[i * src->C];
	}
	temp2 = (double*)malloc(src->C * src->K * sizeof(double));
	src->H = (double**)malloc(src->C * sizeof(double*));
	for (int i = 0; i < src->C; ++i) {
		src->H[i] = &temp2[i * src->K];
	}
}