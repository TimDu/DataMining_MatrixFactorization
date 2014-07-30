#include "preprocess.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define _SEP " \t\n"

/*
	Function: file_to_matrix
	-------------------------
	Fill up matrix entries that read from a source file.
	
	Parameter:
	file - source file
	src - source structure

	Returns:
	0 for success, 1 for failure.
 */
int file_to_matrix(FILE *file, Source *src)
{
	// Set file pointer to the beginning
	if (!fseek(file, 0, SEEK_SET)) {
		char line[MAX_CHARS];
		char **seg = (char**)malloc(sizeof(char*) * 3);
		char *token = NULL;
		int iLength = src->items->length;
		int nIndex;
		int kIndex;
		int value;
		fgets(line, MAX_CHARS, file);

		while (fgets(line, MAX_CHARS, file) != NULL) {
			seg[0] = strtok_s(line, _SEP, &token);
			if (seg[0] != NULL) {
				seg[1] = strtok_s(NULL, _SEP, &token);

				if (seg[1] != NULL) {
					nIndex = find_number(seg[1]);
					seg[2] = strtok_s(NULL, _SEP, &token);

					if ((nIndex > 0) && (seg[2] != NULL)) {
						value = find_number(seg[2]);

						if (value > 0) {
							kIndex = find_index(
								src->items, seg[0], 0, iLength - 1);
							--nIndex;
							if ((kIndex >= 0) && nIndex < src->N) {
								src->V[nIndex][kIndex] = value;
							}
						}
					}
				}
			}
		}

		return 0;
	}
	else {
		fprintf(stderr, "Fatal error: Failed to roll back file pointer "
			"while attempting to input data set.\n");
		return 1;
	}

}

/*
	Function: get_assigned
	-----------------------
	Inputs Item names to this specified source structure based on the
	presented hint line at the first row.

	Parameter:
	file - source file
	name - source name
	src - source structure
 */
int get_assigned(FILE *file, char *name, Source *src)
{
	char line[MAX_CHARS];
	char **seg = (char**)malloc(sizeof(char*) * 3);
	char *token = NULL;
	src->items = (Item*)malloc(src->K * sizeof(Item));
	src->items->length = 0;
	
	while(fgets(line, MAX_CHARS, file) != NULL) {
		seg[0] = strtok_s(line, _SEP, &token);
		if (seg[0] != NULL) {
			seg[1] = strtok_s(NULL, _SEP, &token);

			if (seg[1] != NULL) {
				seg[2] = strtok_s(NULL, _SEP, &token);

				if (seg[2] != NULL) {
					if (insert_item(name, src, seg[0])) {
						return 1;
					}
				}
			}
		}
	}

	free(seg);
	return 0;
}

/*
	Function: get_dimension
	------------------------
	Traverses a given file and finds the total number of users and items
	for this source. The acquired number is stored in this given source
	structure. This method is used in the case that the hint line is not
	presented.

	Parameter:
	file - source file
	src - source structure
*/
void get_dimension(FILE *file, Source *src)
{
	int user_num = 0;
	char chars[MAX_CHARS];
	char *token = NULL;
	char **seg = (char**)malloc(3 * sizeof(*seg));
	struct Item_Tree *item_tree = initialize();
	Item *items = NULL;
	
	while (fgets(chars, MAX_CHARS, file) != NULL) {
		// Gets all segments
		seg[0] = strtok_s(chars, _SEP, &token);
		if (seg[0] != NULL) {
			seg[1] = strtok_s(NULL, _SEP, &token);

			if (seg[1] != NULL) {
				seg[2] = strtok_s(NULL, _SEP, &token);

				if (seg[2] != NULL) {
					// Handles item segment
					input_item(item_tree, seg[0]);
					// Handles user IDs
					long temp = find_number(seg[1]);
					if (temp > user_num) {
						user_num = temp;
					}
				}
			}
		}
	}

	free(seg);

	// Retrieves all item names and free its item tree
	get_items(item_tree, &items);
	src->items = items;
	// Sets user numbers
	src->N = user_num;
}