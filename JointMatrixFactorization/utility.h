#ifndef UTILITY_H_
#define UTILITY_H_

#include "preprocess.h"
#include <stdio.h>
#include <stdbool.h>

#define MAX_CHARS 1000	// Maximum characters per line

typedef struct Item
{
	int length;	// Length of whole item pointer space
	char *name;
} Item;

/*
   Stores inputs from a source file in here.
   NOTE: It is assumed that item IDs could be a string, while
   user IDs are only consecutive integers that starts from 1.
   Plus, all item IDs should be aligned in ascending order.
 */
typedef struct Source
{
	int N;	// Number of users
	int K;	// Number of items
	int C;	// Number of groups
	int **V;	// User ratings matrix
	double **W; // Goup membership matrix
	double **H;	// Group ratings matrix
	Item *items;	// Item names
} Source;

bool check_empty(FILE *file);
int find_number(char *str);
void matrix_initialize(Source *src);

#endif