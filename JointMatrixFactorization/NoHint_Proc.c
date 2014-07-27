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
	int index_length;	// The length of index block
	struct Item_Tree *char_table[TABLE_RANGE];	// A table that stores other structures
												// by the order of ASCII code
} Item_Tree;

typedef struct Item_Chain {
	struct Item_Chain *next;
	char *name;
	int length;	// Length of string stored
} Item_Chain;

Item_Chain* get_chain(Item_Tree*, Item_Chain*);
int* do_sort(int *, int, int);

/*
	Function: initialize
	---------------------
	Creates a storing structure of items. This structure can 
	efficiently read and store items with the name consisted of
	ASCII codes ranging from 33 to 126. It also generates item
	array for all input items faster than normal coarsed way.

	Returns:
	An item storing structure for preprocessing
 */
Item_Tree* initialize()
{
	Item_Tree *tree = (Item_Tree*)malloc(sizeof(*tree));
	tree->index = NULL;
	tree->index_length = 0;
	tree->end = false;

	return tree;
}

/*
	Function: input_item
	---------------------
	Reads the first character of the passed name and stores it into the
	given storing structure if this provided character does not exist 
	in this structure. The existence is compared literally and recursively
	with the elements stored in this structure. Therefore, this is a 
	recrusive method.
	Suppose m is the length of item name, C is the constant of table size,
	which is 94, it takes O(m) time to finish.

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
				const int index_num = tree->index_length;

				// Case 1. Found character from stored elements
				for (int i = 0; i < index_num; ++i) {
					if ((first_ascii - START_ASCII) == tree->index[i]) {
						if (last_str != NULL) {
							//printf("%c", first_ascii);
							input_item(tree->char_table[tree->index[i]], last_str);
							return;
						}
						else {
							//printf("%c\n", first_ascii);
							tree->char_table[tree->index[i]]->end = true;
							return;
						}
					}
				}
			}

			// Case 2. Create a space for this new input
			const int index_num = tree->index_length;
			int mem_size = index_num * sizeof(int);
			
			tree->index = (int*)realloc(tree->index, mem_size + sizeof(int));
			if (tree->index == NULL) {
				fprintf(stderr, "Fatal Error: Out of memory!\n");
				return;
			}

			tree->index_length++;
			tree->index[index_num] = first_ascii - START_ASCII;
			tree->char_table[tree->index[index_num]] = initialize();
			if (last_str != NULL) {
				//printf("%c", first_ascii);
				input_item(tree->char_table[tree->index[index_num]], last_str);
				return;
			}
			else {
				//printf("%c\n", first_ascii);
				tree->char_table[tree->index[index_num]]->end = true;
				return;
			}
		}
	}
}

/*
	Function: get_items
	--------------------
	Gets an items array from this storing structure. This method
	uses a intermediate structure to help storing items. To get the
	minimum available running time.
	Suppose k is the total number of items stored in this structure, 
	the running time is O(k).

	Parameter:
	tree - storing structure
	item - item array to be filled with
*/
void get_items(Item_Tree *tree, Item **items)
{
	int index = 0;
	Item_Chain *chain_head = (Item_Chain*)malloc(sizeof(Item_Chain));
	Item_Chain *pchain = (Item_Chain*)malloc(sizeof(Item_Chain));
	Item *item = *items;
	pchain->next = NULL;
	pchain->name = NULL;
	pchain->length = 0;
	chain_head->next = pchain;

	get_chain(tree, pchain);
	pchain = chain_head->next;
	free(chain_head);

	while (pchain != NULL) {
		if (*items == NULL) {
			*items = (Item*)malloc(sizeof(Item));
			item = *items;
			item->length = 1;
		}
		else {
			item = (Item*)realloc(item, (item[0].length + 1) * sizeof(Item));
			item[0].length++;
		}
		item[index].name = (char*)malloc(sizeof(char) * pchain->length);
		item[index].name = pchain->name;
		chain_head = pchain;
		pchain = pchain->next;

		free(chain_head);
		++index;
	}
	*items = item;
}

/*
	Function: get_chain
	--------------------
	Internal function. It converts tree-like storing structure of items into
	a chain, so that get_items method is able to easily output all items. It
	also destroy the item tree once items are stored in chain structure.
	Suppose k is the total number of stored item names, its time consumption
	is O(k).

	Parameter:
	tree - Original storing tree
	chain - intermediate storing structure to be used

	Returns:
	updated chain structure
 */
Item_Chain* get_chain(Item_Tree *tree, Item_Chain *chain)
{
	char *p_name;
	int p_length;

	if (chain->name == NULL) {
		p_name = NULL;
		p_length = 0;
	}
	else {
		const int str_index = chain->length - 1;
		p_name = (char*)malloc(sizeof(char) * chain->length);
		memcpy(p_name, chain->name, sizeof(char) * chain->length);
		p_length = chain->length;

		if (tree->end) {
			// Stores it if a new item name reaches end
			chain->name[str_index] = '\0';
			tree->end = false;	// Eliminates end bit when this item has been stored
		}
	}

	if (tree->index != NULL) {
		int *index = do_sort(tree->index, 0, tree->index_length - 1);

		// Finds its child characters
		for (int i = 0; i < tree->index_length; ++i) {
			if (chain->name == NULL) {
				chain->name = (char*)malloc(2 * sizeof(char));
				chain->name[0] = (char)(index[i] + START_ASCII);
				chain->length = 2;
			}
			else {
				int str_index = (p_name == NULL) ? 0 : p_length - 1;

				if (chain->name[chain->length - 1] == '\0') {
					// Initializes a new chain to store characters
					chain->next = (Item_Chain*)malloc(sizeof(Item_Chain));
					chain->next->next = NULL;
					chain = chain->next;

					if (p_name == NULL) {
						chain->name = (char*)malloc(2 * sizeof(char));
						chain->length = 2;
					}
					else {
						chain->name = 
							(char*)malloc((p_length + 1) * sizeof(char));
						memcpy(chain->name, p_name, p_length * sizeof(char));
						chain->length = p_length + 1;
					}
				}
				else {
					chain->name = (char*)
						realloc(chain->name, (chain->length++ + 1) * sizeof(char));
				}

				// Appends this current character
				chain->name[str_index] = (char)(index[i] + START_ASCII);
			}
			chain = get_chain(tree->char_table[index[i]], chain);
		}
	}

	free(p_name);
	free(tree->index);
	free(tree);
	return chain;
}

/*
	Function: do_sort
	------------------
	Internal function. Sorts index array in Item_Tree structure 
	by content value in ascending order. This improves running 
	time of get_chain method.

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

			const int lBound = mid - start + 1;
			const int rBound = end - mid;

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