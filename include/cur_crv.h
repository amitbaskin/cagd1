#pragma once

#include "expr2tree.h"
#include "cagd.h"


#define SPACE_DIM        3
#define DERIVATIVE_LEVEL 4
#define K_NOT_USED      -1
#define NUM_SAMPS        200


enum derivative_level
{
  POSITION     = 0,
  VELOCITY     = 1,
  ACCELERATION = 2,
  JERK         = 3
};


enum trees
{
  X_TREE = 0,
  Y_TREE = 1,
  Z_TREE = 2
};


typedef struct
{
  int            defined;
  e2t_expr_node *trees[ SPACE_DIM ][ DERIVATIVE_LEVEL ];
  double         domain[ 2 ];
  int            frenet_segs[ 3 ];
  int            osc_circ_seg;
  int            helix_seg;
  int            evolute_seg;
  int            offset_seg;
} crv_t;

/*
  trees[ 3 ][ 4 ]:

  x | x' | x'' | x'''
  --------------------
  y | y' | y'' | y'''
  --------------------
  z | z' | z'' | z'''
*/

crv_t cur_crv;



void print_err( char *p_str );

void eval_cur_crv( double param, int d_level, CAGD_POINT *rp_out );

int draw_cur_crv( int num_pnts );

void clean_cur_crv();
