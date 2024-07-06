#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"
#include "circle.h"


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
    double frac_1;
    double frac_2;

    CAGD_POINT d1;
    CAGD_POINT d2;
    CAGD_POINT d3;
    CAGD_POINT d4;
    CAGD_POINT d1xd2;
    CAGD_POINT d1xd3;
    CAGD_POINT d1xd4;
    CAGD_POINT d2xd3;

    eval_cur_crv( param, VELOCITY, &d1 );
    eval_cur_crv( param, ACCELERATION, &d2 );
    eval_cur_crv( param, JERK, &d3 );
    eval_cur_crv( param, D4, &d4 );

    l_d1 = vec_len( &d1 );

    cross_vecs( &d1, &d2, &d1xd2 );
    l_d1xd2 = vec_len( &d1xd2 );

    cross_vecs( &d1, &d4, &d1xd3 );
    cross_vecs( &d1, &d4, &d1xd4 );
    cross_vecs( &d1, &d4, &d2xd3 );

    // calc first numerator
    add_vecs( &d2xd3, &d1xd4, &d2xd3 );
    numer_1 = multiply_vecs( &d1xd3, &d2xd3 );

    // calc second numerator
    numer_2 = multiply_vecs( &d2, &d3 );
    numer_2 = 3 * numer_2 * l_d1xd2;

    // calc first denominator
    denom_1 = l_d1 * pow( l_d1xd2, 2 );
    denom_1 = get_scale_inv_or_zero( denom_1 );

    // calc second denominator
    denom_2 = pow( l_d1, 2 );
    denom_2 = get_scale_inv_or_zero( denom_2 );

    // calc fracs
    frac_1 = denom_1 * numer_1;
    frac_2 = denom_2 * numer_2;

    *rp_out = frac_1 - frac_2;
  }

  return is_error;
}


/******************************************************************************
* get_sphere_vec
******************************************************************************/
int get_sphere_vec( double      param,
                    frenet_t   *p_frenet,
                    CAGD_POINT *rp_out )
{
  double d_crvtr;

  CAGD_POINT N_vec;
  CAGD_POINT B_vec;

  int is_error = get_crvtr_derivative( param, p_frenet, &d_crvtr );

  if( is_error == FALSE )
  {
    copy_vec( &p_frenet->csys[ NN ], rp_out );
    scale_div_vec( p_frenet->crvtr, &N_vec );
    copy_vec( &N_vec, rp_out );

    copy_vec( &p_frenet->csys[ BB ], &B_vec );
    scale_vec( d_crvtr, &B_vec );
    scale_div_vec( pow( p_frenet ->crvtr, 2 ) * p_frenet->trsn, &B_vec );

    diff_vecs( &N_vec, &B_vec, rp_out );
  }

  return is_error;
}
