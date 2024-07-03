#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"


static void rotate_xy( double deg, double *xx, double *yy )
{
  double in_x = *xx;
  double in_y = *yy;

  double cos_deg = cos( deg );
  double sin_deg = sin( deg );

  *xx = in_x * cos_deg - in_y * sin_deg;
  *yy = in_x * sin_deg + in_y * cos_deg;
}


int draw_osc_circle( double param, frenet_t *frenet )
{
  CAGD_POINT *circ_pnts;
  CAGD_POINT crv_pnt;
  CAGD_POINT center_pnt;
  CAGD_POINT circ_vec;

  circ_pnts = ( CAGD_POINT * ) malloc( sizeof( CAGD_POINT ) * ( NUM_OSC_PNTS + 1 ) );
  double deg = ( 360 / NUM_OSC_PNTS ) * ( M_PI / 180 );

  double radius = frenet->crvtr != 0 ? 1 / frenet->crvtr : 0.0;
  double cur_x = 0.0;
  double cur_y = radius;

  if( scale_not_zero( radius )              &&
      vec_3d_not_zero( &frenet->csys[ 0 ] ) &&
      vec_3d_not_zero( &frenet->csys[ 1 ] ) &&
      circ_pnts != NULL )
  {
    eval_cur_crv( param, 0, &crv_pnt );

    copy_vec( &frenet->csys[ 1 ], &circ_vec );
    scale_vec( radius, &circ_vec );
    add_vecs( &crv_pnt, &frenet->csys[ 1 ], &center_pnt );

    for( int i = 0; i < NUM_OSC_PNTS; ++i )
    {
      CAGD_POINT cur_tan;
      CAGD_POINT cur_normal;
      CAGD_POINT cur_pnt;

      double cur_deg = i * deg;

      rotate_xy( i * deg, &cur_x, &cur_y );

      copy_vec( &frenet->csys[ 0 ], &cur_tan );
      scale_vec( cur_x, &cur_tan );

      copy_vec( &frenet->csys[ 1 ], &cur_normal );
      scale_vec( cur_y, &cur_normal );

      add_vecs( &center_pnt, &cur_tan, &cur_pnt );
      add_vecs( &cur_pnt, &cur_normal, &cur_pnt );

      circ_pnts[ i ] = cur_pnt;
    }

    if( cur_crv.osc_circ_seg == K_NOT_USED )
      cur_crv.osc_circ_seg = cagdAddPolyline( circ_pnts, NUM_OSC_PNTS );
    else
      cagdReusePolyline( cur_crv.osc_circ_seg, circ_pnts, NUM_OSC_PNTS );

    free( circ_pnts );

    cagdRedraw();
  }

  return 0;
}
