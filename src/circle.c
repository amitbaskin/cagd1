#define _USE_MATH_DEFINES
#include <math.h>

#include "vectors.h"
#include "frenet.h"
#include "crvtr.h"
#include "circle.h"


/******************************************************************************
* eval_circ
******************************************************************************/
void eval_circ( double            param,
                double            radius,
                const CAGD_POINT *p_center,
                const frenet_t   *p_frenet,
                CAGD_POINT       *rp_out )
{
  CAGD_POINT T_axis;
  CAGD_POINT N_axis;

  copy_vec( &p_frenet->csys[ TT ], &T_axis );
  copy_vec( &p_frenet->csys[ NN ], &N_axis );
  copy_vec( p_center, rp_out );

  scale_vec( radius * sin( param ), &T_axis );
  scale_vec( radius * cos( param ), &N_axis );

  add_vecs( rp_out, &T_axis, rp_out );
  add_vecs( rp_out, &N_axis, rp_out );
}


/******************************************************************************
* get_circle_data
******************************************************************************/
void get_circle_data( double         param,
                      double        *p_radius,
                      frenet_t      *p_frenet,
                      circle_data_t *rp_crvtr_data )
{
  rp_crvtr_data->radius = p_radius != NULL ? *p_radius :
                          scale_not_zero( p_frenet->crvtr ) ?
                          1 / p_frenet->crvtr : 0.0;

  eval_cur_crv( param, POSITION, &rp_crvtr_data->crv_pos );
  copy_vec( &p_frenet->csys[ NN ], &rp_crvtr_data->vec );
  scale_vec( rp_crvtr_data->radius, &rp_crvtr_data->vec );

  add_vecs( &rp_crvtr_data->crv_pos,
            &rp_crvtr_data->vec,
            &rp_crvtr_data->center );
}
