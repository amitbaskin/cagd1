#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"
#include "circle.h"


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
    circle_data_t crvtr_data;

    get_circle_data( param, &radius, p_frenet, &crvtr_data );

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
