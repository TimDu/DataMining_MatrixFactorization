#include "utility.h"
#include "itemproc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
  Function: get_dimension
  ------------------------
  Traverses a given file and finds the total number of users and items
  for this source. The acquired number is stored in this given source
  structure.
 
  Parameter:
  file - source file
  src - source structure
 */
void get_dimension(FILE *file, Source *src)
{
	int user_num = 0;
	char chars[MAX_CHARS];
	char *token = NULL;
	char *pEnd;
	char **seg = (char**)malloc(3 * sizeof(*seg));
	struct Item_Tree *item_tree = initialize();
	Item *items = NULL;

	while (!feof(file)) {
		fgets(chars, MAX_CHARS, file);
		// Gets all segments
		seg[0] = strtok_s(chars, " \t\n", &token);
		if (seg[0] != NULL) {
			seg[1] = strtok_s(NULL, " \t\n", &token);
		}
		if (seg[1] != NULL) {
			seg[2] = strtok_s(NULL, " \t\n", &token);
		}
		if (seg[2] != NULL) {
			// Handles item segment
			input_item(item_tree, seg[0]);
			// Handles user IDs
			pEnd = NULL;
			long temp = strtol(seg[1], &pEnd, 10);
			if (temp > user_num) {
				user_num = temp;
			}
		}
	}

	// Retrieves all item names
	get_items(item_tree, &items);
	src->items = items;
	// Sets item numbers
	src->K = _msize(items) / sizeof(Item);
	// Sets user numbers
	src->N = user_num;
}

/*
  Function: find_number
  ---------------------
  Finds number from a given string.
  Only the group of first legal digits combination is considered. If
  there is no valid number, then 0 is returned;
 
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
			if (index > _countof(num)) {
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
  Function: matrix_initialize
  ---------------------------
  Initializes variable matrices of this source.
 
  Parameter:
  src - the object source to be initialized
 */
void matrix_initialize(Source *src)
{
	int *temp1 = (int*)malloc(src->N * src->K * sizeof(int));
	double *temp2 = (double*)malloc(src->N * src->C * sizeof(double));

	src->V = (int**)malloc(src->N * sizeof(int*));
	for (int i = 0; i < src->N; ++i) {
		src->V[i] = &temp1[i * src->K];
	}
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