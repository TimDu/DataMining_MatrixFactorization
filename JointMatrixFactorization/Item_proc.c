#include "itemproc.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define TABLE_RANGE 94	// Consecutive ASCII character from code 33 to code 126.

typedef struct Item_Tree {
	bool end;	// Records whether its precessor is a item with the last character
	int *index;	// Records the table entries that have item inputs on them
	struct Item_Tree *char_table[TABLE_RANGE];	// A table that stores other structures
												// by the order of ASCII code
} Item_Tree;

/*
   Function: initialize
   ---------------------
   Creates a storing structure of items. This structure can efficiently read and store 
   items with the name consisted of ASCII codes ranging from 33 to 126. It also generates
   item array for all input items faster than normal coarsed way.

   Returns:
   An item storing structure for preprocessing
 */
Item_Tree* initialize()
{
	Item_Tree *tree = (Item_Tree*)malloc(sizeof(*tree));
	tree->index = NULL;
	tree->end = false;

	return tree;
}

/*
   Function: input_item
   ---------------------
   Reads the first character of the passed name and stores it into the given storing
   structure if this provided character does not exist in this structure. The existence
   is compared literally and recursively with the elements stored in this structure.
   Therefore, this is a recrusive method. 
   Supposing m is the length of item name, C is the constant of table size, which is 94,
   it takes O(m) time to finish, and consumes O(Cm) memory for each item storage.

   Parameters:
   tree - storing structure
   item - item name
 */
void input_item(Item_Tree *tree, char *item)
{
	int length = strlen(item);

	// Check if there is any character for this item
	if (length >= 1) {
		int first_ascii = (int)item[0];
		char *last_str = NULL;

		// Check if more than one characters left in this item string
		if (length > 1) {
			last_str = (char*)malloc(strlen(item) * sizeof(char));
			memcpy(last_str, &item[1], strlen(item) - 1);
			last_str[strlen(item) - 1] = '\0';
		}

		// Compares the first character with this current structure
		if ((first_ascii >= 33) && (first_ascii <= 126)) {
			// Looks inside the structure to check the existence of the
			// first character
			if (tree->index != NULL) {
				const int index_num = _msize(tree->index) / sizeof(*tree->index);

				// Case 1. Found character from stored elements
				for (int i = 0; i < index_num; i++) {
					if ((first_ascii - 33) == tree->index[i]) {
						if (last_str != NULL) {
							input_item(tree->char_table[tree->index[i]], last_str);
							return;
						}
						else {
							tree->char_table[tree->index[i]]->end = true;
							return;
						}
					}
				}
			}

			// Case 2. Create a space for this new input
			int mem_size = (tree->index == NULL) ? 0 : _msize(tree->index);
			const int index_num = mem_size / sizeof(Item_Tree);
				
			tree->index = (int*)realloc(tree->index, mem_size + sizeof(Item_Tree));
			if (tree->index == NULL) {
				fprintf(stderr, "Fatal Error: Out of memory!\n");
				return;
			}

			tree->index[index_num] = first_ascii - 33;
			tree->char_table[tree->index[index_num]] = initialize();
			if (last_str != NULL) {
				input_item(tree->char_table[tree->index[index_num]], last_str);
				return;
			}
			else {
				tree->char_table[tree->index[index_num]]->end = true;
				return;
			}
		}
	}
}