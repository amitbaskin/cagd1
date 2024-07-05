#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"
#include "circle.h"


/******************************************************************************
* eval_circ
******************************************************************************/
void eval_circ( double            param,
                double            radius,
                const CAGD_POINT *p_center,
                const frenet_t   *p_frenet,
                CAGD_POINT       *rp_out )
{
  CAGD_POINT T_axis;
  CAGD_POINT N_axis;

  copy_vec( &p_frenet->csys[ TT ], &T_axis );
  copy_vec( &p_frenet->csys[ NN ], &N_axis );
  copy_vec( p_center, rp_out );

  scale_vec( radius * sin( param ), &T_axis );
  scale_vec( radius * cos( param ), &N_axis );

  add_vecs( rp_out, &T_axis, rp_out );
  add_vecs( rp_out, &N_axis, rp_out );
}


/******************************************************************************
* get_circle_data
******************************************************************************/
void get_circle_data( double         param,
                      double        *p_radius,
                      CAGD_POINT    *p_vec,
                      frenet_t      *p_frenet,
                      circle_data_t *rp_crvtr_data )
{
  if( p_vec != NULL )
    copy_vec( p_vec, &rp_crvtr_data->vec );
  else
    copy_vec( &p_frenet->csys[ NN ], &rp_crvtr_data->vec );

  rp_crvtr_data->radius = p_radius != NULL ? *p_radius :
                          get_scale_inv_or_zero( p_frenet->crvtr );

  eval_cur_crv( param, POSITION, &rp_crvtr_data->crv_pos );
  scale_vec( rp_crvtr_data->radius, &rp_crvtr_data->vec );

  add_vecs( &rp_crvtr_data->crv_pos,
            &rp_crvtr_data->vec,
            &rp_crvtr_data->center );
}


/******************************************************************************
* rotate_pnt
******************************************************************************/
void rotate_pnt( double      angle,
                 CAGD_POINT *p_in,
                 CAGD_POINT *p_rot_vec,
                 CAGD_POINT *p_out )
{
  CAGD_POINT rot_x_in;
  CAGD_POINT rot_m_in;

  double cos_ang = cos( angle );

  copy_vec( p_in, p_out );
  scale_vec( cos_ang, p_out );

  cross_vecs( p_rot_vec, p_in, &rot_x_in );
  scale_vec( sin( angle ), &rot_x_in );

  multiply_vecs( p_rot_vec, p_in, &rot_m_in );
  multiply_vecs( p_rot_vec, &rot_m_in, &rot_m_in );
  scale_vec( 1 - cos_ang, &rot_m_in );

  add_vecs( p_out, &rot_x_in, &p_out );
  add_vecs( p_out, &rot_m_in, &p_out );
}


/******************************************************************************
* rotate_circ
******************************************************************************/
void rotate_circ( double      angle,
                  int         num_pnts,
                  CAGD_POINT *p_in_arr,
                  CAGD_POINT *p_rot_vec,
                  CAGD_POINT *p_out_arr )
{
  for( int i = 0; i < num_pnts; ++i )
  {
    CAGD_POINT rot_pnt;

    rotate_pnt( angle,
                &p_in_arr[ i ],
                p_rot_vec,
                &p_out_arr[ i ] );
  }
}

