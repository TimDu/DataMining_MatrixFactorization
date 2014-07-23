#ifndef ITEM_PROCESS_H_
#define ITEM_PROCESS_H_

#include "utility.h"

struct Item_Tree;

struct Item_Tree* initialize();
void input_item(struct Item_Tree *tree, char *item);
void get_items(struct Item_Tree *tree, struct Item **items);

#endif