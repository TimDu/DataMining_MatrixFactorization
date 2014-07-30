/********************************************************************************
 * Program developed by Tianmiao Du.											*
 ********************************************************************************/

#include "menu.h"
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
	char cmd[256];	// Input commands
	int srcSz;	// Number of source files
	int srcIndex = 0;	// Index of source that is being operated
	bool isReadData;	// Tells if it should read data from the source now

	printf("\n%s\n", menu);

	// Enter number of sources
	printf("%s: ", cmd1);
	fgets(cmd, sizeof(cmd) / sizeof(cmd[0]), stdin);
	cmd[strlen(cmd) - 1] = '\0';	// Eliminates 'LF' Ascii code
	srcSz = find_number(cmd);
	if (srcSz > 0) {
		source = (Source*)malloc(srcSz * sizeof(*source));

		while (srcIndex < srcSz) {
			// Enter path
			printf("%s(%d left):\n> ", cmd2, srcSz - srcIndex);
			fgets(cmd, sizeof(cmd) / sizeof(cmd[0]), stdin);
			cmd[strlen(cmd) - 1] = '\0';	// Eliminates 'LF' Ascii code

			// Quit command detector
			if (!(strcmp(cmd, "Q") && strcmp(cmd, "q"))) {
				break;
			}

			// Opens source files
			printf("Reading source file: %s\n", cmd);
			fopen_s(&input, cmd, "r");
			if (!input) {
				printf("Error: Invalid file path! Discarded.\n");
				continue;
			}
			if (check_empty(input)) {
				printf("Error: This source file is empty. Discarded.\n");
				continue;
			}

			// Initializations
			int count = 0;	// Used to count the number of read data items
			isReadData = false;	// Tells whether a data set has being reading
			source[srcIndex].N = 0;
			source[srcIndex].K = 0;
			source[srcIndex].C = 0;

			// Read the first row of this source file
			char *line = (char*)malloc(MAX_CHARS * sizeof(char));
			char *token = NULL;

			fgets(line, MAX_CHARS, input);

			// Case 1. First row is set
			// Sets user and item numbers from the first row
			if (strcmp(line, "\n")) {
				char *ptr;
				source[srcIndex].K = strtol(line, &ptr, 0);
				source[srcIndex].N = strtol(ptr, &ptr, 0);
				if ((source[srcIndex].N > 0) && (source[srcIndex].K > 0)) {
					get_assigned(input, cmd, source);
				}
			}

			// Case 2. First row is unqualified
			// Find user and item numbers by traversing the file
			if ((source[srcIndex].N == 0) || (source[srcIndex].K == 0)) {
				get_dimension(input, source);
			}

			inputs_initialize(source);

			// Read dataset
			if (file_to_matrix(input, source)) {
				continue;
			}

			++srcIndex;
			fclose(input);
		}

		free(source);
	}
	else if ((strcmp(cmd, "q") && strcmp(cmd, "Q"))) {
		printf("Error: Unexpected input. Exited.\n");
		return 1;
	}

	printf("%s\n", end);
	return 0;
}
