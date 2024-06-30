#pragma once

#include "expr2tree.h"
#include "cagd.h"


#define SPACE_DIM 3
#define DERIVATIVE_DEPTH 4
#define K_NOT_USED -1
#define NUM_SAMPS 70


typedef struct
{
  int defined;
  e2t_expr_node *trees[ SPACE_DIM ][ DERIVATIVE_DEPTH ];
  double domain[ 2 ];
  int num_samples;
  int frenet_segs[ 3 ];
} crv_t;

/*
  trees[ 3 ][ 4 ]:

  x |  x' |  x'' |  x'''
  ----------------------
  y |  y' |  y'' |  y'''
  ----------------------
  z |  z' |  z'' |  z'''
*/

crv_t cur_crv;

void eval_cur_crv( double param, int d_level, CAGD_POINT *out );
void draw_cur_crv( int num_pnts );
void clean_cur_crv();
