/********************************************************************************
 * Program developed by Tianmiao Du.											*
 ********************************************************************************/

#include "algorithms.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define SEP " \t\n";	// Delimmiters used to split a string

int main()
{
	Source *source;
	FILE *input = (FILE*)malloc(sizeof(FILE));
	char path[256];	// Path name of data set
	bool isReadData;		// Tells if it should read data from the source now

	source = (Source*)malloc(sizeof(*source));

	printf("Please input the path of your source dataset: ");
	fgets(path, sizeof(path) / sizeof(path[0]), stdin);
	path[strlen(path) - 1] = '\0';

	// Opens source files
	printf("Reading source file: %s\n", path);
	fopen_s(&input, "example.txt", "r");
	if (!input) {
		printf("Error: Invalid file path! Discarded.\n");
		return 1;
	}
	if (check_empty(input)) {
		printf("Error: This source file is empty. Discarded.\n");
		return 1;
	}

	// Initializations
	int count = 0;	// Used to count the number of read data items
	isReadData = false;	// Tells whether a data set has being reading
	source->N = 0;
	source->K = 0;
	source->C = 0;

	// Read the first row of this source file
	char *line = (char*)malloc(MAX_CHARS * sizeof(char));
	char *token = NULL;

	fgets(line, MAX_CHARS, input);

	// Case 1. First row is set
	// Sets user and item numbers from the first row
	if (_strcmpi(line, "\n")) {
		char *ptr;
		source->K = strtol(line, &ptr, 0);
		source->N = strtol(ptr, &ptr, 0); 
		if ((source->N > 0) && (source->K > 0)) {
			get_assigned(input, path, source);
		}
	}

	// Case 2. First row is unqualified
	// Find user and item numbers by traversing the file
	if ((source->N == 0) || (source->K == 0)) {
		get_dimension(input, source);
	}

	inputs_initialize(source);

	// Read dataset
	if (file_to_matrix(input, source)) {
		return 1;
	}

	
	fclose(input);

	return 0;
}
