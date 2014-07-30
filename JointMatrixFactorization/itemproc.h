#ifndef ITEM_PROCESS_H_
#define ITEM_PROCESS_H_

#include "utility.h"
#include <stdbool.h>

/*	Process no hint sources	*/

struct Item_Tree;

struct Item_Tree* initialize();
// Input items from source file to Item_Tree structure
void input_item(struct Item_Tree *tree, char *item);
// Retrieve items from Item_Tree structure
void get_items(struct Item_Tree *tree, struct Item **items);

/*	Process hint sources	*/

// Directly insert an item from source file to source storing structure
int insert_item(char *name, struct Source *src, char *item);

#endif