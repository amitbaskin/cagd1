#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"
#include "circle.h"
#include "color.h"


/******************************************************************************
* get_crvtr_derivative
******************************************************************************/
static int get_crvtr_derivative( double    param,
                                 frenet_t *p_frenet,
                                 double   *rp_out )
{
  int is_error = scale_not_zero( p_frenet->crvtr ) ? FALSE : TRUE;

  if( is_error == FALSE )
  {
    frenet_t frenet_1 = { 0 };
    frenet_t frenet_2 = { 0 };

    double crvtr_diff_1 = 0.0;
    double crvtr_diff_2 = 0.0;
    double crvtr_diff_3 = 0.0;

    double eps = 0.01;
    double param_1 = param - eps;
    double param_2 = param + eps;

    if( IS_DEBUG )
    {
      is_error = calc_frenet( param_1, &frenet_1 );
      is_error = calc_frenet( param_2, &frenet_2 ) || is_error;

      crvtr_diff_1 = ( p_frenet->crvtr - frenet_1.crvtr ) / eps;
      crvtr_diff_2 = ( frenet_2.crvtr - p_frenet->crvtr ) / eps;
      crvtr_diff_3 = ( frenet_2.crvtr - frenet_1.crvtr ) / ( 2 * eps );

      /**rp_out = fabs( ( crvtr_diff_2 - crvtr_diff_1 ) / 2 );*/
    }

    double l_d1    = 0.0;
    double l_d1xd2 = 0.0;
    double numer_1 = 0.0;
    double numer_2 = 0.0;
    double denom_1 = 0.0;
    double denom_2 = 0.0;

    CAGD_POINT d1    = { 0 };
    CAGD_POINT d2    = { 0 };
    CAGD_POINT d3    = { 0 };
    CAGD_POINT d1xd2 = { 0 };
    CAGD_POINT d1xd3 = { 0 };

    eval_cur_crv( param, VELOCITY,     &d1 );
    eval_cur_crv( param, ACCELERATION, &d2 );
    eval_cur_crv( param, JERK,         &d3 );
    l_d1 = vec_len( &d1 );

    cross_vecs( &d1, &d2, &d1xd2 );
    l_d1xd2 = vec_len( &d1xd2 );

    cross_vecs( &d1, &d3, &d1xd3 );

    // calc first numerator
    numer_1 = multiply_vecs( &d1xd2, &d1xd3 );

    // calc second numerator
    numer_2 = multiply_vecs( &d1, &d2 );
    numer_2 *= 3.0 * l_d1xd2;

    // calc first denominator
    denom_1 = l_d1xd2 * pow( l_d1, 3 );
    is_error = get_scale_inv_or_zero( denom_1, &denom_1 );

    if( is_error == FALSE )
    {
      // calc second denominator
      denom_2 = pow( l_d1, 5 );
      is_error = get_scale_inv_or_zero( denom_2, &denom_2 );
    }

    if( is_error == FALSE )
    {
      *rp_out = denom_1 * numer_1 - denom_2 * numer_2;
      *rp_out /= l_d1;
    }
  }

  return is_error;
}


/******************************************************************************
* get_sphere_vec
******************************************************************************/
static int init_circ_data( double         param,
                           frenet_t      *p_frenet,
                           circle_data_t *rp_circle_data )
{
  CAGD_POINT N_vec = { 0 };
  CAGD_POINT B_vec = { 0 };

  double d_crvtr = 0.0;

  int is_error = !scale_not_zero( p_frenet->trsn ) ||
                  get_crvtr_derivative( param, p_frenet, &d_crvtr );

  if( is_error == FALSE )
  {
    // T axis
    copy_vec( &p_frenet->csys[ TT ], &rp_circle_data->T_axis );

    // calc B axis
    copy_vec( &p_frenet->csys[ BB ], &B_vec );

    /*is_error = scale_div_vec( p_frenet->trsn, &B_vec );*/ // check

    scale_vec( d_crvtr, &B_vec );
    is_error = scale_div_vec( pow( p_frenet->crvtr, 2 ) *
                              p_frenet->trsn, &B_vec );
  }

  if( is_error == FALSE )
  {
    // calc N axis
    copy_vec( &p_frenet->csys[ NN ], &N_vec );
    is_error = scale_div_vec( p_frenet->crvtr, &N_vec );
  }

  if( is_error == FALSE )
  {
    diff_vecs( &N_vec, &B_vec, &rp_circle_data->N_axis );
    rp_circle_data->radius = vec_len( &rp_circle_data->N_axis );
    is_error = normalize_vec( &rp_circle_data->N_axis );
  }

  /*if( is_error == FALSE )
  {
    add_vecs( &N_vec, &B_vec, &rp_circle_data->N_axis );
    rp_circle_data->radius = vec_len( &rp_circle_data->N_axis );
    is_error = normalize_vec( &rp_circle_data->N_axis );
  }*/ // check

  if( is_error == FALSE )
  {
    // crv pos and circ center
    get_center_pnt( param, rp_circle_data );
  }

  if( IS_DEBUG && is_error == FALSE )
  {
    printf( "d_crvtr: %f\n\n", d_crvtr );
    printf( "sphere radius: %f\n\n", rp_circle_data->radius );
  }

  return is_error;
}


/******************************************************************************
* draw_sphere
******************************************************************************/
int draw_sphere( double param, frenet_t *p_frenet )
{
  circle_data_t circle_data;

  int is_error = init_circ_data( param, p_frenet, &circle_data );

  if( is_error == FALSE )
  {
    set_sphere_color();

    double angle = 2.0 * M_PI / NUM_SPHERE_CIRCS;

    for( int i = 0; is_error == FALSE && i < NUM_SPHERE_CIRCS; ++i )
    {
      circle_data_t cur_circ_data = circle_data;

      CAGD_POINT *p_out = &cur_circ_data.T_axis;
      CAGD_POINT  in    = *p_out;
      CAGD_POINT  rot   = cur_circ_data.N_axis;

      rotate_vec( i * angle, &in, &rot, p_out );

      is_error = draw_circle( param,
                             &cur_circ_data,
                             &cur_crv.sphere_segs[ i ] );
    }
  }

  return is_error;
}
