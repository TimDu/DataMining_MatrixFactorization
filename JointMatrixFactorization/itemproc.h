#ifndef ITEM_PROCESS_H_
#define ITEM_PROCESS_H_

#include "utility.h"

/*	Process no hint sources	*/
struct Item_Tree;

struct Item_Tree* initialize();
void input_item(struct Item_Tree *tree, char *item);
void get_items(struct Item_Tree *tree, struct Item **items);

/*	Process hint sources	*/
void push_item(char *name, struct Source *src, char *item);

#endif