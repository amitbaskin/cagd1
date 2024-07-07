#include <stdio.h>
#include "frenet.h"
#include "vectors.h"
#include "color.h"
#include "crvtr.h"
#include "trsn.h"
#include "circle.h"
#include "sphere.h"
#include "menus.h"


extern int frenet_anim_smoothness;
extern int frenet_anim_running;
extern HMENU g_anim_settings_menu;
extern HMENU g_animation_menu;

int frenet_anim_iteration = 0;


/******************************************************************************
* calc_frenet
******************************************************************************/
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

  double tmp;
  double d1d1;
  double d1d2;
  double l_d1;
  double l_d1xd2;
  double d3_mul_d1xd2;
  double l_d2_diff_d1;

  printf( "param: %f\n", param );

  // calc pos
  eval_cur_crv( param, POSITION, &d0 );
  copy_vec( &d0, &frenet->pos );

  // calc T
  eval_cur_crv( param, VELOCITY, &d1 );
  copy_vec( &d1, &frenet->csys[ TT ] );
  l_d1 = vec_len( &d1 );
  scale_div_vec( l_d1, &frenet->csys[ TT ] );

  // calc B
  eval_cur_crv( param, ACCELERATION, &d2 );
  cross_vecs( &d1, &d2, &d1xd2 );
  copy_vec( &d1xd2, &frenet->csys[ BB ] );
  l_d1xd2 = vec_len( &d1xd2 );
  scale_div_vec( l_d1xd2, &frenet->csys[ BB ] );

  // calc curvature
  get_scale_inv_or_zero( pow( l_d1, 3 ), &tmp );
  frenet->crvtr = tmp * l_d1xd2;

  printf( "curvature: %f\n", frenet->crvtr );

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
  scale_div_vec( l_d1 * l_d1xd2, &d2_diff_d1 );
  l_d2_diff_d1 = vec_len( &d2_diff_d1 );
  scale_div_vec( l_d2_diff_d1, &d2_diff_d1 );
  copy_vec( &d2_diff_d1, &frenet->csys[ NN ] );

  // calc torsion
  eval_cur_crv( param, JERK, &d3 );
  d3_mul_d1xd2 = multiply_vecs( &d3, &d1xd2 );
  get_scale_inv_or_zero( pow( l_d1xd2, 2 ), &tmp );
  frenet->trsn = d3_mul_d1xd2 * tmp;

  printf( "torsion: %f\n\n", frenet->trsn );
}


/******************************************************************************
* draw_frenet
******************************************************************************/
void draw_frenet( double param, frenet_t *frenet )
{
  CAGD_POINT T_axis[ 2 ] = { frenet->pos };
  CAGD_POINT N_axis[ 2 ] = { frenet->pos };
  CAGD_POINT B_axis[ 2 ] = { frenet->pos };

  add_vecs( &frenet->pos, &frenet->csys[ TT ], &T_axis[ 1 ] );
  add_vecs( &frenet->pos, &frenet->csys[ NN ], &N_axis[ 1 ] );
  add_vecs( &frenet->pos, &frenet->csys[ BB ], &B_axis[ 1 ] );

  if( cur_crv.frenet_segs[ TT ] == K_NOT_USED )
  {
    set_tan_color();
    cur_crv.frenet_segs[ TT ] = cagdAddPolyline( T_axis, 2 );
    set_norm_color();
    cur_crv.frenet_segs[ NN ] = cagdAddPolyline( N_axis, 2 );
    set_bi_color();
    cur_crv.frenet_segs[ BB ] = cagdAddPolyline( B_axis, 2 );
  }
  else
  {
    set_tan_color();
    cagdReusePolyline( cur_crv.frenet_segs[ TT ], T_axis, 2 );
    set_norm_color();
    cagdReusePolyline( cur_crv.frenet_segs[ NN ], N_axis, 2 );
    set_bi_color();
    cagdReusePolyline( cur_crv.frenet_segs[ BB ], B_axis, 2 );
  }

  set_default_color();
}


/******************************************************************************
* frenet_anim_cb
******************************************************************************/
void frenet_anim_cb( int x, int y, PVOID userData )
{
  if( are_all_anim_menus_unchecked() )
  {
    stop_frenet_animation();
    return;
  }

  frenet_t frenet;

  double jump = ( cur_crv.domain[ 1 ] - cur_crv.domain[ 0 ] ) /
    frenet_anim_smoothness;

  double param = cur_crv.domain[ 0 ] + jump * frenet_anim_iteration;

  if( param > cur_crv.domain[ 1 ] )
  {
    frenet_anim_iteration = 0;
    param = cur_crv.domain[ 0 ] + jump * frenet_anim_iteration;
  }

  free_all_segs( FALSE );

  calc_frenet( param, &frenet );

  if( is_menu_checked( g_anim_settings_menu, CAGD_ANIM_FRENET_MENU ) )
    draw_frenet( param, &frenet );

  if( is_menu_checked( g_anim_settings_menu, CAGD_ANIM_OSCULATING_MENU ) )
    draw_osc_circle( param, &frenet );

  if( is_menu_checked( g_anim_settings_menu, CAGD_ANIM_TORSION_MENU ) )
    draw_helix( param, &frenet );

  if( is_menu_checked( g_anim_settings_menu, CAGD_ANIM_SPHERE_MENU ) )
    draw_sphere( param, &frenet );

  set_default_color();

  cagdRedraw();

  ++frenet_anim_iteration;
}

/******************************************************************************
* frenet_start_animation
******************************************************************************/
int frenet_start_animation()
{
  int is_error = 0;

  if( cur_crv.defined == 0 )
  {
    print_err( "Please Load a Curve first" );
    is_error = 1;
  }
  if( is_error == 0 )
  {
    frenet_anim_running = 1;
    cagdRegisterCallback( CAGD_TIMER, frenet_anim_cb, NULL );
  }

  EnableMenuItem( g_animation_menu,
                  CAGD_FRENET_ANIM_STOP,
                  frenet_anim_running ? MF_ENABLED : MF_GRAYED );

  EnableMenuItem( g_animation_menu,
                  CAGD_FRENET_ANIM_START,
                  frenet_anim_running ? MF_GRAYED : MF_ENABLED );

  return is_error;
}

/******************************************************************************
* reset_frenet_anim_iteration
******************************************************************************/
void reset_frenet_anim_iteration()
{
  frenet_anim_iteration = 0;
}

/******************************************************************************
* stop_frenet_animation
******************************************************************************/
void stop_frenet_animation()
{
  cagdRegisterCallback( CAGD_TIMER, NULL, NULL );

  free_all_segs( FALSE );

  cagdRedraw();
  reset_frenet_anim_iteration();
  frenet_anim_running = 0;

  EnableMenuItem( g_animation_menu,
                  CAGD_FRENET_ANIM_STOP,
                  frenet_anim_running ? MF_ENABLED : MF_GRAYED );

  EnableMenuItem( g_animation_menu,
                  CAGD_FRENET_ANIM_START,
                  frenet_anim_running ? MF_GRAYED : MF_ENABLED );
}
