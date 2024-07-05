#define _USE_MATH_DEFINES
#include <math.h>

#include "trsn.h"
#include "vectors.h"
#include "frenet.h"


typedef struct
{
  double pos[ 3 ];
  double radius;
  double pitch;
} helix_t;


void eval_helix( double      param,
                 double      radius,
                 double      pitch,
                 CAGD_POINT *start_pnt,
                 CAGD_POINT *out )
{
  out->x = radius * cos( param ) + start_pnt->x - radius;
  out->y = radius * sin( param ) + start_pnt->y;
  out->z = pitch * param + start_pnt->z;
}


int draw_helix( double param, frenet_t *frenet )
{
  int ok = scale_not_zero( frenet->crvtr ) && scale_not_zero( frenet->trsn );

  if( ok )
  {
    CAGD_POINT start_pnt;

    CAGD_POINT *pnts =
      ( CAGD_POINT * ) malloc( sizeof( CAGD_POINT ) * (TRSN_CYCLES * TRSN_DENSITY_SAMPLES + 2 ) );

    double radius = 1 / frenet->crvtr;
    double pitch = frenet->trsn;

    eval_cur_crv( param, POSITION, &start_pnt );

    if( pnts != NULL )
    {
      double jump = ( double ) 1 / TRSN_DENSITY_SAMPLES;

      for( int i = 0; i < TRSN_CYCLES * TRSN_DENSITY_SAMPLES + 1; ++i )
      {
        CAGD_POINT pnt = { 0 };

        eval_helix( jump * i, radius, frenet->trsn, &start_pnt, &pnt );
        pnts[ i ] = pnt;

        //cagdAddPoint( &pnt ); // temporary for debug
        //cagdRedraw(); // temporary for debug
      }

      set_helix_color();

      if( cur_crv.helix_seg == K_NOT_USED )
        cur_crv.helix_seg = cagdAddPolyline( pnts, TRSN_CYCLES * TRSN_DENSITY_SAMPLES + 1);
      else
        cagdReusePolyline( cur_crv.helix_seg, pnts, TRSN_CYCLES * TRSN_DENSITY_SAMPLES + 1);
    }

    free( pnts );
  }

  return !ok;
}
