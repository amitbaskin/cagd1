#include "cur_crv.h"
#include "cagd.h"
#include "vectors.h"
#include "frenet.h"
#include "evolute_n_offset.h"
#include "color.h"

extern int num_samples;


/******************************************************************************
* print_err
******************************************************************************/
void print_err( char *str )
{
  errno = EPERM;
  perror( str );
  cagdSetHelpText( str );
  cagdShowHelp();
}


/******************************************************************************
* eval_cur_crv
******************************************************************************/
void eval_cur_crv( double param, int d_level, CAGD_POINT *rp_out )
{
  e2t_setparamvalue( param, E2T_PARAM_T );
  rp_out->x = e2t_evaltree( cur_crv.trees[ X_TREE ][ d_level ] );
  rp_out->y = e2t_evaltree( cur_crv.trees[ Y_TREE ][ d_level ] );
  rp_out->z = e2t_evaltree( cur_crv.trees[ Z_TREE ][ d_level ] );
}


/******************************************************************************
* free_all_segs
******************************************************************************/
void free_all_segs( BOOL clean_cur_crv_seg )
{
  if( clean_cur_crv_seg )
  {
    cagdFreeSegment( cur_crv.my_seg );
    cur_crv.my_seg = K_NOT_USED;
  }

  cagdFreeSegment( cur_crv.osc_circ_seg );
  cur_crv.osc_circ_seg = K_NOT_USED;

  cagdFreeSegment( cur_crv.helix_seg );
  cur_crv.helix_seg = K_NOT_USED;

  cagdFreeSegment( cur_crv.evolute_seg );
  cur_crv.evolute_seg = K_NOT_USED;

  cagdFreeSegment( cur_crv.offset_seg );
  cur_crv.offset_seg = K_NOT_USED;

  for( int i = 0; i < 3; ++i )
  {
    cagdFreeSegment( cur_crv.frenet_segs[ i ] );
    cur_crv.frenet_segs[ i ] = K_NOT_USED;
  }
}


/******************************************************************************
* clean_cur_crv
******************************************************************************/
void clean_cur_crv()
{
  system( "cls" );
  free_all_segs( TRUE );
  cagdRedraw();
  cur_crv.defined = 0;

  for( int i = 0; i < SPACE_DIM; ++i )
  {
    for( int j = 0; j < DERIVATIVE_LEVEL; ++j )
    {
      e2t_freetree( cur_crv.trees[ i ][ j ] );
      cur_crv.trees[ i ][ j ] = NULL;
    }
  }
}


/******************************************************************************
* init_all_segs
******************************************************************************/
void init_all_segs()
{
  cur_crv.my_seg       = K_NOT_USED;
  cur_crv.osc_circ_seg = K_NOT_USED;
  cur_crv.helix_seg    = K_NOT_USED;
  cur_crv.evolute_seg  = K_NOT_USED;
  cur_crv.offset_seg   = K_NOT_USED;

  for( int i = 0; i < 3; ++i )
    cur_crv.frenet_segs[ i ] = K_NOT_USED;
}


/******************************************************************************
* draw_cur_crv
******************************************************************************/
int draw_cur_crv( int num_pnts )
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
                                                ( num_pnts + 2 ) );

    if( pnts != NULL )
    {
      double jump = ( cur_crv.domain[ 1 ] - cur_crv.domain[ 0 ] ) / num_pnts;

      for( int i = 0; i < num_pnts + 1; ++i )
      {
        CAGD_POINT pnt = { 0 };
        double param = cur_crv.domain[ 0 ] + jump * i;

        eval_cur_crv( param, POSITION, &pnt );
        pnts[ i ] = pnt;
      }

      cur_crv.my_seg = cagdAddPolyline( pnts, num_pnts + 1 );
      cagdRedraw();
    }

    free( pnts );
  }

  /*if( cur_crv.draw_evolute )
  {
    set_evolute_color();
    draw_other_crv( num_pnts, NULL, &cur_crv.evolute_seg );
  }

  if( cur_crv.draw_offset )
  {
    set_offset_color();
    draw_other_crv( num_pnts, &cur_crv.offset, &cur_crv.offset_seg );
  }*/

  set_default_color();

  cagdRedraw();

  return is_error;
}

/******************************************************************************
* get_param_from_segment_number
******************************************************************************/
double get_param_from_segment_number( int seg_num )
{
  double domain_size = cur_crv.domain[1] - cur_crv.domain[0];
  double seg_size = domain_size / (double)num_samples;
  double param = cur_crv.domain[0] + seg_size * (double)seg_num;

  if( param > cur_crv.domain[1] )
    param = cur_crv.domain[1];

  return param;
}
