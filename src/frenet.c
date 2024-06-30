#include "frenet.h"
#include "vectors.h"
#include "color.h"


int frenet_anim_iteration = 0;

void calc_frenet( double param, frenet_t *frenet )
{
  CAGD_POINT d0;
  CAGD_POINT d1;
  CAGD_POINT d2;
  CAGD_POINT d3;
  CAGD_POINT d1xd2;
  CAGD_POINT d1d1_d2;
  CAGD_POINT d1d2_d1;
  CAGD_POINT d2_diff_d1;

  double d1d1;
  double d1d2;
  double l_d1;
  double l_d1xd2;
  double d3_mul_d1xd2;
  double l_d2_diff_d1;

  // calc pos
  eval_cur_crv( param, 0, &d0 );
  copy_vec( &d0, &frenet->pos );

  // calc T
  eval_cur_crv( param, 1, &d1 );
  copy_vec( &d1, &frenet->csys[ 0 ] );
  l_d1 = vec_len( &d1 );
  scale_vec( l_d1, &frenet->csys[ 0 ] );

  // calc B
  eval_cur_crv( param, 2, &d2 );
  cross_vecs( &d1, &d2, &d1xd2 );
  copy_vec( &d1xd2, &frenet->csys[ 2 ] );
  l_d1xd2 = vec_len( &d1xd2 );
  scale_vec( 1 / l_d1xd2, &frenet->csys[ 2 ] );

  // calc curvature
  frenet->crvtr = l_d1xd2 / pow( l_d1, 3 );

  // calc d1d1_d2
  d1d1 = multiply_vecs( &d1, &d1 );
  copy_vec( &d2, &d1d1_d2 );
  scale_vec( d1d1, &d1d1_d2 );

  // calc d1d2_d1
  d1d2 = multiply_vecs( &d1, &d2 );
  copy_vec( &d1, &d1d2_d1 );
  scale_vec( d1d2, &d1d2_d1 );

  // calc N
  diff_vecs( &d1d1_d2, &d1d2_d1, &d2_diff_d1 );
  scale_vec( 1 / ( l_d1 * l_d1xd2 ), &d2_diff_d1 );
  l_d2_diff_d1 = vec_len( &d2_diff_d1 );
  scale_vec( 1 / l_d2_diff_d1, &d2_diff_d1 );
  copy_vec( &d2_diff_d1, &frenet->csys[ 1 ] );

  // calc torsion
  eval_cur_crv( param, 3, &d3 );
  d3_mul_d1xd2 = multiply_vecs( &d3, &d1xd2 );
  frenet->trsn = d3_mul_d1xd2 / pow( l_d1xd2, 2 );
}


void draw_frenet( double param, frenet_t *frenet )
{
  CAGD_POINT T[ 2 ] = { frenet->pos };
  CAGD_POINT N[ 2 ] = { frenet->pos };
  CAGD_POINT B[ 2 ] = { frenet->pos };

  add_vecs( &frenet->pos, &frenet->csys[ 0 ], &T[ 1 ] );
  add_vecs( &frenet->pos, &frenet->csys[ 1 ], &N[ 1 ] );
  add_vecs( &frenet->pos, &frenet->csys[ 2 ], &B[ 1 ] );

  if( cur_crv.frenet_segs[ 0 ] == K_NOT_USED )
  {
    set_tan_color();
    cur_crv.frenet_segs[ 0 ] = cagdAddPolyline( T, 2 );
    set_norm_color();
    cur_crv.frenet_segs[ 1 ] = cagdAddPolyline( N, 2 );
    set_bi_color();
    cur_crv.frenet_segs[ 2 ] = cagdAddPolyline( B, 2 );
  }
  else
  {
    set_tan_color();
    cagdReusePolyline( cur_crv.frenet_segs[ 0 ], T, 2 );
    set_norm_color();
    cagdReusePolyline( cur_crv.frenet_segs[ 1 ], N, 2 );
    set_bi_color();
    cagdReusePolyline( cur_crv.frenet_segs[ 2 ], B, 2 );
  }

  cagdRedraw();

  set_default_color();
}

void frenet_anim_cb( int x, int y, PVOID userData )
{
  int anim_smoothness = 200;
  double jump = ( cur_crv.domain[1] - cur_crv.domain[0] ) / anim_smoothness;
  double param = cur_crv.domain[0] + jump * frenet_anim_iteration;

  if( param > cur_crv.domain[1] )
  {
    frenet_anim_iteration = 0;
    param = cur_crv.domain[0] + jump * frenet_anim_iteration;
  }

  frenet_t frenet;
  calc_frenet( param, &frenet );
  draw_frenet( param, &frenet );

  cagdRedraw();

  frenet_anim_iteration++;
}

void reset_frenet_anim_iteration()
{
  frenet_anim_iteration = 0;
}
