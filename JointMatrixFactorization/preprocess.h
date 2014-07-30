#ifndef PREPROCESS_H_
#define PREPROCESS_H_

#include "itemproc.h"
#include "utility.h"
#include <stdio.h>

int get_assigned(FILE *file, char *name, struct Source *src);
void get_dimension(FILE *file, struct Source *src);
int file_to_matrix(FILE *file, struct Source *src);

#endif