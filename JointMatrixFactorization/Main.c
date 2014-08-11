/********************************************************************************
 * Program developed by Tianmiao Du.											*
 ********************************************************************************/

#include "menu.h"
#include "algorithms.h"
#include "utility.h"
#include "matrix.h"
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

	while (true) {
	Starting:
		// Enter number of sources
		printf("%s: ", cmd1);
		gets_s(cmd, sizeof(cmd));
		srcSz = (int) find_number(cmd);

		if (srcSz > 0) {
			source = (Source*)malloc(srcSz * sizeof(*source));

			while (srcIndex < srcSz) {
				// Enter path
				printf("%s(%d left):\n> ", cmd2, srcSz - srcIndex);
				gets_s(cmd, sizeof(cmd));

				// Reset commande detector
				if (!(strcmp(cmd, "R") && strcmp(cmd, "r"))) {
					free(source);
					printf("[RESET]\n");
					goto Starting;
				}
				// Quit command detector
				if (!(strcmp(cmd, "Q") && strcmp(cmd, "q"))) {
					goto Ending;
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
						get_assigned(input, cmd, &source[srcIndex]);
					}
				}

				// Case 2. First row is unqualified
				// Find user and item numbers by traversing the file
				if ((source[srcIndex].N == 0) || (source[srcIndex].K == 0)) {
					get_dimension(input, &source[srcIndex]);
				}

				inputs_initialize(&source[srcIndex]);

				// Read dataset
				if (file_to_matrix(input, &source[srcIndex])) {
					// File not qualified
					continue;
				}

				++srcIndex;
				fclose(input);
				printf("Reading finished.\n");
			}
			printf("\n");

			while (true) {
				int val_c;
				double alpha;

				// Enter number of groups
				printf("%s: ", cmd3);
				gets_s(cmd, sizeof(cmd));

				// Reset commande detector
				if (!(strcmp(cmd, "R") && strcmp(cmd, "r"))) {
					free(source);
					printf("[RESET]\n");
					goto Starting;
				}
				// Quit command detector
				if (!(strcmp(cmd, "Q") && strcmp(cmd, "q"))) {
					goto Ending;
				}

				// Assign group number
				if ((val_c = (int) find_number(cmd)) > 0) {
					printf("%s: ", cmd4);
					gets_s(cmd, sizeof(cmd));

					// Reset command detector
					if (!(strcmp(cmd, "R") && strcmp(cmd, "r"))) {
						free(source);
						printf("[RESET]\n");
						goto Starting;
					}
					// Quit command detector
					if (!(strcmp(cmd, "Q") && strcmp(cmd, "q"))) {
						goto Ending;
					}

					if ((alpha = find_number(cmd)) > 0) {
						// Initialize joint matrices
						joints_initialize(source, srcSz, val_c);
						// Perform algorithms
						matrix_factorization(source, srcSz, alpha);

						// TODO: Continue or not?
					}
				}
				else {
					printf("Error: Invalid input.\n");
				}
			}

			free(source);
		}
		else if (!(strcmp(cmd, "q") && strcmp(cmd, "Q"))) {
			break;
		}
		else if (!(strcmp(cmd, "r") && strcmp(cmd, "R"))) {
			printf("[RESET]\n");
			continue;
		}
		else {
			printf("Error: Unexpected input.\n");
		}
	}
	
Ending:
	printf("%s\n", end);
	return 0;
}
