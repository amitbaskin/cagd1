#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"
#include "circle.h"
#include "color.h"


/******************************************************************************
* init_circle_circle
******************************************************************************/
static int init_circle_data( frenet_t      *p_frenet,
                              circle_data_t *p_circle_data )
{
  int is_error = get_scale_inv_or_zero( p_frenet->crvtr,
                                        &p_circle_data->radius );

  if( is_error == FALSE )
  {
    copy_vec( &p_frenet->csys[ TT ], &p_circle_data->T_axis );
    copy_vec( &p_frenet->csys[ NN ], &p_circle_data->N_axis );
  }

  return is_error;
}


/******************************************************************************
* draw_osc_circle
******************************************************************************/
int draw_osc_circle( double param, frenet_t *p_frenet )
{
  int is_error = scale_not_zero( p_frenet->crvtr ) ? FALSE : TRUE;

  if( is_error == FALSE )
  {
    circle_data_t circle_data;
    circle_data.is_center_defined = FALSE;

    init_circle_data( p_frenet, &circle_data );
    set_osc_circ_color();

    is_error = draw_circle(  param,
                            &circle_data,
                            &cur_crv.osc_circ_seg );
  }

  return is_error;
}
