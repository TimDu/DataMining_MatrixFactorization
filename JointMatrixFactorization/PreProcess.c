#include "preprocess.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

void file_to_matrix(FILE *file, Source *src)
{

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
void get_assigned(FILE *file, char *name, Source *src)
{
	char line[MAX_CHARS];
	char **seg = (char**)malloc(sizeof(char*) * 3);
	char *token = NULL;
	src->items = (Item*)malloc(src->K * sizeof(Item));
	src->items->length = 0;
	
	while(!feof(file)) {
		fgets(line, MAX_CHARS, file);
		seg[0] = strtok_s(line, " \t\n", &token);
		if (seg[0] != NULL) {
			seg[1] = strtok_s(NULL, " \t\n", &token);
		}
		if (seg[1] != NULL) {
			seg[2] = strtok_s(NULL, " \t\n", &token);
		}
		if (seg[2] != NULL) {
			push_item(name, src, seg[0]);
		}
	}

	free(seg);
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

	free(seg);

	// Retrieves all item names and free its item tree
	get_items(item_tree, &items);
	src->items = items;
	// Sets item numbers
	src->K = items[0].length;
	// Sets user numbers
	src->N = user_num;
}