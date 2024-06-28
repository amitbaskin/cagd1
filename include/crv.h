#pragma once

#include "expr2tree.h"
#include "cagd.h"

#define SPACE_DIM 3
#define DERIVATIVE_DEPTH 4

typedef struct
{
  int defined;
  e2t_expr_node *trees[SPACE_DIM][DERIVATIVE_DEPTH];
  double domain[2];
} crv_t;

/*
    trees[3][4]:

            x |  x' |  x'' |  x'''
            ----------------------
            y |  y' |  y'' |  y'''
            ----------------------
            z |  z' |  z'' |  z'''
*/

void draw_cur_crv( int num_segments );
CAGD_POINT eval_cur_crv_at_param( double param ); // assuming 3 dimentions because CAGD_POINT is 3D

crv_t cur_crv;
