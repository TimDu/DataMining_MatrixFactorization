/********************************************************************************
 * This C program is developed by Tianmiao Du.									*
 ********************************************************************************/

#include "algorithms.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

const char* _seperator = " \t\n";	// Delimmiters used to split a string

int main()
{
	Source *source;
	FILE *input = (FILE*)malloc(sizeof(FILE));
	char c;			// Data
	char path[256];	// Path name of data set
	char param[MAX_CHARS];	// Parameters defined for the data set
	bool isReadData;		// Tells if it should read data from the source now

	source = (Source*)malloc(sizeof(*source));

	printf("Please input the path of your source dataset: ");
	fgets(path, _countof(path), stdin);
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
		source->N = strtol(line, &ptr, 0);
		source->K = strtol(ptr, &ptr, 0);
	}

	// Case 2. First row is unqualified
	// Find user and item numbers by traversing the file
	if ((source->N == 0) || (source->K == 0)) {
		get_dimension(input, source);
		printf("%d %d\n", source->K, source->N);
	}

	// Process the content of source
	/*do {
		c = fgetc(input);

		if ((c == ' ') || (c == '\n')) { continue; }
		else if (c == EOF) {
			if (count == (source->N * source->K)) {
				printf("Succeeds.\n");
				break;
			}
			else {
				printf("Error: Invalid input data set! Discarded.\n");
				free(source);
				source = NULL;
				break;
			}
		}
		else if ((c == 'C') || c == 'c') {
			// This is a column parameter
			char *param_copy = (char*)malloc(sizeof(param));
			char *token = NULL;

			if (isReadData) {
				printf("Error: Invalid data! Discarded this source->");
				break;
			}

			fgets(param, _countof(param), input);
			strcpy_s(param_copy, _countof(param), param);
			char *seg = strtok_s(param_copy, _seperator, &token);

			if (!_strcmpi(seg, "OL_NAMES")) {
				// Get column names
				int count = -1;	// Counts how many column names are given
				
				while (seg != NULL) {
					count++;
					seg = strtok_s(NULL, _seperator, &token);
				}

				source->items = (char**)malloc(count * sizeof(char*));
				seg = strtok_s(param, _seperator, &token);
				for (int i = 0; i < count; i++) {
					seg = strtok_s(NULL, _seperator, &token);
					//source->items[i] = seg;
					printf("%s\n", source->items[i]);
				}
			}
			else if (!_strcmpi(seg, "OL_NUM")) {
				// Get column number
				if (seg != NULL) {
					// Find valid column number
					seg = strtok_s(NULL, _seperator, &token);
					source->K = findNumber(seg);
				}
			}
		}
		else if ((c == 'R') || (c == 'r')) {
			// This is a row operation
			char *param_copy = (char*)malloc(sizeof(param));
			char *token = NULL;

			if (isReadData) {
				printf("Error: Invalid data! Discarded this source->");
				break;
			}

			fgets(param, _countof(param), input);
			strcpy_s(param_copy, _countof(param), param);
			char *seg = strtok_s(param_copy, _seperator, &token);

			if (!_strcmpi(seg, "OW_NUM")) {
				// Get row number
				if (seg != NULL) {
					// Find valid row number
					seg = strtok_s(NULL, _seperator, &token);
					source->N = findNumber(seg);
				}
			}
		}
		else if ((c == 'G') || (c == 'g')) {
			// Get group number
			char *param_copy = (char*)malloc(sizeof(param));
			char *token = NULL;

			if (isReadData) {
				printf("Error: Invalid data! Discarded this source->");
				break;
			}

			fgets(param, _countof(param), input);
			strcpy_s(param_copy, _countof(param), param);
			char *seg = strtok_s(param_copy, _seperator, &token);

			if (!_strcmpi(seg, "ROUP_NUM")) {
				if (seg != NULL) {
					// Find valid group number
					seg = strtok_s(NULL, _seperator, &token);
					source->C = findNumber(seg);
				}
			}
		}
		else if ((c < '9') && (c > '0')){
			// Get input data
			if (!isReadData) {
				// First data reading check
				if ((source->N != 0) && (source->K != 0) && (source->C != 0)) {
					isReadData = true;
					matrixInitialize(source);
					//temp = source->V[0];
					printf("Reading %dx%d matrix...\n", source->N, source->K);
				}
				else {
					printf("Error: Invalid input column/row number!\n");
					return 1;
				}
			}

			//temp[count] = (int)(c - '0');
			count++;
		}
		else {
			if (isReadData) {
				printf("Error: Invalid data! Discarded this source->");
				break;
			}
		}
	} while (c != EOF);*/

	fclose(input);

	return 0;
}
