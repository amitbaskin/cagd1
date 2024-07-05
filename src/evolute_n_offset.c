#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"


/******************************************************************************
* draw_other_crv
******************************************************************************/
int draw_other_crv( int     num_pnts,
                    double *p_radius,
                    int    *p_seg_id )
{
  int is_error = 0;

  if( num_pnts < 0 )
  {
    print_err( "Invalid number of pnts" );
    is_error = 1;
  }

  if( is_error == 0 && cur_crv.defined == 0 )
  {
    print_err( "current curve is not defined" );
    is_error = 1;
  }

  if( is_error == 0 )
  {
    CAGD_POINT *pnts = ( CAGD_POINT * ) malloc( sizeof( CAGD_POINT ) *
                                                (num_pnts + 2) );

    if( pnts != NULL )
    {
      double jump = ( cur_crv.domain[ 1 ] - cur_crv.domain[ 0 ] ) / num_pnts;

      for( int i = 0; i < num_pnts + 1; ++i )
      {
        crvtr_data_t crvtr_data;
        frenet_t     frenet;

        double param = cur_crv.domain[ 0 ] + jump * i;

        calc_frenet( param, &frenet );

        get_crvtr_data( param, p_radius , &frenet, &crvtr_data );

        pnts[ i ] = crvtr_data.center;
      }

      if( *p_seg_id == K_NOT_USED )
        *p_seg_id = cagdAddPolyline( pnts, num_pnts + 1 );
      else
        cagdReusePolyline( *p_seg_id, pnts, num_pnts + 1 );
    }

    free( pnts );
  }

  cagdRedraw();

  return is_error;
}
