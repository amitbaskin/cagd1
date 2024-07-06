#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"
#include "circle.h"


/******************************************************************************
* init_circle_data
******************************************************************************/
static void init_circle_data( double         param,
                              double        *p_radius,
                              circle_data_t *p_circle_data )
{
  frenet_t frenet = { 0 };

  calc_frenet( param, &frenet );

  if (p_radius == NULL)
    p_circle_data->radius = get_scale_inv_or_zero( frenet.crvtr );
  else
    p_circle_data->radius = *p_radius;

  copy_vec( &frenet.csys[ TT ], &p_circle_data->T_axis );
  copy_vec( &frenet.csys[ NN ], &p_circle_data->N_axis );
  get_center_pnt( param, p_circle_data );

  p_circle_data->is_center_defined = TRUE;
}


/******************************************************************************
* draw_other_crv
******************************************************************************/
int draw_other_crv( int     num_pnts,
                    double *p_radius,
                    int    *p_seg_id )
{
  int is_error = validate_pre_draw( num_pnts );

  if( is_error == 0 )
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
        circle_data_t circle_data = { 0 };

        double param = cur_crv.domain[ 0 ] + jump * i;

        init_circle_data( param, p_radius, &circle_data );
        copy_vec( &circle_data.center, &pnts[ i ] );
      }

      if( *p_seg_id == K_NOT_USED )
        *p_seg_id = cagdAddPolyline( pnts, num_pnts );
      else
        cagdReusePolyline( *p_seg_id, pnts, num_pnts );
    }

    free( pnts );
  }

  cagdRedraw();

  return is_error;
}
