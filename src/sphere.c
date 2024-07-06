#define _USE_MATH_DEFINES
#include <math.h>

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
  int is_error = !scale_not_zero( p_frenet->crvtr );

  if( is_error == FALSE )
  {
    double l_d1;
    double l_d1xd2;
    double numer_1;
    double numer_2;
    double denom_1;
    double denom_2;

    CAGD_POINT d1;
    CAGD_POINT d2;
    CAGD_POINT d3;
    CAGD_POINT d1xd2;
    CAGD_POINT d1xd3;

    eval_cur_crv( param, VELOCITY,     &d1 );
    eval_cur_crv( param, ACCELERATION, &d2 );
    eval_cur_crv( param, JERK,         &d3 );

    l_d1 = vec_len( &d1 );

    cross_vecs( &d1, &d2, &d1xd2 );
    l_d1xd2 = vec_len( &d1xd2 );

    cross_vecs( &d1, &d2, &d1xd3 );

    // calc first numerator
    numer_1 = multiply_vecs( &d1xd2, &d1xd3 );

    // calc second numerator
    numer_2 = multiply_vecs( &d1, &d2 );
    numer_2 = 3.0 * l_d1xd2 * numer_2;

    // calc first denominator
    denom_1 = l_d1xd2 * pow( l_d1, 3 );
    denom_1 = get_scale_inv_or_zero( denom_1 );

    // calc second denominator
    denom_2 = pow( l_d1, 5 );
    denom_2 = get_scale_inv_or_zero( denom_2 );

    *rp_out = denom_1 * numer_1 - denom_2 * numer_2;
  }

  return is_error;
}


/******************************************************************************
* get_sphere_vec
******************************************************************************/
int get_sphere_vec( double      param,
                    frenet_t   *p_frenet,
                    double     *rp_radius,
                    CAGD_POINT *rp_out )
{
  double d_crvtr;

  int is_error = get_crvtr_derivative( param, p_frenet, &d_crvtr );

  if( is_error == FALSE )
  {
    CAGD_POINT N_vec;
    CAGD_POINT B_vec;

    // calc N axis
    copy_vec( &p_frenet->csys[ NN ],&N_vec );
    scale_div_vec( p_frenet->crvtr, &N_vec );

    // calc B axis
    copy_vec( &p_frenet->csys[ BB ], &B_vec );
    scale_vec( d_crvtr, &B_vec );
    scale_div_vec( pow( p_frenet->crvtr, 2 ) * p_frenet->trsn, &B_vec );

    // calc final result
    diff_vecs( &N_vec, &B_vec, rp_out );

    *rp_radius = vec_len( rp_out );
    scale_div_vec( *rp_radius, rp_out );
  }

  return is_error;
}


/******************************************************************************
* draw_init_sphere_circle
******************************************************************************/
int draw_init_sphere_circle( double      param,
                             frenet_t   *p_frenet,
                             CAGD_POINT *rp_pnts )
{
  CAGD_POINT center_vec;

  double radius;

  int is_error = get_sphere_vec( param, p_frenet, &radius, &center_vec );

  if( is_error == FALSE )
  {
    circle_data_t circle_data;
    circle_data.radius = 1.0;
    copy_vec( &p_frenet->csys[ TT ], &circle_data.T_axis );
    copy_vec( &center_vec, &circle_data.N_axis );

    set_sphere_color();

    is_error = draw_circle( param,
                            &circle_data,
                            &cur_crv.sphere_segs[ 0 ],
                            rp_pnts );
  }

  return is_error;
}
