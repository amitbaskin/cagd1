#ifndef PARSE_FILE_H
#define PARSE_FILE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "expr2tree.h"

#define MAX_LEN 256

typedef struct
{
  e2t_expr_node *xtree;
  e2t_expr_node *ytree;
  e2t_expr_node *ztree;
  double tmin;
  double tmax;
} crv_t;


static crv_t cur_crv;

// Function to parse the file
void parse_file( int x, int y, void *p_data );

#endif // PARSE_FILE_H
