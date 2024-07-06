#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"
#include "circle.h"
#include "color.h"



/******************************************************************************
* eval_osc_circ
******************************************************************************/
static void eval_osc_circ( double          param,
                           circle_data_t  *p_circle_data,
                           const frenet_t *p_frenet,
                           CAGD_POINT     *rp_out )
{
  CAGD_POINT T_axis;
  CAGD_POINT N_axis;

  copy_vec( &p_frenet->csys[ TT ], &T_axis );
  copy_vec( &p_frenet->csys[ NN ], &N_axis );
  copy_vec( &p_circle_data->center, rp_out );

  scale_vec( p_circle_data->radius * sin( param ), &T_axis );
  scale_vec( p_circle_data->radius * cos( param ), &N_axis );

  add_vecs( rp_out, &T_axis, rp_out );
  add_vecs( rp_out, &N_axis, rp_out );
}


/******************************************************************************
* draw_osc_circle
******************************************************************************/
int draw_osc_circle( double param, frenet_t *p_frenet )
{
  int is_error = FALSE;

  CAGD_POINT *pnts =
    ( CAGD_POINT * ) malloc( sizeof( CAGD_POINT ) * ( NUM_OSC_PNTS + 1 ) );

  is_error = pnts == NULL;

  if( is_error == FALSE )
  {
    circle_data_t circle_data;
    circle_data.radius = get_scale_inv_or_zero( p_frenet->crvtr );
    copy_vec( &p_frenet->csys[ NN ], &circle_data.vec_to_center );

    set_osc_circ_color();

    is_error = draw_circle(  param,
                             p_frenet,
                            &circle_data,
                            &cur_crv.osc_circ_seg,
                             pnts );

    free( pnts );
  }

  return is_error;
}
