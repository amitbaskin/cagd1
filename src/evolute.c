#define _USE_MATH_DEFINES
#include <math.h>

#include "trsn.h"
#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"


/******************************************************************************
* draw_evolute
******************************************************************************/
int draw_evolute( int num_pnts )
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
        frenet_t frenet;

        double param = cur_crv.domain[ 0 ] + jump * i;

        calc_frenet( param, &frenet );

        get_crvtr_data( param, &frenet, &crvtr_data );

        pnts[ i ] = crvtr_data.center;
        //cagdAddPoint( &crvtr_data.center ); // temporary for debug
        //cagdRedraw(); // temporary for debug
      }

      set_evolute_color();

      if( cur_crv.evolute_seg == K_NOT_USED )
        cur_crv.evolute_seg = cagdAddPolyline( pnts, num_pnts + 1 );
      else
        cagdReusePolyline( cur_crv.evolute_seg, pnts, num_pnts + 1 );
    }

    free( pnts );
  }

  cagdRedraw();

  return is_error;
}
