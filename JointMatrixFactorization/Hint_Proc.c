#include "itemproc.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

int _find_index(Item*, char*, int, int);

/*
	Function: insert_item
	--------------------
	Stores directly new item names into Source structure.

	Parameter:
	sName - source name
	src - source structure
	item - item name
 */
void insert_item(char *sName, Source *src, char* item)
{
	if (src->items->length > 0) {
		int endInd = src->items->length - 1;
		int index = _find_index(src->items, item, 0, endInd);

		if (index >= 0) {
			// Store new item
			if (endInd < (src->K - 1)) {
				for (int i = endInd + 1; i > index; --i) {
					src->items[i].name = src->items[i - 1].name;
				}
				src->items[index].name =(char*)
					malloc((strlen(item) + 1) * sizeof(char));
				strcpy_s(src->items[index].name,
					strlen(item) * sizeof(char) + 1, item);
				src->items->length++;
			}
			else {
				printf("Error: Item names in Source %s is out of the "
					"bound set in the first row, some items might "
					"be missed\n", sName);
				return;
			}
		}
	}
	else {
		src->items->name = (char*)malloc((strlen(item) + 1) * sizeof(char));
		strcpy_s(src->items->name, strlen(item) * sizeof(char)+1, item);
		src->items->length++;
	}
}

/*
	Function: _find_index
	---------------------
	Internal function. Finds the position of this item name in
	source structure should be inserted to, which is aligned in
	ascending order.

	Parameter:
	items - items in source structure
	item - the item to be processed
	start - start index of items
	end - end index of items

	Returns:
	position that should be inserted to. If the position has
	been taken by other items in the structure, shift those
	items to higher index.
	If -1 is returned, it means source stucture has contained
	this item.
*/
int _find_index(Item *items, char *item, int start, int end)
{
	// First, special cases
	if (start == end) {
		if (strcmp(items[start].name, item) > 0) {
			return start;
		}
		else if (strcmp(items[start].name, item) < 0) {
			return start + 1;
		}
		else {
			return -1;
		}
	}
	if ((end - start) == 1) {
		if (strcmp(items[end].name, item) > 0) {
			if (strcmp(items[start].name, item) < 0) {
				return end;
			}
			else if (strcmp(items[start].name, item) > 0) {
				return start;
			}
			else {
				return -1;
			}
		}
		else {
			return end;
		}
	}

	// General case
	int mid = (end + start) / 2;
	char *cmp_item = items[mid].name;

	if (strcmp(cmp_item, item) > 0) {
		return _find_index(items, item, start, mid);
	}
	else if (strcmp(cmp_item, item) < 0) {
		return _find_index(items, item, mid + 1, end);
	}
	else {
		return -1;
	}
}