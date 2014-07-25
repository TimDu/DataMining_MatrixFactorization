#include "itemproc.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define START_ASCII 33
#define END_ASCII	126
#define TABLE_RANGE 94	// Consecutive ASCII character from code 33 to code 126.
#define SORT_FACTOR 100	// A threshold for merge sort

typedef struct Item_Tree {
	bool end;	// Records whether its precessor is a item with the last character
	int *index;	// Records the table entries that have item inputs on them
	struct Item_Tree *char_table[TABLE_RANGE];	// A table that stores other structures
												// by the order of ASCII code
} Item_Tree;

typedef struct Item_Chain {
	struct Item_Chain *next;
	char *name;
} Item_Chain;

Item_Chain* get_chain(Item_Tree*, Item_Chain*);
int* do_sort(int *, int, int);

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
		int ind = 0;
		int first_ascii = (int)item[ind];
		while ((first_ascii < START_ASCII) || (first_ascii) > END_ASCII) {
			// Find next valid character
			if (length > 1) {
				++ind;
				--length;
				first_ascii = (int)item[ind];
			}
		}
		char *last_str = NULL;

		// Check if more than one characters left in this item string
		if (length > 1) {
			last_str = (char*)malloc(length * sizeof(char));
			memcpy(last_str, &item[ind + 1], length - 1);
			last_str[length - 1] = '\0';
		}

		// Compares the first character with this current structure
		if ((first_ascii >= START_ASCII) && (first_ascii <= END_ASCII)) {
			// Looks inside the structure to check the existence of the
			// first character
			if (tree->index != NULL) {
				const int index_num = _msize(tree->index) / sizeof(*tree->index);

				// Case 1. Found character from stored elements
				for (int i = 0; i < index_num; ++i) {
					if ((first_ascii - START_ASCII) == tree->index[i]) {
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
			const int index_num = mem_size / sizeof(int);
			
			tree->index = (int*)realloc(tree->index, mem_size + sizeof(int));
			if (tree->index == NULL) {
				fprintf(stderr, "Fatal Error: Out of memory!\n");
				return;
			}

			tree->index[index_num] = first_ascii - START_ASCII;
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

/*
Function: get_items
--------------------
Gets an items array from this storing structure. This method uses a intermediate structure
to help storing items. To get the minimum available running time, it is not guranteed that
this item array would be aligned in any order.
Suppose k is the total number of items stored in this structure, the running time is O(k).

Parameter:
tree - storing structure
item - item array to be filled with
*/
void get_items(Item_Tree *tree, Item **items)
{
	int index = 0;
	Item_Chain *chain = (Item_Chain*)malloc(sizeof(Item_Chain));
	Item *item = *items;
	chain->name = NULL;

	get_chain(tree, chain);

	while (chain != NULL) {
		if (*items == NULL) {
			*items = (Item*)malloc(sizeof(Item));
			item = *items;
		}
		else {
			item = (Item*)realloc(item, _msize(item) + sizeof(Item));
		}
		item[index].name = (char*)malloc(_msize(chain->name));
		item[index].name = chain->name;
		chain = chain->next;
		++index;
	}
	*items = item;
}

/*
   Function: get_chain
   --------------------
   Internal function. It converts tree-like storing structure of items into
   a chain, so that get_items method is able to easily output all items.
   Suppose k is the total number of stored item names, its time consumption
   is O(k).

   Parameter:
   tree - Original storing tree
   chain - intermediate storing structure to be used

   Returns:
   updated chain structure, which is used in internal recursions.
 */
Item_Chain* get_chain(Item_Tree *tree, Item_Chain *chain)
{
	char *p_name;

	if (chain->name == NULL) {
		p_name = NULL;
	}
	else {
		const int str_index = _msize(chain->name) / sizeof(char) - 1;
		p_name = (char*)malloc(_msize(chain->name));
		memcpy(p_name, chain->name, _msize(chain->name));

		if (tree->end) {
			// Stores it if a new item name reaches end
			chain->name[str_index] = '\0';
			tree->end = false;	// Eliminates end bit when this item has been stored
		}
	}

	if (tree->index != NULL) {
		int *index = do_sort(tree->index, 0, _msize(tree->index) / sizeof(int) - 1);

		// Finds its child characters
		for (int i = 0; i < (int)(_msize(index) / sizeof(int)); ++i) {
			if (chain->name == NULL) {
				chain->name = (char*)malloc(2 * sizeof(char));
				chain->name[0] = (char)(index[i] + START_ASCII);
			}
			else {
				int str_index = (p_name == NULL) ?
					0 : _msize(p_name) / sizeof(char) - 1;

				if (chain->name[_msize(chain->name) / sizeof(char) - 1] == '\0') {
					// Initializes a new chain to store characters
					chain->next = (Item_Chain*)malloc(sizeof(Item_Chain));
					chain->next->next = NULL;
					chain = chain->next;

					if (p_name == NULL) {
						chain->name = (char*)malloc(2 * sizeof(char));
					}
					else {
						chain->name = (char*)malloc(_msize(p_name) + sizeof(char));
						memcpy(chain->name, p_name, _msize(p_name));
					}
				}
				else {
					chain->name = (char*)
						realloc(chain->name, _msize(chain->name) + sizeof(char));
				}

				// Appends this current character
				chain->name[str_index] = (char)(index[i] + START_ASCII);
			}

			chain = get_chain(tree->char_table[index[i]], chain);
		}
	}

	return chain;
}

/*
   Function: do_sort
   ------------------
   Internal function. Sorts index array in Item_Tree structure by content value in
   ascending order. This improves running time of get_chain method.

   Parameter:
   ary - An integer array
   start - Start index of this array to be sorted
   end - End index of this array to be sorted

   Returns:
   An sorted integer array
 */
int* do_sort(int *ary, int start, int end)
{
	int *result;

	if (ary != NULL) {
		// Return if array is not able to split
		if (start == end) {
			result = (int*)malloc(sizeof(int));
			result[0] = ary[start];
			return result;
		}

		const size = end - start + 1;
		result = (int*)malloc(size * sizeof(int));

		if ((end - start) > SORT_FACTOR) {
			// Merge sort if array is able to split
			int mid = (end + start) / 2;
			int *lAry = do_sort(ary, start, mid);
			int *rAry = do_sort(ary, mid + 1, end);
			int lIndex = 0;
			int rIndex = 0;

			const int lBound = (int)_msize(lAry) / sizeof(int);
			const int rBound = (int)_msize(rAry) / sizeof(int);

			for (; lIndex < lBound; ++lIndex) {
				for (; rIndex < rBound; ++rIndex) {
					if (lAry[lIndex] >= rAry[rIndex]) {
						result[lIndex + rIndex] = rAry[rIndex];
					}
					else {
						break;
					}
				}
				result[lIndex + rIndex] = lAry[lIndex];
			}

			for (; rIndex < rBound; ++rIndex) {
				result[lIndex + rIndex] = rAry[rIndex];
			}
		}
		else {
			// Insertion sort if array size is too small to split
			int temp_value;
			int temp_index;

			for (int i = start; i <= end; ++i) {
				result[i - start] = ary[i];
			}

			for (int i = 1; i <= (end - start); ++i) {
				temp_value = result[i];
				temp_index = i;

				for (int j = i - 1; j >= 0; --j) {
					if (result[temp_index] < result[j]) {
						result[temp_index] = result[j];
						result[j] = temp_value;
						temp_index = j;
					}
					else {
						break;
					}
				}
			}
		}

		return result;
	}
	else {
		return NULL;
	}
}