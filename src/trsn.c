#define _USE_MATH_DEFINES
#include <math.h>

#include "trsn.h"
#include "vectors.h"
#include "frenet.h"


/******************************************************************************
* eval_helix
******************************************************************************/
static void eval_helix( double            param,
                        double            radius,
                        double            pitch,
                        const CAGD_POINT *p_start_pnt,
                        CAGD_POINT       *rp_out )
{
  rp_out->x = radius * cos( param ) + p_start_pnt->x - radius;
  rp_out->y = radius * sin( param ) + p_start_pnt->y;
  rp_out->z = pitch * param + p_start_pnt->z;
}


/******************************************************************************
* draw_helix
******************************************************************************/
int draw_helix( double param, frenet_t *p_frenet )
{
  int ok = scale_not_zero( p_frenet->crvtr ) &&
           scale_not_zero( p_frenet->trsn );

  if( ok )
  {
    CAGD_POINT start_pnt;

    CAGD_POINT *pnts =
      ( CAGD_POINT * ) malloc( sizeof( CAGD_POINT ) *
                               ( HELIX_CYCLES * HELIX_DENSITY_SAMPLES +
                                 HELIX_CYCLES + 1 ) );

    double radius = 1 / p_frenet->crvtr;
    double pitch = p_frenet->trsn;

    eval_cur_crv( param, POSITION, &start_pnt );

    if( pnts != NULL )
    {
      double jump = ( double ) 1 / HELIX_DENSITY_SAMPLES;

      for( int i = 0;
           i < HELIX_CYCLES * HELIX_DENSITY_SAMPLES + HELIX_CYCLES; ++i )
      {
        CAGD_POINT pnt = { 0 };

        eval_helix( jump * i * 2 * M_PI,
                    radius,
                    p_frenet->trsn,
                    &start_pnt,
                    &pnt );

        pnts[ i ] = pnt;
      }

      set_helix_color();

      if( cur_crv.helix_seg == K_NOT_USED )
      {
        cur_crv.helix_seg =
          cagdAddPolyline( pnts, HELIX_CYCLES * HELIX_DENSITY_SAMPLES +
                                 HELIX_CYCLES );
      }
      else
      {
        cagdReusePolyline( cur_crv.helix_seg,
                           pnts,
                           HELIX_CYCLES * HELIX_DENSITY_SAMPLES +
                           HELIX_CYCLES );
      }
    }

    free( pnts );
  }

  return !ok;
}
