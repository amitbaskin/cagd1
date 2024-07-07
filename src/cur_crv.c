#include "cur_crv.h"
#include "cagd.h"
#include "vectors.h"
#include "frenet.h"
#include "evolute_n_offset.h"
#include "color.h"
#include "menus.h"

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
void free_all_segs( BOOL clean_cur_crv_seg, BOOL clean_evolute_n_offset )
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

  if( clean_evolute_n_offset )
  {
    cagdFreeSegment( cur_crv.evolute_seg );
    cur_crv.evolute_seg = K_NOT_USED;

    cagdFreeSegment( cur_crv.offset_seg );
    cur_crv.offset_seg = K_NOT_USED;
  }

  for( int i = 0; i < 3; ++i )
  {
    cagdFreeSegment( cur_crv.frenet_segs[ i ] );
    cur_crv.frenet_segs[ i ] = K_NOT_USED;
  }

  for( int i = 0; i < NUM_SPHERE_CIRCS; ++i )
  {
    cagdFreeSegment( cur_crv.sphere_segs[ i ] );
    cur_crv.sphere_segs[ i ] = K_NOT_USED;
  }
}


/******************************************************************************
* clean_cur_crv
******************************************************************************/
void clean_cur_crv()
{
  system( "cls" );
  free_all_segs( TRUE, TRUE );
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

  for( int i = 0; i < NUM_SPHERE_CIRCS; ++i )
    cur_crv.sphere_segs[ i ] = K_NOT_USED;
}


/******************************************************************************
* validate_pre_draw
******************************************************************************/
int validate_pre_draw( int num_pnts )
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

  return is_error;
}


/******************************************************************************
* get_jump_sample_val
******************************************************************************/
double get_jump_sample_val( double start, double end, int num_pnts )
{
  return ( end - start ) / ( ( double )num_pnts - 1 );
}


/******************************************************************************
* draw_cur_crv
******************************************************************************/
int draw_cur_crv( int num_pnts )
{
  int is_error = validate_pre_draw( num_pnts );

  if( is_error == FALSE )
  {
    CAGD_POINT *pnts = ( CAGD_POINT * ) malloc( sizeof( CAGD_POINT ) *
                                                num_pnts );

    if( pnts != NULL )
    {
      double jump = get_jump_sample_val( cur_crv.domain[ 0 ],
                                         cur_crv.domain[ 1 ],
                                         num_pnts );

      for( int i = 0; i < num_pnts; ++i )
      {
        double param = cur_crv.domain[ 0 ] + jump * i;
        eval_cur_crv( param, POSITION, &pnts[ i ] );
      }

      set_default_color();
      cur_crv.my_seg = cagdAddPolyline( pnts, num_pnts );
    }

    free( pnts );
  }

  if( is_show_evolute_menu_checked() )
  {
    set_evolute_color();
    draw_other_crv( num_pnts * 3, NULL, &cur_crv.evolute_seg );
  }

  if( is_show_offset_curve_menu_checked() )
  {
    set_offset_color();
    draw_other_crv( num_pnts * 2, &cur_crv.offset, &cur_crv.offset_seg );
  }

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
