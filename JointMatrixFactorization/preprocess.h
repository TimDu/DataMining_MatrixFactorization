#ifndef PREPROCESS_H_
#define PREPROCESS_H_

#include "itemproc.h"
#include "utility.h"
#include <stdio.h>

void get_dimension(FILE *file, struct Source *src);
void file_to_matrix(FILE *file, struct Source *src);

#endif