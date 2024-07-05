#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"


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


int draw_osc_circle( double param, frenet_t *p_frenet )
{
  int ok = TRUE;

  CAGD_POINT *pnts =
    ( CAGD_POINT * ) malloc( sizeof( CAGD_POINT ) * ( NUM_OSC_PNTS + 2 ) );

  double radius = scale_not_zero( p_frenet->crvtr ) ? 1 / p_frenet->crvtr : 0.0;

  ok = scale_not_zero( radius )               &&
       vec_3d_not_zero( &p_frenet->csys[ TT ] ) &&
       vec_3d_not_zero( &p_frenet->csys[ NN ] ) &&
       pnts != NULL;

  if( ok )
  {
    CAGD_POINT crv_pnt;
    CAGD_POINT center;
    CAGD_POINT circ_vec;

    double jump = ( double ) 1 / NUM_OSC_PNTS;

    eval_cur_crv( param, POSITION, &crv_pnt );
    copy_vec( &p_frenet->csys[ NN ], &circ_vec );
    scale_vec( radius, &circ_vec );
    add_vecs( &crv_pnt, &circ_vec, &center );

    for( int i = 0; i < NUM_OSC_PNTS + 1; ++i )
    {
      CAGD_POINT pnt = { 0 };

      eval_circ( i * jump * 2 * M_PI,
                 radius,
                 &center,
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
