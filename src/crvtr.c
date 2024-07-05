#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"


/******************************************************************************
* eval_circ
******************************************************************************/
static void eval_circ( double            param,
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
* get_crvtr_data
******************************************************************************/
void get_crvtr_data( double        param,
                     frenet_t     *p_frenet,
                     crvtr_data_t *rp_crvtr_data )
{
  rp_crvtr_data->radius = scale_not_zero( p_frenet->crvtr ) ?
                           1 / p_frenet->crvtr : 0.0;

  eval_cur_crv( param, POSITION, &rp_crvtr_data->crv_pos );
  copy_vec( &p_frenet->csys[ NN ], &rp_crvtr_data->vec );
  scale_vec( rp_crvtr_data->radius, &rp_crvtr_data->vec );

  add_vecs( &rp_crvtr_data->crv_pos,
            &rp_crvtr_data->vec,
            &rp_crvtr_data->center );
}


/******************************************************************************
* draw_osc_circle
******************************************************************************/
int draw_osc_circle( double param, frenet_t *p_frenet )
{
  int ok = TRUE;

  CAGD_POINT *pnts =
    ( CAGD_POINT * ) malloc( sizeof( CAGD_POINT ) * ( NUM_OSC_PNTS + 2 ) );

  double radius = scale_not_zero( p_frenet->crvtr ) ?
                  1 / p_frenet->crvtr : 0.0;

  ok = scale_not_zero( radius )               &&
       vec_not_zero( &p_frenet->csys[ TT ] ) &&
       vec_not_zero( &p_frenet->csys[ NN ] ) &&
       pnts != NULL;

  if( ok )
  {
    crvtr_data_t crvtr_data;

    get_crvtr_data( param, p_frenet, &crvtr_data );

    double jump = ( double ) 1 / NUM_OSC_PNTS;

    eval_cur_crv( param, POSITION, &crvtr_data.crv_pos );
    copy_vec( &p_frenet->csys[ NN ], &crvtr_data.vec );
    scale_vec( radius, &crvtr_data.vec );
    add_vecs( &crvtr_data.crv_pos, &crvtr_data.vec, &crvtr_data .center);

    for( int i = 0; i < NUM_OSC_PNTS + 1; ++i )
    {
      CAGD_POINT pnt = { 0 };

      eval_circ( i * jump * 2 * M_PI,
                 radius,
                 &crvtr_data.center,
                 p_frenet,
                 &pnt );

      pnts[ i ] = pnt;

      //cagdAddPoint( &pnt ); // temporary for debug
      //cagdRedraw(); // temporary for debug
    }

    set_circ_color();

    if( cur_crv.osc_circ_seg == K_NOT_USED )
      cur_crv.osc_circ_seg = cagdAddPolyline( pnts, NUM_OSC_PNTS + 1 );
    else
      cagdReusePolyline( cur_crv.osc_circ_seg, pnts, NUM_OSC_PNTS + 1 );

    free( pnts );
  }

  return !ok;
}
