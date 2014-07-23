#ifndef PREPROCESS_H_
#define PREPROCESS_H_

#include "itemproc.h"
#include "utility.h"
#include <stdio.h>

void file_to_matrix(FILE *file, struct Source *src);
void struct_to_matrix(struct Item_Tree *tree, struct Source *src);

#endif