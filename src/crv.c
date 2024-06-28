#include "crv.h"
#include "cagd.h"

CAGD_POINT eval_cur_crv_at_param( double param )
{
  CAGD_POINT result = { 0.0, 0.0, 0.0 };

  e2t_setparamvalue( param, E2T_PARAM_T );
  result.x = e2t_evaltree( cur_crv.trees[0][0] );
  result.y = e2t_evaltree( cur_crv.trees[1][0] );
  result.z = e2t_evaltree( cur_crv.trees[2][0] );

  return result;
}

void draw_cur_crv( int num_segments )
{
  if( num_segments <= 0 )
  {
    perror( "Invalid number of segments" );
    return;
  }

  if( cur_crv.defined == 0 )
  {
    perror( "current curve is not defined" );
    return;
  }

  CAGD_POINT *points_on_crv = (CAGD_POINT*) malloc( sizeof( CAGD_POINT ) * ( num_segments + 1 ) );

  if( points_on_crv != NULL )
  {
    for( int i = 0; i < num_segments + 1; i++ )
    {
      double param = ( cur_crv.domain[0] + cur_crv.domain[1] ) * ( (double) i / num_segments );
      CAGD_POINT point = eval_cur_crv_at_param( param );
      points_on_crv[i] = point;
    }

    cagdAddPolyline( points_on_crv, num_segments + 1 ); // returns the polyline ID
    cagdRedraw();
  }
}