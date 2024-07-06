#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"
#include "circle.h"


/******************************************************************************
* get_center_pnt
******************************************************************************/
void get_center_pnt( double param, circle_data_t *rp_circle_data )
{
  CAGD_POINT N_axis;
  copy_vec( &rp_circle_data->N_axis, &N_axis );
  scale_vec( rp_circle_data->radius, &N_axis );

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

  //normalize_vec( &T_axis );
  //normalize_vec( &N_axis );

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
                 int           *p_seg_id )
{
  int ok = TRUE;

  CAGD_POINT *p_pnts = ( CAGD_POINT * )malloc( sizeof( CAGD_POINT ) *
                                               NUM_OSC_PNTS );

  ok = scale_not_zero( p_circle_data->radius ) &&
       vec_not_zero( &p_circle_data->T_axis )  &&
       vec_not_zero( &p_circle_data->N_axis )  &&
       p_pnts != NULL;

  if( ok )
  {
    double jump = 2.0 * M_PI / ( NUM_OSC_PNTS - 1 );

    if( p_circle_data->is_center_defined == FALSE )
      get_center_pnt( param, p_circle_data );

    for( int i = 0; i < NUM_OSC_PNTS; ++i )
      eval_circ( i * jump, p_circle_data, &p_pnts[ i ] );

    if( *p_seg_id == K_NOT_USED )
      *p_seg_id = cagdAddPolyline( p_pnts, NUM_OSC_PNTS );
    else
      cagdReusePolyline( *p_seg_id, p_pnts, NUM_OSC_PNTS );
  }

  free( p_pnts );

  return !ok;
}
