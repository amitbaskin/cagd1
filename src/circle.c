#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"
#include "circle.h"


/******************************************************************************
* rotate_pnt
******************************************************************************/
static void rotate_pnt( double      angle,
                        CAGD_POINT *p_in,
                        CAGD_POINT *p_rot_vec,
                        CAGD_POINT *p_out )
{
  CAGD_POINT rot_scaled;
  CAGD_POINT rot_x_in;

  double rot_m_in;
  double cos_ang = cos( angle );

  // first item
  copy_vec( p_in, p_out );
  scale_vec( cos_ang, p_out );

  // second item
  cross_vecs( p_rot_vec, p_out, &rot_x_in );
  scale_vec( sin( angle ), &rot_x_in );

  // third item
  copy_vec( p_rot_vec, &rot_scaled );
  rot_m_in = multiply_vecs( p_rot_vec, p_out );
  scale_vec( rot_m_in, &rot_scaled );
  scale_vec( 1 - cos_ang, &rot_scaled );

  // adding items
  add_vecs( p_out, &rot_x_in, p_out );
  add_vecs( p_out, &rot_scaled, p_out );
}


/******************************************************************************
* rotate_circ
******************************************************************************/
void rotate_circ( double      angle,
                  CAGD_POINT *p_in_arr,
                  CAGD_POINT *p_rot_vec,
                  CAGD_POINT *p_out_arr )
{
  for( int i = 0; i < NUM_OSC_PNTS; ++i )
  {
    rotate_pnt(  angle,
                &p_in_arr[ i ],
                 p_rot_vec,
                &p_out_arr[ i ] );
  }
}


/******************************************************************************
* get_center_pnt
******************************************************************************/
void get_center_pnt( double param, circle_data_t *rp_circle_data )
{
  CAGD_POINT N_axis;
  copy_vec( &rp_circle_data->N_axis, &N_axis );
  scale_vec(rp_circle_data->radius, &N_axis);

  eval_cur_crv( param, POSITION, &rp_circle_data->crv_pos );

  add_vecs( &rp_circle_data->crv_pos,
            &N_axis,
            &rp_circle_data->center );
}


/******************************************************************************
* eval_circ
******************************************************************************/
void eval_circ( double         param,
                circle_data_t *p_circle_data,
                CAGD_POINT    *rp_out )
{
  CAGD_POINT T_axis;
  CAGD_POINT N_axis;

  copy_vec( &p_circle_data->T_axis, &T_axis );
  copy_vec( &p_circle_data->N_axis, &N_axis );
  copy_vec( &p_circle_data->center, rp_out );

  scale_vec( p_circle_data->radius * sin( param ), &T_axis );
  scale_vec( p_circle_data->radius * cos( param ), &N_axis );

  add_vecs( rp_out, &T_axis, rp_out );
  add_vecs( rp_out, &N_axis, rp_out );
}


/******************************************************************************
* draw_circle
******************************************************************************/
int draw_circle( double         param,
                 circle_data_t *p_circle_data,
                 int           *p_seg_id,
                 CAGD_POINT    *rp_pnts )
{
  int ok = TRUE;

  ok = scale_not_zero( p_circle_data->radius ) &&
       vec_not_zero( &p_circle_data->T_axis )  &&
       vec_not_zero( &p_circle_data->N_axis )  &&
       rp_pnts != NULL;

  if( ok )
  {
    double jump = 2.0 * M_PI / ( NUM_OSC_PNTS - 1 );

    get_center_pnt( param, p_circle_data );

    for( int i = 0; i < NUM_OSC_PNTS; ++i )
    {
      CAGD_POINT pnt = { 0 };

      eval_circ(  i * jump,
                      p_circle_data,
                     &pnt );

      rp_pnts[ i ] = pnt;
    }

    if( *p_seg_id == K_NOT_USED )
      *p_seg_id = cagdAddPolyline( rp_pnts, NUM_OSC_PNTS );
    else
      cagdReusePolyline( *p_seg_id, rp_pnts, NUM_OSC_PNTS );
  }

  return !ok;
}
