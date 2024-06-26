#ifndef PARSE_FILE_H
#define PARSE_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "expr2tree.h"


#define SPACE_DIM 3
#define DERIVATIVE_DEPTH 4


typedef struct
{
  int defined;
  e2t_expr_node *trees[ SPACE_DIM ][ DERIVATIVE_DEPTH ];
  double domain[ 2 ];
} crv_t;


static crv_t cur_crv;

// Function to parse the file
void parse_file( int x, int y, void *p_data );

#endif // PARSE_FILE_H
