#include "cur_crv.h"
#include "cagd.h"


void print_err( char *str )
{
  errno = EPERM;
  perror( str );
  cagdSetHelpText( str );
  cagdShowHelp();
}


void eval_cur_crv( double param, int d_level, CAGD_POINT *pnt )
{
  e2t_setparamvalue( param, E2T_PARAM_T );
  pnt->x = e2t_evaltree( cur_crv.trees[ 0 ][ d_level ] );
  pnt->y = e2t_evaltree( cur_crv.trees[ 1 ][ d_level ] );
  pnt->z = e2t_evaltree( cur_crv.trees[ 2 ][ d_level ] );
}


void draw_cur_crv( int num_pnts )
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
    CAGD_POINT *pnts = ( CAGD_POINT * ) malloc( sizeof( CAGD_POINT ) * ( num_pnts + 2 ) );

    if( pnts != NULL )
    {
      double jump = ( cur_crv.domain[ 1 ] - cur_crv.domain[ 0 ] ) / num_pnts;

      for( int i = 0; i < num_pnts + 1; ++i )
      {
        CAGD_POINT pnt = { 0 };
        double param = cur_crv.domain[ 0 ] + jump * i;
        printf( "%f\n", param );

        eval_cur_crv( param, 0, &pnt );
        pnts[ i ] = pnt;

        cagdAddPoint( &pnt ); // temporary for debug
        cagdRedraw(); // temporary for debug
        int x = 5; // dummy line for break point
      }

      cagdAddPolyline( pnts, num_pnts + 1 ); // returns the polyline ID
      cagdRedraw();
    }

    free( pnts );
  }
}